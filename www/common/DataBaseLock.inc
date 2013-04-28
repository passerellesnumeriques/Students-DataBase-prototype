<?php 
class DataBaseLock {
	
	public static function clean() {
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		DataBase::$conn->execute("DELETE FROM DataLocks WHERE timestamp <= ".(time()-5*60));
		DataBase::$conn->execute("DELETE FROM DataLocksFields WHERE timestamp <= ".(time()-5*60));
		DataBase::$conn->execute("UNLOCK TABLES");
	}
	
	public static function lock($table, $fields, &$locked_by, $generate_script = true) {
		DataBaseLock::clean();
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		$id = DataBaseLock::_lock($table, $fields, $locked_by, $generate_script);
		DataBase::$conn->execute("UNLOCK TABLES");
		return $id;
	}
	private static function _lock($table, $fields, &$locked_by, $generate_script) {
		$res = DataBase::$conn->execute("SELECT * FROM DataLocks WHERE locked_table='".DataBase::$conn->escape_string($table)."'");
		if ($res <> null) {
			$where = "";
			foreach ($fields as $name=>$value) {
				if ($where <> "") $where .= " OR ";
				$where .= "(`field`='".DataBase::$conn->escape_string($name)."' AND `value`='".DataBase::$conn->escape_string($value).")";
			}
			while (($row = DataBase::$conn->next_row($res)) <> null) {
				if ($row["locker_domain"] <> PNApplication::$instance->user_management->domain ||
					$row["locker_username"] <> PNApplication::$instance->user_management->username) {
					if ($fields == null) {
						// we want to lock the full table -> not possible
						$locked_by = $row["locker_domain"]."\\".$row["locker_username"];
						return null;
					}
					if ($row["full_table"] == 1) {
						$locked_by = $row["locker_domain"]."\\".$row["locker_username"];
						return null;
					}
					$lock_id = $row["id"];
					$res2 = DataBase::$conn->execute("SELECT * FROM DataLocksFields WHERE lock_id=".$lock_id." AND ".$where);
					if ($res2 <> null && DataBase::$conn->next_row($res2) <> null) {
						$locked_by = $row["locker_domain"]."\\".$row["locker_username"];
						return null;
					}
				}
			}
		}
		$ts = time();
		DataBase::$conn->execute(
			"INSERT INTO DataLocks (`timestamp`,`locker_domain`,`locker_username`,`locked_table`,`full_table`)".
			" VALUE (".
			$ts.
			",'".DataBase::$conn->escape_string(PNApplication::$instance->user_management->domain)."'".
			",'".DataBase::$conn->escape_string(PNApplication::$instance->user_management->username)."'".
			",'".DataBase::$conn->escape_string($table)."'".
			",".($fields == null ? 1 : 0).
			")"
		);
		$id = DataBase::$conn->get_insert_id();
		if ($fields <> null) {
			$sql = "INSERT INTO DataLocksFields (`lock_id`,`timestamp`,`field`,`value`) VALUES ";
			$first = true;
			foreach ($fields as $name=>$value) {
				if ($first) $first = false; else $sql .= ",";
				$sql .= "(";
				$sql .= $id.",".$ts;
				$sql .= ",'".DataBase::$conn->escape_string($name)."'";
				$sql .= ",'".DataBase::$conn->escape_string($value)."'";
				$sql .= ")";
			}
			DataBase::$conn->execute($sql);
		}
		if ($generate_script)
			DataBaseLock::generate_script($id);
		return $id;
	}
	
	public static function unlock($id) {
		$result = null;
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		$r = DataBase::$conn->execute("SELECT locker_domain,locker_username FROM DataLocks WHERE id=".$id);
		if ($r) {
			$row = DataBase::$conn->next_row($r);
			if ($row) {
				if ($row["locker_domain"] == PNApplication::$instance->user_management->domain &&
					$row["locker_username"] == PNApplication::$instance->user_management->username) {
					DataBase::$conn->execute("DELETE FROM DataLocks WHERE id=".$id);
					DataBase::$conn->execute("DELETE FROM DataLocksFields WHERE lock_id=".$id);
				} else
					$result = "You are not the owner of this lock";
			}
		}
		DataBase::$conn->execute("UNLOCK TABLES");
		return $result;
	}
	
	public static function update($id) {
		$result = null;
		$ts = time();
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		$r = DataBase::$conn->execute("SELECT locker_domain,locker_username FROM DataLocks WHERE id=".$id);
		if ($r) {
			$row = DataBase::$conn->next_row($r);
			if ($row) {
				if ($row["locker_domain"] == PNApplication::$instance->user_management->domain &&
					$row["locker_username"] == PNApplication::$instance->user_management->username) {
					DataBase::$conn->execute("UPDATE DataLocks SET timestamp=".$ts." WHERE id=".$id);
					DataBase::$conn->execute("UPDATE DataLocksFields SET timestamp=".$ts." WHERE lock_id=".$id);
				} else
					$result = "You are not the owner of this lock";
			} else
				$result = "This lock does not exist";
		} else
			$result = "This lock does not exist";
		DataBase::$conn->execute("UNLOCK TABLES");
		DataBaseLock::clean();
		return $result;
	}
	
	public static function check($id, $table, $fields) {
		$ts = time();
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		$r = DataBase::$conn->execute("SELECT * FROM DataLocks WHERE id=".$id);
		// check if it exist
		if (!$r) { DataBase::$conn->execute("UNLOCK TABLES"); return false; }
		$r = DataBase::$conn->next_row($r);
		if (!$r) { DataBase::$conn->execute("UNLOCK TABLES"); return false; }
		// check the locker is the current user
		if ($r["locker_domain"] <> PNApplication::$instance->user_management->domain ||
			$r["locker_username"] <> PNApplication::$instance->user_management->username) {
			DataBase::$conn->execute("UNLOCK TABLES");
			return false;
		}
		// check the table
		if ($r["locked_table"] <> $table) { DataBase::$conn->execute("UNLOCK TABLES"); return false; }
		// check the fields
		if (!$r["full_table"]) {
			$res = DataBase::$conn->execute("SELECT * FROM DataLocksFields WHERE lock_id=".$id);
			$remaining = array_merge($fields);
			while (($r = DataBase::$conn->next_row($res)) <> null) {
				if (!isset($remaining[$r["field"]])) continue; // strange, but acceptable
				if ($remaining[$r["field"]] <> $r["value"]) {
					DataBase::$conn->execute("UNLOCK TABLES");
					return false;
				}
				unset($remaining[$r["field"]]);
			}
			if (count($remaining) > 0) {
				DataBase::$conn->execute("UNLOCK TABLES");
				return false;
			}
		}
		// ok, let's update the timestamps
		DataBase::$conn->execute("UPDATE DataLocks SET timestamp=".$ts." WHERE id=".$id);
		DataBase::$conn->execute("UPDATE DataLocksFields SET timestamp=".$ts." WHERE lock_id=".$id);
		DataBase::$conn->execute("UNLOCK TABLES");
		return true;
	}
	
	public static function has_lock($table, $field = null, $value = null) {
		DataBase::$conn->execute("LOCK TABLES DataLocks WRITE, DataLocksFields WRITE");
		$res = DataBase::$conn->execute(
			"SELECT * FROM DataLocks WHERE ".
			"locker_domain='".DataBase::$conn->escape_string(PNApplication::$instance->user_management->domain)."'".
			" AND locker_username='".DataBase::$conn->escape_string(PNApplication::$instance->user_management->username)."'".
			" AND locked_table='".DataBase::$conn->escape_string($table)."'");
		// check if it exist
		if (!$res) { DataBase::$conn->execute("UNLOCK TABLES"); return false; }
		while (($r = DataBase::$conn->next_row($res)) <> null) {
			if (!$r) { DataBase::$conn->execute("UNLOCK TABLES"); return false; }
			// if all table is locked, this is ok
			if ($r["full_table"] == 1) { DataBase::$conn->execute("UNLOCK TABLES"); return true; }
			// else, we need to check the field
			if ($field == null) break; // we need the full table
			$lock_id = $r["id"];
			$res2 = DataBase::$conn->execute("SELECT `value` FROM DataLocksFields WHERE lock_id=".$lock_id." AND field='".DataBase::$conn->escape_string($field)."'");
			if (!$res2) continue;
			$res2 = DataBase::$conn->next_row($res2);
			if ($res2 == null) continue;
			// field found
			if ($value !== null && $value <> $res2['value']) continue; // value do not match
			DataBase::$conn->execute("UNLOCK TABLES");
			return true;
		}
		DataBase::$conn->execute("UNLOCK TABLES");
		return true;
	}
	
	private static function generate_script($id) {
?>
<script type='text/javascript'>
window.pn_database_locks.add_lock(<?php echo $id?>);
</script>
<?php
	}
	
}
?>
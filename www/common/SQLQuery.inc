<?php 
class SQLQuery {

	/** utility method to ease chained usage: SQLQuery::create()->select(...)->field(...)->.... */
	public static function &create() { $q = new SQLQuery(); return $q; }
	
	public $tables = array();
	public $fields = array();
	public $where = array();
	public $joins = array();
	public $limit_from = 0;
	public $limit_number = -1;
	
	private $field_id_counter = 0;
	private $table_id_counter = 0;
	
	public function field_id() { return "f".($this->field_id_counter++); }
	public function table_id() { return "t".($this->table_id_counter++); }
	
	public function &select() {
		$nb = func_num_args();
		if ($nb > 1) {
			for ($i = 0; i < $nb; $i++)
				$this->tables[func_get_arg($i)] = null;
		} else {
			$o = func_get_arg(0);
			if (is_string($o))
				$this->tables[$o] = null;
			else foreach ($o as $table_name=>$alias)
				if (is_string($table_name))
					$this->tables[$table_name] = $alias;
				else
					$this->tables[$alias] = null;
		}
		return $this;
	}
	
	/** if name is null, it means only one table is selected, and the parameter table is used as the name of the field */
	public function &field($table,$name=null,$alias=null) {
		if ($name == null)
			foreach ($this->tables as $t=>$a)
				array_push($this->fields, array($t,$table,null));
		else
			array_push($this->fields, array($table,$name,$alias));
		return $this;
	}
	
	/**
	 * Usages:
	 *  - SQL condition: where("`field1`='value'")
	 *  - map of values: where(array("field1"=>"value1","field2"=>"value2",...))
	 *  - single value:  where("field1","value1")
	 *  - condition:     where("field1", "<=", "value1")
	 */
	public function &where() {
		$nb = func_num_args();
		if ($nb == 1) {
			$o = func_get_arg(0);
			if (is_array($o))
				foreach (func_get_arg(0) as $field=>$value)
					$this->where($field, "=", $value);
			else
				array_push($this->where, $o);
		} else if ($nb == 2)
			$this->where(func_get_arg(0), "=", func_get_arg(1));
		else {
			$field = func_get_arg(0);
			$op = func_get_arg(1);
			$value = func_get_arg(2);
			if ($value === null) {
				if ($op == "=")
					array_push($this->where, "`".$field."` IS NULL");
				else
					array_push($this->where, "`".$field."` IS NOT NULL");
			} else
				array_push($this->where, "`".$field."`='".DataBase::$conn->escape_string($value)."'");
		}
		return $this;
	}
	
	public function &join($table_from, $table_to, $fields_matching, $alias = null) {
		array_push($this->joins, array($table_from, $table_to, $fields_matching, $alias));
		return $this;
	}
	public function get_join_alias($table_from, $table_to, $fields_matching) {
		foreach ($this->joins as $join) {
			if ($join[0] <> $table_from) continue;
			if ($join[1] <> $table_to) continue;
			$remaining = array_merge($fields_matching);
			$ok = true;
			foreach ($join[2] as $f1=>$f2) {
				if (!isset($remaining[$f1])) { $ok = false; break; }
				if ($remaining[$f1] <> $f2) { $ok = false; break; }
				$remaining[$f1] = null;
			}
			if (!$ok) continue;
			foreach ($remaining as $f=>$v) if ($v !== null) { $ok = false; break; }
			if (!$ok) continue;
			return $join[3] <> null ? $join[3] : $table_to;
		}
		return null;
	}
	
	public function limit($from, $number) {
		$this->limit_from = $from;
		$this->limit_number = $number;
	}
	
	public function generate() {
		$sql = "SELECT ";
		if (count($this->fields) == 0)
			$sql .= "*";
		else {
			$first = true;
			foreach ($this->fields as $f) {
				if ($first) $first = false; else $sql .= ",";
				$sql .= "`".$f[0]."`.`".$f[1]."`";
				if ($f[2] <> null) $sql .= " AS `".$f[2]."`";
			}
		}
		$sql .= " FROM ";
		$first = true;
		foreach ($this->tables as $name=>$alias) {
			if ($first) $first = false; else $sql .= ",";
			$sql .= "`".$name."`";
			if ($alias <> null) $sql .= " AS `".$alias."`";
		}
		foreach ($this->joins as $join) {
			$sql .= " LEFT JOIN `".$join[1]."`";
			if ($join[3] <> null) $sql .= " AS `".$join[3]."`";
			$sql .= " ON ";
			$first = true;
			foreach ($join[2] as $field_from=>$field_to) {
				if ($first) $first = false; else $sql .= " AND ";
				$sql .= "`".$join[0]."`.`".$field_from."`=`".($join[3] <> null ? $join[3] : $join[1])."`.`".$field_to."`";
			}
		}
		if (count($this->where) > 0) {
			$sql .= " WHERE ";
			$first = true;
			foreach ($this->where as $w) {
				if ($first) $first = false; else $sql .= " AND ";
				$sql .= $w;
			}
		}
		if ($this->limit_number > 0) {
			$sql .= " LIMIT ".$this->limit_from.",".$this->limit_number;
		}
		return $sql;
	}
	
	public function execute() {
		$sql = $this->generate();
		$res = DataBase::$conn->execute($sql);
		if ($res === FALSE) return null;
		$a = array();
		while (($r = DataBase::$conn->next_row($res)) !== FALSE)
			array_push($a, $r);
		return $a;
	}
	
	public function execute_single_field() {
		$a = $this->execute();
		if ($a === null) return null;
		$a2 = array();
		foreach ($a as $r) array_push($a2, $r[0]);
		return $a2;
	}
	
	public function execute_single_value() {
		$a = $this->execute_single_field();
		if ($a === null) return null;
		return $a[0];
	}
	
	public function execute_single_row() {
		$sql = $this->generate();
		$res = DataBase::$conn->execute($sql);
		if ($res === FALSE) return null;
		$r = DataBase::$conn->next_row($res);
		if ($r === FALSE) return null;
		return $r;
	}
}
?>
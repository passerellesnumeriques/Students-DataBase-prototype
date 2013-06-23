<?php
/** Helper to build and execute a SQL query */
class SQLQuery {

	/** utility method to ease chained usage: SQLQuery::create()->select(...)->field(...)->.... */
	public static function &create() { $q = new SQLQuery(); return $q; }

	public function __construct($copy = null) {
		if ($copy) {
			$this->tables = $copy->tables;
			$this->fields = $copy->fields;
			$this->where = $copy->where;
			$this->joins = $copy->joins;
			$this->limit_from = $copy->limit_from;
			$this->limit_number = $copy->limit_number;
			$this->group_by = $copy->group_by;
		}
	}

	public $database = null;
	public $tables = array();
	public $fields = array();
	public $count = null;
	public $where = array();
	public $joins = array();
	public $limit_from = 0;
	public $limit_number = -1;
	public $group_by = array();
	public $order_by = array();
	public $bypass_security = false;

	private $field_id_counter = 0;
	private $table_id_counter = 0;

	/** Generates a unique field alias name (useful when building a big list) */
	public function field_id() { return "f".($this->field_id_counter++); }
	/** Generates a unique table alias name (useful when building a big list) */
	public function table_id() { return "t".($this->table_id_counter++); }

	public function &database($db_name) { $this->database = $db_name; return $this; }

	/** Prepares a select in the given tables.
	 * Usage:
	 *  - 1 parameter as string: the name of the table in which the select will be performed
	 *  - 1 parameter as array: array containing the list of table_name=>table_alias
	 *  - more than 1 parameter: list of table names (no aliases)
	 * @return SQLQuery
	 */
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

	/**
	 * Indicates a field should be returned by the quer.
	 * Usage:
	 *  (field_name): in case only one table was indicated in the select() method, this table will be used together with the given field
	 *  (table_name, field_name)
	 *  (table_name, field_name, field_alias)
	 */
	public function &field($table,$name=null,$alias=null) {
		if ($name == null)
			foreach ($this->tables as $t=>$a)
				array_push($this->fields, array($t,$table,null));
		else
			array_push($this->fields, array($table,$name,$alias));
		return $this;
	}

	public function &reset_fields() {
		$this->fields = array();
		return $this;
	}
	public function &count($alias = null) {
		$this->count = array($alias);
		return $this;
	}

	public function &bypass_security() { $this->bypass_security = true; return $this; }

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

	/** if only one table is used, we can use it as ($field,$values), meaning no need to specify the table */
	public function &where_in($table, $field, $values = null) {
		if ($values === null) {
			$values = $field;
			$field = $table;
			foreach ($this->tables as $name=>$alias)
				$table = $name;
		}
		$w = "`".$table."`.`".$field."` IN (";
		$first = true;
		foreach ($values as $value) {
			if ($first) $first = false; else $w .= ",";
			$w .= "'".$value."'";
		}
		$w .= ")";
		array_push($this->where, $w);
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

	public function &limit($from, $number) {
		$this->limit_from = $from;
		$this->limit_number = $number;
		return $this;
	}

	/** If only one parameter, it means that only one table is selected, and we will use this table */
	public function &group_by($table, $field=null) {
		if ($field == null) {
			$field = $table;
			foreach ($this->tables as $t=>$a) $table = $t;
		}
		array_push($this->group_by, array($table, $field));
		return $this;
	}

	/** Usage: (table,field,asc) or (alias,asc) or (alias) */
	public function &order_by($table, $field=null, $asc=true) {
		if ($field === null) {
			$field = $table;
			$table = null;
		} else if (is_bool($field)) {
			$asc = $field;
			$field = $table;
			$table = null;
		}
		array_push($this->order_by, array($table, $field, $asc));
		return $this;
	}

	public function generate() {
		if (!$this->bypass_security) {
			// validate the query
			require_once("component/data_model/DataModel.inc");
			$final_fields = array();
			foreach ($this->tables as $t=>$alias) {
				$table = DataModel::get()->getTable($t);
				if ($alias == null) $alias = $table->getName();
				if (count($this->fields) == 0 && $this->count === null) {
					foreach ($table->getColumns() as $col)
						array_push($final_fields, array($alias, $col->name, null));
				} else foreach ($this->fields as $f) {
					if ($f[0] == $t || $f[0] == $alias)
						array_push($final_fields, array($f[0], $table->getColumn($f[1])->name, $f[2])); // check the field is accessible
				}
				if (!$table->canReadAll())
					foreach ($table->table_specific_read_access as $specific) {
						$this->where($specific[0], $specific[1]());
					}
			}
			foreach ($this->joins as $join) {
				$table = DataModel::get()->getTable($join[1]);
				$alias = $join[3];
				if ($alias == null) $alias = $join[1];
				if (count($this->fields) == 0 && $this->count === null) {
					foreach ($table->getColumns() as $col)
						array_push($final_fields, array($alias, $col->name, null));
				} else foreach ($this->fields as $f) {
					if ($f[0] == $join[1] || $f[0] == $alias)
						array_push($final_fields, array($alias, $table->getColumn($f[1])->name, $f[2])); // check the field is accessible
				}
				if (!$table->canReadAll())
					foreach ($table->table_specific_read_access as $specific) {
						$this->where($specific[0], $specific[1]());
					}
			}
			$this->fields = $final_fields;
		}

		$sql = "SELECT ";
		if (count($this->fields) == 0 && $this->count === null && $this->bypass_security)
			$sql .= "*";
		else {
			$first = true;
			foreach ($this->fields as $f) {
				if ($first) $first = false; else $sql .= ",";
				$sql .= "`".$f[0]."`.`".$f[1]."`";
				if ($f[2] <> null) $sql .= " AS `".$f[2]."`";
			}
			if ($this->count !== null) {
				if ($first) $first = false; else $sql .= ",";
				$sql .= "COUNT(*)";
				if ($this->count[0] <> null)
					$sql .= " AS `".$this->count[0]."`";
			}
		}
		$sql .= " FROM ";
		$first = true;
		foreach ($this->tables as $name=>$alias) {
			if ($first) $first = false; else $sql .= ",";
			if ($this->database) $sql .= "`".$this->database."`.";
			$sql .= "`".$name."`";
			if ($alias <> null) $sql .= " AS `".$alias."`";
		}
		foreach ($this->joins as $join) {
			$sql .= " LEFT JOIN ";
			if ($this->database) $sql .= "`".$this->database."`.";
			$sql .= "`".$join[1]."`";
			if ($join[3] <> null) $sql .= " AS `".$join[3]."`";
			$sql .= " ON ";
			$first = true;
			foreach ($join[2] as $field_from=>$field_to) {
				if ($first) $first = false; else $sql .= " AND ";
				$sql .= "`".$join[0]."`.`".$field_from."`=";
				if (is_string($field_to))
					$sql .= "`".($join[3] <> null ? $join[3] : $join[1])."`.`".$field_to."`";
				else
					$sql .= "'".DataBase::escape_string($field_to[0])."'";
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
		if (count($this->group_by) > 0) {
			$sql .= " GROUP BY ";
			$first = true;
			foreach ($this->group_by as $g) {
				if ($first) $first = false; else $sql .= ",";
				$sql .= "`".$g[0]."`.`".$g[1]."`";
			}
		}
		if (count($this->order_by) > 0) {
			$sql .= " ORDER BY ";
			$first = true;
			foreach ($this->order_by as $o) {
				if ($first) $first = false; else $sql .= ",";
				if ($o[0] !== null)
					$sql .= "`".$o[0]."`.";
				$sql .= "`".$o[1]."` ".($o[2] ? "ASC" : "DESC");
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
		if (count($a) == 0) return null;
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

	public static function update($table, $fields_values, $where) {
		require_once("component/data_model/DataModel.inc");
		/* @var $t datamodel\Table */
		$t = DataModel::get()->getTable($table);
		$sql = "UPDATE `".$table."` SET ";
		$first = true;
		foreach ($fields_values as $field=>$value) {
			if (!$t->canModifyField($field)) throw new Exception("Access denied for column '".$field."' on table '".$table."'");
			if ($first) $first = false; else $sql .= ",";
			$sql .= "`".$field."`=";
			if ($value === null) $sql .= "NULL";
			else $sql .= "'".DataBase::$conn->escape_string($value)."'";
		}
		if ($where <> null) {
			$sql .= " WHERE ";
			if (is_string($where)) $sql .= $where;
			else {
				$first = true;
				foreach ($where as $field=>$value) {
					if ($first) $first = false; else $sql .= " AND ";
					$sql .= "`".$field."`";
					if ($value === null) $sql .= " IS NULL";
					else $sql .= "='".DataBase::$conn->escape_string($value)."'";
				}
			}
		}
		DataBase::$conn->execute($sql);
		if (DataBase::$conn->get_last_error_number() == 0)
			return DataBase::$conn->affected_rows();
		return -1;
	}

	public static function insert($table, $fields_values) {
		require_once("component/data_model/DataModel.inc");
		/* @var $t datamodel\Table */
		$t = DataModel::get()->getTable($table);
		if (!$t->canAdd()) throw new Exception("Access denied to add an entry in table '".$table."'");
		$sql = "INSERT INTO `".$table."` (";
		$first = true;
		foreach ($fields_values as $field=>$value) {
			if ($first) $first = false; else $sql .= ",";
			$sql .= "`".$field."`";
		}
		$sql .= ") VALUES (";
		$first = true;
		foreach ($fields_values as $field=>$value) {
			if ($first) $first = false; else $sql .= ",";
			if ($value === null) $sql .= "NULL";
			else $sql .= "'".DataBase::$conn->escape_string($value)."'";
		}
		$sql .= ")";
		DataBase::$conn->execute($sql);
		return DataBase::$conn->get_insert_id();
	}

	public static function remove_key($table, $key) {
		require_once("component/data_model/DataModel.inc");
		/* @var $t datamodel\Table */
		$t = DataModel::get()->getTable($table);
		$t->remove_key($key);
	}
	public static function remove_keys($table, $keys) {
		require_once("component/data_model/DataModel.inc");
		/* @var $t datamodel\Table */
		$t = DataModel::get()->getTable($table);
		$t->remove_keys($keys);
	}
	public static function remove_rows($table, $rows) {
		require_once("component/data_model/DataModel.inc");
		/* @var $t datamodel\Table */
		$t = DataModel::get()->getTable($table);
		$t->remove_rows($rows);
	}
}
?>
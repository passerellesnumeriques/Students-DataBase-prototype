<?php 
class DataContext {
	public $list;
	public $restrict_values = array();
	public $is_sub_request = false;
	public function __construct(&$list) {
		$this->list = &$list;
	}
	public function restrict_value($table, $field, $value) {
		if (!isset($this->restrict_values[$table]))
			$this->restrict_values[$table] = array();
		$this->restrict_values[$table][$field] = $value;
	}
}

abstract class DataPath {
	public $final_name;
	public $table;
	public $sql_table;
	public function __construct($table) {
		$this->table = $table;
	}
	public abstract function is_unique();
	public abstract function generate_sql(&$sql, &$ctx, &$top_path);
	public abstract function get_path();
}

/** root of path: starting table */
class DataPathRoot extends DataPath {
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		$sql->select($this->table);
		$this->sql_table = $this->table;
		if (isset($ctx->restrict_values[$this->table]))
			foreach ($ctx->restrict_values[$this->table] as $field=>$value)
				$sql->where($this->sql_table."`.`".$field, $value);
		return true;
	}
	public function is_unique() { return true; }
	public function debug() { return "Table ".$this->table." AS ".$this->sql_table; }
	public function get_path() { return $this->table."."; }
	public function get_update_where(&$ctx, &$model) {
		$where = "";
		if (isset($ctx->restrict_values[$this->table]))
			foreach ($ctx->restrict_values[$this->table] as $field=>$value) {
				if (strlen($where) > 0) $where .= " AND ";
				$where .= "`".$this->sql_table."`.`".$field."`='".DataBase::$conn->escape_string($value)."'";
			}
		return $where;
	}
}

/** end of path: specific field in current table */
class DataPathField extends DataPath {
	public $path;
	public $field;
	public $sql_field;
	public function __construct($path, $field) {
		parent::__construct($path->table);
		$this->path = $path;
		$this->field = $field;
	}
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		$this->sql_field = $sql->field_id();
		$sql->field($this->path->sql_table, $this->field, $this->sql_field);
		$this->sql_table = $this->path->sql_table;
		$search = @$ctx->list->search[$this->get_path()];
		if ($search !== null) {
			$col = $ctx->list->model->getTable($this->table)->getColumn($this->field);
			if ($col instanceof \data\ColumnString)
				$sql->where("LOWER(`".$this->sql_table."`.`".$this->field."`) LIKE LOWER('%".DataBase::$conn->escape_string($search)."%')");
			else
				$sql->where("`".$this->sql_table."`.`".$this->field."` = '".DataBase::$conn->escape_string($search)."'");
		}
		return true;
	}
	public function is_unique() { return $this->path->is_unique(); }
	public function get_path() { return $this->path->get_path().$this->field; }
	public function debug() { return $this->path->debug()." [Field ".$this->field." AS ".$this->sql_field."]"; }
	public function update_value(&$ctx, $value, &$model) {
		$sql = "UPDATE `".$this->table."` SET `".$this->field."`=";
		$col = $model->getTable($this->table)->getColumn($this->field);
		$value = $col->validate($value);
		if ($value === null) $sql .= "NULL";
		else $sql .= "'".DataBase::$conn->escape_string($value)."'";
		$sql .= " WHERE ";
		$sql .= $this->path->get_update_where($ctx, $model);
		DataBase::$conn->execute($sql);
	}
}

/** follows a foreign key from the current table */
class DataPathForeign extends DataPath {
	/** @var DataPath */
	public $path;
	/** @var \data\ForeignKey */
	public $foreign_key;
	public function __construct($path, $foreign_key) {
		parent::__construct($foreign_key->foreign_table);
		$this->path = $path;
		$this->foreign_key = $foreign_key;
	}
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->getTable($this->foreign_key->foreign_table)->getPrimaryKey()->name));
		if ($this->sql_table == null) {
			$this->sql_table = $sql->table_id();
			$sql->join($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->getTable($this->foreign_key->foreign_table)->getPrimaryKey()->name), $this->sql_table);
		}
		return true;
	}
	public function is_unique() { return $this->path->is_unique(); }
	public function get_path() {
		if ($this->path instanceof DataPathField)
			$s = $this->path->path->get_path(); // skip the field, which is the same
		else
			$s = $this->path->get_path();
		$s .= $this->foreign_key->name.">";
		return $s;
	}
	public function debug() { return $this->path->debug()." => Foreign key link on ".$this->foreign_key->name." To table ".$this->foreign_key->foreign_table; }
	public function get_update_where(&$ctx, &$model) {
		$where = "`".$model->getTable($this->foreign_key->foreign_table)->getPrimaryKey()->name."`=";
		$sql = new SQLQuery();
		$this->path->generate_sql($sql, $ctx, $this);
		$r = $sql->execute();
		if (!$r) {
			PNApplication::error("Entry to update not found");
			return null;
		}
		if (count($r) > 1) {
			PNApplication::error("More than one entry matches the update !");
			return null;
		}
		$where .= $r[0][0];
		return $where;
	}
}

/** reverse foreign key: join current table with the table containing the foreign key */
class DataPathJoin extends DataPath {
	public $path;
	public $foreign_key;
	public function __construct($path, $foreign_key) {
		parent::__construct($foreign_key->table->getName());
		$this->path = $path;
		$this->foreign_key = $foreign_key;
	}
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->foreign_key->table->getName(), array($ctx->list->model->getTable($this->path->table)->getPrimaryKey()->name=>$this->foreign_key->name));
		if ($this->sql_table == null) {
			$this->sql_table = $sql->table_id();
			$sql->join($this->path->sql_table, $this->foreign_key->table->getName(), array($ctx->list->model->getTable($this->path->table)->getPrimaryKey()->name=>$this->foreign_key->name), $this->sql_table);
		}
		return true;
	}
	public function is_unique() { return $this->path->is_unique(); }
	public function get_path() {
		// TODO
		if ($this->path instanceof DataPathField)
			$s = $this->path->path->get_path(); // skip the field, which is the same
		else
			$s = $this->path->get_path();
		$s .= $this->foreign_key->name.">";
		return $s;
	}
	public function debug() { return $this->path->debug()." => Join foreign key from ".$this->foreign_key->table->getName()." on ".$this->foreign_key->name; }
}

/** follows a link in the current table */
class DataPathLink extends DataPath {
	public $path;
	public $link;
	public function __construct($path, $link) {
		parent::__construct($link->table);
		$this->path = $path;
		$this->link = $link;
	}
	public function is_unique() { return $this->path->is_unique() && $this->link->unique; }
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if ($this->link->unique) {
			if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
			$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->link->table, $this->link->fields_matching);
			if ($this->sql_table == null) {
				$this->sql_table = $sql->table_id();
				$sql->join($this->path->sql_table, $this->link->table, $this->link->fields_matching, $this->sql_table);
			}
			return true;
		}
		// TODO
		return false;
	}
	public function get_path() { return $this->link->table."."; }
	public function debug() { 
		$s = $this->path->debug();
		$s .= " => LINK FORWARD ".($this->link->unique ? "UNIQUE" : "NOT UNIQUE");
		$s .= " (Table ".$this->link->table." to table ".$this->link->table_from->getName();
		$s .= " ON ";
		foreach ($this->link->fields_matching as $f1=>$f2)
			$s .= $f1."=".$f2.", ";
		$s .= ")";
		return $s;
	}
}

/** reverse link: a link to the current table exists in another table: join the 2 tables, using the linked data */
class DataPathLinkReverse extends DataPath {
	public $path;
	public $link;
	public function __construct($path, $table, $link) {
		parent::__construct($table);
		$this->path = $path;
		$this->link = $link;
	}
	public function is_unique() { return $this->path->is_unique() && $this->link->unique; }
	private $table_for_sub_requests = null;
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if ($this->link->unique) {
			if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
			$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->link->table_from->getName(), $this->link->fields_matching);
			if ($this->sql_table == null) {
				$this->sql_table = $sql->table_id();
				$sql->join($this->path->sql_table, $this->link->table_from->getName(), $this->link->fields_matching, $this->sql_table);
			}
			return true;
		}
		if ($ctx->is_sub_request) {
			$sql->select($this->link->table_from->getName());
			$this->sql_table = $this->link->table_from->getName();
			$table = $this->table_for_sub_requests <> null ? $this->table_for_sub_requests : $this->path->table;
			foreach ($this->link->fields_matching as $src=>$dst)
				$sql->where("`".$this->link->table_from->getName()."`.`".$src."`='".$ctx->restrict_values[$table][$dst]."'");
			return true;
		}
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		// get the needed fields, to make sub request with restricted values
		foreach ($this->link->fields_matching as $f1=>$f2) {
			$alias = $sql->field_id();
			$sql->field($this->path->sql_table, $f1,$alias);
		}
		$this->table_for_sub_requests = $this->path->sql_table;
		$search = @$ctx->list->search[$top_path->get_path()];
		if ($search !== null) {
			$join_alias = $sql->table_id();
			$sql->join($this->path->sql_table, $this->link->table_from->getName(), $this->link->fields_matching, $join_alias);
			$this->sql_table = $join_alias;
			foreach ($this->link->fields_matching as $f1=>$f2)
				$sql->group_by($this->path->sql_table, $f1);
			return true;
		}
		return false;
	}
	public function get_path() { return $this->link->table_from->getName()."."; }
	public function debug() { 
		$s = $this->path->debug();
		$s .= " => LINK BACKWARD ".($this->link->unique ? "UNIQUE" : "NOT UNIQUE");
		$s .= " (Table ".$this->link->table_from->getName()." to table ".$this->link->table;
		$s .= " ON ";
		foreach ($this->link->fields_matching as $f1=>$f2)
			$s .= $f1."=".$f2.", ";
		$s .= ")";
		return $s;
	}
}
?>
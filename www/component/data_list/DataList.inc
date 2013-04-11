<?php
require_once("common/SQLQuery.inc");
class DataList {
	
	public $model;
	public $starting_table;
	public $primary_key = array();
	public $sort = array();
	public $columns = array();
	public $search = array();
	public $start_entry = 0;
	public $max_entries = 100;
	
	public function __construct($starting_table) {
		$this->starting_table = $starting_table;
		global $app;
		$this->model = $app->data_model();
		$pk = $this->model->tables[$starting_table]->getPrimaryKey();
		if ($pk <> null) $this->primary_key = array($starting_table.".".$pk->name);
		$this->sql = new SQLQuery();
		$this->sql->select($starting_table);
	}
	
	public function primary_key() {
		$this->primary_key = array();
		for ($i = 0; $i < func_num_args(); $i++)
			array_push($this->primary_key, func_get_arg($i));
	}
	
	public function sort() {
		$this->sort = array();
		for ($i = 0; $i < func_num_args(); $i++)
			array_push($this->sort, func_get_arg($i));
	}
	
	public function search($field, $value) {
		$this->search[$field] = $value;
	}
	
	public function add($field, $editable) {
		//echo "Build field ".$field."<br/>";
		$i = strpos($field, ".");
		$table = substr($field, 0, $i);
		$field = substr($field, $i+1);
		$col = $this->build_column(new DataPathRoot($this->starting_table), $table, $field);
		if ($col <> null)
			array_push($this->columns, $col);
	}
	private function build_column($current_path, $table, $field) {
		$i = strpos($field, ">");
		$indirection = null;
		if ($i !== FALSE) {
			$indirection = substr($field, $i+1);
			$field = substr($field, 0, $i);
		}
		$visited_tables = array();
		$col = $this->build_simple_column($current_path, $table, $field, $visited_tables);
		if ($col == null) {
			echo "ERROR: unable to build simple column:<br/>";
			echo " current table: ".$current_path->table."<br/>";
			echo " to table: ".$table."<br/>";
			echo " field: ".$field."<br/>";
			return null;
		}
		while ($indirection !== null) {
			$col_model = $this->model->tables[$col->table]->getColumn($col->field);
			$i = strpos($indirection, ">");
			if ($i === FALSE) { 
				$field = $indirection; 
				$indirection = null; 
			} else {
				$field = substr($indirection, 0, $i);
				$indirection = substr($indirection, $i+1);
			}
			if ($col_model instanceof data\ForeignKey) {
				$col = new DataPathField(new DataPathForeign($col, $col_model), $field);
				continue;
			}
			// TODO
			break;
		};
		return $col;
	}
	private function build_simple_column($current_path, $table, $field, &$visited_tables) {
		if (in_array($current_path->table, $visited_tables)) return null;
		array_push($visited_tables, $current_path->table);
		
		// check if this is a field on the current table
		if ($table == $current_path->table)
			return new DataPathField($current_path, $field);
		// search for a foreign key from the current table, to the given table
		foreach ($this->model->tables[$current_path->table]->columns as $col)
			if ($col instanceof \data\ForeignKey && $col->foreign_table == $table)
				return new DataPathField(new DataPathForeign($current_path, $col), $field);
		// search for a link from the current table, to the given table
		foreach ($this->model->tables[$current_path->table]->links as $link) {
			if ($link->table == $table) {
				// link found
				return new DataPathField(new DataPathLink($current_path, $link), $field);
			}
		}
		// search for a foreign key in the given table, to the current table
		foreach ($this->model->tables[$table]->columns as $col) {
			if (!($col instanceof \data\ForeignKey)) continue;
			if ($col->foreign_table == $current_path->table)
				return new DataPathField(new DataPathJoin($current_path, $col), $field);
		}
		// search for a link from the given table, to the current table (reverse link)
		foreach ($this->model->tables[$table]->links as $link) {
			if ($link->table == $current_path->table) {
				// link found
				return new DataPathField(new DataPathLinkReverse($current_path, $table, $link), $field);
			}
		}
		// try with different foreign keys we have in the current table
		foreach ($this->model->tables[$current_path->table]->columns as $col)
			if ($col instanceof \data\ForeignKey) {
				$temp_path = new DataPathForeign($current_path, $col);
				$res = $this->build_simple_column($temp_path, $table, $field, $visited_tables);
				if ($res <> null) return $res;
			}
		// try with different links we have to our current table
		foreach ($this->model->tables as $t) {
			foreach ($t->links as $link) {
				if ($link->table == $current_path->table) {
					// link found
					$temp_path = new DataPathField(new DataPathLinkReverse($current_path, $table, $link), $field);
					$res = $this->build_simple_column($temp_path, $table, $field, $visited_tables);
					if ($res <> null) return $res;
				}
			}
		}
		// try with tables containing a foreign key to our current table
		foreach ($this->model->tables as $t) {
			foreach ($t->columns as $col) {
				if (!($col instanceof \data\ForeignKey)) continue;
				if ($col->foreign_table == $current_path->table) {
					// foreign key found
					$temp_path = new DataPathField(new DataPathJoin($current_path, $col), $field);
					$res = $this->build_simple_column($temp_path, $table, $field, $visited_tables);
					if ($res <> null) return $res;
				}
			}
		}
		// didn't find any path
		return null;
	}
	
	private function find_all_possible_paths() {
		$paths = array();
		$visited_tables = array();
		$this->find_possible_paths(new DataPathRoot($this->starting_table), $paths, $visited_tables);
		return $paths;
	}
	private function find_possible_paths($current_path, &$paths, &$visited_tables) {
		// avoid infinite recursion
		if (in_array($current_path->table, $visited_tables)) return;
		array_push($visited_tables, $current_path->table);
		// search foreign keys in current table
		foreach ($this->model->tables[$current_path->table]->columns as $field) {
			if ($field instanceof \data\ForeignKey) {
				$p = new DataPathForeign($current_path, $field);
				$this->find_possible_paths($p, $paths, $visited_tables);
			} else {
				$name = $this->model->getDataName($current_path->table, $field->name);
				if ($name == null) continue; // not a displayable data
				array_push($paths, new DataPathField($current_path, $field->name));
			}
		}
		// search links in current table
		foreach ($this->model->tables[$current_path->table]->links as $link) {
			$p = new DataPathLink($current_path, $link);
			$this->find_possible_paths($p, $paths, $visited_tables);
		}
		// search foreign keys from other tables to current table
		foreach ($this->model->tables as $table) {
			foreach ($table->columns as $field) {
				if ($field instanceof \data\ForeignKey && $field->foreign_table == $current_path->table) {
					$p = new DataPathJoin($current_path, $field);
					$this->find_possible_paths($p, $paths, $visited_tables);
				}
			}
		}
		// search links from other tables to current table
		foreach ($this->model->tables as $table) {
			foreach ($table->links as $link) {
				if ($link->table <> $current_path->table) continue;
				$p = new DataPathLinkReverse($current_path, $table->name, $link);
				$this->find_possible_paths($p, $paths, $visited_tables);
			}
		}
	}
	
	public function update_from_request() {
		for ($i = 0; isset($_POST["pk".$i]); $i++)
			$this->primary_key($_POST["pk".$i]);
		for ($i = 0; isset($_POST["field_".$i]); $i++)
			$this->add($_POST["field_".$i], false);
		foreach ($_POST as $name=>$value) {
			if (substr($name, 0, 7) == "search_" && $value <> "")
				$this->search($this->decode_field(substr($name,7)), $value);
		}
		return true;
	}
	private function decode_field($s) {
		return str_replace("__dot__",".",str_replace("__ind__",">",$s));
	}
	private function encode_field($s) {
		return str_replace(".","__dot__",str_replace(">","__ind__",$s));
	}
	
	public function build($page, $header = null) {
		include "layout.inc";
	}
	
	public function process() {
		$context = new DataContext($this);
		$sql = new SQLQuery();
		foreach ($this->columns as $col) {
			$col->generate_sql($sql, $context, $col);
		}
		$sql->limit($this->start_entry, $this->max_entries);
		$list = $sql->execute();
		if ($list) {
			foreach ($this->columns as $col) {
				if ($col->is_unique()) {
					$col->final_name = $col->sql_field;
					continue;
				}
				foreach ($list as &$r) {
					$sub_ctx = new DataContext($this);
					$sub_ctx->is_sub_request = true;
					foreach ($sql->fields as $f)
						$sub_ctx->restrict_value($f[0], $f[1], $r[$f[2]]);
					$sub_sql = new SQLQuery();
					$col->generate_sql($sub_sql, $sub_ctx, $col);
					$result = $sub_sql->execute();
					if ($result !== FALSE) {
						$first = true;
						$s = "";
						foreach ($result as $sr) {
							if ($first) $first = false; else $s .= ", ";
							$s .= $sr[$col->sql_field];
						}
						if ($col->final_name == null)
							$col->final_name = $sql->field_id();
						$r[$col->final_name] = $s;
					}
				}
			}
		}
		
		return $list;
	}
	
}

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
class DataPathRoot extends DataPath {
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		$sql->select($this->table);
		$this->sql_table = $this->table;
		return true;
	}
	public function is_unique() { return true; }
	public function debug() { return "Table ".$this->table." AS ".$this->sql_table; }
	public function get_path() { return $this->table."."; }
}
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
			$col = $ctx->list->model->tables[$this->table]->getColumn($this->field);
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
}
class DataPathForeign extends DataPath {
	public $path;
	public $foreign_key;
	public function __construct($path, $foreign_key) {
		parent::__construct($foreign_key->foreign_table);
		$this->path = $path;
		$this->foreign_key = $foreign_key;
	}
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->tables[$this->foreign_key->foreign_table]->getPrimaryKey()->name));
		if ($this->sql_table == null) {
			$this->sql_table = $sql->table_id();
			$sql->join($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->tables[$this->foreign_key->foreign_table]->getPrimaryKey()->name), $this->sql_table);
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
}
class DataPathLinkReverse extends DataPath {
	public $path;
	public $link;
	public function __construct($path, $table, $link) {
		parent::__construct($table);
		$this->path = $path;
		$this->link = $link;
	}
	public function is_unique() { return $this->path->is_unique() && $this->link->unique; }
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if ($this->link->unique) {
			if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
			$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->link->table_from->name, $this->link->fields_matching);
			if ($this->sql_table == null) {
				$this->sql_table = $sql->table_id();
				$sql->join($this->path->sql_table, $this->link->table_from->name, $this->link->fields_matching, $this->sql_table);
			}
			return true;
		}
		if ($ctx->is_sub_request) {
			$sql->select($this->link->table_from->name);
			$this->sql_table = $this->link->table_from->name;
			foreach ($this->link->fields_matching as $src=>$dst)
				$sql->where("`".$this->link->table_from->name."`.`".$src."`='".$ctx->restrict_values[$this->path->table][$dst]."'");
			return true;
		}
		$search = @$ctx->list->search[$top_path->get_path()];
		if ($search !== null) {
			$join_alias = $sql->table_id();
			$sql->join($this->link->table, $this->link->table_from->name, $this->link->fields_matching, $join_alias);
			$this->sql_table = $join_alias;
			foreach ($this->link->fields_matching as $f1=>$f2)
				$sql->group_by($this->link->table, $f1);
			return true;
		}
		return false;
	}
	public function get_path() { return $this->link->table_from->name."."; }
	public function debug() { 
		$s = $this->path->debug();
		$s .= " => LINK BACKWARD ".($this->link->unique ? "UNIQUE" : "NOT UNIQUE");
		$s .= " (Table ".$this->link->table_from->name." to table ".$this->link->table;
		$s .= " ON ";
		foreach ($this->link->fields_matching as $f1=>$f2)
			$s .= $f1."=".$f2.", ";
		$s .= ")";
		return $s;
	}
}
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
		$s .= " (Table ".$this->link->table." to table ".$this->link->table_from->name;
		$s .= " ON ";
		foreach ($this->link->fields_matching as $f1=>$f2)
			$s .= $f1."=".$f2.", ";
		$s .= ")";
		return $s;
	}
}
class DataPathJoin extends DataPath {
	public $path;
	public $foreign_key;
	public function __construct($path, $foreign_key) {
		parent::__construct($foreign_key->table->name);
		$this->path = $path;
		$this->foreign_key = $foreign_key;
	}
	public function generate_sql(&$sql, &$ctx, &$top_path) {
		if (!$this->path->generate_sql($sql, $ctx, $top_path)) return false;
		$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->foreign_key->table->name, array($ctx->list->model->tables[$this->path->table]->getPrimaryKey()->name=>$this->foreign_key->name));
		if ($this->sql_table == null) {
			$this->sql_table = $sql->table_id();
			$sql->join($this->path->sql_table, $this->foreign_key->table->name, array($ctx->list->model->tables[$this->path->table]->getPrimaryKey()->name=>$this->foreign_key->name), $this->sql_table);
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
	public function debug() { return $this->path->debug()." => Join foreign key from ".$this->foreign_key->table->name." on ".$this->foreign_key->name; }
}
?>
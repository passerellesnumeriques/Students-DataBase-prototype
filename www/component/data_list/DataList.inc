<?php
require_once("common/SQLQuery.inc");
class DataList {
	
	/** keep the data model got from components */
	public $model;
	/** the table containing the primary keys, and being the starting point for the list */
	public $starting_table;
	/** the primary keys from the starting table. Those fields are mandatory */
	public $primary_key = array();
	/** fields to be sorted. each element in this array is an array containing two values: the field to sort, and a boolean to true for ascending, or false for descending order */
	public $sort = array();
	/** the columns to be displayed: each element of this array is a DataListPath */
	public $columns = array();
	/** map of searches: field => value to search */
	public $search = array();
	/** index of the first entry to get */
	public $start_entry = 0;
	/** maximum number of entries to get */
	public $max_entries = 100;
	/** html parts to be included in the list header */
	public $headers = array();
	/** possible actions on an item */
	public $item_actions = array();
	
	public function __construct($starting_table) {
		$this->starting_table = $starting_table;
		global $app;
		$this->model = $app->data_model();
		$pk = $this->model->getTable($starting_table)->getPrimaryKey();
		if ($pk <> null) $this->primary_key = array($starting_table.".".$pk->name);
		$this->sql = new SQLQuery();
		$this->sql->select($starting_table);
	}
	
	public function primary_key() {
		$this->primary_key = array();
		for ($i = 0; $i < func_num_args(); $i++)
			array_push($this->primary_key, func_get_arg($i));
	}
	
	public function search($field, $value) {
		$this->search[$field] = $value;
	}
	
	public function add($field, $editable) {
		//echo "Build field ".$field."<br/>";
		$i = strpos($field, ".");
		$table = substr($field, 0, $i);
		$field = substr($field, $i+1);
		try {
			$t = $this->model->getTable($table);
		} catch (Exception $e) {
			PNApplication::error($e);
			return false;
		}
		if (!$t->canAccessField($field))
			return false; // access denied
		$col = null;
		try { $col = $this->build_column(new DataPathRoot($this->starting_table), $table, $field); }
		catch (Exception $e) {
			PNApplication::error($e);
			return false;
		}
		if ($col <> null) {
			array_push($this->columns, $col);
			return true;
		}
		return false;
	}
	private function build_column($current_path, $table, $field) {
		$i = strpos($field, ">");
		$indirection = null;
		if ($i !== FALSE) {
			$indirection = substr($field, $i+1);
			$field = substr($field, 0, $i);
		}
		$visited_tables = array();
		try {
			$col = $this->build_simple_column($current_path, $table, $field, $visited_tables);
		} catch (Exception $e) {
			return null;
		}
		if ($col == null)
			return null;
		while ($indirection !== null) {
			try {
				$col_model = $this->model->getTable($col->table)->getColumn($col->field);
			} catch (Exception $e) { return null; }
			$i = strpos($indirection, ">");
			if ($i === FALSE) { 
				$field = $indirection; 
				$indirection = null; 
			} else {
				$field = substr($indirection, 0, $i);
				$indirection = substr($indirection, $i+1);
			}
			if ($col_model instanceof data\ForeignKey) {
				try { $this->model->getTable($col_model->foreign_table); }
				catch (Exception $e) { return null; }
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
		foreach ($this->model->getTable($current_path->table)->getColumns() as $col)
			if ($col instanceof \data\ForeignKey && $col->foreign_table == $table)
				return new DataPathField(new DataPathForeign($current_path, $col), $field);
		// search for a link from the current table, to the given table
		foreach ($this->model->getTable($current_path->table)->getLinks() as $link) {
			if ($link->table == $table) {
				// link found
				return new DataPathField(new DataPathLink($current_path, $link), $field);
			}
		}
		// search for a foreign key in the given table, to the current table
		foreach ($this->model->getTable($table)->getColumns() as $col) {
			if (!($col instanceof \data\ForeignKey)) continue;
			if ($col->foreign_table == $current_path->table)
				return new DataPathField(new DataPathJoin($current_path, $col), $field);
		}
		// search for a link from the given table, to the current table (reverse link)
		foreach ($this->model->getTable($table)->getLinks() as $link) {
			if ($link->table == $current_path->table) {
				// link found
				return new DataPathField(new DataPathLinkReverse($current_path, $table, $link), $field);
			}
		}
		// try with different foreign keys we have in the current table
		foreach ($this->model->getTable($current_path->table)->getColumns() as $col)
			if ($col instanceof \data\ForeignKey) {
				$temp_path = new DataPathForeign($current_path, $col);
				$visited = array_merge($visited_tables);
				try {
					$res = $this->build_simple_column($temp_path, $table, $field, $visited);
					if ($res <> null) return $res;
				} catch (Exception $e) {}
			}
		// try with different links we have in our current table
		foreach ($this->model->getTable($current_path->table)->getLinks() as $link) {
			$temp_path = new DataPathLink($current_path, $link);
			$visited = array_merge($visited_tables);
			try {
				$res = $this->build_simple_column($temp_path, $table, $field, $visited);
				if ($res <> null) return $res;
			} catch (Exception $e) {}
		}
		// try with different links we have to our current table
		foreach ($this->model->getTables() as $table_name=>$t) {
			foreach ($t->getLinks() as $link) {
				if ($link->table == $current_path->table) {
					// link found
					$temp_path = new DataPathLinkReverse($current_path, $t->getName(), $link);
					$visited = array_merge($visited_tables);
					try {
						$res = $this->build_simple_column($temp_path, $table, $field, $visited);
						if ($res <> null) return $res;
					} catch (Exception $e) {}
				}
			}
		}
		// try with tables containing a foreign key to our current table
		foreach ($this->model->getTables() as $table_name=>$t) {
			foreach ($t->getColumns() as $col) {
				if (!($col instanceof \data\ForeignKey)) continue;
				if ($col->foreign_table == $current_path->table) {
					// foreign key found
					$temp_path = new DataPathJoin($current_path, $col);
					$visited = array_merge($visited_tables);
					try {
						$res = $this->build_simple_column($temp_path, $table, $field, $visited);
						if ($res <> null) return $res;
					} catch (Exception $e) {}
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
		// check we can access
		try { $this->model->getTable($current_path->table); }
		catch (Exception $e) { return; }
		// search foreign keys in current table
		foreach ($this->model->getTable($current_path->table)->getColumns() as $field) {
			if ($field instanceof \data\ForeignKey) {
				$p = new DataPathForeign($current_path, $field);
				$this->find_possible_paths($p, $paths, $visited_tables);
			} else {
				$name = $this->model->getTable($current_path->table)->getDisplayableDataName($field->name);
				if ($name == null) continue; // not a displayable data
				array_push($paths, new DataPathField($current_path, $field->name));
			}
		}
		// search links in current table
		foreach ($this->model->getTable($current_path->table)->getLinks() as $link) {
			$p = new DataPathLink($current_path, $link);
			$this->find_possible_paths($p, $paths, $visited_tables);
		}
		// search foreign keys from other tables to current table
		foreach ($this->model->getTables() as $table_name=>$table) {
			foreach ($table->getColumns() as $field) {
				if ($field instanceof \data\ForeignKey && $field->foreign_table == $current_path->table) {
					$p = new DataPathJoin($current_path, $field);
					$this->find_possible_paths($p, $paths, $visited_tables);
				}
			}
		}
		// search links from other tables to current table
		foreach ($this->model->getTables() as $table_name=>$table) {
			foreach ($table->getLinks() as $link) {
				if ($link->table <> $current_path->table) continue;
				$p = new DataPathLinkReverse($current_path, $table->getName(), $link);
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
	
	public function add_header($header) {
		array_push($this->headers, $header);
	}
	
	/** url can contains fields corresponding to the item, with format %field% */
	public function add_item_action($icon_url, $alt_text, $url) {
		array_push($this->item_actions, array($icon_url, $alt_text, $url));
	}
	
	public function build($page) {
		include "layout.inc";
	}
	
	public function process() {
		if (count($this->columns) == 0)
			return array("count"=>0,"list"=>array());
		
		$context = new DataContext($this);
		$sql = new SQLQuery();
		foreach ($this->columns as $col) {
			$col->generate_sql($sql, $context, $col);
		}
		$sql_count = new SQLQuery($sql);
		$sql_count->reset_fields();
		$sql_count->count();
		$count = $sql_count->execute_single_value();
		
		$sql->limit($this->start_entry, $this->max_entries);
		//echo $sql->generate();
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
					//echo $sub_sql->generate()."<br/>";
					$result = $sub_sql->execute();
					//var_dump($result);
					//echo "<br/><br/><br/>";
					if ($result !== FALSE) {
						$first = true;
						$s = array();
						foreach ($result as $sr)
							array_push($s, $sr[$col->sql_field]);
						if ($col->final_name == null)
							$col->final_name = $sql->field_id();
						$r[$col->final_name] = $s;
					}
				}
			}
		}
		
		return array("count"=>$count,"list"=>$list);
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
?>
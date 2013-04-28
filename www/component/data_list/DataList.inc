<?php
require_once("common/SQLQuery.inc");
require_once("DataPath.inc");
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
		for ($i = 0; $i < func_num_args(); $i++)
			if (!in_array(func_get_arg($i), $this->primary_key))
				array_push($this->primary_key, func_get_arg($i));
	}
	
	public function search($field, $value) {
		$this->search[$field] = $value;
	}
	
	public function add($field, $editable) {
		$col = $this->get_column($field);
		if ($col <> null) {
			array_push($this->columns, $col);
			return true;
		}
		return false;
	}
	public function get($field) {
		foreach ($this->columns as $col)
			if ($col->get_path() == $field)
				return $col;
		return null;
	}
	public function get_index($field) {
		for ($i = 0; $i < count($this->columns); $i++)
			if ($this->columns[$i]->get_path() == $field)
				return $i;
		return -1;
	}
	private function get_column($field) {
		$i = strpos($field, ".");
		$table = substr($field, 0, $i);
		$field = substr($field, $i+1);
		try {
			$t = $this->model->getTable($table);
		} catch (Exception $e) {
			PNApplication::error($e);
			return null;
		}
		if (!$t->canAccessField($field))
			return null; // access denied
		$col = null;
		try { $col = $this->build_column(new DataPathRoot($this->starting_table), $table, $field); }
		catch (Exception $e) {
			PNApplication::error($e);
			return null;
		}
		return $col;
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
		
		// generate sql request
		$context = new DataContext($this);
		$sql = new SQLQuery();
		foreach ($this->columns as $col)
			$col->generate_sql($sql, $context, $col);
		// use sql request to count total number of entries
		$sql_count = new SQLQuery($sql);
		$sql_count->reset_fields();
		$sql_count->count();
		$count = $sql_count->execute_single_value();
		
		// limit results
		$sql->limit($this->start_entry, $this->max_entries);
		
		// add missing primary keys, and get their aliases
		$keys = array();
		for ($i = 0; $i < count($this->primary_key); $i++) {
			$pk = $this->primary_key[$i];
			$found = false;
			foreach ($this->columns as $c)
				if ($c->get_path() == $pk) { $found = true; array_push($keys, $c->sql_field); break; }
			if (!$found) {
				$col = $this->get_column($pk);
				$col->generate_sql($sql, $context, $col);
				array_push($keys, $col->sql_field);
			}
		}
		
		// execute request
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
					//echo "Request for field '".$col->get_path()."': ".$sub_sql->generate()."<br/>";
					//echo "Field path is ".$col->debug()."<br/>";
					$result = $sub_sql->execute();
					//var_dump($result);
					//echo "<br/><br/><br/>";
					if (is_array($result)) {
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
		
		return array("count"=>$count,"list"=>$list,"primary_keys"=>$keys);
	}
	
	public function update_data($keys, $field_path, $value) {
		$col = null;
		foreach ($this->columns as $c)
			if ($c->get_path() == $field_path) { $col = $c; break; }
		if ($col == null) {
			PNApplication::error("Invalid field to update '".$field_path."'");
			return false;
		}
		$ctx = new DataContext($this);
		foreach ($keys as $key_path=>$key_value) {
			foreach ($this->columns as $c)
				if ($c->get_path() == $key_path)
					$ctx->restrict_value($c->table, $c->field, $key_value);
		}
		$col->update_value($ctx, $value, $this->model);
	}
	
}
?>
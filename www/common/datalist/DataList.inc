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
		$i = strpos($field, ".");
		$table = substr($field, 0, $i);
		$field = substr($field, $i+1);
		array_push($this->columns, $this->build_column(new DataPathRoot($this->starting_table), $table, $field));
	}
	private function build_column($current_path, $table, $field) {
		$i = strpos($field, ">");
		$indirection = null;
		if ($i !== FALSE) {
			$indirection = substr($field, $i+1);
			$field = substr($field, 0, $i);
		}
		$col = $this->build_simple_column($current_path, $table, $field);
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
	private function build_simple_column($current_path, $table, $field) {
		// check if this is a field on the current table
		if ($table == $current_path->table)
			return new DataPathField($current_path, $field);
		// search for a foreign key from the current table, to the given table
		foreach ($this->model->tables[$current_path->table]->columns as $col)
			if ($col instanceof \data\ForeignKey && $col->foreign_table == $table)
				return new DataPathField(new DataPathForeign($current_path, $col), $field);
		// search for a link from the given table, to the current table
		foreach ($this->model->tables[$table]->links as $link) {
			if ($link->table == $current_path->table) {
				// link found
				return new DataPathField(new DataPathLink($current_path, $table, $link), $field);
			}
		}
		// try with different foreign keys we have in the current table
		foreach ($this->model->tables[$current_path->table]->columns as $col)
			if ($col instanceof \data\ForeignKey) {
				$temp_path = new DataPathForeign($current_path, $col);
				$res = $this->build_simple_column($temp_path, $table, $field);
				if ($res <> null) return $res;
			}
		// try with different links we have to our current table
		foreach ($this->model->tables as $t) {
			foreach ($t->links as $link) {
				if ($link->table == $current_path->table) {
					// link found
					$temp_path = new DataPathField(new DataPathLink($current_path, $table, $link), $field);
					$res = $this->build_simple_column($temp_path, $table, $field);
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
		// search foreign keys from other tables to current table
		foreach ($this->model->tables as $table) {
			foreach ($table->columns as $field) {
				if ($field instanceof \data\ForeignKey && $field->foreign_table == $current_path->table) {
					// TODO
					//$p = new DataPathJoin($field);
					//$this->find_possible_paths($p, $paths, $visited_tables);
				}
			}
		}
		// search links from other tables to current table
		foreach ($this->model->tables as $table) {
			foreach ($table->links as $link) {
				if ($link->table <> $current_path->table) continue;
				$p = new DataPathLink($current_path, $table->name, $link);
				$this->find_possible_paths($p, $paths, $visited_tables);
			}
		}
	}
	
	public function update_from_request() {
		if (!isset($_POST["from_refresh"])) return false;
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
	
	public function build($header = null) {
		// get all possible fields
		$all_fields = $this->find_all_possible_paths();
		
		// form of fields
		echo "<form name='datalist_names' onsubmit='return false'>";
		$i = 0;
		foreach ($all_fields as $path) {
			$f = $this->model->getDataCategoryAndName($path->table, $path->field);
			echo "<input type='hidden' name='field_".$i."' value='".$path->get_path()."'/>";
			echo "<input type='hidden' name='field_".$i."_category' value='".$f[0]."'/>";
			echo "<input type='hidden' name='field_".$i."_name' value='".$f[1]."'/>";
			$type = 'unknown';
			$col = $this->model->tables[$path->table]->getColumn($path->field);
			if ($col instanceof \data\ColumnString) $type = 'string';
			else if ($col instanceof \data\ColumnInteger) $type = 'integer';
			echo "<input type='hidden' name='field_".$i."_type' value='".$type."'/>";
			$i++;
		}
		echo "</form>";
		echo "<form name='datalist' method='post'>";
		echo "<input type='hidden' name='from_refresh' value='yes'/>";
		$i = 0;
		foreach ($this->columns as $col) {
			echo "<input type='hidden' name='field_".$i."' value='".$col->get_path()."'/>";
			$i++;
		}
		foreach ($this->search as $field=>$value) {
			echo "<input type='hidden' name='search_".$this->encode_field($field)."' value='".$value."'/>";
		}
		echo "</form>";
		
		// count total number of entries
		$count = DataBase::$conn->execute("SELECT COUNT(*) FROM `".$this->starting_table."`");
		// TODO reduce in case of filters
		if ($count === false)
			$count = 0;
		else {
			$count = DataBase::$conn->next_row($count);
			$count = $count[0];
		}
		// process request
		$context = new DataContext($this);
		$sql = new SQLQuery();
		foreach ($this->columns as $col) {
			if ($col->is_unique())
				$col->generate_sql($sql, $context);
		}
		$sql->limit($this->start_entry, $this->max_entries);
		$list = $sql->execute();
		
		// page
		Locale::load_file("datalist","common/datalist/locale/");
		echo "<table style='width:100%;height:100%' cellspacing=0 cellpadding=0 id='datalist_container'>";
		// Header
		echo "<tr id='datalist_header'><td>";
		echo "<button id='datalist_refresh' onclick='datalist_refresh();return false;' disabled='disabled'><img src='/static/application/refresh.png' style='vertical-align:bottom'/> ".Locale::get_string("datalist", "Refresh")."</button>";
		echo "</td><td>";
		echo "</td><td>";
		echo "<span style='border-right:1px solid #C0C0C0;padding-right:3px'>";
		echo $count." ".get_locale("datalist","entries");
		if ($count > 0)
			echo ", ".get_locale("datalist","Display")." ".($this->start_entry+1)."-".($this->start_entry+count($list));
		echo "</span>";
		if ($header <> null)
			echo $header;
		echo "</td></tr>";
		// List of available fields, by category
		echo "<tr><td valign=top width=25% id='datalist_fields_area'>";
		echo "<div style='width:100%;height:100%;overflow:auto' id='datalist_fields_area_div'>";
		// Visible fields
		echo "<div class='datalist_fields_section'>";
		echo "<div class='datalist_fields_section_title'>";
		echo "<img src='/static/application/collapse.gif' style='border:0px;cursor:pointer' onclick='datalist_section_collapse_expand(this);'/> ";
		echo Locale::get_string("datalist", "Visible Fields");
		echo "</div>";
		echo "<div class='datalist_fields_section_content' style='visibility:visible'>";
		echo "<table width=100% cellspacing=0 cellpadding=0><tbody id='datalist_visible_fields'>";
		echo "</tbody></table>";
		echo "</div>";
		echo "</div>";
		// Available fields
		echo "<div class='datalist_fields_section'>";
		echo "<div class='datalist_fields_section_title'>";
		echo "<img src='/static/application/collapse.gif' style='border:0px;cursor:pointer' onclick='datalist_section_collapse_expand(this);'/> ";
		echo Locale::get_string("datalist", "Available Fields");
		echo "</div>";
		echo "<div class='datalist_fields_section_content' style='visibility:visible'>";
		echo "<table width='100%' cellspacing=0 cellpadding=0><tbody id='datalist_avail_fields'>";
		echo "</tbody></table>";
		echo "</div>";
		echo "</div>";
		echo "</div></td>";

		// Separator
		echo "<td class='datalist_separator' onmousedown='datalist_start_resize(event);return false;'></td>";
		
		// The big list
		echo "<td valign=top align=left style='padding-left:2px;padding-right:2px' id='datalist_data_area'>";
		//echo $sql->generate();
		echo "<div style='width:100%;height:100%;overflow:auto;position:relative'>";
		echo "<table rules=all style='border:1px solid black;position:absolute'>";
		echo "<tr>";
		foreach ($this->columns as $col) {
			echo "<th nowrap='nowrap'>";
			echo $this->model->getDataName($col->table, $col->field);
			echo "</th>";
		}
		echo "</tr>";
		if ($list)
		foreach ($list as $r) {
			echo "<tr>";
			foreach ($this->columns as $col) {
				echo "<td nowrap='nowrap'>";
				if ($col->is_unique())
					echo $r[$col->sql_field];
				else {
					$sub_ctx = new DataContext($this);
					foreach ($sql->fields as $f)
						$sub_ctx->restrict_value($f[0], $f[1], $r[$f[2]]);
					//foreach ($this->columns as $c) {
					//	if (!$col->is_unique()) continue;
					//	$sub_ctx->restrict_value($c->table, $c->field, $r[$c->sql_field]);
					//}
					$sub_sql = new SQLQuery();
					$col->generate_sql($sub_sql, $sub_ctx);
					//echo $sub_sql->generate();
					//echo "<br/>";
					$first = true;
					foreach ($sub_sql->execute() as $sr) {
						if ($first) $first = false; else echo ", ";
						echo $sr[$col->sql_field];
					}
				}
				echo "</td>";
			}
			echo "</tr>";
		}
		echo "</table>";
		echo "</div>";
		echo "</td></tr>";
		echo "</table>";
		echo "<script type='text/javascript'>datalist_init_fields();</script>";
		PNApplication::print_errors();
		//echo "<br/><br/>";
		//foreach ($this->columns as $col)
		//	echo $col->debug()."<br/>";
	}
	
}

class DataContext {
	public $list;
	public $restrict_values = array();
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
	public $table;
	public $sql_table;
	public function __construct($table) {
		$this->table = $table;
	}
	public abstract function is_unique();
	public abstract function generate_sql(&$sql, &$ctx); 
	public abstract function get_path();
}
class DataPathRoot extends DataPath {
	public function generate_sql(&$sql, &$ctx) {
		$sql->select($this->table);
		$this->sql_table = $this->table;
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
	public function generate_sql(&$sql, &$ctx) {
		$this->path->generate_sql($sql, $ctx);
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
	public function generate_sql(&$sql, &$ctx) {
		$this->path->generate_sql($sql, $ctx);
		$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->tables[$this->foreign_key->foreign_table]->getPrimaryKey()->name));
		if ($this->sql_table == null) {
			$this->sql_table = $sql->table_id();
			$sql->join($this->path->sql_table, $this->foreign_key->foreign_table, array($this->foreign_key->name => $ctx->list->model->tables[$this->foreign_key->foreign_table]->getPrimaryKey()->name), $this->sql_table);
		}
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
class DataPathLink extends DataPath {
	public $path;
	public $link;
	public function __construct($path, $table, $link) {
		parent::__construct($table);
		$this->path = $path;
		$this->link = $link;
	}
	public function is_unique() { return $this->path->is_unique() && $this->link->unique; }
	public function generate_sql(&$sql, &$ctx) {
		if ($this->link->unique) {
			$this->path->generate_sql($sql, $ctx);
			$this->sql_table = $sql->get_join_alias($this->path->sql_table, $this->link->table_from->name, $this->link->fields_matching);
			if ($this->sql_table == null) {
				$this->sql_table = $sql->table_id();
				$sql->join($this->path->sql_table, $this->link->table_from->name, $this->link->fields_matching, $this->sql_table);
			}
		} else {
			$sql->select($this->link->table_from->name);
			$this->sql_table = $this->link->table_from->name;
			foreach ($this->link->fields_matching as $src=>$dst)
				$sql->where("`".$this->link->table_from->name."`.`".$src."`='".$ctx->restrict_values[$this->path->table][$dst]."'");
		}
	}
	public function get_path() { return $this->link->table_from->name."."; }
	public function debug() { 
		$s = $this->path->debug();
		$s .= " => LINK ".($this->link->unique ? "UNIQUE" : "NOT UNIQUE");
		$s .= " (Table ".$this->link->table_from->name." to table ".$this->link->table;
		$s .= " ON ";
		foreach ($this->link->fields_matching as $f1=>$f2)
			$s .= $f1."=".$f2.", ";
		$s .= ")";
		return $s;
	}
}
?>
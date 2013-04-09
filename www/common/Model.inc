<?php 
namespace data {

/** Defines the data model of the application. Each component is supposed to populate this model with its own part of the model */
class Model {
	
	public $tables = array();
	public $data_per_category = array();
	public $data_per_table = array();
	
	/** Add a table to the model, and returns it
	 * 
	 * @param string $name name of the table to ad
	 * @return \data\Table
	 */
	public function &addTable($name) {
		$table = new Table($this, $name);
		$this->tables[$name] = &$table;
		return $table;
	}
	
	/** Indicates that a particular column is a displayable data (not internal).
	 * 
	 * @param string $category name of the category in which this data will be displayed. This string is supposed to be used with Locale in order to obtain a localized name of the category.
	 * @param string $name name of the data to display. This string is supposed to be used with Locale, in order to display a localized name.
	 * @param string $table table containing the data.
	 * @param string $field column corresponding to the data
	 */
	public function addData($category, $name, $table, $field) {
		if (strpos($category, "::") === FALSE) $category = \Locale::$current_component."::".$category;
		if (strpos($name, "::") === FALSE) $name = \Locale::$current_component."::".$name;
		if (!isset($this->data_per_category[$category]))
			$this->data_per_category[$category] = array();
		$this->data_per_category[$category][$name] = array($table, $field);
		if (!isset($this->data_per_table[$table]))
			$this->data_per_table[$table] = array();
		$this->data_per_table[$table][$field] = array($category, $name);
	}
	
	/** Returns the localized name of the given data */
	public function getDataName($table, $field) {
		$data = @$this->data_per_table[$table][$field];
		if ($data == null) return null;
		$i = strpos($data[1], "::");
		return \Locale::get_string(substr($data[1], 0, $i), substr($data[1], $i+2));
	}
	/** Returns the localized category and name of the given data */
	public function getDataCategoryAndName($table, $field) {
		$data = @$this->data_per_table[$table][$field];
		if ($data == null) return "?field ".$field." in table ".$table."?";
		$cat = $data[0];
		$name = $data[1];
		$i = strpos($cat, "::");
		$cat = \Locale::get_string(substr($cat, 0, $i), substr($cat, $i+2));
		$i = strpos($name, "::");
		$name = \Locale::get_string(substr($name, 0, $i), substr($name, $i+2));
		return array($cat,$name);
	}
	
}

class Table {
	
	public $model;
	public $name;
	public $columns = array();
	public $indexes = array();
	public $links = array();
	
	public function __construct(&$model, $name) {
		$this->model = &$model;
		$this->name = $name;
	}
	
	/** Add a primary key (integer ID) */
	public function addPrimaryKey($name, $size=32) {
		$c = new PrimaryKey($this, $name, $size);
		array_push($this->columns, $c);
		array_push($this->indexes, array("primary",$name));
		return $this;
	}
	
	/** Add a foreign key to the given table */
	public function addForeignKey($name, $table) {
		$c = new ForeignKey($this, $name, $table);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add an integer column. Size is in bits */
	public function addInteger($name, $size=32) {
		$c = new ColumnInteger($this, $name, $size);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add a string column */
	public function addString($name, $max_length=255) {
		$c = new ColumnString($this, $name, $max_length);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add an enum column, with the given possible values */
	public function addEnum($name, $values) {
		$c = new ColumnEnum($this, $name, $values);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Indicate one or several columns should be indexed for faster access.
	 * As parameters, this method expects the list of columns to be indexed together.
	 * @return \data\Table
	 */
	public function addIndex() {
		$nb = func_num_args();
		if ($nb == 1)
			array_push($this->indexes, array(func_get_arg(0),func_get_arg(0)));
		else {
			$a = array("index".count($this->indexes));
			for ($i = 0; $i < $nb; $i++)
				array_push($a, func_get_arg($i));
			array_push($this->indexes, $a);
		}
		return $this;
	}
	
	/** Indicate a link to another table, not using a foreign key. */
	public function &addLink($table_name, $fields_matching, $unique) {
		array_push($this->links, new Link($this, $table_name, $fields_matching, $unique));
		return $this;
	}
	
	public function getPrimaryKey() {
		foreach ($this->columns as $c)
			if ($c instanceof PrimaryKey)
				return $c;
		return null;
	}
	public function getColumn($name) {
		foreach ($this->columns as $c)
			if ($c->name == $name)
				return $c;
		return null;
	}
	
}

class Link {
	public $table_from;
	public $table;
	public $fields_matching;
	public $unique;
	public function __construct(&$table_from, $table, $fields_matching, $unique) {
		$this->table_from = &$table_from;
		$this->table = $table;
		$this->fields_matching = $fields_matching;
		$this->unique = $unique;
	}
}

abstract class Column {
	
	public $table;
	public $name;
	
	public function __construct(&$table, $name) {
		$this->table = &$table;
		$this->name = $name;
	}
	
	public function get_sql() {
		$sql = "`".$this->name."` ";
		$sql .= $this->get_sql_type();
		return $sql;
	}
	public abstract function get_sql_type();
	
}

class ColumnInteger extends Column {

	public $size;
	
	public function __construct(&$table, $name, $size) {
		parent::__construct($table, $name);
		$this->size = $size;
	}
	
	public function get_sql_type() { return "INT(".$this->size.")"; }

}

class ColumnString extends Column {

	public $max_length;
	
	public function __construct(&$table, $name, $max_length) {
		parent::__construct($table, $name);
		$this->max_length = $max_length;
	}

	public function get_sql_type() { /* TODO choose the type depending on max_length */ return "VARCHAR(".$this->max_length.")"; }
}

class ColumnEnum extends Column {

	public $values;

	public function __construct(&$table, $name, $values) {
		parent::__construct($table, $name);
		$this->values = $values;
	}

	public function get_sql_type() {
		$sql = "ENUM(";
		$first = true;
		foreach ($this->values as $v) {
			if ($first) $first = false; else $sql.=",";
			$sql .= "'".\DataBase::$conn->escape_string($v)."'";
		}
		$sql .= ")";
		return $sql;
	}
}

class PrimaryKey extends ColumnInteger {
	
	public function __construct(&$table, $name, $size) {
		parent::__construct($table, $name, $size);
	}
	
	public function get_sql_type() { return "INT(".$this->size.") NOT NULL AUTO_INCREMENT"; }
}

class ForeignKey extends ColumnInteger {

	public $foreign_table;
	
	public function __construct(&$table, $name, $foreign_table) {
		$this->foreign_table = $foreign_table;
		$size = $table->model->tables[$foreign_table]->getPrimaryKey()->size;
		parent::__construct($table, $name, $size);
	}

	public function get_sql_type() { return "INT(".$this->table->model->tables[$this->foreign_table]->getPrimaryKey()->size.")"; }
}

} // namespace data
?>
<?php 
namespace data {

/** Defines the data model of the application. Each component is supposed to populate this model with its own part of the model */
class Model {
	
	private $tables = array();
	
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
	
	public function &getTable($name) {
		if (!isset($this->tables[$name])) throw new \Exception("Unknown table ".$name);
		if ($this->tables[$name]->canAccess()) return $this->tables[$name];
		throw new \Exception("Access denied: table ".$name);
	}
	
	/** should be used only in very specific places, while ensuring that this will not give privileges to the user */ 
	public function &internalGetTable($name) {
		return $this->tables[$name];
	}
	
	public function &getTables() {
		$a = array();
		foreach ($this->tables as $name=>&$table)
			if ($table->canAccess())
				$a[$name] = &$table;
		return $a;
	}
	
}

class Table {
	
	private $model;
	private $name;
	private $columns = array();
	private $indexes = array();
	private $links = array();
	private $table_read_access = array();
	private $fields_read_access = array();
	private $displayable_data = array();
	
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
	
	public function &restrictReadAccess($right_name, $right_value) {
		$this->table_read_access[$right_name] = $right_value;
		return $this;
	}
	public function &restrictFieldReadAccess($field, $right_name, $right_value) {
		if (!isset($this->fields_read_access[$field]))
			$this->fields_read_access[$field] = array();
		$this->fields_read_access[$field][$right_name] = $right_value;
		return $this;
	}
	
	public function canAccess() {
		foreach ($this->table_read_access as $right_name=>$right_value)
			if (!\PNApplication::$instance->user_management->has_right($right_name, $right_value))
				return false;
		return true;
	}
	public function canAccessField($field) {
		if (!isset($this->fields_read_access[$field])) return true;
		foreach ($this->fields_read_access[$field] as $right_name=>$right_value)
			if (!\PNApplication::$instance->user_management->has_right($right_name, $right_value))
			return false;
		return true;
	}
	
	/** Indicates that a particular column is a displayable data (not internal).
	 *
	 * @param string $category name of the category in which this data will be displayed. This string is supposed to be used with Locale in order to obtain a localized name of the category.
	 * @param string $name name of the data to display. This string is supposed to be used with Locale, in order to display a localized name.
	 * @param string $field column corresponding to the data
	 */
	public function &addDisplayableData($field, $category, $name) {
		if (strpos($category, "::") === FALSE) $category = \Locale::$current_component."::".$category;
		if (strpos($name, "::") === FALSE) $name = \Locale::$current_component."::".$name;
		$this->displayable_data[$field] = array($category, $name);
		return $this;
	}
	
	/** Returns the localized name of the given data */
	public function getDisplayableDataName($field) {
		$data = @$this->displayable_data[$field];
		if ($data == null) return null;
		$i = strpos($data[1], "::");
		return \Locale::get_string(substr($data[1], 0, $i), substr($data[1], $i+2));
	}
	/** Returns the localized category and name of the given data */
	public function getDisplayableDataCategoryAndName($field) {
		$data = @$this->displayable_data[$field];
		if ($data == null) return "?field ".$field." in table ".$this->name."?";
		$cat = $data[0];
		$name = $data[1];
		$i = strpos($cat, "::");
		$cat = \Locale::get_string(substr($cat, 0, $i), substr($cat, $i+2));
		$i = strpos($name, "::");
		$name = \Locale::get_string(substr($name, 0, $i), substr($name, $i+2));
		return array($cat,$name);
	}
	
	public function &getModel() { return $this->model; }
	public function getName() { return $this->name; }
	
	public function getPrimaryKey() {
		foreach ($this->columns as $c)
			if ($c instanceof PrimaryKey) {
				if (!$this->canAccessField($c->name)) throw new Exception("Access denied");
				return $c;
			}
		return null;
	}
	public function getColumn($name) {
		foreach ($this->columns as $c)
			if ($c->name == $name) {
				if (!$this->canAccessField($c->name)) throw new Exception("Access denied");
				return $c;
			}
		return null;
	}
	public function getColumns() {
		$cols = array();
		foreach ($this->columns as $c)
			if ($this->canAccessField($c->name))
				array_push($cols, $c);
		return $cols;
	}
	
	public function getLinks() {
		return $this->links;
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
		$size = $table->getModel()->internalGetTable($foreign_table)->getPrimaryKey()->size;
		parent::__construct($table, $name, $size);
	}

	public function get_sql_type() { return "INT(".$this->table->getModel()->internalGetTable($this->foreign_table)->getPrimaryKey()->size.")"; }
}

} // namespace data
?>
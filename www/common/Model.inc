<?php 
namespace data {

/** Defines the data model of the application. Each component is supposed to populate this model with its own part of the model */
class Model {
	
	private $tables = array();
	private $data_category_link = array();
	
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
	
	/** indicates which page to go in order to display information about the given category */ 
	public function addDataCategoryLink($category, $link) {
		if (strpos($category, "::") === FALSE) $category = \Locale::$current_component."::".$category;
		$this->data_category_link[$category] = $link;
	}
	public function getDataCategoryLink($category) {
		if (strpos($category, "::") === FALSE) $category = \Locale::$current_component."::".$category;
		return @$this->data_category_link[$category];
	} 
	
}

class Table {
	
	private $model;
	private $name;
	private $columns = array();
	private $indexes = array();
	private $links = array();
	private $table_read_access = array();
	private $table_add_access = array();
	private $table_remove_access = array();
	private $fields_read_access = array();
	private $fields_write_access = array();
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
	public function addInteger($name, $size=32, $min = null, $max = null) {
		$c = new ColumnInteger($this, $name, $size, $min, $max);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add a boolean column */
	public function addBoolean($name) {
		$c = new ColumnBoolean($this, $name);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add a string column */
	public function addString($name, $max_length=255, $min_length = 0, $can_be_null = true) {
		$c = new ColumnString($this, $name, $max_length, $min_length, $can_be_null);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add an enum column, with the given possible values */
	public function addEnum($name, $values) {
		$c = new ColumnEnum($this, $name, $values);
		array_push($this->columns, $c);
		return $this;
	}
	
	/** Add a date column */
	public function addDate($name, $can_be_null = true) {
		$c = new ColumnDate($this, $name, $can_be_null);
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
	
	public function &allowReadAccess() {
		$this->table_read_access = null;
		return $this;
	}
	public function &restrictReadAccess($right_name, $right_value) {
		$this->table_read_access[$right_name] = $right_value;
		return $this;
	}
	public function &restrictAddAccess($right_name, $right_value) {
		$this->table_add_access[$right_name] = $right_value;
		return $this;
	}
	public function &restrictRemoveAccess($right_name, $right_value) {
		$this->table_remove_access[$right_name] = $right_value;
		return $this;
	}
	public function &restrictFieldReadAccess($field, $right_name, $right_value) {
		if (!isset($this->fields_read_access[$field]))
			$this->fields_read_access[$field] = array();
		$this->fields_read_access[$field][$right_name] = $right_value;
		return $this;
	}
	public function &restrictFieldWriteAccess($field, $right_name, $right_value) {
		if (!isset($this->fields_write_access[$field]))
			$this->fields_write_access[$field] = array();
		$this->fields_write_access[$field][$right_name] = $right_value;
		return $this;
	}
	public function &restrictWriteAccess($right_name, $right_value) {
		foreach ($this->columns as $col)
			$this->restrictFieldWriteAccess($col->name, $right_name, $right_value);
		return $this;
	}
	public function &preventWriteAccess($field) {
		$this->fields_write_access[$field] = false;
	}
	
	public function canAccess() {
		if ($this->table_read_access === null) return true;
		if (count($this->table_read_access) == 0) return false;
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
	public function canModifyField($field) {
		if (!isset($this->fields_write_access[$field])) return false;
		$val = $this->fields_write_access[$field];
		if ($val === true) return true;
		if ($val === false) return false;
		foreach ($val as $right_name=>$right_value)
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
	
	/** returns the list of displayable data in this table: field=>array(category,name). category and name are localized string prefixed by namespace:: */
	public function getDisplayableData() {
		return $this->displayable_data;
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
				if (!$this->canAccessField($c->name)) throw new \Exception("Access denied");
				return $c;
			}
		return null;
	}
	public function getColumn($name) {
		foreach ($this->columns as $c)
			if ($c->name == $name) {
				if (!$this->canAccessField($c->name)) throw new \Exception("Access denied");
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
	
	public function validate($value) { return $value; }
	
}

class ColumnInteger extends Column {

	public $size;
	public $min, $max;
	
	public function __construct(&$table, $name, $size, $min = null, $max = null) {
		parent::__construct($table, $name);
		$this->size = $size;
		$this->min = $min;
		$this->max = $max;
	}
	
	public function get_sql_type() { return "INT(".$this->size.")"; }
	
	public function validate($value) {
		if (!is_integer($value)) $value = intval($value);
		if ($this->min !== null && $value < $this->min) throw new \Exception("Minimum is ".$this->min); // TODO locale
		if ($this->max !== null && $value > $this->max) throw new \Exception("Maximum is ".$this->max); // TODO locale
		return $value;
	}

}

class ColumnBoolean extends ColumnInteger {

	public function __construct(&$table, $name) {
		parent::__construct($table, $name, 1);
	}
	
	public function validate($value) {
		return $value == true;
	}

}

class ColumnString extends Column {

	public $max_length;
	public $min_length;
	public $can_be_null;
	
	public function __construct(&$table, $name, $max_length, $min_length = 0, $can_be_null = true) {
		parent::__construct($table, $name);
		$this->max_length = $max_length;
		$this->min_length = $min_length;
		$this->can_be_null = $can_be_null;
	}

	public function get_sql_type() { /* TODO choose the type depending on max_length */ return "VARCHAR(".$this->max_length.")"; }
	
	public function validate($value) {
		if ($value === null) {
			if (!$this->can_be_null) throw new \Exception("Cannot be empty"); // TODO locale
			return null;
		}
		if (strlen($value) < $this->min_length) {
			if (strlen($value) == 0) throw new \Exception("Cannot be empty"); // TODO locale
			throw new \Exception("Must have a length of at least ".$this->min_length); // TODO locale
		}
		if (strlen($value) > $this->max_length)
			throw new \Exception("Cannot be greater than ".$this->max_length); // TODO locale
		return $value;
	}
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

class ColumnDate extends Column {

	public $can_be_null;
	
	public function __construct(&$table, $name, $can_be_null = true) {
		parent::__construct($table, $name);
		$this->can_be_null = $can_be_null;
	}
	
	
	public function get_sql_type() { return "DATE"; }
	
	public function validate($value) {
		if ($value === "") $value = null;
		if ($value === null) {
			if (!$this->can_be_null) throw new \Exception("You must enter a valid date"); // TODO locale
			return null;
		}
		// TODO check if valid format
		return $value;
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
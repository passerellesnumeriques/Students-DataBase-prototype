<?php
namespace datamodel {
class Table {

	private $model;
	private $name;
	private $columns = array();
	private $indexes = array();
	private $links = array();
	private $table_read_access = array();
	public $table_specific_read_access = array();
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
	public function addInteger($name, $size=32, $min = null, $max = null, $can_be_null = true, $unique = false) {
		$c = new ColumnInteger($this, $name, $size, $min, $max, $can_be_null, $unique);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add a boolean column */
	public function addBoolean($name, $can_be_null = true) {
		$c = new ColumnBoolean($this, $name, $can_be_null);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add a string column */
	public function addString($name, $max_length=255, $min_length = 0, $can_be_null = true, $unique = false) {
		$c = new ColumnString($this, $name, $max_length, $min_length, $can_be_null, $unique);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add an enum column, with the given possible values */
	public function addEnum($name, $values, $can_be_null = true, $unique = false) {
		$c = new ColumnEnum($this, $name, $values);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add a date column */
	public function addDate($name, $can_be_null = true, $unique = false) {
		$c = new ColumnDate($this, $name, $can_be_null, $unique);
		array_push($this->columns, $c);
		return $this;
	}

	/** Add a time column */
	public function addTime($name, $can_be_null = true, $unique = false) {
		$c = new ColumnTime($this, $name, $can_be_null, $unique);
		array_push($this->columns, $c);
		return $this;
	}

	/** Indicate one or several columns should be indexed for faster access.
	 * As parameters, this method expects the list of columns to be indexed together.
	 * @return \datamodel\Table
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
	public function &allowSpecificReadAccess($field, $value_func) {
		array_push($this->table_specific_read_access, array($field, $value_func));
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
	public function &noRestriction() {
		$this->table_read_access = null;
		$this->fields_read_access = array();
		foreach ($this->columns as $col)
			$this->fields_write_access[$col->name] = true;
		$this->table_add_access = null;
		$this->table_remove_access = null;
		return $this;
	}

	public function canAccess() {
		if ($this->table_read_access === null) return true;
		if (count($this->table_specific_read_access) > 0) return true;
		if (count($this->table_read_access) == 0) return false;
		foreach ($this->table_read_access as $right_name=>$right_value)
			if (!\PNApplication::$instance->user_management->has_right($right_name, $right_value))
				return false;
		return true;
	}
	public function canReadAll() {
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
	public function canAdd() {
		if ($this->table_add_access === null) return true;
		if (count($this->table_add_access) == 0)
			return false;
		foreach ($this->table_add_access as $right_name=>$right_value)
			if (!\PNApplication::$instance->user_management->has_right($right_name, $right_value))
				return false;
		return true;
	}
	public function canRemove() {
		if ($this->table_remove_access === null) return true;
		if (count($this->table_remove_access) == 0)
			return false;
		foreach ($this->table_remove_access as $right_name=>$right_value)
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

	public function hide($field) {
		unset($this->displayable_data[$field]);
	}

	/** returns the list of displayable data in this table: field=>array(category,name). category and name are localized string prefixed by namespace:: */
	public function getDisplayableData() {
		return $this->displayable_data;
	}
	/** same as getDisplayableData but the localized strings are resolved */
	public function getDisplayableDataResolved() {
		$a = array();
		foreach ($this->displayable_data as $field=>$locale) {
			$i = strpos($locale[0], "::");
			$cat = \Locale::get_string(substr($locale[0], 0, $i), substr($locale[0], $i+2));
			$i = strpos($locale[1], "::");
			$name = \Locale::get_string(substr($locale[1], 0, $i), substr($locale[1], $i+2));
			$a[$field] = array($cat,$name);
		}
		return $a;
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
	/**
	 * @param string $name name of the column to get
	 * @throws \Exception in case the column is not accessible based on the rights of the user
	 * @return Column|NULL the column or NULL if the column does not exist
	 */
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

	/** @return \SQLQuery */
	public function &select() {
		require_once("common/SQLQuery.inc");
		return \SQLQuery::create()->select($this->name);
	}
	/** @return \SQLQuery */
	public function &select_by_key($key) {
		$q = $this->select()->where($this->getPrimaryKey()->name, $key);
		return $q;
	}

	/** Insert a new data in the table */
	public function insert($fields) {
		// check we are allowed
		if (!$this->canAdd()) throw new \Exception("Access denied: add data to table ".$this->name);
		// lock the table
		require_once("common/DataBaseLock.inc");
		require_once("common/SQLQuery.inc");
		$locker = null;
		$lock = \DataBaseLock::lock($this->name, null, $locker, false);
		if ($lock == null)
			throw new \Exception("Access denied: the table ".$this->name." is locked by ".$locker);
		$id = null;
		$error = null;
		try {
			// check unicity
			foreach ($this->columns as $col) {
				if ($col->unique) {
					if (!isset($fields[$col->name]))
						throw new \Exception("Missing column ".$col->name." to insert data in table ".$this->name);
					$r = \SQLQuery::create()->select($this->name)->where($col->name,$fields[$col->name])->execute_single_row();
					if ($r <> null)
						throw new \Exception(get_locale("common","__ already exists",array("name"=>isset($this->displayable_data[$col->name]) ? $this->getDisplayableDataName($col->name) : $col->name)));
				}
			}
			// insert
			$id = \SQLQuery::insert($this->name, $fields);
		} catch (\Exception $e) {
			$error = $e;
		}
		// unlock the table
		\DataBaseLock::unlock($lock);
		if ($error <> null) throw $error;
		return $id;
	}

	/** Remove the data having the given primary key, and all linked data */
	public function remove_key($key) {
		// search for all tables that may contain a data to remove
		$tables = array(&$this);
		$this->search_tables_for_remove($tables);
		// check we can access all the tables
		foreach ($tables as $t)
			if (!$t->canRemove())
				throw new \Exception("Access denied: remove data from table ".$t->name);
		// lock all the tables
		require_once("common/DataBaseLock.inc");
		require_once("common/SQLQuery.inc");
		$locker = null;
		$locks = array();
		foreach ($tables as $t) {
			$lock = \DataBaseLock::lock($t->name, null, $locker, false);
			if ($lock == null) {
				foreach ($locks as $l)
					\DataBaseLock::unlock($l);
				throw new \Exception("Access denied: the table ".$t->name." is locked by ".$locker);
			}
		}
		// remove data
		$this->perform_remove($key);
		// unlock tables
		foreach ($locks as $l)
			\DataBaseLock::unlock($l);
	}
	/** Remove data with given primary keys */
	public function remove_keys($keys) {
		// search for all tables that may contain a data to remove
		$tables = array(&$this);
		$this->search_tables_for_remove($tables);
		// check we can access all the tables
		foreach ($tables as $t)
			if (!$t->canRemove())
				throw new \Exception("Access denied: remove data from table ".$t->name);
		// lock all the tables
		require_once("common/DataBaseLock.inc");
		require_once("common/SQLQuery.inc");
		$locker = null;
		$locks = array();
		foreach ($tables as $t) {
			$lock = \DataBaseLock::lock($t->name, null, $locker, false);
			if ($lock == null) {
				foreach ($locks as $l)
					\DataBaseLock::unlock($l);
				throw new \Exception("Access denied: the table ".$t->name." is locked by ".$locker);
			}
		}
		// remove data
		foreach ($keys as $key)
			$this->perform_remove($key);
		// unlock tables
		foreach ($locks as $l)
			\DataBaseLock::unlock($l);
	}
	/** Remove entries corresponding to the given rows */
	public function remove_rows($rows) {
		$keys = array();
		$pk = $this->getPrimaryKey()->name;
		foreach ($rows as $r) array_push($keys, $r[$pk]);
		$this->remove_keys($keys);
	}

	private function search_tables_for_remove(&$tables) {
		foreach ($this->model->internalGetTables() as $t) {
			if (in_array($t, $tables)) continue;
			foreach ($t->columns as $col)
				if ($col instanceof ForeignKey && $col->foreign_table == $this->name) {
					array_push($tables, $t);
					$t->search_tables_for_remove($tables);
					break;
				}
		}
	}

	private function perform_remove($key) {
		// remove data
		\DataBase::$conn->execute("DELETE FROM `".$this->name."` WHERE `".$this->getPrimaryKey()->name."`=".$key);
		// remove linked data
		require_once("common/SQLQuery.inc");
		foreach ($this->model->internalGetTables() as $t) {
			foreach ($t->columns as $col)
				if ($col instanceof ForeignKey && $col->foreign_table == $this->name) {
					if ($t->getPrimaryKey() <> null) {
						$linked = \SQLQuery::create()->select($t->name)->field($t->getPrimaryKey()->name)->where($col->name, $key)->execute_single_field();
						if ($linked)
						foreach ($linked as $linked_key)
							$t->perform_remove($linked_key);
					} else {
						// no primary key, just remove the entries
						\DataBase::$conn->execute("DELETE FROM `".$t->name."` WHERE `".$col->name."`=".$key);
					}
					break;
				}
		}
	}

	public function update_by_key($key, $fields_values, $lock_id = null) {
		// check we are allowed
		foreach ($fields_values as $field=>$value)
			if (!$this->canModifyField($field)) throw new \Exception("Access Denied: modify ".$field." on table ".$this->name);
		// lock table
		require_once("common/DataBaseLock.inc");
		if ($lock_id <> null) {
			if (!\DataBaseLock::check($lock_id, $this->name, array($this->getPrimaryKey()->name=>$key)))
				throw new \Exception(get_locale("common", "Access Denied"));
		} else {
			$locker = null;
			$lock_id = \DataBaseLock::lock($this->name, array($this->getPrimaryKey()->name=>$key), $locker, false);
			if ($lock_id == null)
				throw new \Exception(get_locate("common", "This data is already locked by")." ".$locker);
		}
		// validate values, and check unicity
		require_once("common/SQLQuery.inc");
		foreach ($fields_values as $field=>$value) {
			$col = $this->getColumn($field);
			try { $fields_values[$field] = $col->validate($value); }
			catch (Exception $e) {
				\DataBaseLock::unlock($lock_id);
				throw new \Exception((isset($this->displayable_data[$field]) ? $this->getDisplayableDataName($field) : $field).": ".$e->getMessage());
			}
			if ($col->unique) {
				$r = \SQLQuery::create()->select($this->name)->where($col->name, $fields_values[$field])->execute_single_row();
				if ($r <> null) {
					\DataBaseLock::unlock($lock_id);
					throw new \Exception(get_locale("common","__ already exists",array("name"=>isset($this->displayable_data[$col->name]) ? $this->getDisplayableDataName($col->name) : $col->name)));
				}
			}
		}
		// everything is valid and allowed, we can perform the update
		$sql = "UPDATE `".$this->name."` SET ";
		$first = true;
		foreach ($fields_values as $field=>$value) {
			if ($first) $first = false; else $sql .= ",";
			$sql .= "`".$field."`=";
			if ($value === null)
				$sql .= "NULL";
			else
				$sql .= "'".\DataBase::$conn->escape_string($value)."'";
		}
		$sql .= " WHERE `".$this->getPrimaryKey()->name."`='".$key."'";
		\DataBase::execute($sql);
		$error = \DataBaseLock::unlock($lock_id);
		if ($error <> null) throw new \Exception($error);
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
class LinkMultiDataBase {
	public $table_from;
	public $table;
	public $table_field_database;
	public $fields_matching;
	public $unique;
	public function __construct(&$table_from, $table, $table_field_database, $fields_matching, $unique) {
		$this->table_from = &$table_from;
		$this->table = $table;
		$this->table_field_database = $table_field_database;
		$this->fields_matching = $fields_matching;
		$this->unique = $unique;
	}
}

abstract class Column {

	public $table;
	public $name;
	public $can_be_null;
	public $unique;

	public function __construct(&$table, $name, $can_be_null, $unique) {
		$this->table = &$table;
		$this->name = $name;
		$this->can_be_null = $can_be_null;
		$this->unique = $unique;
	}

	public function get_sql() {
		$sql = "`".$this->name."` ";
		$sql .= $this->get_sql_type();
		return $sql;
	}
	public abstract function get_sql_type();

	public function validate($value) { return $value; }

	public function create_javascript_field(&$page, $editable = false, $onchanged = null, $onunchanged = null) {
		$page->add_javascript("/static/js/field/field_text.js");
		return "new field_text()";
	}

}

class ColumnInteger extends Column {

	public $size;
	public $min, $max;

	public function __construct(&$table, $name, $size, $min = null, $max = null, $can_be_null = true, $unique = false) {
		parent::__construct($table, $name, $can_be_null, $unique);
		$this->size = $size;
		$this->min = $min;
		$this->max = $max;
	}

	public function get_sql_type() { return "INT(".$this->size.")"; }

	public function validate($value) {
		if ($this->can_be_null && $value === null) return null;
		if (!is_integer($value)) $value = intval($value);
		if ($this->min !== null && $value < $this->min) throw new \Exception("Minimum is ".$this->min); // TODO locale
		if ($this->max !== null && $value > $this->max) throw new \Exception("Maximum is ".$this->max); // TODO locale
		return $value;
	}

}

class ColumnBoolean extends ColumnInteger {

	public function __construct(&$table, $name, $can_be_null = false) {
		parent::__construct($table, $name, 1, $can_be_null, false);
	}

	public function validate($value) {
		if ($this->can_be_null && $value === null) return null;
		return $value == true;
	}

}

class ColumnString extends Column {

	public $max_length;
	public $min_length;

	public function __construct(&$table, $name, $max_length, $min_length = 0, $can_be_null = true, $unique = false) {
		parent::__construct($table, $name, $can_be_null, $unique);
		$this->max_length = $max_length;
		$this->min_length = $min_length;
	}

	public function get_sql_type() { /* TODO choose the type depending on max_length */ return "VARCHAR(".$this->max_length.")"; }

	public function validate($value) {
		if ($value === null) {
			if (!$this->can_be_null) throw new \Exception(get_locale("common","Cannot be empty"));
			return null;
		}
		if (strlen($value) < $this->min_length) {
			if (strlen($value) == 0) throw new \Exception(get_locale("common","Cannot be empty"));
			throw new \Exception("Must have a length of at least ".$this->min_length); // TODO locale
		}
		if (strlen($value) > $this->max_length)
			throw new \Exception("Cannot be greater than ".$this->max_length); // TODO locale
		return $value;
	}

	public function create_javascript_field(&$page, $editable = false, $onchanged = null, $onunchanged = null) {
		if (!$editable) return parent::create_javascript_field($page, false);
		$page->add_javascript("/static/common/js/field/field_editable_text.js");
		return "new field_editable_text(".$this->max_length.",".($onchanged <> null ? $onchanged : 'null').",".($onunchanged <> null ? $onunchanged : 'null').")";
	}
}

class ColumnEnum extends Column {

	public $values;

	public function __construct(&$table, $name, $values, $can_be_null = true, $unique = false) {
		parent::__construct($table, $name, $can_be_null, $unique);
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

	public function __construct(&$table, $name, $can_be_null = true, $unique = false) {
		parent::__construct($table, $name, $can_be_null, $unique);
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

class ColumnTime extends Column {

	public function __construct(&$table, $name, $can_be_null = true, $unique = false) {
		parent::__construct($table, $name, $can_be_null, $unique);
	}


	public function get_sql_type() { return "TIME"; }

	public function validate($value) {
		if ($value === "") $value = null;
		if ($value === null) {
			if (!$this->can_be_null) throw new \Exception("You must enter a valid time"); // TODO locale
			return null;
		}
		// TODO check if valid format
		return $value;
	}
}

class PrimaryKey extends ColumnInteger {

	public function __construct(&$table, $name, $size) {
		parent::__construct($table, $name, $size, false, true);
	}

	public function get_sql_type() { return "INT(".$this->size.") NOT NULL AUTO_INCREMENT"; }
}

class ForeignKey extends ColumnInteger {

	public $foreign_table;

	public function __construct(&$table, $name, $foreign_table) {
		$this->foreign_table = $foreign_table;
		$size = $table->getModel()->internalGetTable($foreign_table)->getPrimaryKey()->size;
		parent::__construct($table, $name, $size, true, false);
	}

	public function get_sql_type() { return "INT(".$this->table->getModel()->internalGetTable($this->foreign_table)->getPrimaryKey()->size.")"; }
}
}
?>
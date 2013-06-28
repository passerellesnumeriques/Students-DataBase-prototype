<?php
require_once("TableDefinition.inc");
/** Defines the data model of the application. Each component is supposed to populate this model with its own part of the model */
class DataModel {

	/** @var $model DataModel */
	private static $model = null;
	/** @return DataModel */
	public static function &get() {
		if (DataModel::$model == null) {
			$model = new DataModel();
			$prev_locale = Locale::$current_component;
			$done = array("data_model");
			foreach (PNApplication::$instance->components as $c) {
				DataModel::_get_component($c, $done, $model);
			}
			Locale::$current_component = $prev_locale;
			DataModel::$model = &$model;
		}
		return DataModel::$model;
	}
	private static function _get_component($c, $done, &$model) {
		if (in_array($c->name, $done)) return;
		array_push($done, $c->name);
		foreach ($c->dependencies() as $dep)
			DataModel::_get_component(PNApplication::$instance->components[$dep], $done, $model);
		Locale::$current_component = $c->name;
		$file = "component/".$c->name."/datamodel.inc";
		if (file_exists($file))
			include $file;
		$file = "component/".$c->name."/datamodel_hidden.inc";
		if (file_exists($file)) {
			$hidden = include $file;
			$hidden = $hidden[PNApplication::$instance->current_domain];
			foreach ($hidden as $field) {
				$i = strpos($field, ".");
				$table = substr($field, 0, $i);
				$field = substr($field, $i+1);
				$model->hide($table,$field);
			}
		}
	}

	private $tables = array();
	private $data_category_link = array();

	/** Add a table to the model, and returns it
	 *
	 * @param string $name name of the table to ad
	 * @return \datamodel\Table
	 */
	public function &addTable($name) {
		$table = new \datamodel\Table($this, $name);
		$this->tables[$name] = &$table;
		return $table;
	}

	/** Returns the table if access is allowed.
	 *
	 * @param string $name name of the table to ad
	 * @return \datamodel\Table
	 * @throws \Exception if the table does not exist or access is not allowed
	 */
	public function &getTable($name) {
		if (!isset($this->tables[$name])) throw new \Exception("Unknown table ".$name);
		if ($this->tables[$name]->canAccess()) return $this->tables[$name];
		throw new \Exception("Access denied: table ".$name);
	}

	/** should be used only in very specific places, while ensuring that this will not give privileges to the user
	 * @return \datamodel\Table
	 */
	public function &internalGetTable($name) {
		return $this->tables[$name];
	}
	/** should be used only in very specific places, while ensuring that this will not give privileges to the user */
	public function &internalGetTables() {
		return $this->tables;
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

	private function hide($table, $field) {
		$this->tables[$table]->hide($field);
	}

}
?>
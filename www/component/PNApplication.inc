<?php
require_once("Component.inc");

function list_components() {
	$list = array();
	$dir = @opendir("component");
	if ($dir == null) return $list;
	while (($filename = readdir($dir)) <> null) {
		if (substr($filename, 0, 1) == ".") continue;
		if (is_dir("component/".$filename)) array_push($list, $filename);
	}
	closedir($dir);
	return $list;	
}
$components_names = list_components();

$app_model = null;

class PNApplication {

	public static $errors = array();
	public $components = array();
	
	public static function error($message) {
		array_push(PNApplication::$errors, htmlentities($message));
	}
	public static function error_html($html_message) {
		array_push(PNApplication::$errors, $html_message);
	}
	
	public static function print_errors() {
		foreach (PNApplication::$errors as $e)
			echo "<div style='color:#C00000'><img src='/static/application/error.png' style='vertical-align:bottom'/> ".$e."</div>";
	}
	
	public function init() {
		global $components_names;
		foreach ($components_names as $name) $this->create_component($name);
		$done = array();
		foreach ($this->components as $c) $this->init_component($c, $done);
		require_once("common/DataBaseModel.inc");
		DataBaseModel::update_model($this->data_model());
		if (count($errors) > 0) {
			PNApplication::print_errors();
			die();
		}
	}
	private function create_component($name) {
		$c = new $name($this, $name);
		$this->components[$name] = &$c;
		$this->{$name} = &$c;
	}
	private function init_component($c, &$done) {
		if (in_array($c->name, $done)) return;
		array_push($done, $c->name);
		foreach ($c->dependencies() as $dep) {
			if (!isset($this->components[$dep])) die("Missing component '".$dep."' which is a dependency of component '".$c->name."'");
			$this->init_component($this->components[$dep], $done);				
		}
		$c->init();
	}
	
	public function &data_model() {
		global $app_model;
		if ($app_model == null) {
			require_once("common/Model.inc");
			$app_model = new data\Model();
			$done = array();
			foreach ($this->components as $name=>&$c)
				$this->populate_model($app_model, $c, $done);
		}
		return $app_model;
	}
	private function populate_model(&$model, &$c, &$done) {
		if (in_array($c->name, $done)) return;
		array_push($done, $c->name);
		foreach ($c->dependencies() as $dep) {
			if (!isset($this->components[$dep])) die("Missing component '".$dep."' which is a dependency of component '".$c->name."'");
			$this->populate_model($model, $this->components[$dep], $done);
		}
		$prev = Locale::$current_component;
		Locale::$current_component = $c->name;
		$c->populate_model($model);
		Locale::$current_component = $prev;
	}
	
}

set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__));
foreach ($components_names as $name) {
	set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__)."/".$name);
	require_once($name.".inc");
}
?>

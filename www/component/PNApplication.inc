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

class PNApplication {

	public $components = array();
	
	public function init() {
		global $components_names;
		foreach ($components_names as $name) $this->init_component($name);
	}
	private function init_component($name) {
		$c = new $name($name);
		$this->components[$name] = &$c;
		$this->{$name} = &$c;
	}

}

set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__));
foreach ($components_names as $name) {
	set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__)."/".$name);
	require_once($name.".inc");
}
?>

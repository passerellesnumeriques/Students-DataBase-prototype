<?php
require_once("Component.inc");
$components_names = array(
	"hello_world"
);

class Application {

	public function __construct() {
		global $components_names;
		foreach ($components_names as $name) {
			$c = new $name($name);
			$this->components[$name] = &$c;
			$this->{$name} = &$c;
		}
	}
	
	public $components = array();

}

foreach ($components_names as $name)
	require_once("component/".$name."/".$name.".inc");
?>

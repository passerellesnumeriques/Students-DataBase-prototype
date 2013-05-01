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

/**
 * PNApplication is the root point, used to:
 *   - access to any component
 *   - access to the global data model
 *   - indicate errors along the process, that may be then display to the user
 */
class PNApplication {

	/** @var PNApplication */
	public static $instance;

	public static $errors = array();
	public $current_domain;
	public $components = array();

	public static function error($message) {
		array_push(PNApplication::$errors, htmlentities($message, ENT_QUOTES, "UTF-8"));
	}
	public static function error_html($html_message) {
		array_push(PNApplication::$errors, $html_message);
	}

	public static function has_errors() { return count(PNApplication::$errors) > 0; }

	public static function print_errors() {
		foreach (PNApplication::$errors as $e)
			echo "<div style='color:#C00000;font-familiy:Tahoma;font-size:10pt'><img src='/static/common/images/error.png' style='vertical-align:bottom'/> ".$e."</div>";
	}
	public static function print_xml_errors() {
		if (count(PNApplication::$errors) == 0) return false;
		echo "<error message=\"";
		foreach (PNApplication::$errors as $e) echo htmlspecialchars($e,ENT_COMPAT,"UTF-8");
		echo "\"/>";
		return true;
	}
	public static function print_json_result($result) {
		echo "{";
		if (count(PNApplication::$errors) > 0) {
			echo "errors:[";
			$first = true;
			foreach (PNApplication::$errors as $e) {
				if ($first) $first = false; else echo ",";
				echo json_encode($e);
			}
			echo"],";
		}
		echo "result:";
		if ($result === null) echo "null"; else echo $result;
		echo "}";
	}

	public function init() {
		global $components_names;
		foreach ($components_names as $name) $this->create_component($name);
		$done = array();
		foreach ($this->components as $c) $this->init_component($c, $done);
	}
	private function create_component($name) {
		require_once("component/".$name."/".$name.".inc");
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

}
?>

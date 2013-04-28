<?php
abstract class Component {
	
	public final function __construct(&$app, $name) {
		$this->app = &$app;
		$this->name = $name;
	}
	function init() {}
	
	public $app;
	public $name;
	
	protected function is_page_allowed($path) { return false; }
	protected function is_service_allowed($path) { return false; }
	
	public function page($path) {
		Locale::$current_component = $this->name;
		if ($this->app->user_management->username == null) {
			echo "<script type='text/javascript'>";
			echo "window.top.location = \"/\";";
			echo "</script>";
			die();
		}
		if (!$this->is_page_allowed($path)) {
			PNApplication::error(get_locale("user_management","Access denied"));
			PNApplication::print_errors();
			die();
		}
		require_once("Page.inc");
		$page = new Page();
		$page->generate($this, $path);
	}
	
	public function service($path) {
		if ($this->app->user_management->username == null || !$this->is_service_allowed($path)) {
			$format = @$_GET["format"];
			if ($format == "xml")
				die("<error message='Access denied'/>");
			else if ($format == "json")
				die("{errors:".json_encode("Access denied")."}");
			die("<error message='Access denied'/>");
		}
		Locale::$current_component = $this->name;
		// TODO check rights
		$file = "component/".$this->name."/service/".$path.".php";
		if (file_exists($file))
			include $file;
		else {
			$format = @$_GET["format"];
			if ($format == "xml")
				die("<error message='service does not exist'/>");
			else if ($format == "json")
				die("{errors:".json_encode("service does not exist")."}");
			die("<error message='service does not exist'/>");
		}
	}
	
	public function populate_model(&$model) {}
	public function dependencies() { return array(); }
	public function get_readable_rights() { return array(); } 
	public function get_writable_rights() { return array(); }
	
}
?>

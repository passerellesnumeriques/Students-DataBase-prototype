<?php
abstract class Component {
	
	public function __construct(&$app, $name) {
		$this->app = &$app;
		$this->name = $name;
	}
	
	public $app;
	public $name;
	protected function is_page_allowed($path) { return false; }
	protected function is_service_allowed($path) { return false; }
	
	public function page($path, $embedded = false) {
		Locale::$current_component = $this->name;
		if ($this->app->user_management->username == null || !$this->is_page_allowed($path)) {
			header("Location: /?page=/dynamic/".$this->name."/page/".$path);
			die("Access denied.");
		}
		global $page_embedded;
		$page_embedded = $embedded;
		include "common/page_header.inc";
		$file = "component/".$this->name."/page/".$path.".php";
		if (file_exists($file))
			include $file;
		else
			PNApplication::error_html("Page <i>".$path."</i> does not exist in component <i>".$this->name."</i>");
		PNApplication::print_errors();
		include "common/page_footer.inc";
	}
	
	public function service($path) {
		if ($this->app->user_management->username == null || !$this->is_service_allowed($path)) {
			die("<error message='Access denied'/>");
		}
		// TODO check rights
		$file = "component/".$this->name."/service/".$path.".php";
		if (file_exists($file))
			include $file;
		else
			die("<error message='service does not exist'/>");
	}
	
	public function populate_model(&$model) {}
	public function dependencies() { return array(); } 
	
}
?>

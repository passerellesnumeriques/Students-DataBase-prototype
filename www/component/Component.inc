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
	
	public function page($path, $embedded = false) {
		Locale::$current_component = $this->name;
		if ($this->app->user_management->username == null || !$this->is_page_allowed($path)) {
			if (!$embedded) {
				header("Location: /?page=/dynamic/".$this->name."/page/".$path);
				die("Access denied.");
			} else {
				echo "<script type='text/javascript'>";
				echo "window.top.location = \"/?page=\"+window.top.location.pathname;";
				echo "</script>";
				die();
			}
		}
		$file = "component/".$this->name."/page/".$path.".php";
		require_once("Page.inc");
		$page = new Page();
		$page->generate($file, $embedded);
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

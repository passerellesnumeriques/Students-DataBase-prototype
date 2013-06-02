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
		$file = "component/".$this->name."/page/".$path.".php";
		if (!file_exists($file)) {
			PNApplication::error_html("Page <i>".$path."</i> does not exist in component <i>".$this->name."</i>");
			PNApplication::print_errors();
			return;
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
		header("Content-Type: text/json;charset=UTF-8");
		$file = "component/".$this->name."/service/".$path.".php";
		if (!file_exists($file)) {
			PNApplication::error("Service does not exist");
			PNApplication::print_json_result(null);
			return;
		}
		if ($this->app->user_management->username == null || !$this->is_service_allowed($path)) {
			PNApplication::error("Access denied");
			PNApplication::print_json_result(null);
			return;
		}
		Locale::$current_component = $this->name;
		ob_start();
		include $file;
		$result = ob_get_clean();
		if ($result === "") $result = null;
		PNApplication::print_json_result($result);
	}
	
	public function dependencies() { return array(); }
	public function get_readable_rights() { return array(); } 
	public function get_writable_rights() { return array(); }
	
}
?>

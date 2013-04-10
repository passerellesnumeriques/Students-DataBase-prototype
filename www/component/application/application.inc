<?php 
class application extends Component {

	/** Override page to allow login page without being logged, and a different layout for login page */
	public function page($path) {
		if ($path == "login" || $path == "enter") {
			include "page/".$path.".php";
			return;
		}
		parent::page($path);
	}
	
	protected function is_page_allowed($path) {
		// for now, all pages of the application are allowed to everybody
		return true;
	}
	protected function is_service_allowed($path) {
		// for now, all pages of the application are allowed to everybody
		return true;
	}
}
?>
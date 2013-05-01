<?php 
class curriculum extends Component {
	
	public function get_readable_rights() {
		return array(
			new AccessRightCategory("curriculum","Curriculum",array(
				new BooleanRight("consult_curricula","Consult curricula"),
			)),
		);
	}
	public function get_writable_rights() {
		return array(
			new AccessRightCategory("curriculum","Curriculum",array(
				new BooleanRight("edit_curricula","Edit curricula",array(true=>array("consult_curricula"=>true))),
			)),
		);
	}
	
	protected function is_page_allowed($path) {
		if ($path == "curricula")
			return $this->app->user_management->has_right("consult_curricula");
		return false;
	}
	protected function is_service_allowed($path) {
		return false;
	}
	
}
?>
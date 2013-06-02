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
		if ($path == "curricula" ||
			$path == "curriculum")
			return $this->app->user_management->has_right("consult_curricula");
		if ($path == "edit_class_types")
			return $this->app->user_management->has_right("edit_curricula");
		return false;
	}
	protected function is_service_allowed($path) {
		if ($path == "create_curriculum" ||
			$path == "create_branch" ||
			$path == "rename_branch" ||
			$path == "create_period" ||
			$path == "create_subject_category" ||
			$path == "edit_subject_category" ||
			$path == "create_subject" ||
			$path == "edit_subject" ||
			$path == "add_period_subject")
			return $this->app->user_management->has_right("edit_curricula");
		return false;
	}

}
?>
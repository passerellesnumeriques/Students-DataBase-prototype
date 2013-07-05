<?php
class people extends Component {

	public function get_readable_rights() {
		return array(
			new AccessRightCategory("people","Personal Information",array(
				new BooleanRight("see_other_people_details","See other people personal information"),
			)),
		);
	}
	public function get_writable_rights() {
		return array(
			new AccessRightCategory("people","Personal Information",array(
				new BooleanRight("edit_people_details","Edit Personal Information",array(true=>array("see_other_people_details"=>true))),
			)),
		);
	}

	protected function is_page_allowed($path) {
		if ($path == "list" || $path == "home")
			return $this->app->user_management->has_right("see_other_people_details");
		if ($path == "profile")
			return true;
		if ($path == "profile_people") {
			if ($_GET["people"] == PNApplication::$instance->user_people->user_people_id) return true;
			return $this->app->user_management->has_right("see_other_people_details");
		}
		return false;
	}

	public function get_profile_pages(&$people) {
		return array(
			"profile_people"=>array("/static/people/profile_16.png", get_locale("people","Personal Information"), "/dynamic/people/page/profile_people?people=".$people, 1),
		);
	}

}
?>
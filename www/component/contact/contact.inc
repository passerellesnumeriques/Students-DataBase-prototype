<?php
class contact extends Component {

	public function dependencies() { return array("people"); }

	public function get_readable_rights() {
		return array(
				new AccessRightCategory("contact","Contacts",array(
						new BooleanRight("consult_people_contacts","See contact information of people"),
				)),
		);
	}
	public function get_writable_rights() {
		return array(
				new AccessRightCategory("people","Personal Information",array(
						new BooleanRight("edit_people_contacts","Edit people contacts",array(true=>array("consult_people_contacts"=>true))),
				)),
		);
	}
	
	public function is_page_allowed($path) {
		if ($path == "profile_contact") {
			if ($_GET["people"] == PNApplication::$instance->user_people->user_people_id) return true;
			return PNApplication::$instance->user_management->has_right("consult_people_contacts");
		}
	}
	
	public function get_profile_pages(&$people) {
		return array(
				"profile_contact"=>array("/static/contact/contact_16.png", get_locale("contact","Contact"), "/dynamic/contact/page/profile_contact?people=".$people,5),
		);
	}
	
}
?>
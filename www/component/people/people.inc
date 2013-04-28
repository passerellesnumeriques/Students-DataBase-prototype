<?php 
class people extends Component {

	public $user_people_id;
	public $first_name;
	public $last_name;

	function init() {
		$this->app->user_management->logged_in->listen($this, "logged_in");
	}
	
	public function logged_in() {
		require_once("common/SQLQuery.inc");
		$people = SQLQuery::create()
			->select("UserPeople")
			->where("domain",$this->app->user_management->domain)->where("username",$this->app->user_management->username)
			->join("UserPeople", "People", array("people"=>"id"))
			->field("People", "id")
			->field("People", "first_name")
			->field("People", "last_name")
			->execute_single_row();
		if ($people <> null) {
			$this->user_people_id = $people["id"];
			$this->first_name = $people["first_name"];
			$this->last_name = $people["last_name"];
		}
	}
	
	public function dependencies() { return array("user_management"); }
	
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
				new BooleanRight("edit_people_details","Edit Personal Information"),
			)),
		);
	}
	
	public function populate_model(&$model) {
		$model->addDataCategoryLink("Personal Information", "/dynamic/people/page/profile?plugin=people&people=%People.id%");
		$model->addTable("People")
			->addPrimaryKey("id")
			->addString("first_name", 100, 1, false)
			->addString("last_name", 100, 1, false)
			->addEnum("sex", array("M","F"))
			->addDate("birth", true)
			->addDisplayableData("first_name", "Personal Information", "First Name")
			->addDisplayableData("last_name", "Personal Information", "Last Name")
			->addDisplayableData("sex", "Personal Information", "Sex")
			->addDisplayableData("birth", "Personal Information", "Birth Date")
			->allowReadAccess()
			->restrictWriteAccess("edit_people_details",true);
		$model->addTable("UserPeople")
			->addString("domain", 100, 1, false)
			->addString("username", 100, 1, false)
			->addForeignKey("people", "People")
			->addIndex("domain","username")
			->addLink("Users",array("domain"=>"domain","username"=>"username"),true)
			->allowReadAccess();
	}
	
	protected function is_page_allowed($path) {
		// TODO
		return true;
	}
	protected function is_service_allowed($path) {
		if ($path == "lock" || $path == "unlock" || $path == "save")
			return $this->app->user_management->has_right("edit_people_details");
		return false;
	}
	
	public function get_user_from_people($people, &$domain, &$username) {
		require_once("common/SQLQuery.inc");
		$r = SQLQuery::create()->select("UserPeople")->field("domain")->field("username")->where("people",$people)->execute_single_row();
		if ($r <> null) {
			$domain = $r["domain"];
			$username = $r["username"];
		}
	}
	public function get_people_from_user($domain, $username) {
		require_once("common/SQLQuery.inc");
		return SQLQuery::create()->select("UserPeople")->field("people")->where("domain",$domain)->where("username",$username)->execute_single_value();
	}
	
	public function get_profile_pages(&$people) {
		return array(
			"profile_people"=>array("<img src='/static/people/profile_16.png'/> ".get_locale("people","Personal Information"),"/dynamic/people/page/profile_people?people=".$people),
		);
	}
	
}
?>
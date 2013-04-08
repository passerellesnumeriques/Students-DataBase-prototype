<?php 
class people extends Component {

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
			->field("People", "first_name")
			->field("People", "last_name")
			->execute_single_value();
		if ($people <> null) {
			$this->first_name = $people["first_name"];
			$this->last_name = $people["last_name"];
		}
	}
	
	public function dependencies() { return array("user_management"); }
	
	public function populate_model(&$model) {
		$model->addTable("People")
			->addPrimaryKey("id")
			->addString("first_name", 100)
			->addString("last_name", 100);
		$model->addData("Personal Information", "First Name", "People", "first_name");
		$model->addData("Personal Information", "Last Name", "People", "last_name");
		$model->addTable("UserPeople")
			->addString("domain")
			->addString("username")
			->addForeignKey("people", "People")
			->addIndex("domain","username");
	}
	
	protected function is_page_allowed($path) {
		// TODO
		return true;
	}
	
}
?>
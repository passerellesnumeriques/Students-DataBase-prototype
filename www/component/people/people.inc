<?php 
class people extends Component {

	public $first_name;
	public $last_name;
	
	/** Used by the user management component, to signal a user logged in, so we can populate information about the user */
	public function logged($people_id) {
		$r = SQLQuery::create()->select("People")->field("first_name")->field("last_name")->execute_single_row();
		if ($r <> null) {
			$this->first_name = $r["first_name"];
			$this->last_name = $r["last_name"];
		}
	}
	
	public function populate_model(&$model) {
		$model->addTable("People")
			->addPrimaryKey("id")
			->addString("first_name", 100)
			->addString("last_name", 100);
		$model->addData("Personal Information", "First Name", "People", "first_name");
		$model->addData("Personal Information", "Last Name", "People", "last_name");
	}
	
	protected function is_page_allowed($path) {
		// TODO
		return true;
	}
	
}
?>
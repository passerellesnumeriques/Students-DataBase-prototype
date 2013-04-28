<?php 
class curriculum extends Component {
	
	public function populate_model(&$model) {
		$model->addTable("Curriculum")
			->addPrimaryKey("id")
			->addString("name", 100, 1, false);
		$model->addTable("CurriculumBranch")
			->addPrimaryKey("id")
			->addString("name", 100, 1, false)
			->addForeignKey("curriculum", "Curriculum")
			->addForeignKey("previous", "CurriculumBranch")
			->addForeignKey("next", "CurriculumBranch");
		$model->addTable("CurriculumPeriod")
			->addPrimaryKey("id")
			->addString("name", 100, 1, false)
			->addInteger("weeks")
			->addForeignKey("curriculum", "Curriculum")
			->addForeignKey("branch", "CurriculumBranch")
			->addForeignKey("next", "CurriculumPeriod")
			->addForeignKey("previous", "CurriculumPeriod");
		$model->addTable("CurriculumClassType")
			->addPrimaryKey("id")
			->addString("name", 100, 1, false);
		$model->addTable("CurriculumSubject")
			->addPrimaryKey("id")
			->addString("code", 30, 1, false)
			->addString("name", 100, 1, false);
		$model->addTable("CurriculumSubjectPreRequisites")
			->addForeignKey("subject","CurriculumSubject")
			->addForeignKey("prerequisite","CurriculumSubject");
		$model->addTable("SubjectTotalHours")
			->addForeignKey("subject", "CurriculumSubject")
			->addForeignKey("class_type", "CurriculumClassType")
			->addInteger("total_hours");
		$model->addTable("PeriodSubjects")
			->addForeignKey("period", "CurriculumPeriod")
			->addForeignKey("subject", "CurriculumSubject")
			->addForeignKey("class_type", "CurriculumClassType")
			->addInteger("hours");
	}
	
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
		if ($path == "curriculum_exists")
			return $this->app->user_management->has_right("edit_curricula");
		return false;
	}
	
}
?>
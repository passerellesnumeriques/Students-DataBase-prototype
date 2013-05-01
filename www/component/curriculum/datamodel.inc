<?php 
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
?>
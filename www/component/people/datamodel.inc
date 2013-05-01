<?php
$model->addDataCategoryLink("Personal Information", "/dynamic/people/page/profile?plugin=people&people=%People.id%");
$model->addTable("People")
	->addPrimaryKey("id")
	->addString("first_name", 100, 1, false)
	->addString("middle_name", 100, 1, false)
	->addString("last_name", 100, 1, false)
	->addString("khmer_name", 100, 1, false)
	->addEnum("sex", array("M","F"))
	->addDate("birth", true)
	->addDisplayableData("first_name", "Personal Information", "First Name")
	->addDisplayableData("middle_name", "Personal Information", "Middle Name")
	->addDisplayableData("last_name", "Personal Information", "Last Name")
	->addDisplayableData("khmer_name", "Personal Information", "Khmer Name")
	->addDisplayableData("sex", "Personal Information", "Sex")
	->addDisplayableData("birth", "Personal Information", "Birth Date")
	->restrictReadAccess("see_other_people_details",true)
	->allowSpecificReadAccess("id",function() { return PNApplication::$instance->people->user_people_id; })
	->restrictWriteAccess("edit_people_details",true);
$model->addTable("UserPeople")
	->addString("domain", 100, 1, false)
	->addString("username", 100, 1, false)
	->addForeignKey("people", "People")
	->addIndex("domain","username")
	->addLink("Users",array("domain"=>"domain","username"=>"username"),true)
	->allowReadAccess();
?>
<?php
/* @var $model DataModel */
$model->addTable("Calendar")
	->addPrimaryKey("id")
	->addString("type", 50, 1, false)
	->addString("name", 100, 1, false)
	->addString("data", 1024, 1, false)
	->noRestriction()
	;
$model->addTable("UserCalendar")
	->addString("username", 100, 1, false)
	->addForeignKey("calendar", "Calendar")
	->addIndex("username")
	->allowSpecificReadAccess("username",function() { return PNApplication::$instance->user_management->username; })
	;
?>
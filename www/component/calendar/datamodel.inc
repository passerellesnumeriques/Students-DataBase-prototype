<?php
/* @var $model DataModel */
$model->addTable("Calendar")
	->addPrimaryKey("id")
	->addString("type", 50, 1, false)
	->addString("name", 100, 1, false)
	->addString("data", 1024, 1, false)
	->addInteger("last_modification")
	->addString("owner", 100, true, false)
	->addString("read_rights", 4096, true, false)
	->addString("write_rights", 4096, true, false)
	->noRestriction()
	;
$model->addTable("UserCalendar")
	->addString("username", 100, 1, false)
	->addForeignKey("calendar", "Calendar")
	->addIndex("username")
	->allowSpecificReadAccess("username",function() { return PNApplication::$instance->user_management->username; })
	;

$model->addTable("CalendarEvent")
	->addPrimaryKey("id")
	->addForeignKey("calendar", "Calendar")
	->addString("uid", 255, 1, false, false)
	->addDate("start_date", false, false)
	->addTime("start_time", true, false)
	->addDate("end_date", false, false)
	->addTime("end_time", true, false)
	->addString("freq", 4096, 0, true, false)
	->addInteger("last_modified")
	->addString("title", 1024, 0, false, false)
	->addString("description", 4096, 0, false, false)
	->addString("location", 1024, 0, true, false)
	->addString("organizer", 1024, 0, true, false)
	->addString("attendees", 16384, 0, true, false)
	->noRestriction()
	;
?>
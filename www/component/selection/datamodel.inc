<?php
/* @var $model DataModel */
$model->addTable("SelectionCampaign")
	->addPrimaryKey("id")
	->addString("name", 100, 1, false)
	->addForeignKey("calendar", "Calendar")
	->restrictReadAccess("consult_selection",true)
	->restrictWriteAccess("edit_selection",true)
	->restrictAddAccess("edit_selection", true)
	->restrictRemoveAccess("edit_selection", true)
	;

$model->addTable("Applicant")
	->addPrimaryKey("id")
	->addForeignKey("campaign", "SelectionCampaign")
	->addString("selection_id", 20, 1, false, false)
	->addForeignKey("people", "People")
	->addForeignKey("high_school", "ContactEntity")
	->addForeignKey("ngo", "ContactEntity")
	->restrictReadAccess("consult_selection",true)
	->restrictWriteAccess("edit_selection",true)
	->restrictAddAccess("edit_selection", true)
	->restrictRemoveAccess("edit_selection", true)
	;

$model->addTable("ApplicantSpecific")
	->addForeignKey("applicant", "Applicant")
	//->applyCustomization("applicant")
	->restrictReadAccess("consult_selection",true)
	->restrictWriteAccess("edit_selection",true)
	->restrictAddAccess("edit_selection", true)
	->restrictRemoveAccess("edit_selection", true)
;

$model->addTable("InformationSession")
	->addPrimaryKey("id")
	->addForeignKey("campaign", "SelectionCampaign")
	->addForeignKey("calendar_event", "CalendarEvent")
	->addForeignKey("where", "ContactEntity")
	->restrictReadAccess("consult_selection",true)
	->restrictWriteAccess("edit_selection",true)
	->restrictAddAccess("edit_selection", true)
	->restrictRemoveAccess("edit_selection", true)
	;
?>
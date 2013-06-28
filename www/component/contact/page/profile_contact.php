<?php
$people_id = $_GET["people"];

$edit = PNApplication::$instance->user_management->has_right("edit_people_contacts");
if (!$edit)
	$edit = $people_id == PNApplication::$instance->user_people->user_people_id;

if ($edit) {
	require_once("common/DataBaseLock.inc");
	$locked_by = null;
	$lock_id = DataBaseLock::lock("PeopleContact", array("people"=>$people_id), $locked_by);
	if ($lock_id === null) {
		$edit = false;
		// TODO add a banner indicating that xx is currently editing this contact
	}
}

require_once("component/contact/page/contact.inc");
contacts_page($this, "PeopleContact", array("people"=>$people_id), "contact", $edit);
?>
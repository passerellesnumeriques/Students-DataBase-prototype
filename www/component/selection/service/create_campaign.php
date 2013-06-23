<?php
$name = $_POST["name"];

require_once("common/SQLQuery.inc");

// create a calendar for the selection process
$cal_id = PNApplication::$instance->calendar->create_calendar("Selection Process ".$name, null, array("consult_selection"=>true), array("edit_selection"=>true));

// create the campaign
$id = SQLQuery::insert("SelectionCampaign", array("name"=>$name, "calendar"=>$cal_id));

echo "{id:".$id."}";
?>
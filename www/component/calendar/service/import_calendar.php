<?php
$name = $_POST["name"];
$type = $_POST["type"];
$data = $_POST["data"];

// check access to the calendar is allowed
if ($type == "internet") {
	// all internet are allowed
} else if ($type == "internal") {
	PNApplication::error("Access Denied");
	return;
}

DataBase::execute("INSERT INTO Calendar (`name`,`type`,`data`) VALUES('".DataBase::escape_string($name)."','".DataBase::escape_string($type)."','".DataBase::escape_string($data)."')");
$id = DataBase::get_insert_id();

DataBase::execute("INSERT INTO UserCalendar (`calendar`,`username`) VALUES (".$id.",'".DataBase::escape_string(PNApplication::$instance->user_management->username)."')");

if ($type == "internet") {
	require_once("update_internet_calendar.inc");
	update_internet_calendar($id, $data, 0);
}

echo "{id:".$id."}";
?>
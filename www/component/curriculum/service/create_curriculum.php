<?php
$name = $_POST["name"];
$copy = @$_POST["copy"];

DataBase::execute("LOCK TABLES Curriculum WRITE");
$res = DataBase::execute("SELECT id FROM Curriculum WHERE `name`='".DataBase::escape_string($name)."'");
$id = 0;
if ($res && DataBase::next_row($res) <> null)
	PNApplication::error(get_locale("common","__ already exists",array("name"=>$name)));
else {
	DataBase::execute("INSERT INTO Curriculum (`name`) VALUE ('".DataBase::escape_string($name)."')");
	$id = DataBase::get_insert_id();
}
PNApplication::print_json_result("{id:".$id."}");
?>
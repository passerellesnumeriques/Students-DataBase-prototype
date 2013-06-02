<?php
$curriculum = $_POST["curriculum"];
$name = $_POST["name"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

DataBase::execute("INSERT INTO CurriculumSubjectCategory (`curriculum`,`name`) VALUE ('".$curriculum."','".DataBase::escape_string($name)."')");
$id = DataBase::get_insert_id();

echo "{id:".$id."}";
?>
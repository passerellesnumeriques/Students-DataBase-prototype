<?php
$curriculum = $_POST["curriculum"];
$name = $_POST["name"];
$previous = $_POST["previous"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

DataBase::execute("INSERT INTO CurriculumBranch (`curriculum`,`name`,`previous`) VALUE ('".$curriculum."','".DataBase::escape_string($name)."','".$previous."')");
$id = DataBase::get_insert_id();

echo "{id:".$id."}";
?>
<?php
$curriculum = $_POST["curriculum"];
$id = $_POST["id"];
$name = $_POST["name"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

DataBase::execute("UPDATE CurriculumBranch SET `name`='".DataBase::escape_string($name)."' WHERE curriculum='".$curriculum."' AND id='".$id."'");
echo "true";
?>
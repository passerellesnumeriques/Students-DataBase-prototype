<?php
$curriculum = $_POST["curriculum"];
$category = $_POST["category"];
$name = $_POST["name"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

DataBase::execute("UPDATE CurriculumSubjectCategory SET `name`='".DataBase::escape_string($name)."' WHERE id=".$category);

echo "true";
?>
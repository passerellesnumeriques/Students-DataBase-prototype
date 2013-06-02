<?php
$curriculum = $_POST["curriculum"];
$subject = $_POST["subject"];
$code = $_POST["code"];
$name = $_POST["name"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

DataBase::execute("UPDATE CurriculumSubject SET `code`='".DataBase::escape_string($code)."',`name`='".DataBase::escape_string($name)."' WHERE id=".$subject);

DataBase::execute("DELETE FROM SubjectTotalHours WHERE `subject`=".$subject);
foreach ($_POST as $key=>$value) {
	if (substr($key,0,3) <> "ct_") continue;
	$ct_id = substr($key,3);
	DataBase::execute("INSERT INTO SubjectTotalHours (`subject`,`class_type`,`total_hours`) VALUE (".$subject.",".$ct_id.",".$value.")");
}

echo "true";
?>
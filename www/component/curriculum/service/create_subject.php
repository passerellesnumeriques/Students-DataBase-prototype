<?php
$curriculum = $_POST["curriculum"];
$category = $_POST["category"];
$code = $_POST["code"];
$name = $_POST["name"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

// check the category is valid
require_once("common/SQLQuery.inc");
$r = SQLQuery::create()->select("CurriculumSubjectCategory")->where("id",$category)->where("curriculum",$curriculum)->execute_single_row();
if (!$r) {
	PNApplication::error("Invalid subject category");
	echo "false";
	return;
}

DataBase::execute("INSERT INTO CurriculumSubject (`category`,`code`,`name`) VALUE ('".$category."','".DataBase::escape_string($code)."','".DataBase::escape_string($name)."')");
$id = DataBase::get_insert_id();

foreach ($_POST as $key=>$value) {
	if (substr($key,0,3) <> "ct_") continue;
	$ct_id = substr($key,3);
	DataBase::execute("INSERT INTO SubjectTotalHours (`subject`,`class_type`,`total_hours`) VALUE (".$id.",".$ct_id.",".$value.")");
}

echo "{id:".$id."}";
?>
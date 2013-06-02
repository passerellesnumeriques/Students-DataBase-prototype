<?php
$curriculum = $_POST["curriculum"];
$period = $_POST["period"];
$subject = $_POST["subject"];

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

// check the period is valid
require_once("common/SQLQuery.inc");
$r = SQLQuery::create()->select("CurriculumPeriod")->where("id",$period)->where("curriculum",$curriculum)->execute_single_row();
if (!$r) {
	PNApplication::error("Invalid period");
	echo "false";
	return;
}

// check the subject is not already in the period
$r = SQLQuery::create()->select("PeriodSubjects")->where("period",$period)->where("subject",$subject)->execute();
if ($r <> null && count($r) > 0) {
	PNApplication::error(get_locale("This period already contains this subject"));
	echo "false";
	return;
}

$hours = SQLQuery::create()->select("SubjectTotalHours")->where("subject",$subject)->execute();

if ($hours == null || count($hours) == 0) {
	PNApplication::error(get_locale("This subject has no hours left"));
	echo "false";
	return;
}

foreach ($hours as $h) {
	DataBase::execute("INSERT INTO PeriodSubjects (`period`,`subject`,`class_type`,`hours`) VALUE (".$period.",".$subject.",".$h["class_type"].",".$h["total_hours"].")");
}

echo "true";
?>
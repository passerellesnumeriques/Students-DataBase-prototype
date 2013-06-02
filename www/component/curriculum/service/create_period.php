<?php
$curriculum = $_POST["curriculum"];
$branch = $_POST["branch"];
$name = $_POST["name"];
$previous = $_POST["previous"];
if ($previous == null) $previous = 0;

require_once("common/DataBaseLock.inc");
if (!DataBaseLock::has_lock("Curriculum", "id", $curriculum)) {
	PNApplication::error(get_locale("common", "Access Denied"));
	echo "false";
	return;
}

require_once("common/SQLQuery.inc");

// check branch is valid
$b = SQLQuery::create()->select("CurriculumBranch")->where("id",$branch)->execute_single_row();
if ($b == null || $b["curriculum"] <> $curriculum) {
	PNApplication::error("Invalid branch");
	echo "false";
	return;
}
$next = 0;
if ($previous == 0) {
	// first period, check if there is already one
	$p = SQLQuery::create()->select("CurriculumPeriod")->where("curriculum",$curriculum)->where("branch",$branch)->where("previous",0)->execute_single_row();
	if ($p <> null) $next = $p["id"];
} else {
	$p = SQLQuery::create()->select("CurriculumPeriod")->where("curriculum",$curriculum)->where("branch",$branch)->where("id",$previous)->execute_single_row();
	if ($p == null) {
		PNApplication::error("Invalid previous period");
		echo "false";
		return;
	}
	$next = $p["next"];
}
DataBase::execute("INSERT INTO CurriculumPeriod (`curriculum`,`branch`,`name`,`previous`,`next`) VALUE ('".$curriculum."','".$branch."','".DataBase::escape_string($name)."','".$previous."','".$next."')");
$id = DataBase::get_insert_id();

if ($previous <> 0)
	DataBase::execute("UPDATE CurriculumPeriod SET `next`='".$id."' WHERE id='".$previous."'");
if ($next <> 0)
	DataBase::execute("UPDATE CurriculumPeriod SET `previous`='".$id."' WHERE id='".$next."'");

echo "{id:".$id."}";
?>
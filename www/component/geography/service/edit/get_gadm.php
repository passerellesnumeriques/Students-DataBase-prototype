<?php
global $country_code, $types;

$country_code = $_GET["country"];
$level = $_GET["level"];
$parent_id = $_GET["parent"];

$res = DataBase::execute("SELECT * FROM `gadm`.`adm".$level."` WHERE `country`='".$country_code."'".($parent_id <> 0 ? " AND `parent_id`=".$parent_id : "")." ORDER BY `name`");
echo "[";
$first = true;
set_time_limit(120);
while (($r = DataBase::next_row($res)) <> null) {
	if ($first) $first = false; else echo ",";
	
	$name = utf8_encode($r["name"]);
	$type = $r["type"];
	if ($type <> null) $type = utf8_encode($type);
	$type_en = $r["type_en"];
	if ($type_en <> null) $type_en = utf8_encode($type_en);
	
	echo "{";
	echo "id:".$r["id"];
	echo ",name:".json_encode($name);
	echo ",type:".json_encode($type);
	echo ",type_en:".json_encode($type_en);
	echo "}";
}
echo "]";
?>
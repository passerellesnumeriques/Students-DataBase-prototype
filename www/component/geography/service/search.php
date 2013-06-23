<?php
$country = $_POST["country"];
$search = $_POST["search"];

$search_level = 1;
$parent_id = null;
for ($level = 4; $level >= 1; $level--) {
	if (isset($_POST["adm".$level])) {
		$search_level = $level+1;
		$parent_id = intval($_POST["adm".$level]);
		break;
	}
}

require_once("common/DataBase.inc");
$nb = 0;
$max = 1000;
echo "[";
for ($level = $search_level; $level <= 5; $level++) {
	$fields = "`geography`.`adm".$level."`.id AS `id".$level."`,`geography`.`adm".$level."`.name AS `name".$level."`,`geography`.`adm".$level."`.type AS `type".$level."`,`geography`.`adm".$level."`.type_en AS `type_en".$level."`";
	$q = " FROM `geography`.`adm".$level."`";
	for ($parent_level = $level-1; $parent_level >= $search_level; $parent_level--) {
		$q .= " LEFT JOIN `geography`.`adm".$parent_level."` ON `geography`.`adm".$parent_level."`.id=`geography`.`adm".($parent_level+1)."`.parent_id";
		$fields .= ",`geography`.`adm".$parent_level."`.id AS `id".$parent_level."`,`geography`.`adm".$parent_level."`.name AS `name".$parent_level."`,`geography`.`adm".$parent_level."`.type AS `type".$parent_level."`,`geography`.`adm".$parent_level."`.type_en AS `type_en".$parent_level."`";
	}
	$q .= " WHERE `geography`.`adm".$level."`.country='".DataBase::escape_string($country)."' AND `geography`.`adm".$level."`.name LIKE '%".DataBase::escape_string($search)."%'";
	if ($parent_id <> null)
		$q .= " AND `geography`.`adm".$search_level."`.parent_id=".$parent_id;
	$q .= " LIMIT ".($max-$nb);
	$q = "SELECT ".$fields.$q;
	$res = DataBase::execute($q);
	while (($r = DataBase::next_row($res)) <> null) {
		if ($nb > 0) echo ",";
		echo "{";
		echo "level:".$level;
		echo ",id:".$r["id".$level];
		echo ",name:".json_encode(utf8_encode($r["name".$level]));
		$type = utf8_encode($r["type".$level]);
		$type_en = $r["type_en".$level];
		if ($type_en <> null) $type = utf8_encode($type_en)." (".$type.")";
		echo ",type:".json_encode($type);
		if ($level > 1) {
			echo ",parents:[";
			for ($p = $level-1; $p >= $search_level; $p--) {
				if ($p < $level-1) echo ",";
				echo "{";
				echo "level:".$p;
				echo ",id:".$r["id".$p];
				echo ",name:".json_encode(utf8_encode($r["name".$p]));
				$type = utf8_encode($r["type".$p]);
				$type_en = $r["type_en".$p];
				if ($type_en <> null) $type = utf8_encode($type_en)." (".$type.")";
				echo ",type:".json_encode($type);
				echo "}";
			}
			echo "]";
		}
		echo "}";
		$nb++;
	}
	if ($nb == $max) break;
}
echo "]";
?>
<?php
global $country_code, $types;

$country_code = $_GET["country"];
$parent = $_GET["parent"];

$q = "SELECT `geography`.`adm`.id,`geography`.`name`.name FROM `geography`.`adm`";
$q .= " LEFT JOIN `geography`.`adm_name` ON (`geography`.`adm_name`.id=`geography`.`adm`.id AND `geography`.`adm_name`.lang='__')";
$q .= " LEFT JOIN `geography`.`name` ON `geography`.`name`.id=`geography`.`adm_name`.name";
$q .= " WHERE `geography`.`adm`.`country`='".$country_code."' AND `geography`.`adm`.`parent_id`".($parent <> 0 ? "=".$parent : " IS NULL");
$q .= " ORDER BY `geography`.`name`.name";
$res = DataBase::execute($q);
echo "[";
$first = true;
set_time_limit(120);
while (($r = DataBase::next_row($res)) <> null) {
	if ($first) $first = false; else echo ",";
	$name = utf8_encode($r["name"]);
	
	echo "{";
	echo "id:".$r["id"];
	echo ",name:".json_encode($name);
	echo "}";
}
echo "]";
?>
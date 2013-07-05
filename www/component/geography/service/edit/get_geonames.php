<?php
global $country_code;
$country_code = $_GET["country"];
$level = intval($_GET["level"]);
$table = $level < 5 ? "adm".$level : "ppl";
$q = "SELECT * FROM `geonames`.`".$table."_temp` WHERE `country`='".$country_code."'";
for ($i = 1; $i < $level; $i++) {
	$q .= " AND `adm".$i."`='".$_GET["adm".$i]."'";
}
set_time_limit(10*60);
$res = DataBase::execute($q);
echo "[";
$first = true;
while (($r = DataBase::next_row($res)) <> null) {
	$res2 = DataBase::execute("SELECT * FROM `geonames`.`".$table."_temp_names` WHERE `geonames`.`".$table."_temp_names`.`id`=".$r["id"]);
	$names = array();
	while (($r2 = DataBase::next_row($res2)) <> null)
		$names[$r2["lang"]] = utf8_encode($r2["name"]);
	$res2 = DataBase::execute("SELECT * FROM `geography_match`.`adm` WHERE `geonames_id`=".$r["id"]." AND `geonames_level`=".$level);
	if ($res2) $res2 = DataBase::next_row($res2);
	
	$name = "";
	if (isset($names["__"])) $name = $names["__"]; else if (isset($names["en"])) $name = $names["en"];
	
	if ($first) $first = false; else echo ",";
	echo "{";
	echo "id:".$r["id"];
	echo ",name:".json_encode($name);
	if ($level < 5)
		echo ",adm".$level.":".json_encode($r["adm".$level]);
	echo "}";
}
echo "]";
?>

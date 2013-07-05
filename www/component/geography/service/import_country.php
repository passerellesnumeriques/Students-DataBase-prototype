<?php
$geon = $_POST["geonames"];
$gadm = $_POST["gadm"];
if ($geon == "null") $geon = null;
if ($gadm == "null") $gadm = null;

$country = array("code"=>null,"sovereign"=>null,"latitude"=>"0","longitude"=>"0","timezone"=>null);
$names = array();
if ($geon <> null) {
	$res = DataBase::execute("SELECT * FROM `geonames`.`country_temp` WHERE `code`='".$geon."'");
	if (!$res) {
		PNApplication::error("Invalid code for geonames");
		return;
	}
	$r = DataBase::next_row($res);
	$geon = $r["code"];
	$country["code"] = $r["code"];
	$country["latitude"] = $r["latitude"];
	$country["longitude"] = $r["longitude"];
	$country["timezone"] = $r["timezone"];
	$res = DataBase::execute("SELECT * FROM `geonames`.`country_temp_names` WHERE `code`='".$geon."'");
	while (($r = DataBase::next_row($res)) <> null)
		$names[$r["lang"]] = $r["name"];
}
if ($gadm <> null) {
	$res = DataBase::execute("SELECT * FROM `gadm`.`country` WHERE `code`='".$gadm."'");
	if (!$res) {
		PNApplication::error("Invalid code for gadm");
		return;
	}
	$r = DataBase::next_row($res);
	if (!isset($names["__"]) && !isset($names["en"])) $names["__"] = $r["name"];
	if ($country["code"] == null)
		$country["code"] = $r["code"];
	$country["sovereign"] = $r["sovereign"];
	$gadm = $r["code"];
}
$q = "INSERT INTO `geography`.`country` (`code`,`sovereign`,`latitude`,`longitude`,`timezone`) VALUE (";
$q .= "'".$country["code"]."'";
$q .= ",".($country["sovereign"] == null ? "NULL" : "'".$country["sovereign"]."'");
$q .= ",".$country["latitude"];
$q .= ",".$country["longitude"];
$q .= ",".($country["timezone"] == null ? "NULL" : "'".$country["timezone"]."'");
$q .= ")";
DataBase::execute($q);
foreach ($names as $lang=>$name) {
	$res = DataBase::execute("SELECT `id` FROM `geography`.`name` WHERE `name`='".DataBase::escape_string($name)."'");
	$id = 0;
	if ($res && ($r = DataBase::next_row($res)) <> null) $id = $r["id"];
	if ($id == 0) {
		DataBase::execute("INSERT INTO `geography`.`name` (`name`) VALUE ('".DataBase::escape_string($name)."')");
		$id = DataBase::get_insert_id();
	}
	DataBase::execute("INSERT INTO `geography`.`country_name` (`code`,`lang`,`name`) VALUE ('".$country["code"]."','".$lang."',".$id.")");
}
$q = "INSERT INTO `geography_match`.`country` (`code`,`geonames_code`,`gadm_code`) VALUE ('".$country["code"]."'";
$q .= ",".($geon <> null ? "'".$geon."'" : "NULL");
$q .= ",".($gadm <> null ? "'".$gadm."'" : "NULL");
$q .= ")";
DataBase::execute($q);
echo "true";
?>
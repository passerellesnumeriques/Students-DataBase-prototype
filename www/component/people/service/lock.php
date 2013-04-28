<?php 
$field = $_POST["field"];
$id = $_POST["id"];
require_once("common/DataBaseLock.inc");
$locked_by = null;
$lock = DataBaseLock::lock("People", array("id"=>$id), $locked_by, false);
if ($lock == null) {
	die("{errors:".json_encode(get_locale("common","this data is already locked by")." ".$locked_by)."}");
}
require_once("common/SQLQuery.inc");
$value = SQLQuery::create()->select("People")->field($field)->where("id",$id)->execute_single_value();
$result = "{lock:".json_encode($lock).",value:".json_encode($value)."}";
PNApplication::print_json_result($result);
?>
<?php
$table = $_POST["table"];
$key = $_POST["key"];
$field = $_POST["field"];
$value = $_POST["value"];
$lock_id = $_POST["lock"];
require_once("component/data_model/DataModel.inc");
try {
	$table = DataModel::get()->getTable($table);
	$table->update_by_key($key, array($field=>$value), $lock_id);
} catch (Exception $e) {
	PNApplication::error($e->getMessage());
}
echo PNApplication::has_errors() ? "false" : "true";
?>
<?php 
$name = $_POST["name"];
require_once("component/data_model/DataModel.inc");
$id = null;
try { $id = DataModel::get()->getTable("Role")->insert(array("name"=>$name)); }
catch (Exception $e) {
	PNApplication::error($e->getMessage());
}
PNApplication::print_json_result($id <> null ? "{id:".$id."}" : "false");
?>
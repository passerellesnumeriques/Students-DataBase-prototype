<?php 
require_once("component/data_model/DataModel.inc");
try {
	DataModel::get()->getTable("Role")->remove_key($_POST["id"]);
} catch (Exception $e) {
	PNApplication::error($e->getMessage());
}
PNApplication::print_json_result("true");
?>
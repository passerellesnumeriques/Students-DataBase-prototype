<?php 
require_once("component/data_model/list/DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

$lock = $_POST["lock"];
require_once("common/DataBaseLock.inc");
$error = DataBaseLock::unlock($lock);
if ($error <> null)
	PNApplication::error($error);
else echo "true";
?>
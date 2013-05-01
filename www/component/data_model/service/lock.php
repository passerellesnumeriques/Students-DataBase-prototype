<?php 
require_once("component/data_model/list/DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

$lock_field = $_POST["lock_field"];
$col = null;
foreach ($list->columns as $c) {
	if ($c->get_path() == $lock_field) {
		$col = $c;
		break;
	}
}
if ($col == null) die("<error message=\"".htmlspecialchars("Invalid field '".$lock_field."'",ENT_COMPAT,"UTF-8")."\"/>");

require_once("common/DataBaseLock.inc");
$locked_by = null;
ob_start();
$lock_id = DataBaseLock::lock($col->table, null, $locked_by);
ob_clean();
if ($locked_by <> null) die("<error message=\"".htmlspecialchars(get_locale("common","This data is already locked by")." ".$locked_by,ENT_COMPAT,"UTF-8")."\"/>");
echo "<ok lock='".$lock_id."'/>";
?>
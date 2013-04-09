<?php
require_once("DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

// process request
$table = $list->process();
echo "[";
$first_row = true;
foreach ($table as $row) {
	if ($first_row) $first_row = false; else echo ",";
	echo "[";
	$first_col = true;
	foreach ($list->columns as $col) {
		if ($first_col) $first_col = false; else echo ",";
		if (!isset($row[$col->final_name])) echo "'ERROR:".$col->final_name."'";
		else echo json_encode($row[$col->final_name]);
	}
	echo "]";
}
echo "]";
?>
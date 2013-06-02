<?php
require_once("component/data_model/list/DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

// process request
$table = $list->process();
echo "{";
$first_row = true;
if ($table) {
	$count = $table["count"];
	if (!$count) $count = 0;
	echo "total:".$count.",";
	echo "start:".$list->start_entry.",";
	echo "data:[";
	foreach ($table["list"] as $row) {
		if ($first_row) $first_row = false; else echo ",";
		echo "{k:[";
		$first_key = true;
		foreach ($table["primary_keys"] as $alias) {
			if ($first_key) $first_key = false; else echo ",";
			echo json_encode($row[$alias]);
		}
		echo "],v:[";
		$first_col = true;
		foreach ($list->columns as $col) {
			if ($first_col) $first_col = false; else echo ",";
			if (!isset($row[$col->final_name])) echo "null";
			else echo json_encode($row[$col->final_name]);
		}
		echo "]}";
	}
	echo "]";
}
echo "}";
?>
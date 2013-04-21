<?php
require_once("component/data_list/DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

// process request
$table = $list->process();
$result = "{";
$first_row = true;
if ($table) {
	$count = $table["count"];
	if (!$count) $count = 0;
	$result .= "total:".$count.",";
	$result .= "start:".$list->start_entry.",";
	$result .= "data:[";
	foreach ($table["list"] as $row) {
		if ($first_row) $first_row = false; else $result .= ",";
		$result .= "[";
		$first_col = true;
		foreach ($list->columns as $col) {
			if ($first_col) $first_col = false; else $result .= ",";
			if (!isset($row[$col->final_name])) $result .= "'ERROR:".$col->final_name."'";
			else $result .= json_encode($row[$col->final_name]);
		}
		$result .= "]";
	}
	$result .= "]";
}
$result .= "}";
PNApplication::print_json_result($result);
?>
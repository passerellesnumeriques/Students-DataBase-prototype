<?php
require_once("component/data_model/list/DataList.inc");
// initialize DataList
$list = new DataList($_POST["starting_table"]);
$list->update_from_request();

// process request
$table = $list->process();

// create excel
error_reporting(E_ERROR | E_PARSE);
require_once("common/PHPExcel.php");
$excel = new PHPExcel();
$sheet = new PHPExcel_Worksheet($excel, "List");
$excel->addSheet($sheet);
$excel->removeSheetByIndex(0);
if ($table) {
	$col_index = 0;
	foreach ($list->columns as $col) {
		$name = $list->model->getTable($col->table)->getDisplayableDataCategoryAndName($col->field);
		$sheet->setCellValueByColumnAndRow($col_index, 1, $name[1]);
		$col_index++;
	}
	$row_index = 2;
	foreach ($table["list"] as $row) {
		$col_index = 0;
		foreach ($list->columns as $col) {
			$value = $row[$col->final_name];
			if (is_array($value)) {
				$s = "";
				foreach ($value as $v) {
					if (strlen($s) > 0) $s .= ",";
					$s .= "$v";
				}
				$value = $s;
			}
			$sheet->setCellValueByColumnAndRow($col_index, $row_index, $value);
			$col_index++;
		}
		$row_index++;
	}
}

$format = $_GET["export"];
if ($format == 'excel2007') {
	header("Content-Type: application/vnd.ms-excel");
	header("Content-Disposition: attachment; filename=\"list.xlsx\"");
	$writer = new PHPExcel_Writer_Excel2007($excel);
} else if ($format == 'excel5') {
	header("Content-Type: application/vnd.ms-excel");
	header("Content-Disposition: attachment; filename=\"list.xls\"");
	$writer = new PHPExcel_Writer_Excel5($excel);
} else if ($format == 'csv') {
	header("Content-Type: text/csv;charset=UTF-8");
	header("Content-Disposition: attachment; filename=\"list.csv\"");
	echo "\xEF\xBB\xBF"; // UTF-8 BOM
	$writer = new PHPExcel_Writer_CSV($excel);
} else if ($format == 'pdf') {
	header("Content-Type: application/pdf");
	header("Content-Disposition: attachment; filename=\"list.pdf\"");
	PHPExcel_Settings::setPdfRenderer(PHPExcel_Settings::PDF_RENDERER_MPDF, "common/MPDF");
	$writer = new PHPExcel_Writer_PDF($excel);
}

$writer->save('php://output');
?>
<?php
require_once("component/data_model/list/DataList.inc");
$list = new DataList($_POST["starting_table"]); 
for ($i = 0; isset($_POST["pk".$i]); $i++) {
	$list->primary_key($_POST["pk".$i]);
	$list->add($_POST["pk".$i], false);
}

// get all fields that will be modified
$fields = array();
$i = 0;
do {
	if (!isset($_POST["mod_".$i."_change_0_field"])) break;
	$j = 0;
	do {
		$f = $_POST["mod_".$i."_change_".$j."_field"];
		if (!in_array($f, $fields)) array_push($fields, $f);
		$j++;
	} while (isset($_POST["mod_".$i."_change_".$j."_field"]));
	$i++;
} while (true);
foreach ($fields as $f)
	if (!$list->add($f,true))
		PNApplication::error("Access denied for field ".$f);
if (PNApplication::has_errors()) return;

// check we have locks for all those fields
require_once("common/DataBaseLock.inc");
foreach ($list->columns as $col)
	if (!DataBaseLock::has_lock($col->table))
		PNApplication::error("Access denied for field ".$f.": data must be locked before.");
if (PNApplication::has_errors()) return;

// update
$i = 0;
do {
	if (!isset($_POST["mod_".$i."_change_0_field"])) break;
	$pk = array();
	$j = 0;
	do {
		$pk[$list->primary_key[$j]] = $_POST["mod_".$i."_pk_".$j];
		$j++;
	} while (isset($_POST["mod_".$i."_pk_".$j]));
	$j = 0;
	do {
		$f = $_POST["mod_".$i."_change_".$j."_field"];
		$v = $_POST["mod_".$i."_change_".$j."_value"];
		try { $list->update_data($pk, $f, $v); }
		catch (Exception $e) {
			$col = $list->get($f);
			$table = $list->model->getTable($col->table);
			$name = $table->getDisplayableDataName($col->field);
			if ($name == null) $name = $f;
			PNApplication::error($name.": ".$e->getMessage());
		}
		$j++;
	} while (isset($_POST["mod_".$i."_change_".$j."_field"]));
	$i++;
} while (true);
echo "true";
?>
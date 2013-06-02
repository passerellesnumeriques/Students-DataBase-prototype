<?php
$table = $_POST["table"];
$where_fields = array();
for ($i = 0; isset($_POST["where".$i."_name"]); $i++)
	$where_fields[$_POST["where".$i."_name"]] = $_POST["where".$i."_value"];

require_once("component/data_model/DataModel.inc");
/* @var $model DataModel */
$model = DataModel::get();
try {
	$t = $model->getTable($table);
} catch (Exception $e) {
	PNApplication::error($e->getMessage());
	return;
}

// get all existing
$list = $t->select()->where($where_fields)->execute();

$new = array();
$update = array();
for ($i = 0; isset($_POST["row".$i."_key"]); $i++) {
	$fields = array();
	for ($j = 0; isset($_POST["row".$i."_change".$j."_field"]); $j++)
		$fields[$_POST["row".$i."_change".$j."_field"]] = $_POST["row".$i."_change".$j."_value"];
	$key = $_POST["row".$i."_key"];
	if ($key == 0)
		array_push($new, $fields);
	else {
		array_push($update, array($key, $fields));
		for ($j = 0; $j < count($list); $j++)
			if ($list[$j][$t->getPrimaryKey()->name] == $key) {
				array_splice($list, $j, 1);
				break;
			}
	}
}

/** validate fields value
 * @param \datamodel\Table $t
 */
function validate_fields(&$t, $fields) {
	foreach ($fields as $name=>$value) {
		try {
			$t->getColumn($name)->validate($value);
		} catch (Exception $e) {
			PNApplication::error($t->getDisplayableDataName($name).": ".$e->getMessage());
		}
	}
}
foreach ($new as $fields) validate_fields($t, $fields);
foreach ($update as $a) validate_fields($t, $a[1]);

if (PNApplication::has_errors()) return;

// remove
try {
	$t->remove_rows($list);
} catch (Exception $e) {
	PNApplication::error($e->getMessage());
	return;
}

// update
foreach ($update as $a)
	$t->update_by_key($a[0], $a[1]);

// insert
foreach ($new as $fields) {
	foreach ($where_fields as $n=>$v) $fields[$n] = $v;
	$t->insert($fields);
}

echo "true";
?>
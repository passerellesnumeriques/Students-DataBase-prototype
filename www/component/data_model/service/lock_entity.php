<?php 
$table = $_POST["table"];
$field = $_POST["field"];
$key = $_POST["key"];
require_once("common/DataBaseLock.inc");
require_once("component/data_model/DataModel.inc");
$model = DataModel::get();
$table = $model->getTable($table); // here check is done is the user can access this table
if (!$table->canModifyField($field)) die("Access Denied");
$locked_by = null;
$lock = DataBaseLock::lock($table->getName(), array($table->getPrimaryKey()->name=>$key), $locked_by, false);
if ($lock == null) {
	PNApplication::error(get_locale("common","this data is already locked by")." ".$locked_by);
	return;
}
require_once("common/SQLQuery.inc");
$value = SQLQuery::create()->select($table->getName())->field($field)->where($table->getPrimaryKey()->name,$key)->execute_single_value();
echo "{lock:".json_encode($lock).",value:".json_encode($value)."}";
?>
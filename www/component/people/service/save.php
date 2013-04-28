<?php 
$id = $_POST["id"];
$lock_id = $_POST["lock"];
$field = $_POST["field"];
$value = $_POST["value"];
require_once("common/DataBaseLock.inc");
if (!DataBaseLock::check($lock_id, "People", array("id"=>$id)))
	die("{errors:[".json_encode(get_locale("common","Access Denied"))."]}");
if ($field == "first_name" || $field == "last_name") {
	if ($value == "")
		die("{errors:[".json_encode(get_locale("name cannot be empty"))."]}");
}
DataBase::$conn->execute("UPDATE People SET `".$field."`=".($value == "" ? "NULL" : "'".DataBase::$conn->escape_string($value)."'")." WHERE id=".$id);
$error = DataBaseLock::unlock($lock_id);
if ($error == null) echo "{result:true}";
else echo "{errors:[".json_encode($error)."]}";
?>
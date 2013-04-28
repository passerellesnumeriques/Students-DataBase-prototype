<?php 
$id = $_POST["id"];
require_once("common/DataBaseLock.inc");
$error = DataBaseLock::unlock($id);
if ($error == null) echo "{result:true}";
else echo "{errors:[".json_encode($error)."]}";
?>
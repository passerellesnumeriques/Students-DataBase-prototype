<?php 
$lock = $_POST["lock"];
require_once("common/DataBaseLock.inc");
$error = DataBaseLock::unlock($lock);
if ($error == null) echo "{result:true}";
else echo "{errors:[".json_encode($error)."]}";
?>
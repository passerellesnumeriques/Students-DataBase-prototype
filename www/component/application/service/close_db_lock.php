<?php
require_once("common/DataBaseLock.inc"); 
$error = DataBaseLock::unlock($_POST["id"]);
if (!$error)
	echo "<ok/>";
else
	echo "<error message=".json_encode($error)."/>";
?>
<?php 
$lock = $_POST["lock"];
require_once("common/DataBaseLock.inc");
$error = DataBaseLock::unlock($lock);
if ($error == null) echo "true";
else PNApplication::error($error);
?>
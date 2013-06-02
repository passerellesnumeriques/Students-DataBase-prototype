<?php 
global $app;
$app->user_management->logout();
header("Location: /".(isset($_GET["from"]) ? "?from=".$_GET["from"] : ""));
?>
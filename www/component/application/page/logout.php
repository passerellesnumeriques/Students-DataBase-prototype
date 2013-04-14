<?php 
global $app;
$app->user_management->logout();
header("Location: /");
?>
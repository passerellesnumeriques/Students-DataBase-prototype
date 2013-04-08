<?php
global $app;
if ($app->user_management->username == null)
	header("Location: login".(isset($_GET["page"])?"?page=".$_GET["page"]:""));
else if (isset($_GET["page"]))
	header("Location: ".$_GET["page"]);
else
	header("Location: home");
?>
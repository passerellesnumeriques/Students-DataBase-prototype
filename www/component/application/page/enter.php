<?php
global $app;
if ($app->user_management->username == null)
	include "login.inc";
else
	include "layout.inc";
?>
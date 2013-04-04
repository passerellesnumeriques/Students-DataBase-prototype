<?php
if (!isset($_SERVER["PATH_INFO"]) || strlen($_SERVER["PATH_INFO"]) == 0) $_SERVER["PATH_INFO"] = "/";
$path = substr($_SERVER["PATH_INFO"],1);

if ($path == "favicon.ico") { header("Content-Type: image/ico"); readfile("favicon.ico"); die(); }

set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__));

if ($path == "") {
	header("Location: /dynamic/application/page/enter");
	die();
}

// get type of resource
$i = strpos($path, "/");
if ($i === FALSE) die("Invalid request");
$type = substr($path, 0, $i);
$path = substr($path, $i+1);

// get the component name
$i = strpos($path, "/");
if ($i === FALSE) die("Invalid request");
$component_name = substr($path, 0, $i);
$path = substr($path, $i+1);

switch ($type) {
case "static":
	header('Cache-Control: public', true);
	header('Pragma: public', true);
	$date = date("D, d M Y H:i:s",time());
	header('Date: '.$date, true);
	$expires = time()+($time == -1 ? 365*24*60*60 : $time);
	header('Expires: '.date("D, d M Y H:i:s",$expires).' GMT', true);
	$i = strrpos($path, ".");
	if ($i === FALSE) die("Invalid resource type");
	$ext = substr($path, $i+1);
	switch ($ext) {
	case "gif": header("Content-Type: image/gif"); break;
	case "png": header("Content-Type: image/png"); break;
	case "jpg": case "jpeg": header("Content-Type: image/jpeg"); break;
	default: die("Invalid resource type");
	}
	readfile("component/".$component_name."/static/".$path);
	die();
case "dynamic":
	// get the type of request
	$i = strpos($path, "/");
	if ($i === FALSE) die("Invalid request");
	$request_type = substr($path, 0, $i);
	$path = substr($path, $i+1);

	require_once("component/PNApplication.inc");
	session_set_cookie_params(24*60*60, "/dynamic/");
	session_start();
	
	global $app;
	if (!isset($_SESSION["app"])) {
		$app = new PNApplication();
		$app->init();
		$_SESSION["app"] = &$app;
	} else
		$app = &$_SESSION["app"];

	switch ($request_type) {
	case "page":
		$app->components[$component_name]->page($path);
		break;
	case "service":
		$app->components[$component_name]->service($path);
		break;
	}
	die();
default: die("Invalid request");
}
?>
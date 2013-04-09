<?php
if (isset($_GET["set_language"])) {
	require_once("component/PNApplication.inc");
	session_set_cookie_params(24*60*60, "/dynamic/");
	session_start();
	$_SESSION["lang"] = $_GET["set_language"];
	header("Location: ?");
	setcookie("lang",$_GET["set_language"],time()+2*365*24*60*60,"/dynamic/");
	die();
}
// check last time the user came, it was the same version, in order to refresh its cache if the version changed
$version = include("version.inc");
if (!isset($_COOKIE["pnversion"]) || $_COOKIE["pnversion"] <> $version) {
	setcookie("pnversion",$version,time()+365*24*60*60,"/");
	header("Location: ?");
	die();
}

if (!isset($_SERVER["PATH_INFO"]) || strlen($_SERVER["PATH_INFO"]) == 0) $_SERVER["PATH_INFO"] = "/";
$path = substr($_SERVER["PATH_INFO"],1);

// security: do not allow .. in the path, to avoid trying to access to files which are protected
if (strpos($path, "..") !== FALSE) die("Access denied");

if ($path == "favicon.ico") { header("Content-Type: image/ico"); readfile("favicon.ico"); die(); }

set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__));

if ($path == "") {
	header("Location: /dynamic/application/page/enter".(isset($_GET["page"])?"?page=".$_GET["page"]:""));
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
	$expires = time()+365*24*60*60;
	header('Expires: '.date("D, d M Y H:i:s",$expires).' GMT', true);
	$i = strrpos($path, ".");
	if ($i === FALSE) die("Invalid resource type");
	$ext = substr($path, $i+1);
	switch ($ext) {
	case "gif": header("Content-Type: image/gif"); break;
	case "png": header("Content-Type: image/png"); break;
	case "jpg": case "jpeg": header("Content-Type: image/jpeg"); break;
	case "css": header("Content-Type: text/css"); break;
	case "js": header("Content-Type: text/javascript"); break;
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

	require_once("common/DataBase.inc");
	require_once("component/PNApplication.inc");
	session_set_cookie_params(24*60*60, "/dynamic/");
	session_start();
	require_once("common/Locale.inc");
	
	global $app;
	if (!isset($_SESSION["app"])) {
		$app = new PNApplication();
		$app->init();
		$_SESSION["app"] = &$app;
	} else
		$app = &$_SESSION["app"];
	PNApplication::$instance = &$app;
	
	if (!isset($app->components[$component_name])) die("Invalid request");

	switch ($request_type) {
	case "page":
		$app->components[$component_name]->page($path, false);
		break;
	case "sub_page":
		$app->components[$component_name]->page($path, true);
		break;
	case "service":
		$app->components[$component_name]->service($path);
		break;
	default: die("Invalid request");
	}
	die();
default: die("Invalid request");
}
?>
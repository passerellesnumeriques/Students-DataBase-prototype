<?php
// check domain and username
if (!isset($_POST["domain"]))
	die("<error message='domain missing'/>");
if (!isset($_POST["username"]))
	die("<error message='username missing'/>");
$domain = $_POST["domain"];
$username = $_POST["username"];

require_once("common/SQLQuery.inc");
$r = SQLQuery::create()->select("Users")->field("username")->where("domain",$domain)->where("username",$username);
if ($r == null || count($r) == 0)
	die("<error message='unknown user'/>");

// retrieve all possible rights
$all_rights = array();
foreach ($this->app->components as $c) {
	foreach ($c->get_readable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
	foreach ($c->get_writable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
}

$rights = array();
foreach ($_POST as $name=>$value) {
	if ($name == "domain" || $name == "username") continue;
	if (!isset($all_rights[$name])) die("<error message=\"unknown right ".$name."\"/>");
	$rights[$name] = $all_rights[$name]->get_right_value($value);
}

// save in database
DataBase::$conn->execute("DELETE FROM UserRights WHERE domain='".$domain."' AND username='".$username."'");
if (count($rights) > 0) {
	$sql = "INSERT INTO UserRights (domain,username,`right`,`value`) VALUES ";
	$first = true;
	foreach ($rights as $name=>$value) {
		if ($first) $first = false; else $sql .= ",";
		$sql .= "('".$domain."','".$username."','".DataBase::$conn->escape_string($name)."','".DataBase::$conn->escape_string($value)."')";
	}
	DataBase::$conn->execute($sql);
}
if (!PNApplication::print_xml_errors())
	echo "<ok/>";
?>
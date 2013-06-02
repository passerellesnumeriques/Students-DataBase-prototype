<?php
// check domain and username
if (!isset($_POST["role_id"]))
	die("<error message='role id missing'/>");
$role_id = $_POST["role_id"];

// check data were locked before
if (!isset($_GET["lock"])) die("<error message='lock missing'/>");
require_once("common/DataBaseLock.inc");
if (!DataBaseLock::check($_GET["lock"], "RoleRights", array("role_id"=>$role_id))) {
	PNApplication::error("You do not have the data locked, meaning you cannot modify them. This may be due to a long inactivity. Please refresh the page and try again");
	return;
}

require_once("common/SQLQuery.inc");
$r = SQLQuery::create()->select("Role")->field("name")->where("id",$role_id);
if ($r == null || count($r) == 0) {
	PNApplication::error("unknown role");
	return;
}

// retrieve all possible rights
$all_rights = array();
foreach ($this->app->components as $c) {
	foreach ($c->get_readable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
	foreach ($c->get_writable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
}

$rights = array();
foreach ($_POST as $name=>$value) {
	if ($name == "role_id") continue;
	if (!isset($all_rights[$name])) {
		PNApplication::error("unknown right ".$name);
		return;
	}
	$rights[$name] = $all_rights[$name]->parse_value($value);
}

// save in database: (1) remove all previous rights, (2) add all rights from the request
DataBase::$conn->execute("DELETE FROM RoleRights WHERE role_id=".$role_id."");
if (count($rights) > 0) {
	$sql = "INSERT INTO RoleRights (role_id,`right`,`value`) VALUES ";
	$first = true;
	foreach ($rights as $name=>$value) {
		if ($first) $first = false; else $sql .= ",";
		$sql .= "('".$role_id."','".DataBase::$conn->escape_string($name)."','".DataBase::$conn->escape_string($value)."')";
	}
	DataBase::$conn->execute($sql);
}
echo "true";
?>
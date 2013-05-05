<?php
$users = array();
for ($i = 0; isset($_POST["user".$i]); $i++) {
	$s = $_POST["user".$i];
	$j = strpos($s, ",");
	array_push($users, array(substr($s, 0, $j), substr($s, $j+1)));
}
$roles = array();
for ($i = 0; isset($_POST["role".$i]); $i++)
	array_push($roles, $_POST["role".$i]);

DataBase::$conn->execute("LOCK TABLES UserRole WRITE");
foreach ($users as $user) {
	$sql = "DELETE FROM UserRole WHERE ";
	$sql .= "`domain`='".DataBase::$conn->escape_string($user[0])."'";
	$sql .= " AND ";
	$sql .= "`username`='".DataBase::$conn->escape_string($user[1])."'";
	$sql .= " AND ";
	$sql .= "`role_id` IN (";
	$first = true;
	foreach ($roles as $role_id) {
		if ($first) $first = false; else $sql .= ",";
		$sql .= $role_id;
	}
	$sql .= ")";
	$res = DataBase::$conn->execute($sql);
}
DataBase::$conn->execute("UNLOCK TABLES");
PNApplication::print_json_result("true");
?>
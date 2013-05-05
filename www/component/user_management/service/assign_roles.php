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
	$sql = "SELECT role_id FROM UserRole WHERE ";
	$sql .= "`domain`='".DataBase::$conn->escape_string($user[0])."'";
	$sql .= " AND ";
	$sql .= "`username`='".DataBase::$conn->escape_string($user[1])."'";
	$res = DataBase::$conn->execute($sql);
	$list = array_merge($roles);
	if ($res)
		while (($r = DataBase::$conn->next_row($res)) <> null)
			for($i = 0; $i < count($list); $i++)
				if ($list[$i] == $r["role_id"]) {
					array_splice($list, $i, 1);
					break;
				}
	if (count($list) == 0) continue;
	foreach ($list as $role_id)
		DataBase::$conn->execute("INSERT INTO UserRole (`domain`,`username`,`role_id`) VALUE ('".DataBase::$conn->escape_string($user[0])."','".DataBase::$conn->escape_string($user[1])."',".$role_id.")");
}
DataBase::$conn->execute("UNLOCK TABLES");
PNApplication::print_json_result("true");
?>
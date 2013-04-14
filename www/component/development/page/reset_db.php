<?php 
require_once("common/DataBaseModel.inc");
$model = PNApplication::$instance->data_model();

$domains = preg_split("/,/", file_get_contents("domains",true));
foreach ($domains as $domain) {
	echo "Initialize DataBase for domain ".$domain."<br/>";
	$res = DataBase::$conn->execute("CREATE DATABASE IF NOT EXISTS students_".$domain);
	$res = DataBase::$conn->execute("USE students_".$domain);
	$res = DataBase::$conn->execute("SHOW TABLES");
	if ($res !== FALSE) {
		while (($table = DataBase::$conn->next_row($res)) !== FALSE) {
			echo " + Table ".$table[0]."<br/>";
			DataBase::$conn->execute("DROP TABLE `".$table[0]."`");
			flush();
		}
		DataBaseModel::update_model($model);
	} else
		echo "Failed to get the list of tables<br/>";
	flush();
}

$local_domain = file_get_contents("local_domain", true);
$res = DataBase::$conn->execute("USE students_".$local_domain);
echo "Insert test data for local domain ".$local_domain."<br/>";

$roles = array(
	"Staff"=>array("consult_user_list"=>true),
	"Student"=>array(),
	"Alumni"=>array(),
	"General Manager"=>array(),
	"Education Manager"=>array(),
	"Educator"=>array(),
	"Training Manager"=>array(),
	"SNA"=>array("edit_user_rights"=>true),
	"Selection Manager"=>array(),
	"Selection Officer"=>array(),
	"External Relations Manager"=>array(),
	"External Relations Officer"=>array(),
	"Finance Manager"=>array(),
);
$users = array(
	array("Helene", "Huard", "F", array("General Manager","Staff")),
	array("Julie", "Tardieu", "F", array("Staff", "Education Manager", "Educator")),
	array("Eduard", "Bucad", "M", array("Staff", "Educator")),
	array("Marian", "Lumapat", "F", array("Staff", "Educator")),
	array("Fatima", "Tiah", "F", array("Staff", "Educator")),
	array("Stanley", "Vasquez", "M", array("Staff", "Educator")),
	array("Kranz", "Serino", "F", array("Staff", "Educator")),
	array("Guillaume", "Le Cousin", "M", array("Staff", "Training Manager", 0)),
	array("Jovanih", "Alburo", "M", array("Staff", "SNA")),
	array("Rosalyn", "Minoza", "F", array("Staff", "Selection Manager")),
	array("Isadora", "Gerona", "F", array("Staff", "Selection Officer")),
	array("Sandy", "De Veyra", "M", array("Staff", "External Relations Manager")),
	array("Ann", "Labra", "F", array("Staff", "External Relations Officer")),
	array("Jeanne", "Salve", "F", array("Staff", "Finance Manager")),
	array("Rhey", "Laurente", "M", array("Alumni")),
	array("X", "Y", "F", array("Student")),
);
// create roles
$roles_id = array();
foreach ($roles as $role_name=>$role_rights) {
	DataBase::$conn->execute("INSERT INTO Role (name) VALUES ('".$role_name."')");
	$roles_id[$role_name] = DataBase::$conn->get_insert_id();
}
// create users
foreach ($users as $user) {
	DataBase::$conn->execute("INSERT INTO People (first_name,last_name,sex) VALUES ('".$user[0]."','".$user[1]."','".$user[2]."')");
	$people_id = DataBase::$conn->get_insert_id();
	$username = str_replace(" ","-", strtolower($user[0]).".".strtolower($user[1]));
	DataBase::$conn->execute("INSERT INTO Users (domain,username) VALUES ('".$local_domain."','".$username."')");
	DataBase::$conn->execute("INSERT INTO UserPeople (domain,username,people) VALUES ('".$local_domain."','".$username."',".$people_id.")");
	foreach ($user[3] as $role) {
		DataBase::$conn->execute("INSERT INTO UserRole (domain,username,role_id) VALUES ('".$local_domain."','".$username."',".($role === 0 ? 0 : $roles_id[$role]).")");
	}
}
// assign rights to roles
foreach ($roles as $role_name=>$role_rights) {
	$role_id = $roles_id[$role_name];
	foreach ($role_rights as $right_name=>$right_value) {
		DataBase::$conn->execute("INSERT INTO RoleRights (`role_id`,`right`,`value`) VALUES ('".$role_id."','".$right_name."','".$right_value."')");
	}
}
echo "<a href='/'>Back to application</a>";

PNApplication::print_errors();
?>
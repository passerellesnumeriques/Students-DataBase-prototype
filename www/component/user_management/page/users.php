<?php 
require_once("common/datalist/DataList.inc");
$list = new DataList("Users");
$list->primary_key("Users.domain","Users.username");
if (!$list->update_from_request()) {
	$list->add("Users.domain", false);
	$list->add("Users.username", false);
	$list->add("UserPeople.people>first_name", false);
	$list->add("UserPeople.people>last_name", false);
	$list->add("UserRole.role_id>name", false);
}

$list->build("<button onclick='popup_page(\"".get_locale("Add user")."\",\"/static/application/add.png\",\"user_management\",\"add_user\");return false'><img src='/static/application/add.png' style='vertical-align:bottom'/>".get_locale("Add user")."</button>");
?>
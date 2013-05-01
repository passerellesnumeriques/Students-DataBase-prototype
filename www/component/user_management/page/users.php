<?php 
require_once("component/data_model/list/DataList.inc");
$list = new DataList("Users");
$list->primary_key("Users.domain","Users.username");
$list->add("Users.domain", false);
$list->add("Users.username", false);
$list->add("UserPeople.people>first_name", false);
$list->add("UserPeople.people>last_name", false);
$list->add("UserRole.role_id>name", false);
$list->selectable();
$list->add_header("<button onclick='pn.popup_page(\"".get_locale("Add user")."\",\"/static/common/images/add.png\",\"user_management\",\"add_user\");return false'><img src='/static/common/images/add.png' style='vertical-align:bottom'/>".get_locale("Add user")."</button>");
if (PNApplication::$instance->user_management->has_right("consult_user_rights"))
	$list->add_item_action("/static/user_management/access_list.png",get_locale("Access Rights"),"user_rights?domain=%Users.domain%&username=%Users.username%");
$list->build($this);
?>
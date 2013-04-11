<?php 
require_once("DataList.inc");
$list = new DataList("Users");
$list->primary_key("Users.domain","Users.username");
$list->add("Users.domain", false);
$list->add("Users.username", false);
$list->add("UserPeople.people>first_name", false);
$list->add("UserPeople.people>last_name", false);
$list->add("UserRole.role_id>name", false);
$list->build($this, "<button onclick='popup_page(\"".get_locale("Add user")."\",\"/static/common/images/add.png\",\"user_management\",\"add_user\");return false'><img src='/static/common/images/add.png' style='vertical-align:bottom'/>".get_locale("Add user")."</button>");
?>
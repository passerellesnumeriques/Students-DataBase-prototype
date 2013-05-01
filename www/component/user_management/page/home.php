<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, "/static/user_management/user_management_32.png", get_locale("User Management"), "users");
$p->add_menu_link('/static/user_management/user_list.png', get_locale("Users list"), "users");
$p->add_menu_link('/static/user_management/role.png', get_locale("Roles"), "roles");
$p->generate();
?>
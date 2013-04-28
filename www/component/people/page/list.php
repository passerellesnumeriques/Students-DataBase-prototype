<?php 
require_once("component/data_list/DataList.inc");
$list = new DataList("People");
$list->add("People.first_name", false);
$list->add("People.last_name", false);
$list->add("People.sex", false);
$list->add("People.birth", false);
$list->add_item_action("/static/people/profile_16.png",get_locale("Profile"),"profile?people=%People.id%");
$list->build($this);
?>
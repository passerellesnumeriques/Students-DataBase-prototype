<?php 
require_once("component/data_list/DataList.inc");
$list = new DataList("People");
$list->primary_key("People.id");
$list->add("People.first_name", false);
$list->add("People.last_name", false);
$list->build($this);
?>
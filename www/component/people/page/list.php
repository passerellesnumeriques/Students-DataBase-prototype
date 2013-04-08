<?php 
require_once("common/datalist/DataList.inc");
$list = new DataList("People");
$list->primary_key("People.id");
if (!$list->update_from_request()) {
	$list->add("People.first_name", false);
	$list->add("People.last_name", false);
}

$list->build();
?>
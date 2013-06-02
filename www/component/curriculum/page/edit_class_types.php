<?php
$curriculum = $_GET["curriculum"];

// lock class types for this curriculum
require_once("common/DataBaseLock.inc");
$locked_by = null;
DataBaseLock::lock("CurriculumClassType", array("curriculum"=>$curriculum), $locked_by);
if ($locked_by <> null) {
	PNApplication::error(get_locale("common", "This page is already locked by")." ".$locked_by);
	return;
}

require_once("component/data_model/page/editable_entity_list.inc");
editable_entity_list($this, "CurriculumClassType", array("curriculum"=>$curriculum));

?>
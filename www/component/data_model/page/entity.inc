<?php
require_once("component/data_model/DataModel.inc");

function data_entity_page_from_key($table, $key, &$page) {
	if (is_string($table)) {
		try { $table = DataModel::get()->getTable($table); }
		catch (Exception $e) {
			echo "Access denied to table ".$table.": ".$e->getMessage();
			return;
		}
	}
	require_once("common/SQLQuery.inc");
	$entity = SQLQuery::create()->select($table->getName())->where($table->getPrimaryKey()->name,$key)->execute_single_row();
	if (!$entity) return;
	data_entity_page($table, $entity, $page);
}

function data_entity_page($table, &$entity, &$page) {
	if (is_string($table)) {
		try { $table = DataModel::get()->getTable($table); }
		catch (Exception $e) {
			echo "Access denied to table ".$table.": ".$e->getMessage();
			return;
		}
	}
	echo "<table>";
	echo "<tr>";
	foreach ($table->getDisplayableData() as $field=>$localized) {
		echo "<td>";
		echo $table->getDisplayableDataName($field);
		echo "</td>";
		echo "<td>";
		echo $entity[$field];
		echo "</td>";
		echo "<td>";
		if ($table->canModifyField($field))
			echo "<img src='/static/common/images/edit.png' onclick=\"data_entity_edit(this,'".$table->getName()."','".$field."','".htmlentities(PNApplication::$instance->data_model->generate_field_type($table->getColumn($field)),ENT_QUOTES,"UTF-8")."','".$entity[$table->getPrimaryKey()->name]."');\" style='cursor:pointer'/>";
		echo "</td>";
		echo "</tr>";
	}
	echo "</table>";
	$page->add_javascript("/static/data_model/datamodel.js");
	$page->add_javascript("/static/data_model/entity.js");
	$page->add_javascript("/static/common/js/field/field_text.js");
	$page->add_javascript("/static/common/js/field/field_icon_link.js");
	$page->add_javascript("/static/common/js/field/field_editable_text.js");
	$page->add_javascript("/static/common/js/field/field_enum.js");
	$page->add_javascript("/static/common/js/field/field_date.js");
}
?>
<?php
$people = $_GET["people"];
if ($people <> PNApplication::$instance->user_people->user_people_id) {
	if (!PNApplication::$instance->user_management->has_right("see_other_people_details")) {
		PNApplication::error(get_locale("common","Access Denied"));
		return;
	}
}
$can_edit = PNApplication::$instance->user_management->has_right("edit_people_details");
require_once("common/SQLQuery.inc");
$people = SQLQuery::create()->select("People")->where("id",$people)->execute_single_row();
?>
<table>
<tr>
<td>
	<img height='150px' src='/static/people/default_<?php if ($people["sex"] == "F") echo "female"; else echo "male";?>.jpg'/>
</td>
<td>
	<?php
	require_once("component/data_model/page/entity.inc");
	data_entity_page("People",$people,$this);
	?>
</td>
</tr>
</table>

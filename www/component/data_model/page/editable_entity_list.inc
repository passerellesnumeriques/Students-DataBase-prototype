<?php
require_once("component/data_model/DataModel.inc");

function editable_entity_list(&$page, $table, $where_fields = null) {

	/* @var $model DataModel */
	$model = DataModel::get();
	$t = $model->getTable($table);
	$q = $t->select();
	if ($where_fields) foreach ($where_fields as $name=>$value) $q->where($name,$value);
	$list = $q->execute();

	echo "<div class='button disabled' id='save_table_".$table."' onclick='save_table_".$table."(this);'><img src='/static/common/images/save.png'/> ".get_locale("common","Save")."</div>";

	$display = $t->getDisplayableDataResolved();
	echo "<table class='table_all_border'><tbody id='table_".$table."'>";
	echo "<tr>";
	foreach ($display as $field=>$locale)
		echo "<th>".$locale[1]."</th>";
	echo "</tr>";
	echo "</tbody></table>";
?>
<script type='text/javascript'>
function table_<?php echo $table?>_add(key,data) {
	var table = document.getElementById('table_<?php echo $table?>');
	var tr = document.createElement('TR'); table.appendChild(tr);
	tr.data = key;
	var field, td, input;
<?php
foreach ($display as $field=>$locale) {
	$col = $t->getColumn($field);
	echo "\tfield = ".$col->create_javascript_field($page, true, "function(input,value){table_".$table."_changed(key,'".$field."',input,value);}","function(input,value){table_".$table."_unchanged(key,'".$field."',input,value);}").";\n";
	echo "\ttd = document.createElement('TD'); tr.appendChild(td);\n";
	echo "\ttd.data = field;\n";
	echo "\tinput = field.create(td,data ? data['".$field."'] : null);\n";
	echo "\tinput.db_field = \"".$field."\";\n";
}
?>
}
var table_<?php echo $table?>_modifications = [];
function table_<?php echo $table?>_changed(key,field,input,value) {
	var found = false;
	for (var i = 0; i < table_<?php echo $table?>_modifications.length; ++i)
		if (table_<?php echo $table?>_modifications[i] == input) { found = true; break; }
	if (!found) table_<?php echo $table?>_modifications.push(input);
	table_<?php echo $table?>_update_save();
	if (key == 0) {
		var td = input.parentNode;
		var tr = td.parentNode;
		var this_tr = tr;
		tr = tr.nextSibling;
		while (tr != null) {
			if (tr.data == 0) {
				var all_empty = true;
				for (var i = 0; i < tr.childNodes.length; ++i) {
					td = tr.childNodes[i];
					var inp = td.childNodes[0];
					if (inp.value) { all_empty = false; break; }
				}
				if (all_empty) break;
			}
			tr = tr.nextSibling;
		}
		if (tr == null && value)
			table_<?php echo $table?>_add(0,null);
		else if (tr != null && !value) {
			for (var i = 0; i < table_<?php echo $table?>_modifications.length; ++i) {
				var inp = table_<?php echo $table?>_modifications[i];
				var inp_td = inp.parentNode;
				var inp_tr = inp_td.parentNode;
				if (inp_tr == tr) {
					table_<?php echo $table?>_modifications.splice(i,1);
					i--;
				}
			}
			tr.parentNode.removeChild(tr);
			this_tr.parentNode.appendChild(this_tr);
			table_<?php echo $table?>_update_save();
		} else if (!value)
			this_tr.parentNode.appendChild(this_tr);
	}
}
function table_<?php echo $table?>_unchanged(key,field,input,value) {
	table_<?php echo $table?>_modifications.remove(input);
	table_<?php echo $table?>_update_save();
}
function save_table_<?php echo $table?>(button) {
	if (button.className == "button disabled") return;
	var data = {};
	data["table"] = "<?php echo $table?>";
	<?php
	if ($where_fields) {
		$i = 0;
		foreach ($where_fields as $name=>$value) {
			echo "data[\"where".$i."_name\"] = ".json_encode($name).";";
			echo "data[\"where".$i."_value\"] = ".json_encode($value).";";
			$i++;
		}
	}
	?>
	var row_index = 0;
	var table = document.getElementById('table_<?php echo $table?>');
	for (var row = 0; row < table.childNodes.length; ++row) {
		var tr = table.childNodes[row];
		var changes = [];
		for (var col = 0; col < tr.childNodes.length; ++col) {
			var td = tr.childNodes[col];
			var field = td.data;
			if (!field) continue;
			var input = td.childNodes[0];
			if (typeof field.hasChanged != 'undefined' && field.hasChanged(input) && (tr.data != 0 || field.getValue(input)))
				changes.push({field:input.db_field,value:field.getValue(input)});
		}
		if (changes.length == 0) continue;
		data["row"+row_index+"_key"] = tr.data;
		for (var i = 0; i < changes.length; ++i) {
			data["row"+row_index+"_change"+i+"_field"] = changes[i].field;
			data["row"+row_index+"_change"+i+"_value"] = changes[i].value;
		}
		row_index++;
	}
	var screen_lock = lock_screen(null,"<span><img src='/static/common/images/saving.gif' style='vertical-align:bottom'/> <?php locale("common","Saving")?></span>");
	ajax.post_parse_result("/dynamic/data_model/service/save_list",data,function(result) {
		if (!result)
			unlock_screen(screen_lock);
		else
			location.reload();
	});
}
function table_<?php echo $table?>_update_save() {
	var button = document.getElementById('save_table_<?php echo $table?>');
	button.className = "button"+(table_<?php echo $table?>_modifications.length > 0 ? "" : " disabled");
}
<?php
foreach ($list as $e) {
	echo "table_".$table."_add('".$e[$t->getPrimaryKey()->name]."',{";
	$first = true;
	foreach ($t->getDisplayableData() as $field=>$locale) {
		if ($first) $first = false; else echo ",";
		echo $field.":".json_encode($e[$field]);
	}
	echo "});\n";
}
?>
table_<?php echo $table?>_add(0,null);
</script>
<?php

}
?>
<?php 
$page->add_stylesheet("/static/data_list/datalist.css");
$page->add_stylesheet("/static/common/dhtmlx/skins/dhtmlxaccordion_dhx_skyblue.css");
$page->add_stylesheet("/static/common/dhtmlx/dhtmlxgrid.css");
$page->add_stylesheet("/static/common/dhtmlx/skins/dhtmlxgrid_dhx_skyblue.css");
$page->add_javascript("/static/data_list/datalist.js");
$page->add_javascript("/static/common/dhtmlx/dhtmlxcommon.js");
$page->add_javascript("/static/common/dhtmlx/dhtmlxcontainer.js");
$page->add_javascript("/static/common/dhtmlx/dhtmlxaccordion.js");
$page->add_javascript("/static/common/dhtmlx/dhtmlxgrid.js");
$page->add_javascript("/static/common/dhtmlx/dhtmlxgridcell.js");
$page->onload("datalist_init_fields();datalist_refresh();");
?>
<table style='width:100%;height:100%' cellspacing=0 cellpadding=0 id='datalist_container'>
<tr id='datalist_header'>
	<td>
		<input type='checkbox' checked='checked' onchange='var e = document.getElementById("datalist_refresh"); if (!this.checked) { e.style.visibility = "visible"; e.style.position = "static"; } else { e.style.visibility = "hidden"; e.style.position = "absolute"; }'/> <?php locale("data_list", "Auto refresh")?>
		<button id='datalist_refresh' onclick='datalist_refresh();return false;' disabled='disabled' style='position:absolute;visibility:hidden'><img src='/static/common/images/refresh.png' style='vertical-align:bottom'/><?php locale("data_list", "Refresh")?></button>
	</td>
	<td></td>
	<td>
		<span style='border-right:1px solid #C0C0C0;padding-right:3px' id='datalist_numbers'></span>
		<span id='datalist_header'><?php if ($header<>null) echo $header;?></span>
	</td>
</tr>
<tr>
	<td valign=top width=25% id='datalist_fields_area'>
		<div style='width:100%;height:100%;overflow:auto' id='datalist_fields_area_div'>
			<div id='datalist_fields_accordion' style='width:100%;'></div>
			<table width=100% cellspacing=0 cellpadding=0 id='datalist_table_visible' title="<?php locale("data_list", "Visible Fields");?>">
				<tbody id='datalist_visible_fields'></tbody>
			</table>
			<table width='100%' cellspacing=0 cellpadding=0 id='datalist_table_available' title="<?php locale("data_list", "Available Fields");?>">
				<tbody id='datalist_avail_fields'></tbody>
			</table>
		</div>
	</td>
	<td class='datalist_separator' onmousedown='datalist_start_resize(event);return false;'>&nbsp;</td>
	<td class='set_height' valign=top align=left id='datalist_data_area'>
			<div style='width:100%;height:100%' id='datalist_data'>
				<img src='/static/common/images/loading.gif'/>
			</div>
	</td>
</tr>
</table>
<script type='text/javascript'>
function DataList_FieldName(path, category, name, type) {
	this.path = path;
	this.category = category;
	this.name = name;
	this.type = type;
}
var datalist_fields = [
<?php 
// get all possible fields
$all_fields = $this->find_all_possible_paths();
$first = true;
foreach ($all_fields as $path) {
	$name = $this->model->getDataCategoryAndName($path->table, $path->field);
	$type = 'unknown';
	$col = $this->model->tables[$path->table]->getColumn($path->field);
	if ($col instanceof \data\ColumnString) $type = 'string:'.$col->max_length;
	else if ($col instanceof \data\ColumnInteger) $type = 'integer';
	else if ($col instanceof \data\ColumnEnum) {
		$type = 'enum[';
		$first = true;
		foreach ($col->values as $v) {
			if ($first) $first = false; else $type .= ",";
			$type .= json_encode($v);
		}
		$type .= "]";
	}
	if ($first) { $first = false; echo " "; } else echo ",";
	echo "new DataList_FieldName(".
		json_encode($path->get_path()).",".
		json_encode($name[0]).",".
		json_encode($name[1]).",".
		json_encode($type).
	")\n";
}
?>
];
var datalist_visible_fields = [<?php 
$first = true;
foreach ($this->columns as $col) {
	if ($first) $first = false; else echo ",";
	echo json_encode($col->get_path());
}
?>];
var datalist_table = <?php echo json_encode($this->starting_table);?>;
var datalist_primary_keys = [
<?php
$first = true;
foreach ($this->primary_key as $pk) {
	if ($first) $first = false; else echo ",";
	echo json_encode($pk); 
}
?>
];
var datalist_search = {};
<?php 
foreach ($this->search as $field=>$value)
	echo "datalist_search[".$this->encode_field($field)."] = ".json_encode($value).";\n";
?>
</script>
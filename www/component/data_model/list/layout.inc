<?php
$page->add_javascript("/static/data_model/datamodel.js");
$page->add_stylesheet("/static/data_model/datalist.css");
$page->add_javascript("/static/data_model/datalist.js");
$page->add_stylesheet("/static/common/js/component/context_menu.css");
$page->add_javascript("/static/common/js/component/context_menu.js");
$page->add_stylesheet("/static/common/js/component/splitter_vertical.css");
$page->add_javascript("/static/common/js/component/splitter_vertical.js");
$page->add_stylesheet("/static/common/js/component/collapsable_section.css");
$page->add_javascript("/static/common/js/component/collapsable_section.js");
$page->add_stylesheet("/static/common/js/component/grid.css");
$page->add_javascript("/static/common/js/component/grid.js");
$page->add_javascript("/static/common/js/field/field_text.js");
$page->add_javascript("/static/common/js/field/field_icon_link.js");
$page->add_javascript("/static/common/js/field/field_editable_text.js");
$page->add_javascript("/static/common/js/field/field_enum.js");
$page->add_javascript("/static/common/js/field/field_date.js");
$page->onload("new splitter_vertical('datalist_container',0.25);datalist_init_fields();datalist_refresh();");
?>
<div style='width:100%;height:100%' id='datalist_container'>
	<div>
		<table width=100% height=100% cellspacing=0 cellpadding=0>
			<tr class='datalist_header'><td>
				<input type='checkbox' checked='checked' onchange='var e = document.getElementById("datalist_refresh"); if (!this.checked) { e.style.visibility = "visible"; e.style.position = "static"; } else { e.style.visibility = "hidden"; e.style.position = "absolute"; }'/> <?php locale("data_model", "Auto refresh")?>
				<button id='datalist_refresh' onclick='datalist_refresh();return false;' disabled='disabled' style='position:absolute;visibility:hidden'><img src='/static/common/images/refresh.png' style='vertical-align:bottom'/><?php locale("data_model", "Refresh")?></button>
			</td></tr>
			<tr><td valign=top>
				<div style='width:100%;height:100%;overflow:auto' id='datalist_fields_area_div'>
					<div class='collapsable_section' id='datalist_visible_section'>
						<div class='collapsable_section_header' style='padding-left:2px'>
							<img src='/static/common/images/visible.png' style='vertical-align:bottom'/>
							<?php locale("data_model", "Visible Fields");?>
						</div>
						<table class='collapsable_section_content' width=100% cellspacing=0 cellpadding=0>
							<tbody id='datalist_visible_fields'></tbody>
						</table>
					</div>
					<div class='collapsable_section' id='datalist_available_section'>
						<div class='collapsable_section_header' style='padding-left:2px'>
							<img src='/static/common/images/list.png' style='vertical-align:bottom'/>
							<?php locale("data_model", "Available Fields");?>
						</div>
						<table class='collapsable_section_content' width=100% cellspacing=0 cellpadding=0>
							<tbody id='datalist_avail_fields'></tbody>
						</table>
					</div>
				</div>
			</td></tr>
		</table>
	</div>
	<div>
		<table width=100% height=100% cellspacing=0 cellpadding=0>
			<tr class='datalist_header'><td nowrap='nowrap'>
				<table style='width:100%;height:100%;border:0px;margin:0px;padding:0px;border-collapse:collapse;'><tr>
				<td style="width:100%;vertical-align:middle;padding:0px 1px 0px 1px;">
					<span id='datalist_selection_actions' style='visitility:hidden;position:absolute;top:-10000px;border-right:1px solid #C0C0C0'>
						<?php foreach ($this->selection_actions as $a) echo $a?>
					</span>
					<span id='datalist_save' style='visitility:hidden;position:absolute;top:-10000px;border-right:1px solid #C0C0C0'>
						<button onclick='datalist_save();'><img src='/static/common/images/save.png' style='vertical-align:bottom'/> <?php locale("common","Save")?></button>
					</span>
					<span id='datalist_header'><?php foreach ($this->headers as $header) echo $header;?></span>
				</td>
				<td style="vertical-align:middle;border-left:1px solid #C0C0C0;padding:0px 1px 0px 1px;">
					<img src='/static/common/images/export.png' class='button' style='vertical-align:middle' onclick='new context_menu("datalist_export_menu").showBelowElement(this);' title="<?php locale("common","Export")?>"/>
					<div id='datalist_export_menu' class='context_menu'>
						<div class='context_menu_title'><?php locale("data_model","Export Format")?></div>
						<div class='context_menu_item' onclick="datalist_export('excel2007');"><img src='/static/common/images/files/excel_16.png'/> Excel 2007 (.xlsx)</div>
						<div class='context_menu_item' onclick="datalist_export('excel5');"><img src='/static/common/images/files/excel_16.png'/> Excel 5 (.xls)</div>
						<div class='context_menu_item' onclick="datalist_export('pdf');"><img src='/static/common/images/files/pdf_16.png'/> PDF</div>
						<div class='context_menu_item' onclick="datalist_export('csv');"><img src='/static/common/images/files/csv.gif'/> CSV</div>
					</div>
				</td>
				<td style="vertical-align:middle;border-left:1px solid #C0C0C0;padding:0px 1px 0px 1px;">
					<span id='datalist_total_entries'></span> <?php locale("data_model","entries")?>,
					<?php locale("data_model","display")?> <span id='datalist_start_entry'></span> - <span id='datalist_end_entry'></span>
				</td>
				</tr></table>
			</td></tr>
			<tr><td valign=top>
				<div id='datalist_data' style='width:100%;height:100%;overflow:auto'></div>
			</td></tr>
		</table>
	</div>
</div>
<script type='text/javascript'>
var datalist_fields = [
<?php
// get all possible fields
$all_fields = $this->find_all_possible_paths();
$first = true;
foreach ($all_fields as $path) {
	$name = $this->model->getTable($path->table)->getDisplayableDataCategoryAndName($path->field);
	$col = $this->model->getTable($path->table)->getColumn($path->field);
	$type = PNApplication::$instance->data_model->generate_field_type($col);
	if ($first) { $first = false; echo " "; } else echo ",";
	echo "new DataList_FieldName(".
		json_encode($path->get_path()).",".
		json_encode($name[0]).",".
		json_encode($name[1]).",".
		json_encode($type).",".
		($path->is_unique() && $col->table->canModifyField($col->name) ? "true" : "false").",".
		"false".
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
var datalist_item_actions = [
<?php
$first = true;
foreach ($this->item_actions as $action)
	echo "\t";
	if ($first) $first = false; else echo ",";
	echo "{icon:".json_encode($action[0]).",text:".json_encode($action[1]).",url:".json_encode($action[2])."}\n";
?>
];
var datalist_selectable = <?php echo $this->selectable ? "true" : "false"?>;
var datalist_sort_asc = [];
var datalist_sort_desc = [];
var datalist_sortable = [];
<?php
foreach ($this->columns as $col)
	if ($col->sort !== null)
		echo "datalist_sort_".($col->sort ? "asc" : "desc").".push(".json_encode($col->get_path()).");";
foreach ($all_fields as $path)
	if ($path->is_unique())
		echo "datalist_sortable.push(".json_encode($path->get_path()).");";
?>
</script>
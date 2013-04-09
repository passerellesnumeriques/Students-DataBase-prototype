<script type='text/javascript'>
load_css('/data_list/datalist.css');
load_css('/common/dhtmlx/dhtmlx.css');
var datalist_loaded = 0;
load_js('/data_list/datalist.js',function() {
	datalist_init_fields();
	datalist_loaded++;
	if (datalist_loaded == 2)
		datalist_refresh();
});
load_js('/common/dhtmlx/dhtmlx.js',function() {
	load_js('/common/dhtmlx/dhtmlxgrid_mcol.js');
	datalist_loaded++;
	if (datalist_loaded == 2)
		datalist_refresh();
});
</script><table style='width:100%;height:100%' cellspacing=0 cellpadding=0 id='datalist_container'>
<tr id='datalist_header'>
	<td>
		<input type='checkbox' checked='checked' onchange='var e = document.getElementById("datalist_refresh"); if (!this.checked) { e.style.visibility = "visible"; e.style.position = "static"; } else { e.style.visibility = "hidden"; e.style.position = "absolute"; }'/> <?php locale("data_list", "Auto refresh")?>
		<button id='datalist_refresh' onclick='datalist_refresh();return false;' disabled='disabled' style='position:absolute;visibility:hidden'><img src='/static/common/images/refresh.png' style='vertical-align:bottom'/><?php locale("data_list", "Refresh")?></button>
	</td>
	<td></td>
	<td>
		<span style='border-right:1px solid #C0C0C0;padding-right:3px' id='datalist_numbers'></span>
		<span id='datalist_header'></span>
	</td>
</tr>
<tr>
	<td valign=top width=25% id='datalist_fields_area'>
		<div style='width:100%;height:100%;overflow:auto' id='datalist_fields_area_div'>
			<div class='datalist_fields_section'>
				<div class='datalist_fields_section_title'>
					<img src='/static/common/images/collapse.gif' style='border:0px;cursor:pointer' onclick='datalist_section_collapse_expand(this);'/>
					<?php locale("data_list", "Visible Fields");?>
				</div>
				<div class='datalist_fields_section_content' style='visibility:visible'>
					<table width=100% cellspacing=0 cellpadding=0>
						<tbody id='datalist_visible_fields'></tbody>
					</table>
				</div>
			</div>
			<div class='datalist_fields_section'>
				<div class='datalist_fields_section_title'>
					<img src='/static/common/images/collapse.gif' style='border:0px;cursor:pointer' onclick='datalist_section_collapse_expand(this);'/>
					<?php locale("data_list", "Available Fields");?>
				</div>
				<div class='datalist_fields_section_content' style='visibility:visible'>
					<table width='100%' cellspacing=0 cellpadding=0>
						<tbody id='datalist_avail_fields'></tbody>
					</table>
				</div>
			</div>
		</div>
	</td>
	<td class='datalist_separator' onmousedown='datalist_start_resize(event);return false;'></td>
	<td class='set_height' valign=top align=left style='padding-left:2px;padding-right:2px' id='datalist_data_area'>
			<div style='width:100%;height:100%' id='datalist_data'>
				<img src='/static/common/images/loading.gif'/>
			</div>
	</td>
</tr>
</table>
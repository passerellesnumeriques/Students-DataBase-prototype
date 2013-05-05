var datalist_data = null;
var datalist_data_keys = null;
var datalist_grid = null;
function DataList_FieldName(path, category, name, type, editable, edited) {
	this.path = path;
	this.category = category;
	this.name = name;
	this.type = type;
	this.editable = editable;
	this.edited = edited;
}

function datalist_init_fields() {
	// set as collapsable sections
	new collapsable_section("datalist_visible_section");
	new collapsable_section("datalist_available_section");
	// fill tables with visible and available fields
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		_datalist_add_visible_field(datalist_visible_fields[i]);
	for (var i = 0; i < datalist_fields.length; ++i) {
		if (!datalist_visible_fields.contains(datalist_fields[i].path))
			_datalist_add_available_field(datalist_fields[i]);
	}
}
var datalist_fields_changed = [];
function datalist_init_grid() {
	if (datalist_grid == null)
		datalist_grid = new grid('datalist_data');
	else
		datalist_grid.reset();
	datalist_grid.startLoading();
	if (datalist_selectable) {
		datalist_grid.setSelectable(true);
		datalist_grid.onselect = datalist_on_selection;
	}
	datalist_fields_changed = [];
	var button = document.getElementById('datalist_save');
	button.style.visibility = 'hidden';
	button.style.position = 'absolute';
	for (var i = 0; i < datalist_visible_fields.length; ++i) {
		var field = _datalist_get_field(datalist_visible_fields[i]);
		if (field == null) continue;
		var type = datamodel_create_field(field.type, field.edited,function(input){
			_datalist_field_changed(input);
		},function(input){
			_datalist_field_unchanged(input);
		});
		var col_title = field.name;
		if (datalist_sort_asc.contains(field.path))
			col_title += " <img src='/static/common/images/arrow_up_10.gif'/>";
		else if (datalist_sort_desc.contains(field.path))
			col_title += " <img src='/static/common/images/arrow_down_10.gif'/>";
		var sort_func = null;
		if (datalist_sortable.contains(field.path))
			sort_func = function(index,field) {
				if (datalist_sort_asc.contains(field.path)) {
					datalist_sort_asc.remove(field.path);
					datalist_sort_desc.push(field.path);
					datalist_grid.setColumnTitle(index, field.name+" <img src='/static/common/images/arrow_down_10.gif'/>");
					datalist_enable_refresh();
				} else if (datalist_sort_desc.contains(field.path)) {
					datalist_sort_desc.remove(field.path);
					datalist_grid.setColumnTitle(index, field.name);
					datalist_enable_refresh();
				} else {
					datalist_sort_asc.push(field.path);
					datalist_grid.setColumnTitle(index, field.name+" <img src='/static/common/images/arrow_up_10.gif'/>");
					datalist_enable_refresh();
				}
			};
		datalist_grid.addColumn(col_title,null,type,field,sort_func);
	}
	for (var i = 0; i < datalist_item_actions.length; ++i) {
		datalist_grid.addColumn("",null,new field_icon_link());
	}
}

function datalist_form_data() {
	var data = {};
	data.starting_table = datalist_table;
	for (var i = 0; i < datalist_primary_keys.length; ++i)
		data["pk"+i] = datalist_primary_keys[i];
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		data["field_"+i] = datalist_visible_fields[i];
	for (var field in datalist_search)
		data["search_"+field] = datalist_search[field];
	for (var i = 0; i < datalist_sort_asc.length; ++i)
		data["sort_asc_"+i] = datalist_sort_asc[i];
	for (var i = 0; i < datalist_sort_desc.length; ++i)
		data["sort_desc_"+i] = datalist_sort_desc[i];
	return data;
}
function datalist_refresh() {
	datalist_init_grid(); // reset columns
	var data = datalist_form_data();
	pn.ajax_service_json("/dynamic/data_model/service/get_data", data, function(result) {
		if (result != null) {
			document.getElementById('datalist_total_entries').innerHTML = result.total;
			document.getElementById('datalist_start_entry').innerHTML = result.data.length == 0 ? 0 : result.start+1;
			document.getElementById('datalist_end_entry').innerHTML = result.data.length == 0 ? 0 : result.start+result.data.length;
			datalist_data = [];
			datalist_data_keys = [];
			for (var i = 0; i < result.data.length; ++i) {
				datalist_data_keys.push(result.data[i].k);
				datalist_data.push(result.data[i].v);
			}
        	if (datalist_item_actions.length > 0) {
        		for (var i = 0; i < datalist_data.length; ++i) {
        			for (var j = 0; j < datalist_item_actions.length; ++j) {
        				var action = datalist_item_actions[j];
        				var url = action.url;
        				for (var k = 0; k < datalist_visible_fields.length; ++k)
        					url = url.replace("%"+datalist_visible_fields[k]+"%", datalist_data[i][k]);
        				for (var k = 0; k < datalist_primary_keys.length; ++k)
        					url = url.replace("%"+datalist_primary_keys[k]+"%", datalist_data_keys[i][k]);
        				datalist_data[i].push({
        					icon: action.icon,
        					text: action.text,
        					url: url,
        					target: "_self"
        				});
        			}
        		}
        	}
        	_datalist_reset_data();
		}
	});
	var button = document.getElementById("datalist_refresh");
	button.disabled = "disabled";
}
function _datalist_reset_data() {
	datalist_grid.setData(datalist_data);
	datalist_grid.endLoading();
}

function getElementIndex(element) {
	var i = 0;
	while (element.previousSibling) {
		i++;
		element = element.previousSibling;
	}
	return i;
}
function _datalist_field_changed(element) {
	var button = document.getElementById('datalist_save');
	button.style.visibility = 'visible';
	button.style.position = 'static';
	var td = element.parentNode;
	var tr = td.parentNode;
	var i = getElementIndex(tr);
	var j = getElementIndex(td);
	if (!datalist_fields_changed[i])
		datalist_fields_changed[i] = [];
	datalist_fields_changed[i][j] = element;
}
function _datalist_field_unchanged(element) {
	var td = element.parentNode;
	var tr = td.parentNode;
	var i = getElementIndex(tr);
	var j = getElementIndex(td);
	datalist_fields_changed[i][j] = null;
	var has_change = false;
	for (var i = 0; i < datalist_fields_changed.length; ++i) {
		if (has_change) break;
		if (!datalist_fields_changed[i]) continue;
		for (var j = 0; j < datalist_fields_changed[i].length; ++j)
			if (datalist_fields_changed[i][j] != null) {
				has_change = true;
				break;
			}
	}
	if (!has_change) {
		var button = document.getElementById('datalist_save');
		button.style.visibility = 'hidden';
		button.style.position = 'absolute';
	}
}
function datalist_save() {
	var button = document.getElementById('datalist_save');
	button.style.visibility = 'hidden';
	button.style.position = 'absolute';
	datalist_grid.startLoading();
	// get the starting table
	var data = "starting_table="+encodeURIComponent(datalist_table);
	// get the primary keys, together with their position in the grid
	var pk_cols = [];
	for (var i = 0; i < datalist_primary_keys.length; ++i) {
		data += "&"+encodeURIComponent("pk"+i)+"="+encodeURIComponent(datalist_primary_keys[i]);
		for (var j = 0; j < datalist_visible_fields.length; ++j) {
			if (datalist_visible_fields[j] == datalist_primary_keys[i]) {
				pk_cols[i] = j;
				break;
			}
		}
	}
	// go through every row in the grid, to find modified data
	var modifications = [];
	for (var row = 0; row < datalist_data.length; ++row) {
		var pks = [];
		var changes = [];
		for (var i = 0; i < datalist_grid.getNbColumns(); ++i) {
			var field = datalist_grid.getColumnField(i);
			if (!field.isEditable()) continue;
			var input = datalist_grid.getCellContent(row, i);
			if (!field.hasChanged(input)) continue;
			changes.push({field:datalist_visible_fields[i],value:field.getValue(input)});
		}
		if (changes.length > 0)
			modifications.push({pks:datalist_data_keys[row],changes:changes});
	}
	for (var i = 0; i < modifications.length; ++i) {
		for (var j = 0; j < modifications[i].pks.length; ++j)
			data += "&"+encodeURIComponent("mod_"+i+"_pk_"+j)+"="+encodeURIComponent(modifications[i].pks[j]);
		for (var j = 0; j < modifications[i].changes.length; ++j) {
			data += "&"+encodeURIComponent("mod_"+i+"_change_"+j+"_field")+"="+encodeURIComponent(modifications[i].changes[j].field);
			data += "&"+encodeURIComponent("mod_"+i+"_change_"+j+"_value")+"="+encodeURIComponent(modifications[i].changes[j].value);
		}
	}
	pn.ajax_service_json("/dynamic/data_model/service/save_data", data, function(result) {
		datalist_refresh();
	});
}

function datalist_enable_refresh() {
	var button = document.getElementById("datalist_refresh");
	if (button.style.visibility == "hidden")
		datalist_refresh();
	else
		button.disabled = "";
}

function datalist_remove_field(icon,field_path) {
	datalist_visible_fields.remove(field_path);
	var row = icon.parentNode.parentNode;
	row.parentNode.removeChild(row);
	_datalist_add_available_field(_datalist_get_field(field_path));
	_datalist_refresh_up_down();
	datalist_enable_refresh();
}
function datalist_add_field(icon,field_path) {
	datalist_visible_fields.push(field_path);
	_datalist_add_visible_field(field_path);
	// remove the field from the list of available fields
	table = document.getElementById("datalist_avail_fields");
	row = icon.parentNode.parentNode;
	var index = 0;
	while (index < table.childNodes.length && table.childNodes[index] != row) index++;
	if (table.childNodes[index-1].className == "datalist_fields_category") {
		// previous row is the category name: check if there is still something in the category
		if (index >= table.childNodes.length-1 || table.childNodes[index+1].className == "datalist_fields_category") {
			table.removeChild(table.childNodes[index-1]);
		}
	}
	table.removeChild(row);
	datalist_enable_refresh();
}
function datalist_field_up(icon, field_path) {
	var oldInd;
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		if (datalist_visible_fields[i] == field_path) {
			oldInd = i;
			break;
		}
	datalist_visible_fields.splice(oldInd, 1);
	datalist_visible_fields.splice(oldInd-1, 0, field_path);
	// update order in the displayed list
	var row = icon.parentNode.parentNode;
	row.parentNode.insertBefore(row, row.previousSibling);
	// update icons
	_datalist_refresh_up_down();
	// reset grid
	datalist_init_grid();
	// update data
	for (var i = 0; i < datalist_data.length; ++i) {
		var tmp = datalist_data[i][oldInd]; 
		datalist_data[i].splice(oldInd, 1);
		datalist_data[i].splice(oldInd-1, 0, tmp);
	}
	_datalist_reset_data();
}
function datalist_field_down(icon, field_path) {
	var oldInd;
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		if (datalist_visible_fields[i] == field_path) {
			oldInd = i;
			break;
		}
	datalist_visible_fields.splice(oldInd, 1);
	datalist_visible_fields.splice(oldInd+1, 0, field_path);
	// update order in the displayed list
	var row = icon.parentNode.parentNode;
	row.parentNode.insertBefore(row.nextSibling, row);
	// update icons
	_datalist_refresh_up_down();
	// reset grid
	datalist_init_grid();
	// update data
	for (var i = 0; i < datalist_data.length; ++i) {
		var tmp = datalist_data[i][oldInd]; 
		datalist_data[i].splice(oldInd, 1);
		datalist_data[i].splice(oldInd+1, 0, tmp);
	}
	_datalist_reset_data();
}
function datalist_edit_field(icon, field_path) {
	pn.lock_screen();
	var data = datalist_form_data();
	data += "&lock_field="+encodeURIComponent(field_path);
	pn.ajax_service_xml("/dynamic/data_model/service/lock", data, function(xml) {
		if (!xml) {
			pn.unlock_screen();
			return;
		}
		icon.src = "/static/common/images/no_edit.png";
		icon.onclick = function() { datalist_noedit_field(this, field_path); };
		var field = _datalist_get_field(field_path);
		field.edited = true;
		field.lock = xml.getAttribute("lock");
		window.pn_database_locks.add_lock(field.lock);
		datalist_init_grid();
		_datalist_reset_data();
		pn.unlock_screen();
	});
}
function datalist_noedit_field(icon, field_path) {
	pn.lock_screen();
	var field = _datalist_get_field(field_path);
	var data = datalist_form_data();
	data += "&lock="+field.lock;
	pn.ajax_service_xml("/dynamic/data_model/service/unlock", data, function(xml) {
		if (!xml) {
			pn.unlock_screen();
			return;
		}
		window.pn_database_locks.remove_lock(field.lock);
		icon.src = "/static/common/images/edit.png";
		icon.onclick = function() { datalist_edit_field(this, field_path); };
		field.edited = false;
		field.lock = null;
		datalist_init_grid();
		_datalist_reset_data();
		pn.unlock_screen();
	});
}
function datalist_add_search(icon, field_path) {
	var td = icon.parentNode;
	icon.onclick = null;
	_datalist_add_search_input(td, field_path);
	//datalist_enable_refresh();
}
function _datalist_add_search_input(td, field_path) {
	var field = _datalist_get_field(field_path);
	var update = function(input, value) {
		if (datalist_search[_datalist_encode_field(field_path)] != value) {
			datalist_search[_datalist_encode_field(field_path)] = value;
			datalist_enable_refresh();
		}
		if (value == "" && input.parentNode) {
			var p = input.parentNode;
			p.removeChild(input);
			for (var i = 0; i < p.childNodes.length; ++i)
				if (p.childNodes[i].nodeName == "IMG") {
					p.childNodes[i].onclick = function() { datalist_add_search(this, field_path); };
					break;
				}
		}
	};
	var typed_field = datamodel_create_field(field.type, true, update, update);
	var input = typed_field.create(td, datalist_search[_datalist_encode_field(field_path)]);
	input.focus();
}
function _datalist_encode_field(field_path) {
	return field_path.replace(".","__dot__").replace(">","__ind__");
}
function _datalist_get_field(field_path) {
	for (var i = 0; i < datalist_fields.length; ++i)
		if (datalist_fields[i].path == field_path)
			return datalist_fields[i];
	return null;
}
function _datalist_add_visible_field(field_path) {
	var field = _datalist_get_field(field_path);
	if (field == null) return;
	var has_editable = false;
	for (var i = 0; i < datalist_visible_fields.length; ++i) {
		var f = _datalist_get_field(datalist_visible_fields[i]);
		if (!f) continue;
		has_editable |= f.editable;
	}
	var table = document.getElementById("datalist_visible_fields");
	var row = document.createElement("TR");
	row.style.height = "20px";
	row.data = field_path;
	var td = document.createElement("TD");
	//td.noWrap = "nowrap";
	var code = field.category+" / "+field.name;
	code += " <img src='/static/common/images/search.png' style='cursor:pointer;vertical-align:bottom;padding-right:2px'";
	var search = datalist_search[_datalist_encode_field(field_path)];
	if (!search)
		code += " onclick=\"datalist_add_search(this,'"+field_path+"')\"";
	code += "/>";
	td.innerHTML = code;
	if (search) _datalist_add_search_input(td, field_path);
	row.appendChild(td);
	td = document.createElement("TD");
	td.noWrap = "nowrap";
	var code = "";
	if (table.childNodes.length > 0)
		code += "<img src='/static/common/images/up.png' style='cursor:pointer' onclick='datalist_field_up(this,\""+field_path+"\")' class='_icon_up'/>";
	else
		code += "<img src='/static/common/images/transparent_16.png' class='_icon_up'/>";
	code += "<img src='/static/common/images/transparent_16.png' class='_icon_down'/>";
	var is_primary = false;
	for (var i = 0; i < datalist_primary_keys.length; ++i)
		if (datalist_primary_keys[i] == field_path) { is_primary = true; break; }
	if (!is_primary)
		code += "<img src='/static/common/images/invisible.png' style='cursor:pointer' onclick='datalist_remove_field(this,\""+field_path+"\")' class='_icon_invisible'/>";
	else
		code += "<img src='/static/common/images/transparent_16.png' class='_icon_invisible'/>";
	if (field.editable) {
		if (field.edited)
			code += "<img src='/static/common/images/no_edit.png' style='cursor:pointer' onclick='datalist_noedit_field(this,\""+field_path+"\")' class='_icon_edit'/>";
		else
			code += "<img src='/static/common/images/edit.png' style='cursor:pointer' onclick='datalist_edit_field(this,\""+field_path+"\")' class='_icon_edit'/>";
	} else if (has_editable)
		code += "<img src='/static/common/images/transparent_16.png' class='_icon_edit'/>";
	td.innerHTML = code;
	td.align = "right";
	row.appendChild(td);
	row.className = "datalist_fields";
	if (table.childNodes.length == 0) {
		table.appendChild(row);
	} else {
		var last_row = table.childNodes[table.childNodes.length-1];
		table.appendChild(row);
	}
	_datalist_refresh_up_down();
}
function _datalist_refresh_up_down() {
	var table = document.getElementById("datalist_visible_fields");
	for (var ir = 0; ir < table.childNodes.length; ++ir) {
		var row = table.childNodes[ir];
		var td = row.childNodes[1];
		for (var i = 0; i < td.childNodes.length; ++i) {
			var img = td.childNodes[i];
			if (img.className == "_icon_up") {
				if (ir == 0 && img.style.cursor == "pointer") {
					img.src = "/static/common/images/transparent_16.png";
					img.style.cursor = null;
					img.onclick = null;
				} else if (ir > 0 && img.style.cursor != "pointer") {
					img.src = "/static/common/images/up.png";
					img.style.cursor = "pointer";
					img.onclick = function() { datalist_field_up(this, this.parentNode.parentNode.data); };
				}
			} else if (img.className == "_icon_down") {
				if (ir == table.childNodes.length-1 && img.style.cursor == "pointer") {
					img.src = "/static/common/images/transparent_16.png";
					img.style.cursor = null;
					img.onclick = null;
				} else if (ir < table.childNodes.length-1 && img.style.cursor != "pointer") {
					img.src = "/static/common/images/down.png";
					img.style.cursor = "pointer";
					img.onclick = function() { datalist_field_down(this, this.parentNode.parentNode.data); };
				}
			}
		}
	}
}
function _datalist_add_available_field(field) {
	// search category
	var table = document.getElementById("datalist_avail_fields");
	var cat_row = null;
	for (i = 0; i < table.childNodes.length; ++i) {
		var tr = table.childNodes[i];
		if (tr.className != "datalist_fields_category") continue;
		if (tr.childNodes[0].innerHTML != field.category) continue;
		cat_row = tr;
		break;
	}
	if (cat_row == null) {
		cat_row = document.createElement("TR");
		cat_row.style.height = "20px";
		var td = document.createElement("TD");
		td.colSpan = 2;
		td.noWrap = "nowrap";
		td.innerHTML = field.category;
		cat_row.appendChild(td);
		cat_row.className = "datalist_fields_category";
		table.appendChild(cat_row);
	}
	var tr = document.createElement("TR");
	tr.style.height = "20px";
	var td = document.createElement("TD");
	td.noWrap = "nowrap";
	td.style.paddingLeft = "10px";
	td.innerHTML = field.name;
	tr.appendChild(td);
	td = document.createElement("TD");
	td.noWrap = "nowrap";
	td.align = "right";
	td.innerHTML = "<img src='/static/common/images/visible.png' style='cursor:pointer' onclick='datalist_add_field(this,\""+field.path+"\");'/>";
	tr.appendChild(td);
	if (cat_row.nextSibling)
		table.insertBefore(tr, cat_row.nextSibling);
	else
		table.appendChild(tr);
}

function datalist_on_selection(selection) {
	var span = document.getElementById('datalist_selection_actions');
	if (selection.length == 0) {
		span.style.visibility = 'hidden';
		span.style.position = 'absolute';
		span.style.top = "-10000px";
	} else {
		span.style.visibility = 'visible';
		span.style.position = 'static';
	}
}

function datalist_export(format) {
	var form = document.createElement("FORM");
	var data = datalist_form_data();
	for (var name in data) {
		var input = document.createElement("INPUT");
		input.type = 'text';
		input.name = name;
		input.value = data[name];
		form.appendChild(input);
	}
	form.method = "POST";
	form.action = "/dynamic/data_model/service/export?format=xml&export="+format;
	form.style.visibility = 'hidden';
	form.style.position = 'absolute';
	form.style.top = '-10000px';
	document.body.appendChild(form);
	form.submit();
}
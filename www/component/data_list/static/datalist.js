var datalist_data = null;
function datalist_refresh() {
	datalist_init_grid();
	var data = "starting_table="+encodeURIComponent(datalist_table);
	for (var i = 0; i < datalist_primary_keys.length; ++i)
		data += "&"+encodeURIComponent("pk"+i)+"="+encodeURIComponent(datalist_primary_keys[i]);
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		data += "&"+encodeURIComponent("field_"+i)+"="+encodeURIComponent(datalist_visible_fields[i]);
	for (var field in datalist_search)
		data += "&"+encodeURIComponent("search_"+field)+"="+encodeURIComponent(datalist_search[field]);
	var xhr = new XMLHttpRequest();
	xhr.open("POST", "/dynamic/data_list/service/get_data", true);
	xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
	xhr.onreadystatechange = function() {
        if (this.readyState === 4 && this.status === 200) {
            //alert(this.responseText);
        	datalist_data = eval(this.responseText);
        	_datalist_reset_data();
        }
    };
    xhr.send(data);
	var button = document.getElementById("datalist_refresh");
	button.disabled = "disabled";
}
function _datalist_reset_data() {
	datalist_grid.parse(datalist_data, "jsarray");
	var nb = datalist_grid.getColumnsNum();
	for (var i = 0; i < nb; ++i) {
		datalist_grid.adjustColumnSize(i);
	//	//if (grid.getColWidth(i)
	}
	document.getElementById("datalist_data").style.width = "100%";
}
function datalist_enable_refresh() {
	var button = document.getElementById("datalist_refresh");
	if (button.style.visibility == "hidden")
		datalist_refresh();
	else
		button.disabled = "";
}
var datalist_accordion = null;
function datalist_init_fields() {
	datalist_accordion = new dhtmlXAccordion("datalist_fields_accordion");
	datalist_accordion.setIconsPath("/static/common/dhtmlx/imgs/");
	datalist_accordion.multiMode = 1;
	var table = document.getElementById("datalist_table_visible");
	datalist_accordion.addItem("visible",table.title); table.title = "";
	table = document.getElementById("datalist_table_available");
	datalist_accordion.addItem("avail",table.title); table.title = "";
	datalist_accordion.cells("visible").attachObject("datalist_table_visible");
	datalist_accordion.cells("avail").attachObject("datalist_table_available");
	for (var i = 0; i < datalist_visible_fields.length; ++i)
		_datalist_add_visible_field(datalist_visible_fields[i]);
	for (var i = 0; i < datalist_fields.length; ++i) {
		if (!datalist_visible_fields.contains(datalist_fields[i].path))
			_datalist_add_available_field(datalist_fields[i]);
	}
	_datalist_reset_accordion_size();
	datalist_accordion.attachEvent("onActive", _datalist_reset_accordion_size);
}
function _datalist_reset_accordion_size() {
	if (!datalist_accordion) return;
	datalist_accordion.setSizes();
	var table = document.getElementById("datalist_table_visible");
	table.parentNode.style.height = "";
	table.parentNode.parentNode.style.height = "";
	var item = table.parentNode.parentNode.parentNode; 
	item.style.height = (item.childNodes[0].scrollHeight+item.childNodes[1].scrollHeight)+"px";
	table = document.getElementById("datalist_table_available");
	table.parentNode.style.height = "";
	table.parentNode.parentNode.style.height = "";
	var item = table.parentNode.parentNode.parentNode; 
	item.style.height = (item.childNodes[0].scrollHeight+item.childNodes[1].scrollHeight+1)+"px";
}
var datalist_grid = null;
function datalist_init_grid() {
	datalist_grid = new dhtmlXGridObject('datalist_data');
	var header = "";
	var types = "";
	for (var i = 0; i < datalist_visible_fields.length; ++i) {
		var field = _datalist_get_field(datalist_visible_fields[i]);
		if (header.length>0) { header+=","; types += ","; }
		header += field.name;
		types += "ro";
	}
	datalist_grid.setHeader(header);
	datalist_grid.setColTypes(types);
	datalist_grid.setImagePath("/static/common/dhtmlx/imgs/");
	datalist_grid.setSkin("dhx_skyblue");
	datalist_grid.init();
	document.getElementById("datalist_data").style.width = "100%";
}
function datalist_remove_field(icon,field_path) {
	datalist_visible_fields.remove(field_path);
	var row = icon.parentNode.parentNode;
	row.parentNode.removeChild(row);
	_datalist_add_available_field(_datalist_get_field(field_path));
	_datalist_refresh_up_down();
	_datalist_reset_accordion_size();
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
	_datalist_reset_accordion_size();
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
function datalist_add_search(icon, field_path) {
	var td = icon.parentNode;
	icon.onclick = null;
	_datalist_add_search_input(td, field_path);
	//datalist_enable_refresh();
}
function _datalist_add_search_input(td, field_path) {
	var field = _datalist_get_field(field_path);
	var input = _datalist_create_input(field.type, datalist_search[_datalist_encode_field(field_path)]);
	var update = function() {
		if (datalist_search[_datalist_encode_field(field_path)] != input.value) {
			datalist_search[_datalist_encode_field(field_path)] = input.value;
			datalist_enable_refresh();
		}
	};
	input.onblur = update; 
	var prev = input.onkeyup;
	input.onkeyup = function(ev) {
		if (prev) if (!prev(ev)) return false;
		setTimeout(update,1);
	}
	input.onchange = update;
	td.appendChild(input);
	input.focus();
}
function _datalist_create_input(type, value) {
	if (type.startsWith("string:")) {
		var max = parseInt(type.substring(7));
		var input = document.createElement('INPUT');
		input.style.padding = "0px";
		input.style.margin = "0px";
		input.type = 'text';
		input.size = max>10?10:max;
		input.maxLength = max;
		if (value)
			input.value = value;
	} else if (type.startsWith("enum[")) {
		var values = eval(type.substring(4));
		var input = document.createElement("SELECT");
		var sel = 0;
		var o = document.createElement("OPTION");
		o.value = "";
		o.innerHTML = "";
		input.add(o);
		for (var i = 0; i < values.length; ++i) {
			o = document.createElement("OPTION");
			o.value = values[i];
			o.innerHTML = values[i];
			if (value && value == values[i]) sel = i+1;
			input.add(o);
		}
		input.selectedIndex = sel;
	}
	return input;
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
	var table = document.getElementById("datalist_visible_fields");
	var row = document.createElement("TR");
	row.style.height = "20px";
	row.data = field_path;
	var td = document.createElement("TD");
	td.noWrap = "nowrap";
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
	td.innerHTML = code;
	td.align = "right";
	row.appendChild(td);
	if (table.childNodes.length == 0) {
		row.className = "datalist_fields_0";
		table.appendChild(row);
	} else {
		var last_row = table.childNodes[table.childNodes.length-1];
		if (last_row.className == "datalist_fields_0")
			row.className = "datalist_fields_1";
		else
			row.className = "datalist_fields_0";
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
var datalist_resize_pos = 0;
function datalist_start_resize(event) {
	if (!event) event = window.event;
	datalist_resize_pos = event.clientX;
	window.onmouseup = function() {
		window.onmousemove = null;
	};
	window.onblur = function() {
		window.onmousemove = null;
	};
	window.onmousemove = function(ev) {
		if (!ev) ev = window.event;
		var diff = ev.clientX - datalist_resize_pos;
		if (diff == 0) return;
		datalist_resize_pos = ev.clientX;
		var table = document.getElementById("datalist_container");
		var td = document.getElementById("datalist_fields_area");
		var div = document.getElementById("datalist_fields_area_div");
		var w = td.scrollWidth;
		w += diff;
		if (w < 10) w = 10;
		if (w > table.scrollWidth-10) w = table.scrollWidth-10;
		td.width = w+"px";
		td.style.width = w+"px";
		div.width = w+"px";
		div.style.width = w+"px";
		document.getElementById("datalist_data").style.width = "100%";
		_datalist_reset_accordion_size();
	}
}
listenEvent(window, 'resize', function() {
	var td = document.getElementById("datalist_fields_area");
	var div = document.getElementById("datalist_fields_area_div");
	var w = td.scrollWidth;
	td.width = w+"px";
	td.style.width = w+"px";
	div.width = w+"px";
	div.style.width = w+"px";
	document.getElementById("datalist_data").style.width = "100%";
	_datalist_reset_accordion_size();
});
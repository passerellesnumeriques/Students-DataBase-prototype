function datalist_section_collapse_expand(img) {
	var div = img.parentNode.parentNode;
	for (var i = 0; i < div.childNodes.length; ++i) {
		if (div.childNodes[i].nodeName == "DIV" && div.childNodes[i].className == "datalist_fields_section_content") {
			div = div.childNodes[i];
			break;
		}
	}
	if (div.style.visibility == "visible") {
		div.style.visibility = "hidden";
		div.style.position = "absolute";
		img.src = "/static/common/images/expand.gif";
	} else {
		div.style.visibility = "visible";
		div.style.position = "static";
		img.src = "/static/common/images/collapse.gif";
	}
}
function datalist_refresh() {
	datalist_init_grid();
	var form = document.forms["datalist"];
	var data = "";
	for (var i = 0; i < form.elements.length; ++i) {
		var e = form.elements[i];
		if (i > 0) data += "&";
		data += encodeURIComponent(e.name) + "=" + encodeURIComponent(e.value);
	}
	var xhr = new XMLHttpRequest();
	xhr.open("POST", "/dynamic/data_list/service/get_data", true);
	xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
	xhr.onreadystatechange = function() {
        if (this.readyState === 4 && this.status === 200) {
            //alert(this.responseText);
        	datalist_grid.parse(eval(this.responseText), "jsarray");
        	var nb = datalist_grid.getColumnsNum();
        	for (var i = 0; i < nb; ++i) {
        		datalist_grid.adjustColumnSize(i);
        		//if (grid.getColWidth(i)
        	}
        }
    };
    xhr.send(data);
	var button = document.getElementById("datalist_refresh");
	button.disabled = "disabled";
}
function datalist_enable_refresh() {
	var button = document.getElementById("datalist_refresh");
	if (button.style.visibility == "hidden")
		datalist_refresh();
	else
		button.disabled = "";
}
function datalist_init_fields() {
	var form = document.forms["datalist"];
	var visible = [];
	var i = 0;
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		_datalist_add_visible_field(input.value);
		visible.push(input.value);
		i++;
	}
	form = document.forms["datalist_names"];
	i = 0;
	while (form.elements["field_"+i]) {
		var field = form.elements["field_"+i].value;
		if (!visible.contains(field))
			_datalist_add_available_field(field);
		i++;
	}
}
var datalist_grid = null;
function datalist_init_grid() {
	datalist_grid = new dhtmlXGridObject('datalist_data');
	var form = document.forms["datalist"];
	var header = "";
	var types = "";
	var i = 0;
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		var info = _datalist_get_field_info(input.value);
		if (header.length>0) { header+=","; types += ","; }
		header += info[1];
		types += "ro";
		i++;
	}
	datalist_grid.setHeader(header);
	datalist_grid.setColTypes(types);
	datalist_grid.setImagePath("/static/common/dhtmlx/imgs/");
	datalist_grid.init();
}
function datalist_remove_field(icon,field) {
	// remove the field from the form, update indexes
	var form = document.forms["datalist"];
	var i = 0;
	while (form.elements["field_"+i]) {
		var input = document.forms["datalist"].elements["field_"+i];
		if (input.value == field) {
			// found in the list of fields
			input.parentNode.removeChild(input);
			var row = icon.parentNode.parentNode;
			row.parentNode.removeChild(row);
			datalist_enable_refresh();
			// update following indexes
			i++;
			while (form.elements["field_"+i]) {
				form.elements["field_"+i].name = "field_"+(i-1);
				i++;
			}
			// create the row in the list of available fields
			_datalist_add_available_field(field);
			return;
		}
		i++;
	}
	// not found
	alert('Error: field not found: '+field);
}
function datalist_add_field(icon,field) {
	// create the field in the form
	var form = document.forms["datalist"];
	var i = 0;
	while (form.elements["field_"+i]) i++;
	var input = document.createElement("INPUT");
	input.type = "hidden";
	input.name = "field_"+i;
	input.value = field;
	form.appendChild(input);
	// create the field in the list of visible fields
	_datalist_add_visible_field(field);
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
function datalist_field_up(icon, field) {
	// update order in datalist form
	var form = document.forms["datalist"];
	var i = 0;
	var oldInd;
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		if (input.value == field) {
			input.name = "_datalist_temp";
			form.elements["field_"+(i-1)].name = "field_"+i;
			input.name = "field_"+(i-1);
			oldInd = i;
			break;
		}
		i++;
	}
	// update order in the displayed list
	var row = icon.parentNode.parentNode;
	row.parentNode.insertBefore(row, row.previousSibling);
	// add the up icon to the previous, if needed
	if (row.previousSibling == null) {
		var td = row.nextSibling.childNodes[1];
		for (i = 0; i < td.childNodes.length; ++i) {
			var c = td.childNodes[i];
			if (c.className == "_icon_up") break;
			if (c.className == "_icon_down" || c.className == "_icon_invisible") {
				var img = document.createElement("IMG");
				img.src = "/static/common/images/up.png";
				img.style.cursor = "pointer";
				img.className = "_icon_up";
				img.onclick = function() { datalist_field_up(this, this.parentNode.parentNode.data); };
				td.insertBefore(img, c);
				break;
			}
		}
	}
	// add the down icon if needed
	for (i = 0; i < icon.parentNode.childNodes.length; ++i) {
		var c = icon.parentNode.childNodes[i];
		if (c.className == "_icon_down") break;
		if (c.className == "_icon_invisible") {
			var img = document.createElement("IMG");
			img.src = "/static/common/images/down.png";
			img.style.cursor = "pointer";
			img.className = "_icon_down";
			img.onclick = function() { datalist_field_down(this, field); };
			icon.parentNode.insertBefore(img, c);
			break;
		}
	}
	// remove the up icon if we are at first position
	if (row.previousSibling == null)
		icon.parentNode.removeChild(icon);
	// move in the grid
	datalist_grid.moveColumn(oldInd,oldInd-1);
}
function datalist_field_down(icon, field) {
	// update order in datalist form
	var form = document.forms["datalist"];
	var i = 0;
	var oldInd;
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		if (input.value == field) {
			input.name = "_datalist_temp";
			form.elements["field_"+(i+1)].name = "field_"+i;
			input.name = "field_"+(i+1);
			oldInd = i;
			break;
		}
		i++;
	}
	// update order in the displayed list
	var row = icon.parentNode.parentNode;
	row.parentNode.insertBefore(row.nextSibling, row);
	// add the down icon to the next, if needed
	if (row.nextSibling == null) {
		var td = row.previousSibling.childNodes[1];
		for (i = 0; i < td.childNodes.length; ++i) {
			var c = td.childNodes[i];
			if (c.className == "_icon_down") break;
			if (c.className == "_icon_invisible") {
				var img = document.createElement("IMG");
				img.src = "/static/common/images/down.png";
				img.style.cursor = "pointer";
				img.className = "_icon_down";
				img.onclick = function() { datalist_field_down(this, this.parentNode.parentNode.data); };
				td.insertBefore(img, c);
				break;
			}
		}
	}
	// add the up icon if needed
	for (i = 0; i < icon.parentNode.childNodes.length; ++i) {
		var c = icon.parentNode.childNodes[i];
		if (c.className == "_icon_up") break;
		if (c.className == "_icon_down" || c.className == "_icon_invisible") {
			var img = document.createElement("IMG");
			img.src = "/static/common/images/up.png";
			img.style.cursor = "pointer";
			img.className = "_icon_up";
			img.onclick = function() { datalist_field_up(this, field); };
			icon.parentNode.insertBefore(img, c);
			break;
		}
	}
	// remove the down icon if we are at last position
	if (row.nextSibling == null)
		icon.parentNode.removeChild(icon);
	// move in the grid
	datalist_grid.moveColumn(oldInd+1,oldInd);
}
function datalist_add_search(icon, field) {
	var td = icon.parentNode;
	icon.onclick = null;
	_datalist_add_search_input(td, field);
	//datalist_enable_refresh();
}
function _datalist_add_search_input(td, field) {
	var info = _datalist_get_field_info(field);
	var form = document.forms["datalist"];
	var i = form.elements["search_"+_datalist_encode_field(field)];
	var input = _datalist_create_input(info[2], i ? i.value : null);
	var update = function() {
		var form = document.forms["datalist"];
		if (!form.elements["search_"+_datalist_encode_field(field)]) {
			var i = document.createElement("INPUT");
			i.type = 'hidden';
			i.name = 'search_'+_datalist_encode_field(field);
			form.appendChild(i);
		}
		if (form.elements["search_"+_datalist_encode_field(field)].value != input.value) {
			form.elements["search_"+_datalist_encode_field(field)].value = input.value;
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
function _datalist_encode_field(field) {
	return field.replace(".","__dot__").replace(">","__ind__");
}
function _datalist_get_field_info(field) {
	// search category name and field name
	var f = document.forms["datalist_names"];
	i = 0;
	var cat_name = "";
	var field_name = "";
	var type = "";
	while (f.elements["field_"+i]) {
		if (f.elements["field_"+i].value == field) {
			cat_name = f.elements["field_"+i+"_category"].value;
			field_name = f.elements["field_"+i+"_name"].value;
			type = f.elements["field_"+i+"_type"].value;
			break;
		}
		i++;
	}
	return [cat_name,field_name,type];
}
function _datalist_add_visible_field(field) {
	var a = _datalist_get_field_info(field);
	var cat_name = a[0];
	var field_name = a[1];
	var table = document.getElementById("datalist_visible_fields");
	var row = document.createElement("TR");
	row.data = field;
	var td = document.createElement("TD");
	td.noWrap = "nowrap";
	var code = cat_name+" / "+field_name;
	code += " <img src='/static/common/images/search.png' style='cursor:pointer;vertical-align:bottom;padding-right:2px'";
	var search = document.forms["datalist"].elements["search_"+_datalist_encode_field(field)];
	if (!search)
		code += " onclick=\"datalist_add_search(this,'"+field+"')\"";
	code += "/>";
	td.innerHTML = code;
	if (search) _datalist_add_search_input(td, field);
	row.appendChild(td);
	td = document.createElement("TD");
	td.noWrap = "nowrap";
	var code = "";
	if (table.childNodes.length > 0)
		code += "<img src='/static/common/images/up.png' style='cursor:pointer' onclick='datalist_field_up(this,\""+field+"\")' class='_icon_up'/>";
	code += "<img src='/static/common/images/invisible.png' style='cursor:pointer' onclick='datalist_remove_field(this,\""+field+"\")' class='_icon_invisible'/>";
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
	while (row.previousSibling) {
		row = row.previousSibling;
		td = row.childNodes[1];
		for (var i = 0; i < td.childNodes.length; ++i) {
			if (td.childNodes[i].className == "_icon_down") break;
			if (td.childNodes[i].className == "_icon_invisible") {
				var img = document.createElement("IMG");
				img.src = "/static/common/images/down.png";
				img.style.cursor = "pointer";
				img.className = "_icon_down";
				img.onclick = function() { datalist_field_down(this, field); };
				td.insertBefore(img, td.childNodes[i]);
				break;
			}
		}
	}
}
function _datalist_add_available_field(field) {
	var a = _datalist_get_field_info(field);
	var cat_name = a[0];
	var field_name = a[1];
	// search category
	var table = document.getElementById("datalist_avail_fields");
	var cat_row = null;
	for (i = 0; i < table.childNodes.length; ++i) {
		var tr = table.childNodes[i];
		if (tr.className != "datalist_fields_category") continue;
		if (tr.childNodes[0].innerHTML != cat_name) continue;
		cat_row = tr;
		break;
	}
	if (cat_row == null) {
		cat_row = document.createElement("TR");
		var td = document.createElement("TD");
		td.colSpan = 2;
		td.noWrap = "nowrap";
		td.innerHTML = cat_name;
		cat_row.appendChild(td);
		cat_row.className = "datalist_fields_category";
		table.appendChild(cat_row);
	}
	var tr = document.createElement("TR");
	var td = document.createElement("TD");
	td.noWrap = "nowrap";
	td.style.paddingLeft = "10px";
	td.innerHTML = field_name;
	tr.appendChild(td);
	td = document.createElement("TD");
	td.noWrap = "nowrap";
	td.align = "right";
	td.innerHTML = "<img src='/static/common/images/visible.png' style='cursor:pointer' onclick='datalist_add_field(this,\""+field+"\",\""+cat_name+"\",\""+field_name+"\");'/>";
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
	}
}
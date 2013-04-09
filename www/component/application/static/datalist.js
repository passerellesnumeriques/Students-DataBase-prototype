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
		img.src = "/static/application/expand.gif";
	} else {
		div.style.visibility = "visible";
		div.style.position = "static";
		img.src = "/static/application/collapse.gif";
	}
}
function datalist_refresh() {
	document.forms["datalist"].submit();
}
function datalist_enable_refresh() {
	var button = document.getElementById("datalist_refresh");
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
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		if (input.value == field) {
			input.name = "_datalist_temp";
			form.elements["field_"+(i-1)].name = "field_"+i;
			input.name = "field_"+(i-1);
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
				img.src = "/static/application/up.png";
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
			img.src = "/static/application/down.png";
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
	datalist_enable_refresh();
}
function datalist_field_down(icon, field) {
	// update order in datalist form
	var form = document.forms["datalist"];
	var i = 0;
	while (form.elements["field_"+i]) {
		var input = form.elements["field_"+i];
		if (input.value == field) {
			input.name = "_datalist_temp";
			form.elements["field_"+(i+1)].name = "field_"+i;
			input.name = "field_"+(i+1);
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
				img.src = "/static/application/down.png";
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
			img.src = "/static/application/up.png";
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
	datalist_enable_refresh();
}
function datalist_add_search(icon, field) {
	var td = icon.parentNode;
	icon.onclick = null;
	_datalist_add_search_input(td, field);
	datalist_enable_refresh();
}
function _datalist_add_search_input(td, field) {
	var info = _datalist_get_field_info(field);
	// TODO depending on type
	var input = document.createElement('INPUT');
	input.style.padding = "0px";
	input.style.margin = "0px";
	input.type = 'text';
	input.size = 10;
	var form = document.forms["datalist"];
	var i = form.elements["search_"+_datalist_encode_field(field)];
	if (i)
		input.value = i.value;
	input.onblur = function() {
		var form = document.forms["datalist"];
		if (!form.elements["search_"+_datalist_encode_field(field)]) {
			var i = document.createElement("INPUT");
			i.type = 'hidden';
			i.name = 'search_'+_datalist_encode_field(field);
			form.appendChild(i);
		}
		if (form.elements["search_"+_datalist_encode_field(field)].value != this.value)
			datalist_enable_refresh();
		form.elements["search_"+_datalist_encode_field(field)].value = this.value;
	};
	var prev = input.onkeydown;
	input.onkeydown = function(ev) {
		if (prev) if (!prev(ev)) return false;
		datalist_enable_refresh();
	}
	td.appendChild(input);
	input.focus();
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
	return [cat_name,field_name];
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
	code += " <img src='/static/application/search.png' style='cursor:pointer;vertical-align:bottom;padding-right:2px'";
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
		code += "<img src='/static/application/up.png' style='cursor:pointer' onclick='datalist_field_up(this,\""+field+"\")' class='_icon_up'/>";
	code += "<img src='/static/application/invisible.png' style='cursor:pointer' onclick='datalist_remove_field(this,\""+field+"\")' class='_icon_invisible'/>";
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
				img.src = "/static/application/down.png";
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
	td.innerHTML = "<img src='/static/application/visible.png' style='cursor:pointer' onclick='datalist_add_field(this,\""+field+"\",\""+cat_name+"\",\""+field_name+"\");'/>";
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
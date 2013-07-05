function geography_import_adm_wizard(db, geonames, gadm, parent) {
	add_javascript("/static/common/js/configuration.js", function() {
		add_javascript("/static/common/js/popup_window/popup_window.js", function() {
			var w = new giw_wizard(db, geonames, gadm, parent);
		});
	});
}
function giw_wizard(db, geonames, gadm, parent) {
	var t = this;
	if (parent == 0) {
		t.geonames_level = 1;
		t.gadm_level = 1;
		t.geonames_parent = 0;
		t.gadm_parent = 0;
	} else {
		var match = db.get_geography_match(parent);
		if (match.geonames) {
			t.geonames_level = match.geonames.level+1;
			t.geonames_parent = match.geonames.parent;
		}
		if (match.gadm) {
			t.gadm_level = match.gadm.level+1;
			t.gadm_parent = match.gadm.parent;
		}
	}
	var p = new popup_window("Geography Import","/static/common/images/import.png","");
	var table = document.createElement("TABLE");
	var tbody = document.createElement("TBODY");
	table.appendChild(tbody); table = tbody;
	table.style.border = '1px solid black';
	table.rules = "all";
	var tr,td;
	table.appendChild(tr = document.createElement("TR"));
	tr.appendChild(td = document.createElement("TD"));
	td.innerHTML = "Geonames: ";
	td.appendChild(t.select_geonames_level = document.createElement("SELECT"));
	for (var i = 1; i <= 5; ++i) {
		var o = document.createElement("OPTION");
		o.text = "Level "+i;
		o.value = i;
		t.select_geonames_level.add(o);
	}
	t.select_geonames_level.selectedIndex = t.geonames_level-1;
	t.select_geonames_level.onchange = function() {
		t.geonames_level = this.selectedIndex+1;
		t.build();
	};
	tr.appendChild(td = document.createElement("TD"));
	td.innerHTML = "GADM: ";
	td.appendChild(t.select_gadm_level = document.createElement("SELECT"));
	for (var i = 1; i <= 5; ++i) {
		var o = document.createElement("OPTION");
		o.text = "Level "+i;
		o.value = i;
		t.select_gadm_level.add(o);
	}
	t.select_gadm_level.selectedIndex = t.gadm_level-1;
	t.select_gadm_level.onchange = function() {
		t.gadm_level = this.selectedIndex+1;
		t.build();
	}
	t.tr_levels = tr;
	p.setContent(table);
	p.show();
	t.build = function() {
		while (t.tr_levels.nextSibling)
			table.removeChild(t.tr_levels.nextSibling);
		var list1 = geonames.get_level(t.geonames_level, t.geonames_parent);
		var list2 = gadm.get_level(t.gadm_level, t.gadm_parent);
		
		var tr, td;
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TH"));
		td.colSpan = 2;
		td.innerHTML = "Matches found between Geonames and GADM";
		var match1 = [], match2 = [];
		for (var i = 0; i < list1.length; ++i) {
			var e1 = list1[i];
			var e2 = giw_get_match(e1, list2, match2);
			if (e2 != null) {
				match1.push(e1);
				match2.push(e2);
				giw_add_row(table, list1, e1, list2, e2);
			}
		}
		
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TD"));
		td.colSpan = 2;
		var button = document.createElement("BUTTON");
		td.appendChild(button);
		button.innerHTML = "Import matching divisions";
		button.onclick = function() {
			for (var i = 0; i < table.childNodes.length; ++i) {
				var tr = table.childNodes[i];
				if (!tr._match_row) continue;
				var sel1 = tr.childNodes[0].childNodes[0];
				var sel2 = tr.childNodes[1].childNodes[0];
				// TODO db.import();
			}
		};
		var tr_last_match = tr;
		
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TH"));
		td.colSpan = 2;
		td.innerHTML = "Not matched";
		table.appendChild(tr = document.createElement("TR"));
		var tr_radios = tr;
		tr.appendChild(td = document.createElement("TD"));
		td.style.verticalAlign = "top";
		for (var i = 0; i < list1.length; ++i) {
			if (!match1.contains(list1[i])) {
				var radio = document.createElement("INPUT");
				radio.type = 'radio';
				radio.name = 'list1';
				radio._data = list1[i];
				td.appendChild(radio);
				td.appendChild(document.createTextNode(list1[i].name));
				td.appendChild(document.createElement("BR"));
			}
		}
		tr.appendChild(td = document.createElement("TD"));
		td.style.verticalAlign = "top";
		for (var i = 0; i < list2.length; ++i) {
			if (!match2.contains(list2[i])) {
				var radio = document.createElement("INPUT");
				radio.type = 'radio';
				radio.name = 'list2';
				radio._data = list2[i];
				td.appendChild(radio);
				td.appendChild(document.createTextNode(list2[i].name));
				td.appendChild(document.createElement("BR"));
			}
		}
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TD"));
		td.colSpan = 2;
		button = document.createElement("BUTTON");
		td.appendChild(button);
		button.innerHTML = "Add match";
		button.onclick = function() {
			var td = tr_radios.childNodes[0];
			var value1 = null;
			var radio1, radio2;
			for (var i = 0; i < td.childNodes.length; ++i) {
				var e = td.childNodes[i];
				if (e.nodeType != 1) continue;
				if (e.nodeName != "INPUT") continue;
				if (!e.checked) continue;
				value1 = e._data;
				radio1 = e;
				break;
			}
			if (value1 == null) return;
			td = tr_radios.childNodes[1];
			var value2 = null;
			for (var i = 0; i < td.childNodes.length; ++i) {
				var e = td.childNodes[i];
				if (e.nodeType != 1) continue;
				if (e.nodeName != "INPUT") continue;
				if (!e.checked) continue;
				value2 = e._data;
				radio2 = e;
				break;
			}
			if (value2 == null) return;
			var tr = document.createElement("TR"); table.insertBefore(tr, tr_last_match);
			tr._match_row = true;
			giw_create_select(tr, list1, value1);
			giw_create_select(tr, list2, value2);
			var parent = radio1.parentNode;
			parent.removeChild(radio1.nextSibling);
			parent.removeChild(radio1.nextSibling);
			parent.removeChild(radio1);
			parent = radio2.parentNode;
			parent.removeChild(radio2.nextSibling);
			parent.removeChild(radio2.nextSibling);
			parent.removeChild(radio2);
		};
		
		p.resize();
	};
	t.build();
}
function giw_get_match(e1, list, matched) {
	for (var i = 0; i < list.length; ++i) {
		var e2 = list[i];
		if (matched.contains(e2)) continue;
		if (e1.name.toLowerCase().indexOf(e2.name.toLowerCase()) != -1)
			return e2;
		if (e2.name.toLowerCase().indexOf(e1.name.toLowerCase()) != -1)
			return e2;
	}
	return null;
}
function giw_add_row(table, list1, sel1, list2, sel2) {
	var tr = document.createElement("TR"); table.appendChild(tr);
	tr._match_row = true;
	giw_create_select(tr, list1, sel1);
	giw_create_select(tr, list2, sel2);
}
function giw_create_select(tr, list, sel) {
	var td = document.createElement("TD"); tr.appendChild(td);
	var select = document.createElement("SELECT");
	var sel_i = -1;
	var o = document.createElement("OPTION");
	select.add(o);
	for (var i = 0; i < list.length; ++i) {
		o = document.createElement("OPTION");
		o.text = list[i].name;
		if (list[i] == sel) sel_i = i;
		select.add(o);
	}
	td.appendChild(select);
	if (sel_i != -1)
		select.selectedIndex = sel_i+1;
}
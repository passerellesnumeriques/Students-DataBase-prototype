if (typeof add_javascript != 'undefined') {
	add_javascript("/static/common/js/popup_window/popup_window.js");
	add_stylesheet("/static/common/js/popup_window/popup_window.css");
}
function import_adm_wizard(db_level, db_id) {
	var t=this;
	
	if (db_level == 1) {
		t.geonames_parent = 0;
		t.geonames_level = 1;
		t.gadm_parent = 0;
		t.gadm_level = 1;
	} else {
		var match = db_matched.get(db_id);
		t.geonames_parent = match.geonames.parent;
		t.geonames_level = match.geonames.level;
		t.gadm_parent = match.geonames.parent;
		t.gadm_level = match.geonames.level;
	}
	
	t.create_db_table = function() {
		var table = document.createElement("TABLE");
		table.style.border = "1px solid black";
		table.rules = "all";
		t.tbody = document.createElement("TBODY"); table.appendChild(t.tbody);
		var tr,td;
		t.tbody.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TH"));
		td.innerHTML = "Geonames";
		tr.appendChild(td = document.createElement("TH"));
		td.innerHTML = "GADM";
		tr.appendChild(td = document.createElement("TH")); // actions

		t.tbody.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TD"));
		t.select_geonames_level = document.createElement("SELECT");
		td.innerHTML = "Level ";
		td.appendChild(t.select_geonames_level);
		for (var i = 1; i <= 5; ++i) {
			var o = document.createElement("OPTION");
			o.value = i;
			o.text = i;
			t.select_geonames_level.add(o);
		}
		t.select_geonames_level.selectedIndex = t.geonames_level-1;
		t.select_geonames_level.onchange = function() { t.update_all(); };
		tr.appendChild(td = document.createElement("TD"));
		t.select_gadm_level = document.createElement("SELECT");
		td.innerHTML = "Level ";
		td.appendChild(t.select_gadm_level);
		for (var i = 1; i <= 5; ++i) {
			var o = document.createElement("OPTION");
			o.value = i;
			o.text = i;
			t.select_gadm_level.add(o);
		}
		t.select_gadm_level.selectedIndex = t.gadm_level-1;
		t.select_gadm_level.onchange = function() { t.update_all(); };
		tr.appendChild(td = document.createElement("TD")); // actions

		t.tbody.appendChild(t.tr_match = document.createElement("TR"));
		t.tr_match.appendChild(td = document.createElement("TH"));
		td.colSpan = 3;
		td.innerHTML = "Names matched";

		t.tbody.appendChild(t.tr_nomatch = document.createElement("TR"));
		t.tr_nomatch.appendChild(td = document.createElement("TH"));
		td.colSpan = 3;
		td.innerHTML = "Names not matched";
		t.tbody.appendChild(tr = document.createElement("TR"));
		tr.appendChild(t.td_notmatch_geonames = document.createElement("TD"));
		t.td_notmatch_geonames.style.verticalAlign = "top";
		t.td_notmatch_geonames.style.whiteSpace = "nowrap";
		tr.appendChild(t.td_notmatch_gadm = document.createElement("TD"));
		t.td_notmatch_gadm.style.verticalAlign = "top";
		t.td_notmatch_gadm.style.whiteSpace = "nowrap";
		tr.appendChild(td = document.createElement("TD")); // actions
		var img = document.createElement("IMG");
		img.src = "/static/common/images/add.png";
		img.style.cursor = "pointer";
		img.onclick = function() { t.create_match(); };
		
		return table;
	};
	
	t.update_all = function() {
		// get level from select
		t.geonames_level = t.select_geonames_level.selectedIndex+1;
		t.gadm_level = t.select_gadm_level.selectedIndex+1;
		// remove all content
		while (t.tr_match.nextSibling != t.tr_nomatch)
			t.tr_match.parentNode.removeChild(t.tr_match.nextSibling);
		// get content
		t.geonames_list = null;
		t.gadm_list = null;
		geonames.get_all_under(t.geonames_level, t.geonames_parent, function(list){
			t.geonames_list = list;
			if (t.gadm_list != null) t.update_from_lists();
		});
		gadm.get_all_under(t.gadm_level, t.gadm_parent, function(list){
			t.gadm_list = list;
			if (t.geonames_list != null) t.update_from_lists();
		});
	};
	t.update_from_lists = function() {
		var rem1 = [], rem2 = [];
		for (var i = 0; i < t.gadm_list.length; ++i) rem2.push(t.gadm_list[i]);
		for (var i = 0; i < t.geonames_list.length; ++i) {
			var a1 = t.geonames_list[i];
			var matches = [];
			for (var j = 0; j < rem2.length; ++j) {
				var a2 = rem2[j];
				var score = t.match_score(a1.name, a2.name);
				if (score == 0) continue;
				matches.push({score:score,adm:a2});
			}
			matches.sort(function(a1,a2){
				return a2.score-a1.score;
			});
			if (matches.length > 0) {
				t.add_match(a1, matches[0].adm);
				rem2.remove(matches[0].adm);
			} else {
				rem1.push(a1);
			}
		}
		for (var i = 0; i < rem1.length; ++i)
			t.add_not_match_geonames(rem1[i]);
		for (var i = 0; i < rem2.length; ++i)
			t.add_not_match_gadm(rem2[i]);
		if (t.popup)
			t.popup.resize();
	};
	t.match_score = function(s1,s2) {
		var words1 = s1.toLowerCase().split(" ");
		var words2 = s2.toLowerCase().split(" ");
		var score = 0;
		for (var i = 0; i < words1.length; ++i) {
			if (words1[i].length < 3) continue;
			for (var j = 0; j < words2.length; ++j) {
				if (words2[j].length < 3) continue;
				if (words1[i] == words2[j]) { score++; break; }
			}
		}
		return score;
	};
	t.add_match = function(a1, a2) {
		var tr,td;
		t.tbody.insertBefore(tr = document.createElement("TR"), t.tr_nomatch);
		tr.appendChild(td = document.createElement("TD"));
		td.innerHTML = a1.name;
		td.style.whiteSpace = 'nowrap';
		tr.appendChild(td = document.createElement("TD"));
		td.innerHTML = a2.name;
		td.style.whiteSpace = 'nowrap';
		tr.appendChild(td = document.createElement("TD"));
		var img = document.createElement("IMG");
		td.appendChild(img);
		img.src = "/static/common/images/remove.png";
		img.style.cursor = "pointer";
		img.onclick = function() {
			t.add_not_match_geonames(a1);
			t.add_not_match_gadm(a2);
			t.tbody.removeChild(tr);
		};
	};
	t.add_not_match_geonames = function(adm) {
		var div = document.createElement("DIV");
		var radio = document.createElement("INPUT");
		radio.type = "radio";
		radio.name = "geonames";
		radio._data = adm;
		div.appendChild(radio);
		div.appendChild(document.createTextNode(adm.name));
		t.td_notmatch_geonames.appendChild(div);
	};
	t.add_not_match_gadm = function(adm) {
		var div = document.createElement("DIV");
		var radio = document.createElement("INPUT");
		radio.type = "radio";
		radio.name = "gadm";
		radio._data = adm;
		div.appendChild(radio);
		div.appendChild(document.createTextNode(adm.name));
		t.td_notmatch_gadm.appendChild(div);
	};
	t.create_match = function() {
		var value1 = null;
		var div1, div2;
		for (var i = 0; i < t.td_notmatch_geonames.childNodes.length; ++i) {
			var e = t.td_notmatch_geonames.childNodes[i];
			var radio = e.childNodes[0];
			if (!radio.checked) continue;
			value1 = radio._data;
			div1 = e;
			break;
		}
		if (value1 == null) return;
		var value2 = null;
		for (var i = 0; i < t.td_notmatch_gadm.childNodes.length; ++i) {
			var e = t.td_notmatch_gadm.childNodes[i];
			var radio = e.childNodes[0];
			if (!radio.checked) continue;
			value2 = radio._data;
			div2 = e;
			break;
		}
		if (value2 == null) return;
		t.add_match(value1,value2);
		div1.parentNode.removeChild(div1);
		div2.parentNode.removeChild(div2);
	};
	
	t.element = document.createElement("DIV");
	t.element.appendChild(t.create_db_table());
	t.update_all();
	
	t.popup = new popup_window("Import Geography","/static/common/images/import.png","");
	t.popup.setContent(t.element);
	t.popup.show();
}
function geography_address(container, country) {
	var t=this;
	
	t._createContent = function() {
		container.appendChild(t.table = document.createElement('TABLE'));
		var td;
		t.table.appendChild(t.tr_country = document.createElement('TR'));
		t.tr_country.appendChild(td = document.createElement('TD'));
		td.innerHTML = "Country";
		t.tr_country.appendChild(td = document.createElement('TD'));
		t.tr_country.style.visibility = 'hidden';
		t.tr_country.style.position = 'absolute';
		t.tr_div = [];
		for (var i = 1; i <= 5; ++i) {
			t.table.appendChild(t.tr_div[i] = document.createElement('TR'));
			t.tr_div[i].appendChild(document.createElement('TD'));
			t.tr_div[i].appendChild(document.createElement('TD'));
			t.tr_div[i].style.visibility = 'hidden';
			t.tr_div[i].style.position = 'aboslute';
		}
		t.table.appendChild(t.tr_search = document.createElement('TR'));
		t.tr_search.appendChild(td = document.createElement('TD'));
		td.innerHTML = "Search";
		t.tr_search.appendChild(td = document.createElement('TD'));
		add_javascript('/static/common/js/autocomplete_field/autocomplete_field.js', function() {
			var f = new autocomplete_field(t.tr_search.childNodes[1], 3);
			f.search = function(value, handler) {
				var data = {};
				data["country"] = t.selected[0];
				for (var i = 1; i <= 4; ++i) {
					if (t.selected[i] == 0) break;
					data["adm"+i] = t.selected[i];
				}
				data["search"] = value;
				ajax.post_parse_result("/dynamic/geography/service/search",data,function(result) {
					if (!result || result.length == 0) {
						handler([]);
						return;
					}
					var tree = [];
					for (var i = 0; i < result.length; ++i) {
						var node = tree;
						if (result[i].parents) {
							for (var j = 0; j < result[i].parents.length; ++j) {
								var p = result[i].parents[result[i].parents.length-1-j];
								var pn = null;
								for (var k = 0; k < node.length; ++k)
									if (node[k].id == p.id) { pn = node[k]; break; }
								if (pn == null) {
									pn = p;
									pn.children = [];
									node.push(pn);
								}
								node = pn.children;
							}
							result[i].parents = null;
						}
						var pn = null;
						for (var k = 0; k < node.length; ++k)
							if (node[k].id == result[i].id) { pn = node[k]; break; }
						if (pn == null) {
							pn = result[i];
							pn.children = [];
							node.push(pn);
						}
					}
					var build_tree = function(node, indent, items) {
						var div = document.createElement("DIV");
						div.style.paddingLeft = (20*indent)+"px";
						div.className = 'context_menu_item';
						div.innerHTML = node.type+" "+node.name;
						// TODO onclick
						items.push(div);
						node.children.sort(function(n1,n2){return n1.name.localeCompare(n2.name)});
						for (var i = 0; i < node.children.length; ++i)
							build_tree(node.children[i], indent+1, items);
					}
					var items = [];
					for (var i = 0; i < tree.length; ++i)
						build_tree(tree[i], 0, items);
					handler(items);
				});
			};
		});
		t.tr_search.style.visibility = 'hidden';
		t.tr_search.style.position = 'absolute';
		t.table.appendChild(t.tr_loading = document.createElement('TR'));
		t.tr_loading.appendChild(td = document.createElement('TD'));
		td.innerHTML = "<img src='/static/common/images/loading.gif'/>";
	};
	
	t._createContent();
	add_javascript('/static/geography/geography.js',function(){
		geography.get_countries(function(countries) {
			t.table.removeChild(t.tr_loading);
			if (!country) {
				t.tr_country.style.visibility = 'visible';
				t.tr_country.style.position = 'static';
				add_javascript('/static/common/js/dropdown/dropdown.js',function() {
					var dd = new dropdown(t.tr_country.childNodes[1]);
					for (var i = 0; i < countries.length; ++i)
						dd.add_item("<img src='/static/geography/flags/"+countries[i].code+".png'/> "+countries[i].name, countries[i].code, countries[i].name);
					dd.onselect = function(country_code) {
						t.select_country(country_code);
					};
				});
			} else
				t.select_country(country_code);
		});
	});
	if (!country) {
		add_javascript('/static/common/js/dropdown/dropdown.js');
		add_stylesheet('/static/common/js/dropdown/dropdown.css');
	}
	
	t.select_country = function(country_code) {
		t.selected = [country_code,0,0,0,0,0];
		// hide search
		t.tr_search.style.visibility = 'hidden';
		t.tr_search.style.position = 'absolute';
		// division 1 loading
		t.tr_div[1].style.visibility = 'visible';
		t.tr_div[1].style.position = 'static';
		t.tr_div[1].childNodes[0].innerHTML = "<img src='/static/common/images/loading.gif'/>";
		while (t.tr_div[1].childNodes[1].childNodes.length > 0)
			t.tr_div[1].childNodes[1].removeChild(t.tr_div[1].childNodes[1].childNodes[0]);
		// divisions 2..5 hidden
		for (var level = 2; level <= 5; ++level) {
			t.tr_div[level].style.visibility = 'hidden';
			t.tr_div[level].style.position = 'absolute';
		}
		// load country info
		geography.get_country_div1(country_code, function(country) {
			t.country = country;
			if (country.divisions == 0 || country.div1.list.length == 0) {
				// no division for this country
				t.tr_div[1].style.visibility = 'hidden';
				t.tr_div[1].style.position = 'absolute';
				return;
			}
			t.tr_search.style.visibility = 'visible';
			t.tr_search.style.position = 'static';
			t.tr_div[1].childNodes[0].innerHTML = country.div1.type;
			var select = document.createElement("SELECT");
			select.add(document.createElement("OPTION"));
			for (var i = 0; i < country.div1.list.length; ++i) {
				var o = document.createElement("OPTION");
				o.value = country.div1.list[i].id;
				o.text = country.div1.list[i].name;
				select.add(o);
			}
			t.tr_div[1].childNodes[1].appendChild(select);
			select.onchange = function() {
				t.select_div(1, this.options[this.selectedIndex].value);
			};
		});
	};
	t.select_div = function(level, id) {
		t.selected[level] = id;
		if (level == t.country.divisions) return; // last level
		if (!id) {
			// nothing selected, hide all
			for (var i = level+1; i <= 5; ++i) {
				t.tr_div[i].style.visibility = 'hidden';
				t.tr_div[i].style.position = 'absolute';
			}
			return;
		}
		// next level loading
		t.tr_div[level+1].style.visibility = 'visible';
		t.tr_div[level+1].style.position = 'static';
		t.tr_div[level+1].childNodes[0].innerHTML = "<img src='/static/common/images/loading.gif'/>";
		while (t.tr_div[level+1].childNodes[1].childNodes.length > 0)
			t.tr_div[level+1].childNodes[1].removeChild(t.tr_div[level+1].childNodes[1].childNodes[0]);
		// next divisions hidden
		for (var i = level+2; i <= 5; ++i) {
			t.tr_div[i].style.visibility = 'hidden';
			t.tr_div[i].style.position = 'absolute';
		}
		// load list
		var parents = [];
		for (var i = 1; i <= level; ++i) parents.push(t.selected[i]);
		geography.get_country_sub_div(t.selected[0], level+1, parents, function(divisions) {
			if (divisions.list.length == 0) {
				// no sub division
				t.tr_div[level+1].style.visibility = 'hidden';
				t.tr_div[level+1].style.position = 'absolute';
				return;
			}
			t.tr_div[level+1].childNodes[0].innerHTML = divisions.type;
			var select = document.createElement("SELECT");
			select.add(document.createElement("OPTION"));
			for (var i = 0; i < divisions.list.length; ++i) {
				var o = document.createElement("OPTION");
				o.value = divisions.list[i].id;
				o.text = divisions.list[i].name;
				select.add(o);
			}
			t.tr_div[level+1].childNodes[1].appendChild(select);
			select.onchange = function() {
				t.select_div(level+1, this.options[this.selectedIndex].value);
			};
		});
	};
}
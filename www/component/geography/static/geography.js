if (typeof window.top.geography == 'undefined' && typeof window.top.add_javascript != 'undefined')
	window.top.add_javascript('/static/geography/geography.js');
if (window == window.top) {
	add_javascript("/static/common/js/ajax.js");
	geography = {
		_countries: null,
		get_countries: function(handler) {
			if (this._countries != null) { handler(this._countries); return; }
			var t=this;
			add_javascript("/static/common/js/ajax.js",function() {
				ajax.call("GET", "/static/geography/countries.php", null, null,
					function(error) {
					}, function(xhr) {
						var list = eval('('+xhr.responseText+')');
						list.sort(function(c1,c2){return c1[2].localeCompare(c2[2])});
						t._countries = [];
						for (var i = 0; i < list.length; ++i)
							if (list[i][1] == null)
								t._countries.push({code:list[i][0],name:list[i][2],divisions:list[i][3],sub_countries:[]});
						for (var i = 0; i < list.length; ++i)
							if (list[i][1] != null) {
								for (var j = 0; j < t._countries.length; ++j)
									if (t._countries[j].code == list[i][1]) {
										t._countries[j].sub_countries.push({code:list[i][0],name:list[i][2],divisions:list[i][3],sub_countries:[]});
									}
							}
						handler(t._countries);
					}
				);
			});
		},
		get_country: function(country_code, handler) {
			var h = function(countries) {
				for (var i = 0; i < countries.length; ++i)
					if (countries[i].code == country_code) {
						handler(countries[i]);
						return;
					}
			};
			if (this._countries == null) this.get_countries(h);
			else h(this._countries);
		},
		get_country_div1: function(country_code, handler) {
			this.get_country(country_code, function(country) {
				if (country.divisions == 0) {
					handler(country);
					return;
				}
				if (country.div1) {
					handler(country);
					return;
				}
				ajax.call("GET", "/static/geography/div.php?c="+country_code+"&l=1", null, null,
						function(error) {
						}, function(xhr) {
							country.div1 = eval('('+xhr.responseText+')');
							handler(country);
						}
					);
			});
		},
		get_country_sub_div: function(country_code, level, parents, handler) {
			if (level > 5) { alert("Geography: maximum division level is 5, given is "+level); return; }
			this.get_country_div1(country_code, function(country) {
				if (country.divisions < level) {
					alert("Geography: invalid level "+level+": country "+country_code+" has a maximum level of "+country.divisions);
					return;
				}
				var list = country.div1.list;
				var pos = 0;
				var next;
				do {
					next = null;
					for (var i = 0; i < list.length; ++i)
						if (list[i].id == parents[pos]) { next = list[i]; break; }
					if (next == null) {
						alert("Geography: invalid parent id "+parents[pos]+" at level "+(pos+1));
						return;
					}
					pos++;
					if (pos < level-1) list = next.divisions.list;
				} while (pos < level-1);
				if (next.divisions) {
					handler(next.divisions);
					return;
				}
				ajax.call("GET", "/static/geography/div.php?c="+country_code+"&l="+level+"&p="+parents[level-2], null, null,
					function(error) {
					}, function(xhr) {
						next.divisions = eval('('+xhr.responseText+')');
						handler(next.divisions);
					}
				);
			});
		}
	};
} else {
	geography = {
		get_countries: function(handler) { window.top.add_javascript('/static/geography/geography.js',function() { window.top.geography.get_countries(handler); }); },
		get_country: function(country_code, handler) { window.top.add_javascript('/static/geography/geography.js',function() { window.top.geography.get_country(country_code, handler); }); },
		get_country_div1: function(country_code, handler) { window.top.add_javascript('/static/geography/geography.js',function() { window.top.geography.get_country_div1(country_code, handler); }); },
		get_country_sub_div: function(country_code, level, parents, handler) { window.top.add_javascript('/static/geography/geography.js',function() { window.top.geography.get_country_sub_div(country_code, level, parents, handler); }); }
	};
}
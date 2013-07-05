function adm_editor(container, url_provider, adm_builder) {
	var t=this;
	t.url_provider = url_provider;
	t.adm_builder = adm_builder;
	t.root = new adm(t,null,0,0);
	t.root.element_content = container;
	t.root.loading_img = document.createElement("IMG");
	t.root.loading_img.src = "/static/common/images/loading.gif";
	container.appendChild(t.root.loading_img);
	t.root.load_children(function() {
		t.root.display_children();
	});
	
	t.get_all_under = function(level, parent, handler) {
		var list = [];
		if (parent == 0)
			t.root.get_all_at_level(level, list, function() {
				handler(list);
			});
		else
			t.root.get_all_under(level, parent, list, function() {
				handler(list);
			});
	};
}
function adm(editor, parent, level, id, name, type_en, type_lo) {
	var t=this;
	t.editor = editor;
	t.parent = parent;
	t.level = level;
	t.id = id;
	t.name = name;
	t.type_en = type_en;
	t.type_lo = type_lo;
	t.children = null;
	t.children_error = null;
	
	t.create_page_element = function() {
		t.element = document.createElement("DIV");
		t.element.appendChild(t.element_title = document.createElement("DIV"));
		t.element.appendChild(t.element_content = document.createElement("DIV"));
		t.element_content.style.visibility = 'hidden';
		t.element_content.style.position = 'static';
		t.element_content.style.top = '-10000px';
		t.element_title.style.whiteSpace = "nowrap";
		if (t.parent != null)
			t.element_content.style.paddingLeft = "15px";
		if (t.level < 5) {
			t.element_title.appendChild(t.expand_img = document.createElement("IMG"));
			t.expand_img.src = "/static/common/images/arrow_right_10.gif";
			t.expand_img.style.cursor = 'pointer';
			t.expand_img.onclick = function() { t.expand(); };
		}
		t.element_title.appendChild(t.element_name = document.createElement("SPAN"));
		t.element_name.innerHTML = t.name;
		if (t.type_en || t.type_lo) {
			t.element_title.appendChild(t.element_type = document.createElement("SPAN"));
			t.element_type.innerHTML = t.type_en+" ("+t.type_lo+")";
			t.element_type.style.paddingLeft = "10px";
			t.element_type.style.fontStyle = "italic";
			t.element_type.style.color = "#808080";
		}
		t.element_title.appendChild(t.loading_img = document.createElement("IMG"));
		t.loading_img.style.visibility = 'hidden';
		t.loading_img.src = "/static/common/images/loading.gif";
	}
	
	t.load_children = function(handler) {
		if (t.children == null && t.children_error == null) {
			if (t.loading_img)
				t.loading_img.style.visibility = 'visible';
			var url = t.editor.url_provider(t.level+1,t);
			ajax.call("GET", url, null, null, 
			function(error) {
				if (t.loading_img)
					t.loading_img.style.visibility = 'hidden';
				t.children_error = error;
				if (handler) handler();
			}, function(xhr) {
				if (t.loading_img)
					t.loading_img.style.visibility = 'hidden';
				var output;
				try { output = eval('('+xhr.responseText+')'); }
				catch (e) {
					t.children_error = "Invalid output: "+e;
					if (handler) handler();
					return;
				}
				if (output.errors) {
					t.children_error = output.errors;
					if (handler) handler();
					return;
				}
				t.children = t.editor.adm_builder(t.level+1, t, output.result);
				if (handler) handler();
			});
		} else {
			if (handler) handler();
		}
	};
	
	t.expand = function() {
		t.load_children(function() {
			t.display_children();
		});
	};
	t.display_children = function() {
		if (t.children_error != null)
			t.element_content.innerHTML = "ERROR: "+t.children_error;
		else {
			t.element_content.innerHTML = "";
			for (var i = 0; i < t.children.length; ++i) {
				var c = t.children[i];
				c.create_page_element();
				t.element_content.appendChild(c.element);
			}
		}
		t.element_content.style.visibility = 'visible';
		t.element_content.style.position = 'static';
		if (t.expand_img) {
			t.expand_img.onclick = function() { t.hide_children(); };
			t.expand_img.src = "/static/common/images/arrow_down_10.gif";
		}
	};
	t.hide_children = function() {
		t.element_content.style.visibility = 'hidden';
		t.element_content.style.position = 'absolute';
		t.element_content.innerHTML = "";
		if (t.expand_img) {
			t.expand_img.onclick = function() { t.expand(); };
			t.expand_img.src = "/static/common/images/arrow_right_10.gif";
		}
	};
	
	t.get_all_under = function(level, parent, list, handler) {
		if (level <= t.level) { handler(); return; };
		var f = function() {
			if (t.children != null)
				for (var i = 0; i < t.children.length; ++i) {
					if (t.children[i].id == parent) {
						t.children[i].get_all_at_level(level, list, handler);
						return;
					}
				}
			handler();
		};
		if (t.children == null && t.children_error == null)
			t.load_children(f);
		else
			f();
	};
	t.get_all_at_level = function(level, list, handler) {
		if (level < t.level) { handler(); return; }
		var f = function() {
			if (level == t.level+1) {
				if (t.children != null)
					for (var i = 0; i < t.children.length; ++i)
						list.push(t.children[i]);
				handler();
				return;
			}
			var nb = t.children != null ? t.children.length : 0;
			if (nb > 0) {
				var f = function() {
					if (--nb == 0)
						handler();
				};
				for (var i = 0; i < t.children.length; ++i)
					t.children[i].get_all_at_level(level, list, f);
			} else
				handler();
		};
		if (t.children == null && t.children_error == null)
			t.load_children(f);
		else
			f();
	};
}
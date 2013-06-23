if (typeof add_javascript != 'undefined') {
	var url = get_script_path('autocomplete_field.js');
	add_javascript(url+"../context_menu/context_menu.js");
	add_stylesheet(url+"../context_menu/context_menu.css");
}

function autocomplete_field(input, min_characters) {
	var t=this;
	if (typeof input == 'string') input = document.getElementById(input);
	if (input.nodeName != "INPUT") {
		t.input = document.createElement("INPUT");
		t.input.type = 'text';
		input.appendChild(t.input);
	} else
		t.input = input;
	if (!min_characters) min_characters = 1;
	
	t.search = null;
	
	t._timeout = null;
	t.input.onkeypress = function(ev) {
		if (t._timeout == null)
			t._timeout = setTimeout(function(){t._search();}, 100);
		else
			t._need_refresh = true;
	};
	t._menu = null;
	t._search = function() {
		if (t.input.value.length < min_characters) {
			if (t._menu != null) {
				t._menu.hide();
				t._menu = null;
			}
			t._timeout = null;
			return;
		}
		t._need_refresh = false;
		var items = t.search(t.input.value, function(items) {
			if (items.length == 0) {
				// no result
				if (t._menu != null) {
					t._menu.hide();
					t._menu = null;
					t._timeout = null;
					if (t._need_refresh) t._search();
					return;
				}
			}
			if (t._menu != null) {
				t._menu.clearItems();
			} else {
				t._menu = new context_menu();
				t._menu.removeOnClose = true;
				t._menu.onclose = function() {
					t._menu = null;
				};
			}
			for (var i = 0; i < items.length; ++i)
				t._menu.addItem(items[i]);
			t._menu.showBelowElement(t.input);
			t._timeout = null;
			if (t._need_refresh) t._search();
		});
	};
}
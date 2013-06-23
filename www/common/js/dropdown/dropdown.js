if (typeof add_javascript != 'undefined') {
	var url = get_script_path('dropdown.js');
	add_javascript(url+"../context_menu/context_menu.js");
	add_stylesheet(url+"../context_menu/context_menu.css");
	add_stylesheet(url+"../dropdown.css");
}

function dropdown(element) {
	if (typeof element == 'string') element = document.getElementById(element);
	var t = this;
	t.element = element;
	t.element.widget = t;
	
	t.element.className = 'dropdown';
	t.options = [];
	t.selectedValue = null;
	t.onselect = null;
	
	t._init = function() {
		var max_w = 0;
		var max_h = 0;
		while (t.element.childNodes.length > 0) {
			var e = t.element.childNodes[0];
			if (e.nodeType != 1) { t.element.removeChild(e); continue; }
			e.style.position = "absolute";
			e.className = 'context_menu_item';
			e.style.whiteSpace = 'nowrap';
			if (e.offsetWidth > max_w) max_w = e.offsetWidth;
			if (e.offsetHeight > max_h) max_h = e.offsetHeight;
			e = t.element.removeChild(e);
			t.options.push(e);
		}
		if (max_h < 16) max_h = 16;
		t.element.style.position = "relative";
		t.element.style.border = "1px solid #808080";
		t.selected_content = document.createElement("DIV");
		t.selected_content.style.position = "absolute";
		t.selected_content.style.width = max_w+"px";
		t.selected_content.style.height = max_h+"px";
		t.selected_content.style.top = "0px";
		t.selected_content.style.left = "0px";
		t.button = document.createElement("DIV");
		t.button.style.position = "absolute";
		t.button.style.top = "0px";
		t.button.style.left = (max_w+1)+"px";
		t.button.style.width = "16px";
		t.button.style.height = max_h+"px";
		t.button.style.border = "1px solid #404040";
		setBackgroundGradient(t.button, "diagonal-topleft", [{pos:0,color:'#C0C0C0'},{pos:100,color:'#FFFFFF'}]);
		t.button_arrow = document.createElement('IMG');
		t.button_arrow.src = common_images_url+'arrow_down_10.gif';
		t.button_arrow.style.position = "relative";
		t.button_arrow.style.left = "2px";
		t.button_arrow.style.top = (max_h/2-5)+"px";
		t.button.appendChild(t.button_arrow);
		t.element.style.width = (max_w+17)+"px";
		t.element.style.height = max_h+"px";
		t.element.appendChild(t.selected_content);
		t.element.appendChild(t.button);
	}
	t._init();
	t.add_item = function(html, value, search_value) {
		var div = document.createElement("DIV");
		div.className = 'context_menu_item';
		div.style.position = 'absolute';
		div.style.whiteSpace = 'nowrap';
		div.innerHTML = html;
		document.body.appendChild(div);
		if (div.offsetHeight > t.selected_content.offsetHeight) {
			t.selected_content.style.height = div.offsetHeight+"px";
			t.element.style.height = div.offsetHeight+"px";
			t.button.style.height = (div.offsetHeight)+"px";
			t.button_arrow.style.top = (div.offsetHeight/2-5)+"px";
		}
		if (div.offsetWidth > t.selected_content.offsetWidth) {
			t.selected_content.style.width = div.offsetWidth+"px";
			t.button.style.left = (div.offsetWidth+1)+"px";
			t.element.style.width = (div.offsetWidth+17)+"px";
		}
		document.body.removeChild(div);
		if (value) div.setAttribute("value", value);
		if (search_value) div.setAttribute("search_value", search_value);
		t.options.push(div);
	};
	
	t._searching = "";
	t._searchingTimeout = null;
	t._search = function(ev) {
		ev = getCompatibleKeyEvent(ev);
		if (t._searchingTimeout) {
			clearTimeout(t._searchingTimeout);
			t._searchingTimeout = null;
		}
		if (ev.keyCode == 8)
			t._searching = "";
		else if (ev.keyCode == 13) {
			if (t._menu != null) {
				var items = t._menu.getItems();
				if (items.length > 0)
					t.select(items[0].getAttribute("value"));
				t._menu.hide();
			}
			t._searching = "";
			stopEventPropagation(ev);
			return;
		} else {
			t._searching += String.fromCharCode(ev.keyCode);
			t._searchingTimeout = setTimeout(function() {
				t._searching = "";
				t._searchingTimeout = null;
			},2000);
		}
		stopEventPropagation(ev);
		t.onsearch(t._searching, t._menu);
	};
	
	t.onsearch = function(s, menu) {
		if (menu) {
			if (s.length == 0) {
				while (menu.getItems().length > 0)
					menu.getItems()[0].parentNode.removeChild(menu.getItems()[0]);
				for (var i = 0; i < t.options.length; ++i)
					menu.addItem(t.options[i]);
			} else {
				s = s.toLowerCase();
				for (var i = 0; i < menu.getItems().length; ++i) {
					var item = menu.getItems()[i];
					if (item.hasAttribute("search_value")) {
						if (item.getAttribute("search_value").toLowerCase().indexOf(s) < 0) {
							item.parentNode.removeChild(item);
							i--;
						}
					} else if (item.getAttribute("value").toLowerCase().indexOf(s) < 0) {
						item.parentNode.removeChild(item);
						i--;
					}
				}
			}
		}
	};
	
	t._menu = null;
	t.dropdown = function() {
		add_javascript(get_script_path("dropdown.js")+"../context_menu/context_menu.js", function() {
			t._menu = new context_menu();
			for (var i = 0; i < t.options.length; ++i) {
				t.options[i].data = function() {
					t.selected_content.innerHTML = "<span>"+this.innerHTML+"</span>";
					t.selected_content.style.paddingTop = ((t.selected_content.offsetHeight-t.selected_content.childNodes[0].offsetHeight)/2)+"px";
					t.selectedValue = this.getAttribute("value");
					if (t.onselect) t.onselect(t.selectedValue);
				};
				t._menu.addItem(t.options[i]);
			}
			t._menu.removeOnClose = true;
			var listener = function(ev){t._search(ev);return false;};
			listenEvent(document,'keydown',listener);
			t._menu.onclose = function() {
				unlistenEvent(document,'keydown',listener);
				t._menu = null;
			};
			t._menu.showBelowElement(t.element);
		});
	};
	
	t.select = function(value) {
		for (var i = 0; i < t.options.length; ++i)
			if (t.options[i].getAttribute("value") == value) {
				t.selectedValue = value;
				t.selected_content.innerHTML = "<span>"+t.options[i].innerHTML+"</span>";
				t.selected_content.style.paddingTop = ((t.selected_content.offsetHeight-t.selected_content.childNodes[0].offsetHeight)/2)+"px";
				break;
			}
		if (t.onselect) t.onselect(value);
	};
	
	t.element.onclick = function() { t.dropdown(); };
}
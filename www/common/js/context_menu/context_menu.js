if (typeof add_javascript != 'undefined')
	add_javascript(get_script_path('context_menu.js')+"../animation.js");
function context_menu(menu) {
	if (typeof menu == "string") menu = document.getElementById(menu);
	if (menu != null && menu.parentNode != null && menu.parentNode.nodeType == 1)
		menu.parentNode.removeChild(menu);
	var t = this;
	if (menu == null) {
		menu = document.createElement("DIV");
		menu.className = 'context_menu';
	}
	t.removeOnClose = false;
	t.onclose = null;
	for (var i = 0; i < menu.childNodes.length; ++i)
		if (menu.childNodes[i].nodeType == 1 && menu.childNodes[i].className == "context_menu_item") {
			if (typeof menu.childNodes[i].onclickset == 'undefined' && menu.childNodes[i].onclick && !menu.childNodes[i].data)
				menu.childNodes[i].data = menu.childNodes[i].onclick;
			menu.childNodes[i].onclick = function() {
				t.hide();
				if (this.data) this.data();
				return false;
			};
			menu.childNodes[i].onclickset = true;
		}
	
	t.addItem = function(element) {
		element.style.position = 'static';
		menu.appendChild(element);
		if (typeof element.onclickset == 'undefined' && element.onclick && !element.data)
			element.data = element.onclick;
		element.onclick = function() {
			t.hide();
			if (this.data) this.data();
			return false;
		};
		element.onclickset = true;
	};
	t.addIconItem = function(icon, text, onclick) {
		var div = document.createElement("DIV");
		div.innerHTML = "<img src='"+icon+"' style='vertical-align:bottom'/> "+text;
		div.onclick = onclick;
		div.className = "context_menu_item";
		t.addItem(div);
		return div;
	};
	t.getItems = function() { return menu.childNodes; };
	t.clearItems = function() {
		while (menu.childNodes.length > 0)
			menu.childNodes[0].parentNode.removeChild(menu.childNodes[0]);
	};
	
	t.showBelowElement = function(from) {
		menu.style.visibility = "visible";
		menu.style.position = "absolute";
		document.body.appendChild(menu);
		var x = absoluteLeft(from);
		var y = absoluteTop(from);
		var w = menu.offsetWidth;
		var h = menu.offsetHeight;
		if (y+from.offsetHeight+h > getWindowHeight()) {
			// not enough space below
			var space_below = getWindowHeight()-(y+from.offsetHeight);
			var space_above = y;
			if (space_above > space_below) {
				y = y-h;
				if (y < 0) {
					// not enough space: scroll bar
					y = 0;
					menu.style.overflowY = 'scroll';
					menu.style.height = space_above+"px";
				}
			} else {
				// not enough space: scroll bar
				y = y+from.offsetHeight;
				menu.style.overflowY = 'scroll';
				menu.style.height = space_below+"px";
			}
		} else {
			// by default, show it below
			y = y+from.offsetHeight;
		}
		if (x+w > getWindowWidth()) {
			x = getWindowWidth()-w;
		}
		document.body.removeChild(menu);
		t.showAt(x,y);
	};
	t.showAt = function(x,y) {
		menu.style.visibility = "visible";
		menu.style.position = "absolute";
		menu.style.top = y+"px";
		menu.style.left = x+"px";
		for (var i = 0; i < document.body.childNodes.length; ++i)
			if (document.body.childNodes[i].style) document.body.childNodes[i].style.zIndex = -10;
		document.body.appendChild(menu);
		menu.style.zIndex = 100;
		setTimeout(function() {
			listenEvent(window,'click',t._listener);
		},1);
		if (typeof animation != 'undefined') {
			if (menu.anim) animation.stop(menu.anim);
			menu.anim = animation.fadeIn(menu,300);
		}
	};
	t.hide = function() {
		if (t.onclose) t.onclose();
		if (typeof animation != 'undefined') {
			if (menu.anim) animation.stop(menu.anim);
			menu.anim = animation.fadeOut(menu,300,function() {
				if (t.removeOnClose)
					document.body.removeChild(menu);
			});
		} else {
			if (t.removeOnClose)
				document.body.removeChild(menu);
			else {
				menu.style.visibility = "hidden";
				menu.style.top = "-10000px";
			}
		}
		for (var i = 0; i < document.body.childNodes.length; ++i)
			if (document.body.childNodes[i].style) document.body.childNodes[i].style.zIndex = 1;
		unlistenEvent(window, 'click', t._listener);
	};
	t._listener = function(ev) {
		// check if the target is inside
		var elem = ev.target;
		if (elem) {
			do {
				if (elem == menu) return;
				if (elem.parentNode == elem) break;
				elem = elem.parentNode;
				if (elem == null || elem == document.body || elem == window) break;
			} while (true);
		}
		// check if this is inside
		ev = getCompatibleMouseEvent(ev);
		var x = absoluteLeft(menu);
		var y = absoluteTop(menu);
		if (ev.x >= x && ev.x < x+menu.offsetWidth &&
			ev.y >= y && ev.y < y+menu.offsetHeight) return;
		t.hide();
	};
}

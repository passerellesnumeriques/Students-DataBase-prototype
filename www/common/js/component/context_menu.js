if (typeof pn != 'undefined')
	pn.add_javascript("/static/common/js/animation.js");
function context_menu(menu) {
	if (typeof menu == "string") menu = document.getElementById(menu);
	if (menu != null && menu.parentNode != null && menu.parentNode.nodeType == 1)
		menu.parentNode.removeChild(menu);
	var t = this;
	if (menu == null) {
		menu = document.createElement("DIV");
		menu.className = 'context_menu';
	}
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
	
	t.showBelowElement = function(from) {
		menu.style.visibility = "visible";
		menu.style.position = "absolute";
		document.body.appendChild(menu);
		var x = absoluteLeft(from);
		var y = absoluteTop(from);
		var w = menu.offsetWidth;
		var h = menu.offsetHeight;
		if (y+from.offsetHeight+h > getWindowHeight()) {
			// no space below: show it on the top
			y = y-h;
			if (y < 0) y = 0;
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
		if (pn.animation) {
			if (menu.anim) pn.animation.stop(menu.anim);
			menu.anim = pn.animation.fadeIn(menu,300);
		}
	};
	t.hide = function() {
		if (pn.animation) {
			if (menu.anim) pn.animation.stop(menu.anim);
			menu.anim = pn.animation.fadeOut(menu,300);
		} else {
			menu.style.visibility = "hidden";
			menu.style.top = "-10000px";
		}
		for (var i = 0; i < document.body.childNodes.length; ++i)
			if (document.body.childNodes[i].style) document.body.childNodes[i].style.zIndex = 1;
		unlistenEvent(window, 'click', t._listener);
	};
	t._listener = function(ev) {
		if (!ev) ev = window.event;
		var x = absoluteLeft(menu);
		var y = absoluteTop(menu);
		if (ev.clientX >= x && ev.clientX < x+menu.offsetWidth &&
			ev.clientY >= y && ev.clientY < y+menu.offsetHeight) return;
		t.hide();
	};
}

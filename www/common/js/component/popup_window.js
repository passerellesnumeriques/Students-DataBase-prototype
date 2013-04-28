if (typeof pn != 'undefined')
	pn.add_stylesheet("/static/common/js/component/popup_window.css");

function popup_window(title,icon,content) {
	var t = this;
	
	t.icon = icon;
	t.title = title;
	t.content = content;
	t.onclose = null;
	
	t.setContent = function(content) { t.content = content; };
	t.setContentFrame = function(url) {
		t.content = document.createElement("IFRAME");
		t.content.style.border = "0px";
		t.content.src = url;
		t.content.onload = function() {
			t.table.style.width = "80%";
			t.content.style.width = "100%";
			t.content.style.height = "100%";
			t.resize();
		};
	};
	
	t.show = function() {
		t.table = document.createElement("TABLE");
		t.table.className = 'popup_window';
		t.table.data = t;
		var tr = document.createElement("TR"); t.table.appendChild(tr);
		tr.className = "popup_window_title";
		var move_handler = function(ev) {
			if (!ev) ev = window.event;
			var diff_x = ev.clientX - t._move_x;
			var diff_y = ev.clientY - t._move_y;
			if (diff_x == 0 && diff_y == 0) return;
			t._move_x = ev.clientX;
			t._move_y = ev.clientY;
			var x = absoluteLeft(t.table);
			x += diff_x;
			if (x < 5) x = 5;
			if (x + t.table.offsetWidth > getWindowWidth()-10) x = getWindowWidth()-10-t.table.offsetWidth;
			var y = absoluteTop(t.table);
			y += diff_y;
			if (y < 5) y = 5;
			if (y + t.table.offsetHeight > getWindowHeight()-10) y = getWindowHeight()-10-t.table.offsetHeight;
			t.table.style.top = y+"px";
			t.table.style.left = x+"px";
		};
		var up_handler = function(ev) {
			unlistenEvent(window,'mousemove',move_handler);
			unlistenEvent(window,'mouseup',up_handler);
			unlistenEvent(window,'mouseout',up_handler);
		};
		tr.onmousedown = function(ev) {
			if (!ev) ev = window.event;
			t._move_x = ev.clientX;
			t._move_y = ev.clientY;
			listenEvent(window,'mousemove',move_handler);
			listenEvent(window,'mouseup',up_handler);
			listenEvent(window,'mouseout',up_handler);
			return false;
		};
		var td = document.createElement("TD"); tr.appendChild(td);
		td.innerHTML = (t.icon ? "<img src='"+t.icon+"' style='vertical-align:bottom'/> " : "")+t.title;
		td = document.createElement("TD"); tr.appendChild(td);
		td.onclick = function() { t.close(); };
		tr = document.createElement("TR"); t.table.appendChild(tr);
		t.content_container = document.createElement("TD"); tr.appendChild(t.content_container);
		t.content_container.colSpan = 2;
		document.body.appendChild(t.table);
		if (typeof t.content == 'string') t.content_container.innerHTML = t.content;
		else t.content_container.appendChild(t.content);
		t.resize();
	};
	t.resize = function() {
		var y = getWindowHeight()/2 - t.table.scrollHeight/2;
		if (y < 10) {
			y = 10;
			t.content_container.style.overflow = "auto";
			t.table.style.height = (getWindowHeight()-20)+"px";
		}
		var x = getWindowWidth()/2 - t.table.scrollWidth/2;
		if (x < 10) {
			x = 10;
			t.content_container.style.overflow = "auto";
			t.table.style.width = (getWindowWidth()-20)+"px";
		}
		t.table.style.top = y+"px";
		t.table.style.left = x+"px";
	};
	
	t.close = function() {
		if (t.onclose) t.onclose();
		document.body.removeChild(t.table);
	};
}

function get_popup_window_from_element(e) {
	while (e.parentNode.className != 'popup_window') e = e.parentNode;
	return e.parentNode.data;
}
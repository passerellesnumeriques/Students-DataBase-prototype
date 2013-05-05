if (typeof pn != 'undefined') {
	pn.add_stylesheet("/static/common/js/component/popup_window.css");
	pn.add_javascript("/static/common/js/animation.js");
}
function popup_window(title,icon,content) {
	var t = this;
	
	t.icon = icon;
	t.title = title;
	t.content = content;
	t.onclose = null;
	t.keep_content_on_close = false;
	t.buttons = [];
	
	t.setContent = function(content) { 
		t.content = content;
		if (t.content_container) {
			while (t.content_container.childNodes.length > 0) t.content_container.removeChild(t.content_container.childNodes[0]);
			if (typeof content == 'string')
				t.content_container.innerHTML = content;
			else
				t.content_container.appendChild(content);
			t.resize();
		}
	};
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
	
	t.addButton = function(html, id, onclick) {
		var b = document.createElement("BUTTON");
		b.innerHTML = html;
		b.id = id;
		b.onclick = onclick;
		t.buttons.push(b);
	};
	t.disableButton = function(id) {
		for (var i = 0; i < t.buttons.length; ++i)
			if (t.buttons[i].id == id)
				t.buttons[i].disabled = 'disabled';
	};
	t.enableButton = function(id) {
		for (var i = 0; i < t.buttons.length; ++i)
			if (t.buttons[i].id == id)
				t.buttons[i].disabled = '';
	};
	t.addOkCancelButtons = function(onok) {
		t.addButton("<img src='/static/common/images/ok.png' style='vertical-align:bottom'/> "+pn.locale["Ok"], 'ok', onok);
		t.addButton("<img src='/static/common/images/close.png' style='vertical-align:bottom'/> "+pn.locale["Cancel"], 'cancel', function() { t.close(); });
	};
	t.addYesNoButtons = function(onyes) {
		t.addButton("<img src='/static/common/images/ok.png' style='vertical-align:bottom'/> "+pn.locale["Yes"], 'yes', onyes);
		t.addButton("<img src='/static/common/images/close.png' style='vertical-align:bottom'/> "+pn.locale["No"], 'no', function() { t.close(); });
	};
	
	t.show = function() {
		t.locker = pn.lock_screen(function() {
			t.blink();
		});
		t.table = document.createElement("TABLE");
		t.table.className = 'popup_window';
		t.table.data = t;
		t.header = document.createElement("TR"); t.table.appendChild(t.header);
		t.header.className = "popup_window_title";
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
			if (x + t.table.offsetWidth > getWindowWidth()-10) x = getWindowWidth()-5-t.table.offsetWidth;
			var y = absoluteTop(t.table);
			y += diff_y;
			if (y < 5) y = 5;
			if (y + t.table.offsetHeight > getWindowHeight()-10) y = getWindowHeight()-5-t.table.offsetHeight;
			t.table.style.top = y+"px";
			t.table.style.left = x+"px";
		};
		var up_handler = function(ev) {
			unlistenEvent(window,'mousemove',move_handler);
			unlistenEvent(window,'mouseup',up_handler);
			unlistenEvent(window,'mouseout',up_handler);
		};
		t.header.onmousedown = function(ev) {
			if (!ev) ev = window.event;
			t._move_x = ev.clientX;
			t._move_y = ev.clientY;
			listenEvent(window,'mousemove',move_handler);
			listenEvent(window,'mouseup',up_handler);
			listenEvent(window,'mouseout',up_handler);
			return false;
		};
		var td = document.createElement("TD"); t.header.appendChild(td);
		td.innerHTML = (t.icon ? "<img src='"+t.icon+"' style='vertical-align:bottom'/> " : "")+t.title;
		td = document.createElement("TD"); t.header.appendChild(td);
		td.onclick = function() { t.close(); };
		var tr = document.createElement("TR"); t.table.appendChild(tr);
		var td = document.createElement("TD"); tr.appendChild(td);
		td.colSpan = 2;
		t.content_container = document.createElement("DIV");
		t.content_container.style.width = "100%";
		t.content_container.style.height = "100%";
		td.appendChild(t.content_container);
		if (t.buttons.length > 0) {
			var tr = document.createElement("TR");
			tr.className = 'popup_window_buttons';
			t.table.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.colSpan = 2;
			for (var i = 0; i < t.buttons.length; ++i)
				td.appendChild(t.buttons[i]);
		}
		document.body.appendChild(t.table);
		if (typeof t.content == 'string') t.content_container.innerHTML = t.content;
		else {
			t.content_container.appendChild(t.content);
			t.content.style.position = 'static';
			t.content.style.visibility = 'visible';
		}
		t.resize();
		if (pn.animation) {
			if (t.anim) pn.animation.stop(t.anim);
			t.anim = pn.animation.fadeIn(t.table, 200);
		}
	};
	t.resize = function() {
		if (!t.table) return;
		var y = getWindowHeight()/2 - t.table.scrollHeight/2;
		if (y < 5) {
			y = 5;
			t.content_container.style.overflow = "auto";
			t.table.style.height = (getWindowHeight()-10)+"px";
		}
		var x = getWindowWidth()/2 - t.table.scrollWidth/2;
		if (x < 5) {
			x = 5;
			t.content_container.style.overflow = "auto";
			t.table.style.width = (getWindowWidth()-10)+"px";
		}
		t.table.style.top = y+"px";
		t.table.style.left = x+"px";
	};
	
	t.blink = function() {
		t.table.className = "popup_window blink";
		setTimeout(function() { t.table.className = "popup_window"; },100);
		setTimeout(function() { t.table.className = "popup_window blink"; },200);
		setTimeout(function() { t.table.className = "popup_window"; },300);
		setTimeout(function() { t.table.className = "popup_window blink"; },400);
		setTimeout(function() { t.table.className = "popup_window"; },500);
	};
	
	t.close = function(keep_content_hidden) {
		if (t.onclose) t.onclose();
		pn.unlock_screen(t.locker);
		var do_close = function() {
			if (keep_content_hidden || t.keep_content_on_close) {
				t.content.parentNode.removeChild(t.content);
				t.content.style.position = 'absolute';
				t.content.style.visibility = 'hidden';
				t.content.style.top = '-10000px';
				document.body.appendChild(t.content);
			}
			document.body.removeChild(t.table);
		};
		if (pn.animation) {
			if (t.anim) pn.animation.stop(t.anim);
			pn.animation.fadeOut(t.table, 200, do_close);
		} else
			do_close();
	};
}

function get_popup_window_from_element(e) {
	while (e.parentNode.className != 'popup_window') e = e.parentNode;
	return e.parentNode.data;
}
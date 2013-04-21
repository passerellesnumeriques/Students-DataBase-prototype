function splitter_vertical(element, position) {
	if (typeof element == 'string') element = document.getElementById(element);
	var t = this;
	
	t.element = element;
	t.position = position;
	while (element.childNodes.length > 0) {
		var e = element.removeChild(element.childNodes[0]);
		if (e.nodeType != 1) continue;
		if (!t.part1) t.part1 = e; else t.part2 = e;
	}
	
	t._position = function() {
		var w = t.element.offsetWidth;
		var h = t.element.offsetHeight;
		var sw = t.separator.offsetWidth;
		var x = w*t.position - sw/2;
		t.part1.style.width = x+"px";
		t.part1.style.height = h+"px";
		t.separator.style.left = x+"px";
		t.separator.style.height = h+"px";
		t.part2.style.left = (x+sw)+"px";
		t.part2.style.width = (w-x-sw)+"px";
		t.part2.style.height = h+"px";
	};
	
	t.element.style.position = "relative";
	t.separator = document.createElement("DIV");
	t.separator.style.position = "absolute";
	t.separator.style.top = "0px";
	t.separator.className = "splitter_vertical_separator";
	t.part1.style.position = "absolute";
	t.part1.style.top = "0px";
	t.part1.style.left = "0px";
	t.part2.style.position = "absolute";
	t.part2.style.top = "0px";
	element.appendChild(t.part1);
	element.appendChild(t.separator);
	element.appendChild(t.part2);
	t._position();
	
	listenEvent(window, 'resize', function() { t._position(); t._position(); });
	
	t._stop_move = function() {
		unlistenEvent(window, 'mouseup', t._stop_move);
		unlistenEvent(window, 'blur', t._stop_move);
		unlistenEvent(window, 'mousemove', t._moving);
	}
	t._moving = function(event) {
		if (!event) event = window.event;
		var diff = event.clientX - t.mouse_pos;
		if (diff == 0) return;
		var w = t.element.offsetWidth;
		var x = w*t.position;
		x += diff;
		t.position = x/w;
		t.mouse_pos = event.clientX;
		t._position();
	}
	t.mouse_pos = 0;
	t.separator.onmousedown = function(event) {
		if (!event) event = window.event;
		t.mouse_pos = event.clientX;
		listenEvent(window, 'mouseup', t._stop_move);
		listenEvent(window, 'blur', t._stop_move);
		listenEvent(window, 'mousemove', t._moving);
		return false;
	}
}
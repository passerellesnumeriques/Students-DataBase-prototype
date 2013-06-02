function vertical_layout(container) {
	var t = this;
	t.container = container;
	if (typeof t.container == 'string') t.container = document.getElementById(t.container);
	t.container.style.position = 'relative';
	
	t.layout = function() {
		var w = t.container.offsetWidth;
		var h = t.container.offsetHeight;
		var nb_to_fill = 0;
		var used = 0;
		for (var i = 0; i < t.container.childNodes.length; ++i) {
			var e = t.container.childNodes[i];
			if (e.nodeType != 1) continue;
			var layout;
			if (e.getAttribute('layout')) layout = e.getAttribute('layout'); else layout = 'fixed';
			if (layout == 'fill')
				nb_to_fill++;
			else if (!isNaN(parseInt(layout)))
				used += parseInt(layout);
			else {
				e.style.width = w+"px";
				e.style.height = "";
				used += e.offsetHeight;
			}
		}
		var y = 0;
		for (var i = 0; i < t.container.childNodes.length; ++i) {
			var e = t.container.childNodes[i];
			if (e.nodeType != 1) continue;
			var layout;
			if (e.getAttribute('layout')) layout = e.getAttribute('layout'); else layout = 'fixed';
			e.style.position = 'absolute';
			e.style.top = y+"px";
			e.style.left = "0px";
			e.style.width = w+"px";
			if (layout == 'fill') {
				var hh = Math.floor((h-used)/nb_to_fill--);
				e.style.height = hh+"px";
				y += hh;
			} else if (!isNaN(parseInt(layout))) {
				var hh = parseInt(layout);
				e.style.height = hh+"px";
				y += hh;
			} else {
				y += e.offsetHeight;
			}
		}
	};
	
	t.layout();
	addLayoutEvent(t.container, function(){t.layout();});
}
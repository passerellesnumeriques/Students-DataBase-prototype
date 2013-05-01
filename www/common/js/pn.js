pn = {
	add_javascript: function(url, onload) {
		var p = new URL(url).path;
		if (pn._scripts_loaded.contains(p)) {
			if (onload) onload();
			return;
		}
		var head = document.getElementsByTagName("HEAD")[0];
		for (var i = 0; i < head.childNodes.length; ++i) {
			var e = head.childNodes[i];
			if (e.nodeName != "SCRIPT") continue;
			var u = new URL(e.src);
			if (u.path == p) {
				// we found a script there
				if (e.data) {
					if (onload)
						e.data.add_listener(onload)
					return;
				}
				// didn't use this way...
				e.data = new PNEvent();
				if (onload) e.data.add_listener(onload);
				if (e.onload) e.data.add_listener(e.onload);
				e.onload = function() { pn._scripts_loaded.push(p); this.data.fire(); };
				return;
			}
		}
		// this is a new script
		var s = document.createElement("SCRIPT");
		s.data = new PNEvent();
		if (onload) s.data.add_listener(onload);
		s.type = "text/javascript";
		s.onload = function() { pn._scripts_loaded.push(p); this.data.fire(); };
		s.onreadystatechange = function() { if (this.readyState == 'loaded' || this.readyState == 'complete') { pn._scripts_loaded.push(p); this.data.fire(); this.onreadystatechange = null; } };
		s.src = url;
		head.appendChild(s);
	},
	_scripts_loaded: [],
	_script_loaded: function(url) {
		url = new URL(url);
		if (!pn._scripts_loaded.contains(url.path))
			pn._scripts_loaded.push(url.path);
	},
	add_stylesheet: function(url) {
		if (typeof url == 'string') url = new URL(url);
		var head = document.getElementsByTagName("HEAD")[0];
		for (var i = 0; i < head.childNodes.length; ++i) {
			var e = head.childNodes[i];
			if (e.nodeName != "LINK") continue;
			var u = new URL(e.href);
			if (u.path == url.path) {
				// we found it
				return;
			}
		}
		var s = document.createElement("LINK");
		s.rel = "stylesheet";
		s.type = "text/css";
		s.href = url.toString();
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	},
	error_dialog: function(message) {
		pn.add_javascript("/static/common/js/component/popup_window.js",function() {
			var p = new popup_window(pn.locale["Error"], "/static/common/images/error.png", message);
			p.show();
		});
	},
	confirm_dialog: function(message, handler) {
		pn.add_javascript("/static/common/js/component/popup_window.js",function() {
			var content = document.createElement("TABLE");
			content.style.margin = "0px";
			content.style.borderCollapse = "collapse";
			content.style.borderSpacing = "0px";
			var tr = document.createElement("TR"); content.appendChild(tr);
			var td = document.createElement("TD"); tr.appendChild(td);
			td.style.padding = "3px";
			td.innerHTML = message;
			tr = document.createElement("TR"); content.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.style.backgroundColor = '#D0D0D0';
			td.style.textAlign = 'center';
			td.style.borderTop = '1px solid #808080';
			var yes_button = document.createElement("BUTTON");
			yes_button.innerHTML = "<img src='/static/common/images/ok.png' style='vertical-align:bottom'/> "+pn.locale["Yes"];
			td.appendChild(yes_button);
			var no_button = document.createElement("BUTTON");
			no_button.innerHTML = "<img src='/static/common/images/close.png' style='vertical-align:bottom'/> "+pn.locale["No"];
			td.appendChild(no_button);
			var p = new popup_window(pn.locale["Confirmation"], "/static/common/images/question.png", content);
			yes_button.onclick = function() { p.close(); handler(true); };
			no_button.onclick = function() { p.close(); handler(false); };
			p.show();
		});
	},
	close_this_popup: function() {
		if (!window.parent) alert("We are not in a popup!");
		window.parent.get_popup_window_from_element(window.frameElement).close();
	},
	sub_page: function(frame_id,url) {
		document.getElementById(frame_id).src = url;
	},
	
	lock_screen: function() {
		var div = document.getElementById('lock_screen');
		if (div) return;
		div = document.createElement('DIV');
		div.id = "lock_screen";
		div.style.backgroundColor = "#808080";
		setOpacity(div, 0.5);
		div.style.position = "fixed";
		div.style.top = "0px";
		div.style.left = "0px";
		div.style.width = getWindowWidth()+"px";
		div.style.height = getWindowHeight()+"px";
		document.body.appendChild(div);
	},
	unlock_screen: function() {
		var div = document.getElementById('lock_screen');
		if (!div) return;
		document.body.removeChild(div);
	},
	
	ajax_service_xml: function(url, data, handler,foreground) {
		var xhr = new XMLHttpRequest();
		var url = new URL(url);
		url.params.format = "xml";
		xhr.open("POST", url.toString(), !foreground);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		var sent = function() {
	        if (xhr.status != 200) { pn.error_dialog(xhr.status); handler(null); return; }
	        if (xhr.responseXML && xhr.responseXML.childNodes.length > 0) {
	            if (xhr.responseXML.childNodes[0].nodeName == "ok") {
	            	handler(xhr.responseXML.childNodes[0]);
	            	return;
	            }
                if (xhr.responseXML.childNodes[0].nodeName == "error")
                	pn.error_dialog(xhr.responseXML.childNodes[0].getAttribute("message"));
                else
                	pn.error_dialog(xhr.responseText);
	        } else
	        	pn.error_dialog(xhr.responseText);
	        handler(null);
		};
		xhr.onreadystatechange = function() {
	        if (this.readyState != 4) return;
	        sent();
	    };
	    xhr.send(data);
	},
	ajax_service_json: function(url, data, handler, foreground) {
		var xhr = new XMLHttpRequest();
		var url = new URL(url);
		url.params.format = "json";
		xhr.open("POST", url.toString(), !foreground);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		var sent = function() {
	        if (xhr.status != 200) { pn.error_dialog(xhr.status); handler(null); return; }
	        try {
	        	var output = eval("("+xhr.responseText+")");
	        	if (output.errors) {
	        		pn.error_dialog("Errors:<br/>"+output.errors);
	        		handler(null);
	        		return;
	        	}
	        	if (typeof output.result == 'undefined')
	        		pn.error_dialog("Error: No result from JSON service");
	        	handler(output.result);
	        } catch (e) {
	        	pn.error_dialog("Invalid json output:<br/>Error: "+e+"<br/>Output:<br/>"+xhr.responseText);
		        handler(null);
	        }
		};
		xhr.onreadystatechange = function() {
	        if (this.readyState != 4) return;
	        sent();
	    };
	    if (typeof data == 'object') {
	    	var s = "";
	    	for (var name in data) {
	    		if (s.length > 0) s += "&";
	    		s += encodeURIComponent(name);
	    		s += "=";
	    		s += encodeURIComponent(data[name]);
	    	}
	    	data = s;
	    }
	    xhr.send(data);
	},
}

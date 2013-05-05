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
		pn.add_stylesheet("/static/common/js/component/popup_window.css");
		pn.add_javascript("/static/common/js/component/popup_window.js",function() {
			var p = new popup_window(pn.locale["Error"], "/static/common/images/error.png", message);
			p.show();
		});
	},
	confirm_dialog: function(message, handler) {
		pn.add_stylesheet("/static/common/js/component/popup_window.css");
		pn.add_javascript("/static/common/js/component/popup_window.js",function() {
			var p = new popup_window(pn.locale["Confirmation"], "/static/common/images/question.png", message);
			var result = false;
			p.addYesNoButtons(function() {
				result = true;
				p.close();
			});
			p.onclose = function() {
				handler(result);
			};
			p.show();
		});
	},
	input_dialog: function(icon,title,message,default_value,max_length,validation_handler,ok_handler) {
		pn.add_stylesheet("/static/common/js/component/popup_window.css");
		pn.add_javascript("/static/common/js/component/popup_window.js",function() {
			var content = document.createElement("DIV");
			content.innerHTML = message+"<br/>";
			var input = document.createElement("INPUT");
			input.type = 'text';
			input.value = default_value;
			input.maxLength = max_length;
			content.appendChild(input);
			var error_div = document.createElement("DIV");
			error_div.style.visibility = 'hidden';
			error_div.style.position = 'absolute';
			error_div.innerHTML = "<img src='/static/common/images/error.png' style='vertical-align:bottom'/> ";
			var error_message = document.createElement("SPAN");
			error_message.style.color = 'red';
			error_div.appendChild(error_message);
			content.appendChild(error_div);
			var p = new popup_window(title, icon, content);
			var result = null;
			p.addOkCancelButtons(function() {
				result = input.value;
				p.close();
			});
			var validate = function() {
				var error = validation_handler(input.value);
				if (error != null) {
					p.disableButton('ok');
					input.style.border = "1px solid red";
					error_message.innerHTML = error;
					error_div.style.visibility = 'visible';
					error_div.style.position = 'static';
					p.resize();
				} else {
					p.enableButton('ok');
					input.style.border = "";
					error_div.style.visibility = 'hidden';
					error_div.style.position = 'absolute';
					p.resize();
				}
			};
			validate();
			input.onkeyup = input.onblur = validate;
			p.onclose = function() {
				ok_handler(result);
			};
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
	
	lock_screen: function(onclick) {
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
		if (onclick)
			div.onclick = onclick;
		if (pn.animation)
			div.anim = pn.animation.fadeIn(div,200,null,10,50);
		return document.body.appendChild(div);
	},
	unlock_screen: function(div) {
		if (!div) div = document.getElementById('lock_screen');
		if (!div) return;
		if (pn.animation) {
			if (div.anim) pn.animation.stop(div.anim);
			pn.animation.fadeOut(div,200,function(){
				document.body.removeChild(div);				
			},50,0);
		} else
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

pn = {
	context_menu: function(menu, from) {
		if (typeof menu == "string") {
			menu = document.getElementById(menu);
		} else {
			// TODO build menu
		}
		menu.style.visibility = "visible";
		var x = absoluteLeft(from);
		var y = absoluteTop(from);
		var w = menu.scrollWidth;
		var h = menu.scrollHeight;
		if (y+from.offsetHeight+h > getWindowHeight()) {
			// no space below: show it on the top
			menu.style.top = (y-h)+"px";
		} else {
			// by default, show it below
			menu.style.top = (y+from.offsetHeight)+"px";
		}
		if (x+w > getWindowWidth()) {
			menu.style.left = (getWindowWidth()-w)+"px";
		} else {
			menu.style.left = x+"px";
		}
		for (var i = 0; i < document.body.childNodes.length; ++i)
			if (document.body.childNodes[i].style) document.body.childNodes[i].style.zIndex = -10;
		menu.parentNode.removeChild(menu);
		document.body.appendChild(menu);
		menu.style.zIndex = 100;
		setTimeout(function() {
			var listener = function() {
				menu.style.visibility = "hidden";
				menu.style.top = "-10000px";
				for (var i = 0; i < document.body.childNodes.length; ++i)
					if (document.body.childNodes[i].style) document.body.childNodes[i].style.zIndex = 1;
				unlistenEvent(window, 'click', listener);
			};
			listenEvent(window,'click',listener);
		},1);
	},
	popup: function(title,icon,content,onclose) {
		var div = document.createElement("DIV");
		div.id = 'popup_page';
		div.style.zIndex = 50;
		div.style.position = "fixed";
		div.style.width = "80%";
		div.style.height = "90%";
		div.style.top = "5%";
		div.style.left = "10%";
		div.innerHTML = "<table cellspacing=0 cellpadding=0 class='popup_page'>"+
			"<tr><td>"+(icon ? "<img src='"+icon+"' style='vertical-align:bottom'/> " : "")+title+"</td><td onclick=\"pn.close_popup();\"></td></tr>"+
			"<tr><td colspan=2 class='set_height'>"+content+"</td></tr></table>";
		div.data = onclose;
		document.body.appendChild(div);
		make_height_of_td_compatible();
	},
	popup_page: function(title,icon,component,page,onclose) {
		pn.popup(title, icon, "<iframe src='/dynamic/"+component+"/page/"+page+"' frameborder=0 style='width:100%;height:100%'></iframe>",onclose);
	},
	error_dialog: function(message) {
		pn.popup("Error", "/static/common/images/error.png", message);
	},
	close_this_popup: function() {
		if (!window.parent) alert("We are not in a popup!");
		window.parent.pn.close_popup();
	},
	close_popup: function() {
		var popup = document.getElementById('popup_page');
		if (!popup) alert("No popup!");
		if (popup.data) popup.data();
		document.body.removeChild(popup);
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
	
	ajax_service_xml: function(url, data, handler) {
		var xhr = new XMLHttpRequest();
		var url = new URL(url);
		url.params.format = "xml";
		xhr.open("POST", url.toString(), true);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		xhr.onreadystatechange = function() {
	        if (this.readyState != 4) return;
	        if (this.status != 200) { pn.error_dialog(this.status); handler(null); return; }
	        if (this.responseXML && this.responseXML.childNodes.length > 0) {
	            if (this.responseXML.childNodes[0].nodeName == "ok") {
	            	handler(this.responseXML.childNodes[0]);
	            	return;
	            }
                if (this.responseXML.childNodes[0].nodeName == "error")
                	pn.error_dialog(this.responseXML.childNodes[0].getAttribute("message"));
                else
                	pn.error_dialog(this.responseText);
	        } else
	        	pn.error_dialog(this.responseText);
	        handler(null);
	    };
	    xhr.send(data);
	},
	ajax_service_json: function(url, data, handler) {
		var xhr = new XMLHttpRequest();
		var url = new URL(url);
		url.params.format = "json";
		xhr.open("POST", url.toString(), true);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		xhr.onreadystatechange = function() {
	        if (this.readyState != 4) return;
	        if (this.status != 200) { pn.error_dialog(this.status); handler(null); return; }
	        try {
	        	var output = eval("("+this.responseText+")");
	        	if (output.errors) {
	        		pn.error_dialog("Errors:<br/>"+output.errors);
	        		handler(null);
	        	}
	        	if (!output.result)
	        		pn.error_dialog("Error: No result from JSON service");
	        	handler(output.result);
	        } catch (e) {
	        	pn.error_dialog("Invalid json output:<br/>Error: "+e+"<br/>Output:<br/>"+this.responseText);
		        handler(null);
	        }
	    };
	    xhr.send(data);
	}
}

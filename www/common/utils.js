String.prototype.startsWith=function(s){return this.length<s.length?false:this.substring(0,s.length)==s;};
String.prototype.trim=function() {
	if (this.length == 0) return "";
	var start, end;
	for (start = 0; start < this.length; start++)
		if (!isSpace(this.charAt(start))) break;
	for (end = this.length; end > 0; end--)
		if (!isSpace(this.charAt(end-1))) break;
	return this.substring(start, end);
};
function isSpace(c) { return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); }
function isLetter(c) {
	var ord = c.charCodeAt(0);
	if (ord >= 'a'.charCodeAt(0) && ord <= 'z'.charCodeAt(0)) return true;
	if (ord >= 'A'.charCodeAt(0) && ord <= 'Z'.charCodeAt(0)) return true;
	return false;
}
Array.prototype.contains=function(e){for(var i=0;i<this.length;++i)if(this[i]==e)return true;return false;};
Array.prototype.remove=function(e){for(var i=0;i<this.length;++i)if(this[i]==e){this.splice(i,1);i--;};};

if (typeof document.getElementById != "function")
	document.getElementById = function(id) { return document.all[id]; }

getWindowHeight = function() { return document.body.clientHeight; };
getWindowWidth = function() { return document.body.clientWidth; };

function absoluteLeft(e) {
	var left = e.offsetLeft;
	try { if (e.offsetParent) left += absoluteLeft(e.offsetParent); } catch (ex) {}
	return left;
}
function absoluteTop(e) {
	var top = e.offsetTop;
	try { if (e.offsetParent) top += absoluteTop(e.offsetParent); } catch (ex) {}
	return top;
}
function setOpacity(element, opacity) {
	element.style.opacity = opacity;
	element.style.MozOpacity = opacity;
	element.style.KhtmlOpacity = opacity;
	opacity = Math.round(opacity*100);
	element.style.filter = "alpha(opacity="+opacity+");"
	element.style.MsFilter = "progid:DXImageTransform.Microsoft.Alpha(Opacity="+opacity+")";	
}

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
	popup_page: function(title,icon,component,page) {
		var div = document.createElement("DIV");
		div.id = 'popup_page';
		div.style.position = "fixed";
		//div.style.width = Math.round(getWindowWidth()*80/100)+"px";
		//div.style.height = Math.round(getWindowHeight()*90/100)+"px";
		//div.style.left = Math.round(getWindowWidth()/10)+"px";
		//div.style.top = Math.round(getWindowHeight()/20)+"px";
		div.style.width = "80%";
		div.style.height = "90%";
		div.style.top = "5%";
		div.style.left = "10%";
		div.innerHTML = "<table cellspacing=0 cellpadding=0 class='popup_page'>"+
			"<tr><td>"+(icon ? "<img src='"+icon+"' style='vertical-align:bottom'/> " : "")+title+"</td><td onclick=\"document.body.removeChild(document.getElementById('popup_page'))\"></td></tr>"+
			"<tr><td colspan=2 class='set_height'><iframe src='/dynamic/"+component+"/page/"+page+"' frameborder=0 style='width:100%;height:100%'></iframe></td></tr></table>";
		document.body.appendChild(div);
		make_height_of_td_compatible();
	},
	sub_page: function(frame_id,url) {
		document.getElementById(frame_id).src = url;
	},
	
	ajax_service_xml: function(url, data, handler) {
		var xhr = new XMLHttpRequest();
		xhr.open("POST", url, true);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		xhr.onreadystatechange = function() {
	        if (this.readyState != 4) return;
	        if (this.status != 200) { alert("Error "+this.status); handler(null); return; }
	        if (this.responseXML && this.responseXML.childNodes.length > 0) {
	            if (this.responseXML.childNodes[0].nodeName == "ok") {
	            	handler(this.responseXML.childNodes[0]);
	            	return;
	            }
                if (this.responseXML.childNodes[0].nodeName == "error")
                    alert(this.responseXML.childNodes[0].getAttribute("message"));
                else
                    alert(this.responseText);
	        } else
	            alert(this.responseText);
	        handler(null);
	    };
	    xhr.send(data);
	}
}

function listenEvent(elem, type, handler) {
	if (elem.addEventListener)
	     elem.addEventListener(type,handler,false);
	 else if (elem.attachEvent)
	     elem.attachEvent('on'+type,handler); 
}
function unlistenEvent(elem, type, handler) {
	if (elem.removeEventListener)
		elem.removeEventListener(type,handler,false);
	else
	     elem.detachEvent('on'+type,handler); 
}

function make_height_of_td_compatible() {
	var list = document.getElementsByClassName("set_height");
	for (var i = 0; i < list.length; ++i)
		list[i].style.height = "";
	for (var i = 0; i < list.length; ++i)
		list[i].style.height = list[i].scrollHeight+"px";
}
listenEvent(window, 'load', make_height_of_td_compatible);
listenEvent(window, 'resize', make_height_of_td_compatible);
/* Add some useful functions to basic classes */
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

/* Made some function cross-browser compatible */
if (typeof document.getElementById != "function")
	document.getElementById = function(id) { return document.all[id]; }
function setOpacity(element, opacity) {
	element.style.opacity = opacity;
	element.style.MozOpacity = opacity;
	element.style.KhtmlOpacity = opacity;
	opacity = Math.round(opacity*100);
	element.style.filter = "alpha(opacity="+opacity+");"
	element.style.MsFilter = "progid:DXImageTransform.Microsoft.Alpha(Opacity="+opacity+")";	
}
getWindowHeight = function() { return document.body.clientHeight; };
getWindowWidth = function() { return document.body.clientWidth; };

/* Some useful functions */
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

function URL(s) {
	var i = s.indexOf("://");
	if (i > 0) {
		this.protocol = s.substr(0, i).toLowerCase();
		s = s.substr(i+3);
		i = s.indexOf("/");
		this.host = s.substr(0,i);
		s = s.substr(i);
		i = this.host.indexOf(":");
		if (i > 0) {
			this.port = this.host.substr(i+1);
			this.host = this.host.substr(0,i);
		} else
			this.port = null;
	} else {
		this.protocol = "http";
		this.host = window.location.hostname;
		this.port = window.location.port;
	}
	i = s.indexOf('?');
	this.params = new Object();
	if (i > 0) {
		this.path = s.substr(0,i);
		s = s.substr(i+1);
		while (s.length > 0 && (i = s.indexOf('&')) >= 0) {
			var p = s.substr(0, i);
			s = s.substr(i+1);
			i = p.indexOf('=');
			if (i > 0)
				this.params[decodeURIComponent(p.substr(0,i))] = decodeURIComponent(p.substr(i+1));
			else
				this.params[decodeURIComponent(p)] = "";
		}
		if (s.length > 0) {
			i = s.indexOf('=');
			if (i > 0)
				this.params[decodeURIComponent(s.substr(0,i))] = decodeURIComponent(s.substr(i+1));
			else
				this.params[decodeURIComponent(s)] = "";
		}
	} else
		this.path = s;

	this.toString = function() {
		var s = this.protocol+"://"+this.host;
		if (this.port != null) s += ":"+this.port;
		s += this.path;
		var first = true;
		for (var name in this.params) {
			if (first) { s += "?"; first = false; } else s += "&";
			s += encodeURIComponent(name) + "=" + encodeURIComponent(this.params[name]);
		}
		return s;
	};
}

function PNEvent() {
	this.listeners = [];
	this.add_listener = function(listener) { this.listeners.push(listener); };
	this.fire = function() { for (var i = 0; i < this.listeners.length; ++i) this.listeners[i](); };
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
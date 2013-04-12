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

function load_css(path) {
	var link = document.createElement("LINK");
	link.rel = "stylesheet";
	link.type = "text/css";
	link.href = "/static"+path;
	document.getElementsByTagName("HEAD")[0].appendChild(link);
}
function load_js(path,onload) {
	if (typeof path == "string") {
		var s = document.createElement("SCRIPT");
		s.type = "text/javascript";
		s.src = "/static"+path;
		s.onload = function() { setTimeout(onload,1) };
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	} else {
		var waiting = path.length;
		for (var i = 0; i < path.length; ++i) {
			var s = document.createElement("SCRIPT");
			s.type = "text/javascript";
			s.src = "/static"+path[i];
			s.onload = function() { waiting--; if (waiting == 0) setTimeout(onload,1); };
			document.getElementsByTagName("HEAD")[0].appendChild(s);
		}
	}
}

function popup_page(title,icon,component,page) {
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
		"<tr><td colspan=2 class='set_height'><iframe src='/dynamic/"+component+"/sub_page/"+page+"' frameborder=0 style='width:100%;height:100%'></iframe></td></tr></table>";
	document.body.appendChild(div);
	make_height_of_td_compatible();
}

function listenEvent(elem, type, handler) {
	if (elem.addEventListener)
	     elem.addEventListener(type,handler,false);
	 else if (elem.attachEvent)
	     elem.attachEvent('on'+type,handler); 
}

function make_height_of_td_compatible() {
	var list = document.getElementsByClassName("set_height");
	for (var i = 0; i < list.length; ++i)
		list[i].style.height = "";
	for (var i = 0; i < list.length; ++i)
		list[i].style.height = list[i].scrollHeight+"px";
}
window.onload = make_height_of_td_compatible;
listenEvent(window, 'resize', make_height_of_td_compatible);
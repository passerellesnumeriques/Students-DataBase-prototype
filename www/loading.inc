<!DOCTYPE html>
<html>
<head>
	<title>PN Students Database</title>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8"/>
	<style type='text/css'>body,html{width:100%;height:100%;margin:0px;padding:0px;}</style>
</head>
<body>
<table id='__loading_table' style='width:100%;height:100%;margin:0px;padding:0px;z-index:100;background-color:white;position:fixed;top:0px;left:0px;'>
<tr><td valign=middle align=center>
<img src='/static/common/images/logo.png'/>
<br/>
<img src='/static/common/images/loading_100.gif'/>
</td></tr>
</table>
<script type='text/javascript'>
var _loading_ready = 0;
function _add_javascript(url, callback) {
	var head = document.getElementsByTagName("HEAD")[0];
	var s = document.createElement("SCRIPT");
	s.type = "text/javascript";
	head.appendChild(s);
	s.src = url;
	s.onload = function() { this._loaded = true; if (callback) callback(); };
	s.onreadystatechange = function() { if (this.readyState == 'loaded' || this.readyState == 'complete') { this._loaded = true; if (callback) callback(); this.onreadystatechange = null; } };

}
window.pn_loading = document.all ? document.all['__loading_table'] : document.getElementById('__loading_table');
function __load_enter_page() {
	var frame = document.createElement("IFRAME");
	frame.frameBorder = "0";
	frame.style.width = "100%";
	frame.style.height = "100%";
	var url = "/dynamic/application/page/enter";
	if (location.search) url += location.search;
	frame.src = url;
	frame.style.position = "fixed";
	frame.style.top = "0px";
	frame.style.left = "0px";
	frame.style.zIndex = "0";
	document.body.appendChild(frame);
}

// at least those 3 scripts must be loaded
_add_javascript("/static/common/js/utils.js",function() { if (++_loading_ready == 4) __load_enter_page(); });
_add_javascript("/static/common/js/ajax.js",function() { if (++_loading_ready == 4) __load_enter_page(); });
_add_javascript("/static/common/js/locale.js",function() { if (++_loading_ready == 4) __load_enter_page(); });
_add_javascript("/static/application/databaselock.js",function() { if (++_loading_ready == 4) __load_enter_page(); });
// optional scripts
_add_javascript("/static/common/js/animation.js");

function pn_loading_start() {
	window.pn_loading.style.visibility = 'visible';
	setOpacity(window.pn_loading, 1);
}
function pn_loading_end() {
	if (typeof animation == 'undefined') {
		setTimeout(pn_loading_end, 100);
		return;
	}
	if (typeof setOpacity == 'undefined') {
		setTimeout(pn_loading_end, 100);
		return;
	}
	animation.fadeOut(window.pn_loading,1000);
}
</script>
</body>
</html>
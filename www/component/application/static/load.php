<!DOCTYPE html>
<html>
<head>
<script type='text/javascript'>
var scripts = [];
var css = [];
var images = [];
<?php
function browse($path, $url) {
	$dir = @opendir($path);
	if ($dir == null) return;
	while (($filename = readdir($dir)) <> null) {
		if (substr($filename, 0, 1) == ".") continue;
		if (is_dir($path."/".$filename))
			browse($path."/".$filename, $url.$filename."/");
		else {
			$i = strrpos($filename, ".");
			if ($i === FALSE) continue;
			$ext = substr($filename, $i+1);
			switch ($ext) {
				case "js": echo "scripts.push({url:\"".$url.$filename."\",size:".filesize($path."/".$filename)."});\n"; break;
				case "css": echo "css.push({url:\"".$url.$filename."\",size:".filesize($path."/".$filename)."});\n"; break;
				case "gif":
				case "jpg":
				case "jpeg":
				case "png":
					echo "images.push({url:\"".$url.$filename."\",size:".filesize($path."/".$filename)."});\n";
					break;
			}
		}
	}
	closedir($dir);
}
function browse_components($path) {
	$dir = @opendir($path);
	if ($dir == null) return;
	while (($filename = readdir($dir)) <> null) {
		if (substr($filename, 0, 1) == ".") continue;
		if (is_dir($path."/".$filename))
			browse($path."/".$filename."/static", "/static/".$filename."/");
	}
	closedir($dir);
}
browse($_SERVER["DOCUMENT_ROOT"]."/common", "/static/common/");
browse_components($_SERVER["DOCUMENT_ROOT"]."/component");
?>

window.total_size = 0;
window.size_done = 0;
window.stop_loading = false;
window.loading_start = 0;
for (var i = 0; i < scripts.length; ++i) window.total_size += scripts[i].size;
for (var i = 0; i < css.length; ++i) window.total_size += css[i].size;
for (var i = 0; i < images.length; ++i) window.total_size += images[i].size;

window.size_str = function(size) {
	if (size > 1024*1024) {
		var mega = Math.floor(size/(1024*1024));
		size -= mega*1024*1024;
		size = Math.floor(size * 10 /(1024*1024));
		return mega+"."+size+"MB";
	}
	if (size > 1024) {
		var mega = Math.floor(size/1024);
		size -= mega*1024;
		size = Math.floor(size * 10 /1024);
		return mega+"."+size+"KB";
	}
	return size +"B";
}

window.continue_loading = function() {
	var loading = document.all ? document.all['loading'] : document.getElementById('loading');
	var container = document.all ? document.all['container'] : document.getElementById('container');
	loading.style.height = container.offsetHeight+"px";
	loading.style.width = Math.round(window.size_done*container.offsetWidth/window.total_size)+"px";
	if (window.loading_start == 0) window.loading_start = new Date().getTime();
	if (window.size_done > 0) {
		var text = document.all ? window.parent.document.all['application_loading_text'] : window.parent.document.getElementById('application_loading_text');
		var now = new Date().getTime();
		now -= window.loading_start;
		text.innerHTML = window.size_str(window.size_done)+" / "+window.size_str(window.total_size) + (now > 300 ? " ("+window.size_str(window.size_done*1000/now)+"/s.)" : "");
	}

	if (!window.stop_loading && scripts.length > 0) {
		var script = scripts[0];
		scripts.splice(0,1);
		var s = document.createElement("SCRIPT");
		s.type = "text/javascript";
		s.onload = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); this.onload = this.onreadystatechange = null; };
		s.onerror = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); };
		s.onreadystatechange = function() { if (this.readyState == 'loaded' || this.readyState == 'complete') { window.size_done += script.size; setTimeout(window.continue_loading,1); this.onload = this.onreadystatechange = null; } };
		s.src = script.url;
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	} else if (!window.stop_loading && css.length > 0) {
		var script = css[0];
		css.splice(0,1);
		var s = document.createElement("LINK");
		s.rel = "stylesheet";
		s.type = "text/css";
		s.onload = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); };
		s.onerror = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); };
		s.href = script.url;
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	} else if (!window.stop_loading && images.length > 0) {
		var script = images[0];
		images.splice(0,1);
		var s = document.createElement("IMG");
		s.onload = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); };
		s.onerror = function() { window.size_done += script.size; setTimeout(window.continue_loading,1); };
		s.src = script.url;
		s.style.position = "fixed";
		s.style.top = (container.offsetHeight+10)+"px";
		document.body.appendChild(s);
	} else if (window.stop_loading) {
		var e = document.all ? window.parent.document.all['application_loading'] : window.parent.document.getElementById('application_loading');
		e.parentNode.removeChild(e);
	} else if (window.size_done == window.total_size) {
		window.loading_end = new Date().getTime();
		var e = document.all ? window.parent.document.all['application_loading'] : window.parent.document.getElementById('application_loading');
		for (var i = 0; i < e.childNodes.length; ++i) {
			if (e.childNodes[i].nodeType != 1) continue;
			e.childNodes[i].style.visibility = 'hidden';
			e.childNodes[i].style.position = 'absolute';
		}
		var status = document.createElement("SPAN");
		e.insertBefore(status, e.childNodes[0]);
		status.innerHTML = window.size_str(window.total_size)+", "+Math.round((window.loading_end-window.loading_start)/1000)+"s.";
		animation.fadeOut(e,2000,function(){
			e.parentNode.removeChild(e);
		});
	}
}
</script>
<style type='text/css'>
html,body,#container {
	width: 100%;
	height: 100%;
	margin: 0px;
	padding: 0px;
}
#loading{
	position: fixed;
	top: 0px;
	left: 0px;
	width: 0px;
	background-color: #D0D0FF;
}
#container {
	text-align: center;
}
</style>
</head>
<body onload='setTimeout(window.continue_loading,<?php echo $_GET['delay']?>);'>
<div id='container'>
<div id='loading'></div>
</div>
</body>
</html>
<?php
?>
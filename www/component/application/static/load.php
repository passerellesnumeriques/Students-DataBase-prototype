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

var total_size = 0;
var size_done = 0;
for (var i = 0; i < scripts.length; ++i) total_size += scripts[i].size;
for (var i = 0; i < css.length; ++i) total_size += css[i].size;
for (var i = 0; i < images.length; ++i) total_size += images[i].size;

function continue_loading() {
	var loading = document.all ? document.all['loading'] : document.getElementById('loading');
	var container = document.all ? document.all['container'] : document.getElementById('container');
	loading.style.height = container.offsetHeight+"px";
	loading.style.width = Math.round(size_done*container.offsetWidth/total_size)+"px";

	if (scripts.length > 0) {
		var script = scripts[0];
		scripts.splice(0,1);
		var s = document.createElement("SCRIPT");
		s.type = "text/javascript";
		s.onload = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.onerror = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.onreadystatechange = function() { if (this.readyState == 'loaded' || this.readyState == 'complete') { size_done += script.size; setTimeout(continue_loading,1); this.onreadystatechange = null; } };
		s.src = script.url;
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	} else if (css.length > 0) {
		var script = css[0];
		css.splice(0,1);
		var s = document.createElement("LINK");
		s.rel = "stylesheet";
		s.type = "text/css";
		s.onload = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.onerror = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.href = script.url;
		document.getElementsByTagName("HEAD")[0].appendChild(s);
	} else if (images.length > 0) {
		var script = images[0];
		images.splice(0,1);
		var s = document.createElement("IMG");
		s.onload = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.onerror = function() { size_done += script.size; setTimeout(continue_loading,1); };
		s.src = script.url;
		s.style.position = "fixed";
		s.style.top = (container.offsetHeight+10)+"px";
		document.body.appendChild(s);
	} else {
		var e = document.all ? window.parent.document.all['application_loading'] : window.parent.document.getElementById('application_loading');
		e.parentNode.removeChild(e);
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
</style>
</head>
<body onload='setTimeout(continue_loading,<?php echo $_GET['delay']?>);'>
<div id='container'>
<div id='loading'></div>
</div>
</body>
</html>
<?php 
?>
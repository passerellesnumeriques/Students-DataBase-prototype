<div style='background-color:#A0A0FF'>
Geonames DataBase
&nbsp;
<a href='#' onclick='location.reload();return false'>Reload</a>
&nbsp;
<span id='loaded_count'></span>
</div>
<?php
global $country_code;
$country_code = $_GET["country"];
$res = DataBase::execute("SELECT `geonames_code` FROM `geography_match`.`country` WHERE `code`='".$country_code."'");
if ($res) $res = DataBase::next_row($res);
if ($res <> null) $country_code = $res["geonames_code"];
else die("Country ".$country_code." was not imported from Geonames");

$this->add_javascript("/static/geography/edit_adm.js");

/*
$_GET["country"] = $country_code;
$_GET["level"] = "1";
echo "<div id='top_level'>";
include "component/geography/service/get_geonames.php";
echo "</div>";
*/
?>
<div id='top_level'></div>
<script type='text/javascript'>
new adm_editor(document.getElementById('top_level'), function(level,parent) {
	var url = "/dynamic/geography/service/get_geonames?country=<?php echo $country_code;?>&level="+level;
	var e = parent;
	for (var i = level; i > 1; --i) {
		
	}
});
/*var loading_count = 0;
var loaded_count = 0;
function loaded(div,success,levels) {
	loaded_count++;
	loading_count--;
	//document.getElementById('loaded_count').innerHTML = ""+loaded_count+" ("+loading_count+")";
	div.removeAttribute("loading");
	if (div._handler)
		div._handler(success ? div : null);
	if (success && levels) {
		for (var i = 0; i < div.childNodes.length; ++i) {
			var e = div.childNodes[i];
			if (e.nodeType != 1) continue;
			if (!e.hasAttribute("imported")) continue;
			background_load(e, levels-1);
		}
	}
}
function load(div,url,loading,levels) {
	div.setAttribute("loading","true");
	ajax.call("GET","/dynamic/geography/service/get_geonames?"+url,null,null,function(error) {
		div.innerHTML = error;
		if (loading) loading.parentNode.removeChild(loading);
		loaded(div,false,levels);
	},function(xhr) {
		div.innerHTML = xhr.responseText;
		if (loading) loading.parentNode.removeChild(loading);
		loaded(div,true,levels);
	},false);
}
var to_load_bg = [];
function background_load(div_name, load_levels) {
	var div = div_name.nextSibling;
	if (!div) return;
	if (!div.hasAttribute("url")) return;
	var url = div.getAttribute("url");
	div.removeAttribute("url");
	var loading = document.createElement("IMG");
	loading.src = "/static/common/images/loading.gif";
	div_name.appendChild(loading);
	div.innerHTML = "<img src='/static/common/images/loading.gif'/>";
	to_load_bg.push([div,url,loading, load_levels]);
	if (to_load_bg.length == 1)
		setTimeout(todo_bg,1000);
}
function todo_bg(count) {
	var max = loaded_count < 1000 ? 100 : loaded_count < 2500 ? 50 : loaded_count < 5000 ? 25 : loaded_count < 10000 ? 10 : 5;
	if (loading_count >= max && to_load_bg.length > 0) { setTimeout(todo_bg,500); return; }
	if (to_load_bg.length == 0) return;
	var e = to_load_bg[0];
	to_load_bg.splice(0,1);
	loading_count++;
	load(e[0],e[1],e[2],e[3]);
	if (to_load_bg.length > 0) {
		if (loading_count < max) {
			if (!count || count < 5 || loading_count < max/2)
				todo_bg(count ? 1 : count++);
			else
				setTimeout(todo_bg,50);
		} else
			setTimeout(todo_bg,500);
	}
}
function load_top_level() {
	var top = document.getElementById('top_level');
	for (var i = 0; i < top.childNodes.length; ++i) {
		var e = top.childNodes[i];
		if (e.nodeType != 1) continue;
		if (!e.hasAttribute("imported")) continue;
		background_load(e, 1);
	}
}
function expand(img,handler) {
	var div = img.parentNode;
	div = div.nextSibling;
	img.src = '/static/common/images/arrow_down_10.gif';
	img.onclick = function() { collapse(this); }
	if (!div) { if (handler) handler(null); return; }
	if (div.style.visibility != "hidden") { if (handler) handler(null); return; }
	div.style.visibility = 'visible';
	div.style.position = 'static';
	if (div.hasAttribute("url")) {
		var url = div.getAttribute("url");
		div.removeAttribute("url");
		div.innerHTML = "<img src='/static/common/images/loading.gif'/>";
		div._handler = handler;
		load(div, url);
	} else if (div.hasAttribute("loading"))
		div._handler = handler;
	else
		if (handler) handler(null);
}
function collapse(img) {
	var div = img.parentNode;
	div = div.nextSibling;
	if (!div) return;
	div.style.visibility = 'hidden';
	div.style.position = 'absolute';
	img.src = '/static/common/images/arrow_right_10.gif';
	img.onclick = function() { expand(this); }
	for (var i = 0; i < div.childNodes.length; ++i) {
		if (div.childNodes[i].hasAttribute("imported")) {
			for (var j = 0; j < div.childNodes[i].childNodes.length; ++j)
				if (div.childNodes[i].childNodes[j].nodeName == "IMG") {
					collapse(div.childNodes[i].childNodes[j]);
					break;
				}
		}
	}
}
var toimport = null;
function update_import(cb,level,id) {
	if (cb.checked) {
		if (toimport != null) toimport.cb.checked = "";
		toimport = {cb:cb,level:level,id:id};
	} else
		toimport = null;
}
function imported() {
	if (toimport == null) return;
	var div_name = toimport.cb.parentNode;
	div_name.removeChild(toimport.cb);
	var div_content = div_name.nextSibling;
	var has_content = false;
	var update_color = function(has_content) {
		div_name.style.backgroundColor = has_content ? "#C0C0C0" : "#808080";
		div_name.setAttribute("imported","true");
		update_parent_color(div_name);
	};
	if (div_content) {
		if (div_content.hasAttribute("level") && div_content.getAttribute("level") == toimport.level+1) {
			if (div_content.style.visibility == "hidden") {
				has_content = true;
				for (var i = 0; i < div_name.childNodes.length; ++i)
					if (div_name.childNodes[i].nodeName == "IMG") {
						expand(div_name.childNodes[i],function(div_content){
							setTimeout(function() {
								has_content = false;
								for (var i = 0; i < div_content.childNodes.length; ++i) {
									if (!div_content.childNodes[i].hasAttribute("imported")) continue;
									if (div_content.getAttribute("imported") != "true") {
										has_content = true;
										break;
									}
								}
								update_color(has_content);
							},1);
						});
						break;
					}
			} else {
				for (var i = 0; i < div_content.childNodes.length; ++i) {
					if (!div_content.childNodes[i].hasAttribute("imported")) continue;
					if (div_content.getAttribute("imported") != "true") {
						has_content = true;
						break;
					}
				}
			}
		}
	}
	update_color(has_content);
	toimport = null;
}
function update_parent_color(div) {
	var container = div.parentNode;
	if (!container.hasAttribute("level")) return; // top level
	var parent = container.previousSibling;
	if (!parent) return;
	var has_content = false;
	for (var i = 0; i < container.childNodes.length; ++i)
		if (container.childNodes[i].getAttribute("imported") != "true") {
			has_content = true;
			break;
		}
	if (!has_content) {
		parent.style.backgroundColor = has_content ? "#C0C0C0" : "#808080";
		parent.setAttribute("imported","true");
		update_parent_color(parent);
		for (var i = 0; i < parent.childNodes.length; ++i)
			if (parent.childNodes[i].nodeName == "IMG") {
				collapse(parent.childNodes[i]);
				break;
			}
	}
}
load_top_level();

function get_level(level, parent) {
	var top = document.getElementById('top_level');
	return _get_level(level, parent, 1, 0, top);
}
function _get_level(level, parent, current_level, current_parent, current_div) {
	if (parent == current_parent) {
		// we are on the required parent => let's take all corresponding to the requested level
		var list = [];
		_get_all_level(list, level, current_level, current_div);
		return list;
	}
	if (current_level >= level) return null; // already too deep
	// look for the parent
	for (var i = 0; i < current_div.childNodes.length; ++i) {
		var e = current_div.childNodes[i];
		if (e.nodeType != 1) continue;
		if (!e.hasAttribute("_content_id")) continue;
		if (e.hasAttribute("url")) {
			// not yet loaded
			load_now(e);
		}
		var list = _get_level(level, parent, current_level+1, e.getAttribute("_content_id"), e);
		if (list != null) return list;
	}
	return null;
}
function _get_all_level(list, level, current_level, current_div) {
	if (level == current_level) {
		for (var i = 0; i < current_div.childNodes.length; ++i) {
			var e = current_div.childNodes[i];
			if (e.nodeType != 1) continue;
			if (!e.hasAttribute("imported")) continue;
			list.push({name:e.getAttribute('_name')});
		}
	} else if (level > current_level) {
		for (var i = 0; i < current_div.childNodes.length; ++i) {
			var e = current_div.childNodes[i];
			if (e.nodeType != 1) continue;
			if (!e.hasAttribute("_content_id")) continue;
			if (e.hasAttribute("url")) {
				// not yet loaded
				load_now(e);
			}
			_get_all_level(list, level, current_level+1, e);
		}
	}
}
function load_now(e) {
	var url = e.getAttribute("url");
	e.removeAttribute("url");
	ajax.call("GET","/dynamic/geography/service/get_geonames?"+url,null,null,function(error) {
		e.innerHTML = error;
	},function(xhr) {
		e.innerHTML = xhr.responseText;
	},true);
}*/
</script>

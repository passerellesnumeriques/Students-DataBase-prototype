<div style='background-color:#A0A0FF'>
GADM DataBase
&nbsp;
<a href='#' onclick='location.reload();return false'>Reload</a>
</div>
<?php
global $country_code, $types;

$country_code = $_GET["country"];
$res = DataBase::execute("SELECT `gadm_code` FROM `geography_match`.`country` WHERE `code`='".$country_code."'");
if ($res) $res = DataBase::next_row($res);
if ($res <> null) $country_code = $res["gadm_code"];
else die("Country ".$country_code." was not imported from GADM");

$types = array(array(),array(),array(),array(),array());

function generate_level($level, $parent_id) {
	global $country_code,$types;
	$res = DataBase::execute("SELECT * FROM `gadm`.`adm".$level."` WHERE `country`='".$country_code."'".($parent_id <> null ? " AND `parent_id`=".$parent_id : "")." ORDER BY `name`");
	while (($r = DataBase::next_row($res)) <> null) {
		$name = utf8_encode($r["name"]);
		echo "<div imported='false' _name='".htmlspecialchars($name, ENT_QUOTES, "UTF-8")."'>";
		echo "<img src='/static/common/images/arrow_right_10.gif' style='cursor:pointer' onclick='expand(this);'/> ";
		echo $name;
		$type = $r["type"];
		if ($type <> null) $type = utf8_encode($type);
		$type_en = $r["type_en"];
		if ($type_en <> null) $type_en = utf8_encode($type_en);
		echo " <span style='font-style:italic;color:#808080'>";
		echo $type_en;
		echo " (";
		echo $type;
		echo ")";
		echo "</span> ";
		echo "<input type='checkbox' onchange=\"update_import(this,".$level.",".$r["id"].");\"/>";
		echo "</div>";
		$found = false;
		foreach ($types[$level-1] as $t) {
			if ($t[0] == $type_en && $t[1] == $type) { $found = true; break; }
		}
		if (!$found) array_push($types[$level-1], array($type_en,$type));
		if ($level < 5) {
			echo "<div style='margin-left:15px;visibility:hidden;position:absolute;top:-10000px' _content_id='".$r["id"]."'>";
			generate_level($level+1, $r["id"]);
			echo "</div>";
		}
	}
}
set_time_limit(120);
echo "<div id='top_level'>";
generate_level(1, null);
echo "</div>";
?>
<script type='text/javascript'>
var types = [
<?php
foreach ($types as $ty) {
	echo "[";
	foreach ($ty as $t)
		echo "{e:".json_encode($t[0]).",l:".json_encode($t[1])."},";
	echo "],";
}
?>
];
var toimport = null;
function expand(img) {
	var div = img.parentNode;
	div = div.nextSibling;
	img.src = '/static/common/images/arrow_down_10.gif';
	img.onclick = function() { collapse(this); }
	if (!div) return;
	if (div.style.visibility != "hidden") return;
	div.style.visibility = 'visible';
	div.style.position = 'static';
}
function collapse(img) {
	var div = img.parentNode;
	div = div.nextSibling;
	div.style.visibility = 'hidden';
	div.style.position = 'absolute';
	img.src = '/static/common/images/arrow_right_10.gif';
	img.onclick = function() { expand(this); }
	for (var i = 0; i < div.childNodes.length; ++i) {
		if (div.childNodes[i].nodeType == 1 && div.childNodes[i].hasAttribute("imported")) {
			for (var j = 0; j < div.childNodes[i].childNodes.length; ++j)
				if (div.childNodes[i].childNodes[j].nodeName == "IMG") {
					collapse(div.childNodes[i].childNodes[j]);
					break;
				}
		}
	}
}
function update_import(cb,level,id) {
	if (cb.checked) {
		if (toimport != null) toimport.cb.checked = "";
		toimport = {cb:cb,level:level,id:id};
	} else
		toimport = null;
}
function imported() {
	// TODO
}
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
			_get_all_level(list, level, current_level+1, e);
		}
	}
}
</script>

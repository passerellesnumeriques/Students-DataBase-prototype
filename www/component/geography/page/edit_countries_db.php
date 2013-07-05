<?php
$this->add_javascript("/static/common/js/form.js");
?>
<div style='background-color:#A0A0FF'>
Geography DataBase
&nbsp; &nbsp;
<a href='#' onclick='geo_import();return false'>Import from temp</a>
&nbsp; &nbsp;
<a href='#' onclick='geo_import_match();return false'>Import all matching codes</a>
</div>
<table style='border:1px solid black' rules='all'>
<tr><th>Code</th><th>Name</th><th>Parent</th><th>Latitude</th><th>Longitude</th><th>Timestamp</th></tr>
<?php
$res = DataBase::execute("SELECT * FROM `geography`.`country` ORDER BY `code`");
$codes = array();
while (($r = DataBase::next_row($res)) <> null) {
	echo "<tr>";
	echo "<td><a href='edit_country?code=".$r["code"]."' target='_parent'>".$r["code"]."</a></td>";
	$res2 = DataBase::execute("SELECT `geography`.`name`.`name` AS `n`,`geography`.`country_name`.`lang` AS `l` FROM `geography`.`country_name` LEFT JOIN `geography`.`name` ON `geography`.`name`.`id`=`geography`.`country_name`.`name` WHERE `geography`.`country_name`.`code`='".$r["code"]."'");
	$names = array();
	while (($r2 = DataBase::next_row($res2)) <> null)
		$names[$r2["l"]] = utf8_encode($r2["n"]);
	echo "<td nowrap='nowrap'>".(isset($names["__"]) ? $names["__"] : $names["en"])."</td>";
	echo "<td>".$r["sovereign"]."</td>";
	echo "<td>".$r["latitude"]."</td>";
	echo "<td>".$r["longitude"]."</td>";
	echo "<td>".$r["timezone"]."</td>";
	echo "</tr>";
}
?>
</table>
<script type='text/javascript'>
function geo_import() {
	var f1 = window.parent.frames["geonames"];
	var f2 = window.parent.frames["gadm"];
	var code1 = get_radio_value(f1.document.forms["import"], "code");
	var code2 = get_radio_value(f2.document.forms["import"], "code");
	if (code1 == null && code2 == null) { alert("Select countries to import first"); return; }
	var s = "Import ";
	if (code1 == null) s += "nothing from geonames, "; else s+= code1+" from geonames, ";
	if (code2 == null) s += "nothing from gadm, "; else s+= code2+" from gadm, ";
	if (confirm(s)) {
		ajax.post_parse_result('/dynamic/geography/service/import_country',{geonames:code1,gadm:code2},function(result){
			if (result)
				window.parent.location.reload();
		});
	}
}
var import_match_nb;
var import_match_nb_ok;
function geo_import_match() {
	var f1 = window.parent.frames["geonames"];
	var f2 = window.parent.frames["gadm"];
	var r1 = f1.document.forms["import"].elements["code"];
	var r2 = f2.document.forms["import"].elements["code"];
	import_match_nb = 0;
	import_match_nb_ok = 0;
	for (var i = 0; i < r1.length; ++i) {
		var code1 = r1[i].value;
		for (var j = 0; j < r2.length; ++j) {
			if (r2[j].value == code1) {
				import_match_nb++;
				ajax.post_parse_result('/dynamic/geography/service/import_country',{geonames:code1,gadm:code1},function(result){
					if (result) {
						import_match_nb_ok++;
					}
				});
			}
		}
	}
	setTimeout(check_import_match,1000);
}
function check_import_match() {
	if (import_match_nb == import_match_nb_ok)
		window.parent.location.reload();
	else
		setTimeout(check_import_match,1000);
}
</script>
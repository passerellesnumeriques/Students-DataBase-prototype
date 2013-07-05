<?php 
$this->add_javascript("/static/common/js/collapsable_section/collapsable_section.js");
$this->add_stylesheet("/static/common/js/collapsable_section/collapsable_section.css");
$this->add_javascript("/static/common/js/splitter_vertical/splitter_vertical.js");
$this->add_stylesheet("/static/common/js/splitter_vertical/splitter_vertical.css");
$this->add_javascript("/static/geography/edit/adm.js");
$this->add_javascript("/static/geography/edit/import_adm_wizard.js");

$country_code = $_GET["country"];
$divisions = array();
$res = DataBase::execute("SELECT * FROM `geography`.`country_division` WHERE country='".$country_code."' ORDER BY `level`");
if ($res)
	while (($r = DataBase::next_row($res)) <> null)
		array_push($divisions, $r);

$res = DataBase::execute("SELECT `geonames_code`,`gadm_code` FROM `geography_match`.`country` WHERE `code`='".$country_code."'");
if ($res) $res = DataBase::next_row($res);
$country_code_geonames = null;
$country_code_gadm = null;
if ($res <> null) {
	$country_code_geonames = $res["geonames_code"];
	$country_code_gadm = $res["gadm_code"];
}

?>
<div id='split1' style='height:100%'>
	<div style='overflow:auto'>
		<div>Geography Database</div>
		<div id='db_divisions' style='border:1px solid black;width:200px'>
  			<div class='collapsable_section_header'>Divisions</div>
  			<div class='collapsable_section_content'>
				<table style='border:1px solid black' rules='all'><tbody id='table_divisions'>
				<tr><th>Level</th><th>English name</th><th>Local name</th></tr>
				<?php
				foreach ($divisions as $d) {
					echo "<tr>";
					echo "<td>".$d["level"]."</td>";
					echo "<td>".utf8_encode($d["english_name"])."</td>";
					echo "<td>".utf8_encode($d["name"])."</td>";
					echo "</tr>";
				}
				?>
				<tr><td colspan=3>
				<a href='#' onclick='add_division();return false;'>Create sub-division</a>
				</td></tr>
				</tbody></table>
  			</div>
		</div>
		<div>
			<a href='#' onclick="new import_adm_wizard(1,0);return false;">Import Wizard for level 1</a>
		</div>
		<div id='db'>
		</div>
	</div>
	<div id='split2'>
		<div style='overflow:auto'>
			<div>Geonames</div>
			<div id='geonames'></div>
		</div>
		<div style='overflow:auto'>
			<div>GADM</div>
			<div id='gadm'></div>
		</div>
	</div>
</div>
<script type='text/javascript'>
new splitter_vertical('split2',0.5);
new splitter_vertical('split1',0.5);
new collapsable_section('db_divisions');
db = new adm_editor(document.getElementById('db'),function(level,parent){
	return "/dynamic/geography/service/get_adm?country=<?php echo $country_code?>&parent="+(parent ? parent.id : 0);
},function(level,parent,result){
	var list = [];
	for (var i = 0; i < result.length; ++i) {
		var a = new adm(parent.editor, parent, level, result[i].id, result[i].name, null, null);
		list.push(a);
	}
	return list;
});
<?php
if ($country_code_geonames <> null) { 
?>
geonames = new adm_editor(document.getElementById('geonames'),function(level,parent){
	var url = "/dynamic/geography/service/edit/get_geonames?country=<?php echo $country_code_geonames?>&level="+level;
	for (var i = 1; i < level; ++i)
		url += "&adm"+i+"="+parent["adm"+i];
	return url;
},function(level,parent,result){
	var list = [];
	for (var i = 0; i < result.length; ++i) {
		var a = new adm(parent.editor, parent, level, result[i].id, result[i].name, null, null);
		for (var j = 1; j < level; ++j)
			a["adm"+j] = parent["adm"+j];
		if (level < 5)
			a["adm"+level] = result[i]["adm"+level];
		list.push(a);
	}
	return list;
});
<?php
} 
?>
<?php
if ($country_code_geonames <> null) { 
?>
gadm = new adm_editor(document.getElementById('gadm'),function(level,parent){
	return "/dynamic/geography/service/edit/get_gadm?country=<?php echo $country_code_gadm?>&level="+level+"&parent="+parent.id;
},function(level,parent,result){
	var list = [];
	for (var i = 0; i < result.length; ++i) {
		var a = new adm(parent.editor, parent, level, result[i].id, result[i].name, result[i].type_en, result[i].type);
		list.push(a);
	}
	return list;
});
<?php
} 
?>
</script>
<div style='background-color:#A0A0FF'>
Geography DataBase
&nbsp;
<a href='#' onclick='location.reload();return false'>Reload</a>
</div>
<?php
$country_code = $_GET["country"];
$divisions = array();
$res = DataBase::execute("SELECT * FROM `geography`.`country_division` WHERE country='".$country_code."' ORDER BY `level`");
if ($res)
	while (($r = DataBase::next_row($res)) <> null)
		array_push($divisions, $r);
?>
Country: <?php echo $country_code?><br/>
<table style='border:1px solid black' rules='all'><tbody id='table_divisions'>
<tr><th colspan=3>Divisions</th></tr>
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
<?php
if (count($divisions) > 0) {
	echo "<table style='border:1px solid black' rules='all'>";
	echo "<tr><th>".utf8_encode($divisions[0]["english_name"])." (".utf8_encode($divisions[0]["name"]).")"."</th></tr>";
	echo "<tr><td>";
	echo "<a href='#' onclick='import_adm(this,0);return false;'>Import here</a>";
	echo " &nbsp; ";
	echo "<a href='#' onclick='import_adm_wizard(this,0);return false;'>Import wizard here</a>";
	echo "</td></tr>";
	echo "</table>";
}
?>
<script type='text/javascript'>
function add_division() {
	var level = 1;
	var table = document.getElementById('table_divisions');
	for (var i = 0; i < table.childNodes.length; ++i) if (table.childNodes[i].nodeType == 1) level++;
	level -= 3;
	add_javascript("/static/common/js/popup_window/popup_window.js",function() {
		var p = new popup_window("New division","");
		var table = document.createElement("TABLE");
		var tr, td, input_en, input_lo, select;
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TD"));
		td.innerHTML = "English name";
		tr.appendChild(td = document.createElement("TD"));
		td.appendChild(input_en = document.createElement("INPUT"));
		input_en.type = 'text';
		table.appendChild(tr = document.createElement("TR"));
		tr.appendChild(td = document.createElement("TD"));
		td.innerHTML = "Local name";
		tr.appendChild(td = document.createElement("TD"));
		td.appendChild(input_lo = document.createElement("INPUT"));
		input_lo.type = 'text';
		var frame = window.parent.frames["gadm"];
		for (var i = 0; i < frame.types.length; ++i) {
			table.appendChild(tr = document.createElement("TR"));
			tr.appendChild(td = document.createElement("TD"));
			td.innerHTML = "Import from GADM Level "+(i+1);
			tr.appendChild(td = document.createElement("TD"));
			td.appendChild(select = document.createElement("SELECT"));
			var o = document.createElement("OPTION");
			o.text = "";
			o.value = 51;
			select.add(o);
			for (var j = 0; j < frame.types[i].length; ++j) {
				var n = frame.types[i][j];
				var o = document.createElement("OPTION");
				o.text = n.e+" ("+n.l+")";
				o._en = n.e;
				o._lo = n.l;
				select.add(o);
			}
			select.onchange = function(ev) {
				var sel = ev.target;
				var o = sel.options[sel.selectedIndex];
				if (o.value == 51) return;
				input_en.value = o._en;
				input_lo.value = o._lo;
			};
		}
		p.setContent(table);
		p.addOkCancelButtons(function() {
			var en = input_en.value;
			var lo = input_lo.value;
			ajax.post_parse_result("/dynamic/geography/service/add_country_division",{country:"<?php echo $country_code?>",level:level,english:en,local:lo},function(result){
				if (result) {
					var table = document.getElementById('table_divisions');
					var last = table.childNodes[table.childNodes.length-1];
					while (last.nodeType != 1) last = last.previousSibling;
					var tr = document.createElement("TR"); table.insertBefore(tr, last);
					var td;
					tr.appendChild(td = document.createElement("TD"));
					td.innerHTML = level;
					tr.appendChild(td = document.createElement("TD"));
					td.innerHTML = en;
					tr.appendChild(td = document.createElement("TD"));
					td.innerHTML = lo;
					p.close();
				}
			});
		});
		p.show();
	});
}
function import_adm(link,parent) {
	var geonames = window.parent.frames["geonames"];
	var gadm = window.parent.frames["gadm"];
	var s = "";
	if (geonames.toimport == null)
		s += "nothing from geonames";
	else
		s += "id "+geonames.toimport.id+" level "+geonames.toimport.level+" from geonames";
	s+= ", ";
	if (gadm.toimport == null)
		s += "nothing from gadm";
	else
		s += "id "+gadm.toimport.id+" level "+gadm.toimport.level+" from gadm";
	alert("Import "+s);
	geonames.imported();
	gadm.imported();
}
function import_adm_wizard(link,parent) {
	var geonames = window.parent.frames["geonames"];
	var gadm = window.parent.frames["gadm"];
	var db = window;
	window.top.add_javascript("/static/geography/import_adm_wizard.js",function() {
		window.top.geography_import_adm_wizard(db, geonames, gadm, parent);
	});
}
</script>
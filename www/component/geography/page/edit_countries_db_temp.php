<div style='background-color:#A0A0FF'>
Temporary DataBase: <?php echo $_GET["db"];?>
</div>
<?php
$list = array();
if ($_GET["db"] == "geonames") {
	$res = DataBase::execute("SELECT * FROM `geonames`.`country_temp` LEFT JOIN `geonames`.`country_temp_names` ON (`geonames`.`country_temp_names`.`code`=`geonames`.`country_temp`.`code` AND `geonames`.`country_temp_names`.`lang`='__') ORDER BY `geonames`.`country_temp`.`code`");
	while (($r = DataBase::next_row($res)) <> null) {
		$c = array("code"=>$r["code"],"name"=>utf8_encode($r["name"]),"alt"=>array());
		$res2 = DataBase::execute("SELECT * FROM `geonames`.`country_temp_names` WHERE `code`='".$r["code"]."' AND lang != '__'");
		if ($res2)
			while (($r2 = DataBase::next_row($res2)))
				$c["alt"][$r2["lang"]] = utf8_encode($r2["name"]);
		array_push($list, $c);
	}
} else {
	$res = DataBase::execute("SELECT * FROM `gadm`.`country` ORDER BY `code`");
	while (($r = DataBase::next_row($res)) <> null) {
		array_push($list, array("code"=>$r["code"],"name"=>utf8_encode($r["name"]),"sovereign"=>$r["sovereign"]));
	}
}
$res = DataBase::execute("SELECT * FROM `geography`.`country`");
$codes = array();
while (($r = DataBase::next_row($res)) <> null)
	array_push($codes, $r["code"]);

echo "<form name='import'>";
echo "<table style='border:1px solid black' rules='all'>";
echo "<tr><th></th><th>Code</th><th>Name</th><th>Parent</th></tr>";
$after = array();
foreach ($list as $c) {
	if (in_array($c["code"],$codes)) { array_push($after, $c); continue; }
	echo "<tr>";
	echo "<td><input type='radio' name='code' value='".$c["code"]."'/></td>";
	echo "<td>".$c["code"]."</td>";
	echo "<td>";
	if (isset($c["alt"]) && count($c["alt"] > 0)) {
		$s = "";
		foreach ($c["alt"] as $lang=>$name)
			$s .= $lang.": ".$name."  ";
		echo "<span title=".json_encode($s).">";
	}
	echo $c["name"];
	if (isset($c["alt"]) && count($c["alt"] > 0)) echo "</span>";
	echo "</td>";
	if (isset($c["sovereign"])) echo "<td>".$c["sovereign"]."</td>"; else echo "<td></td>";
	echo "</tr>";
}
foreach ($after as $c) {
	echo "<tr style='background-color:#A0A0A0'>";
	echo "<td><input type='radio' name='code' value='".$c["code"]."'/></td><td>".$c["code"]."</td><td>".$c["name"]."</td>";
	if (isset($c["sovereign"])) echo "<td>".$c["sovereign"]."</td>"; else echo "<td></td>";
	echo "</tr>";
}
echo "</table>";
echo "</form>";
?>
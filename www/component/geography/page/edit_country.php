<?php
$code = $_GET["code"];
$c = DataBase::execute("SELECT * FROM `geography`.`country` WHERE `code`='".$code."'");
$c = DataBase::next_row($c);
$res = DataBase::execute("SELECT `geography`.`country_name`.`lang` AS `l`,`geography`.`name`.`name` AS `n` FROM `geography`.`country_name` LEFT JOIN `geography`.`name` ON `geography`.`name`.`id`=`geography`.`country_name`.`name` WHERE `geography`.`country_name`.`code`='".$code."' ORDER BY `geography`.`country_name`.`lang`");
$names = array();
while(($r = DataBase::next_row($res)) <> null)
	$names[$r["l"]] = utf8_encode($r["n"]);
?>
<div style='float:right'>
<iframe src='https://maps.google.com/maps?ll=<?php echo $c["latitude"].",".$c["longitude"]?>&z=5&output=embed' width="300px" height="300px" frameBorder="0"></iframe>
</div>
<a href="edit_adm?country=<?php echo $code?>">Edit Administrative Divisions</a>
<table>
<tr>
	<td>ISO Code</td>
	<td><?php echo $c["code"]?></td>
</tr>
<tr>
	<td>Latitude</td>
	<td><?php echo $c["latitude"]?></td>
</tr>
<tr>
	<td>Longitude</td>
	<td><?php echo $c["longitude"]?></td>
</tr>
<tr>
	<td>Timezone</td>
	<td><?php echo $c["timezone"]?></td>
</tr>
<tr>
	<td>Sovereign</td>
	<td>TODO</td>
</tr>
<tr>
	<td>Default name</td>
	<td><?php echo $names["__"]?></td>
</tr>
<?php
foreach ($names as $lang=>$name) {
	if ($lang == "__") continue;
	echo "<tr><td>".$lang."</td><td>".$name."</td></tr>";
}
?>
</table>
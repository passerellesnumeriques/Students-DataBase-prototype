<?php
$country = $_GET["c"];
$level = intval($_GET["l"]);

require_once("common/DataBase.inc");
$sql = "SELECT * FROM geography.adm".$level." WHERE country='".DataBase::escape_string($country)."'";
if ($level > 1)
	$sql .= " AND parent_id='".DataBase::escape_string($_GET["p"])."'";
$sql .= " ORDER BY name ASC";
$res = DataBase::execute($sql);
echo "{list:[";
$first = true;
$types = array();
$types_nb = array();
while (($r = DataBase::next_row($res)) <> null) {
	if ($first) $first = false; else echo ",";
	$type = utf8_encode($r["type"]);
	$type_en = $r["type_en"];
	if ($type_en <> null)
		$type = utf8_encode($type_en)." (".$type.")";
	echo "{id:".$r["id"].",name:".json_encode(utf8_encode($r["name"])).",type:".json_encode($type)."}";
	$i = array_search($type, $types);
	if ($i === FALSE) {
		array_push($types, $type);
		array_push($types_nb, 1);
	} else
		$types_nb[$i]++;
}
echo "],type:";
$type = "";
if (count($types) == 1) $type = $types[0];
else if (count($types)>0) {
	$max_i = 0;
	for ($i = 1; $i < count($types); $i++)
		if ($types_nb[$i] > $types_nb[$max_i]) $max_i = $i;
	$type = $types[$max_i];
}
echo json_encode($type);
echo "}";

?>
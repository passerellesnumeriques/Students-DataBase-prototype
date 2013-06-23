<?php
require_once("common/DataBase.inc");
echo "[";
$res = DataBase::execute("SELECT * FROM geography.country");
$first = true;
while (($r = DataBase::next_row($res)) <> null) {
	if ($first) $first = false; else echo ",";
	echo "[".json_encode($r["code"]).",".json_encode($r["sovereign"]).",".json_encode(utf8_encode($r["name"])).",".$r["divisions"]."]";
}
echo "]";
?>
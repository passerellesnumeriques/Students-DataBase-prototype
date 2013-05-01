<?php 
$namespace = isset($_POST["ns"]) ? $_POST["ns"] : "common";
$result = "[";
for ($i = 1; isset($_POST["s".$i]); $i++) {
	if ($i > 1) $result .= ",";
	$result .= json_encode(get_locale($namespace, $_POST["s".$i]));
}
$result .= "]";
PNApplication::print_json_result($result);
?>
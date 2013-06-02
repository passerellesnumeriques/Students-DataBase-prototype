<?php 
$namespace = isset($_POST["ns"]) ? $_POST["ns"] : "common";
echo "[";
for ($i = 1; isset($_POST["s".$i]); $i++) {
	if ($i > 1) $result .= ",";
	echo json_encode(get_locale($namespace, $_POST["s".$i]));
}
echo "]";
?>
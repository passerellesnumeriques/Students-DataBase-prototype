<?php
$main = $_GET["main"];
$small = $_GET["small"];
$where = $_GET["where"];

$icon = imagecreatefrompng($main);
imagealphablending($icon, true);
imagesavealpha($icon, true);
$sm = imagecreatefrompng("common/images/small/".$small.".png");
imagealphablending($sm, true);
imagesavealpha($sm, true);
$w = imagesx($sm);
$h = imagesy($sm);
$i = strpos($where,"_");
$xs = substr($where, 0, $i);
$ys = substr($where, $i+1);
switch ($xs) {
	case "right": $x = imagesx($icon)-$w; break;
	case "left": $x = 0; break;
	case "center": $x = imagesx($icon)/2-$w/2; break;
}
switch ($ys) {
	case "bottom": $y = imagesy($icon)-$h; break;
	case "top": $y = 0; break;
	case "center": $y = imagesy($icon)/2-$h/2; break;
}

imagecopy($icon, $sm, $x, $y, 0, 0, $w, $h);

header("Content-Type: image/png");
imagepng($icon);
?>
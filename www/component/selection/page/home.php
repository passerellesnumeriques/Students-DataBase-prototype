<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/selection/selection_32.png', get_locale("Selection"), "todo");
$p->generate();
?>
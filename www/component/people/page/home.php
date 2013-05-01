<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/people/people_32.png', get_locale("People"), "list");
$p->generate();
?>
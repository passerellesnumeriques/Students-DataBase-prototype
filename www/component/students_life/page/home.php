<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/students_life/students_life_32.png', get_locale("Students' Life"), "todo");
$p->generate();
?>
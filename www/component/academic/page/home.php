<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/academic/academic_32.png', get_locale("Academic"), "/dynamic/curriculum/page/curricula");
$p->add_menu_link("/static/curriculum/curriculum_16.png", get_locale("curriculum","Curricula"), "/dynamic/curriculum/page/curricula");
$p->generate();
?>
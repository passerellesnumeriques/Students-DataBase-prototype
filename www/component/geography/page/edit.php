<?php
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, "", "Geography Database Edit", "edit_countries");
$p->add_menu_link('', "Countries", "edit_countries");
$p->generate();
?>
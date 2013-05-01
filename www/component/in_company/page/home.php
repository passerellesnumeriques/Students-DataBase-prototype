<?php 
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/in_company/in_company_32.png', get_locale("In Company"), "todo");
$p->generate();
?>
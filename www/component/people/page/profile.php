<?php
$plugin = @$_GET["plugin"];
$page = @$_GET["page"];
$people = @$_GET["people"];

if ($plugin == null) $plugin = "people";
if ($page == null) $page = "profile_".$plugin;
$pages = @PNApplication::$instance->components[$plugin]->get_profile_pages($people);
if ($pages == null || !isset($pages[$page])) {
	PNApplication::error("Unknow profile page '".$page."' in '".$plugin."'");
	return;
}
$page = $pages[$page][2];

$full_name = null;
if ($people <> null) {
	require_once("common/SQLQuery.inc");
	$q = SQLQuery::create()->select("People")->field('first_name')->field('last_name')->where('id',$people);
	if ($people == PNApplication::$instance->user_people->user_people_id)
		$q->bypass_security();
	$res = $q->execute_single_row();
	if ($res) $full_name = $res["first_name"]." ".$res["last_name"];
}
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/people/profile_32.png', $full_name <> null ? $full_name : get_locale("Profile"), $page);
$all_pages = array();
foreach (PNApplication::$instance->components as $cname=>$c) {
	$cl = new ReflectionClass($c);
	try { if ($cl->getMethod("get_profile_pages") == null) continue; }
	catch (Exception $e) { continue; }
	$pages = @$c->get_profile_pages($people);
	if ($pages <> null)
		foreach ($pages as $page_id=>$cp)
			array_push($all_pages, $cp);
}
function pages_sort($p1, $p2) {
	return $p1[3]-$p2[3];
}
usort($all_pages, "pages_sort");
foreach ($all_pages as $cp)
	$p->add_menu_link($cp[0], $cp[1], $cp[2]);
$p->generate();
?>

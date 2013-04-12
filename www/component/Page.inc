<?php 
class Page {
	
	private $javascripts = array();
	private $stylesheets = array();
	private $onload = null;
	
	public function add_javascript($path, $onload = "") {
		if (!in_array($path, $this->javascripts))
			array_push($this->javascripts, array($path,$onload));
	}
	public function add_stylesheet($path) {
		if (!in_array($path, $this->stylesheets))
			array_push($this->stylesheets, $path);
	}
	public function onload($script) { $this->onload = $script; }
	
	public function generate($component, $path, $embedded) {
		$file = "component/".$component->name."/page/".$path.".php";
		if (!file_exists($file)) {
			PNApplication::error_html("Page <i>".$path."</i> does not exist in component <i>".$component->name."</i>");
			PNApplication::print_errors();
			return;
		}
		ob_start();
		include $file;
		$content = ob_get_clean();
		$this->start_page();
		if (!$embedded) $this->layout_header();
		$this->layout_content($content);
		if (!$embedded) $this->layout_footer();
		$this->end_page();
	}

	private function start_page() {
?>
<!DOCTYPE HTML>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8"/>
	<title>PN Students DataBase</title>
	<link rel="stylesheet" href="/static/application/style.css"/>
	<script type="text/javascript" src="/static/common/utils.js"></script>
<?php
	foreach ($this->javascripts as $js) echo "<script type='text/javascript' src='".$js[0]."'>".$js[1]."</script>";
	foreach ($this->stylesheets as $css) echo "<link rel='stylesheet' href='".$css."'/>";
?>
</head>
<body<?php if ($this->onload <> null) echo " onload=\"".$this->onload."\"";?>>
<table width=100% height=100% cellspacing=0 cellpadding=0>
<?php
	}

	private function layout_header() {
?>
<tr class='header'><td style='padding:2px'>
<a href="/dynamic/application/page/home"><img src='/static/common/images/logo.png' height=45px border=0 style=';margin-left:0px;margin-bottom:-5px;margin-top:-5px;vertical-align:middle'/></a>
TODO: put a menu here
<div style='float:right;border-left:1px solid #808080;padding-left:2px;text-align:center'>
Language:<br/>
<?php
foreach (Locale::$known as $lang)
	if (Locale::$language <> $lang)
		echo "<a href='?set_language=".$lang."'><img src='/static/common/flags/".$lang.".png' style='border:0px;vertical-align:middle'/></a> ";
?>
</div>
<div style='float:right;border-left:1px solid #808080;padding-left:2px;text-align:center'>
<img src='/static/application/database.png' style='vertical-align:bottom'/><?php locale("application","Database")?>:<br/>
<select style='padding:0px;margin:0px' onchange="location.href='/dynamic/application/service/set_domain?domain='+this.value;">
<?php
foreach (preg_split("/,/", file_get_contents("domains",true)) as $domain)
	echo "<option value='".$domain."'".($domain == PNApplication::$instance->current_domain ? " selected" : "").">".$domain."</option>"; 
?>
</select>
</div>
<div style='float:right;border-left:1px solid #808080;padding-left:2px;text-align:center'>
<?php 
global $app;
echo $app->people->first_name." ".$app->people->last_name." ";
?>
<img src='/static/common/images/settings.png' style='vertical-align:bottom'/>
<br/>
<a href="/dynamic/application/page/logout" style='text-decoration:none;color:black'><img src='/static/common/images/logout.gif' style='border:0px;vertical-align:bottom'/><?php locale("application","Logout")?></a>
</div>
</td></tr>
<?php 
	}
	
	private function layout_content($content) {
?>
<tr><td valign=top class='page set_height'>
<div style='width:100%;height:100%;overflow:auto'>
<?php echo $content; ?>
</div>
</td></tr>
<?php
	}
	
	private function layout_footer() {
?>
<tr class='footer'><td align=center>
Copyright (c) Passerelles numériques
-
<a href="https://github.com/passerellesnumeriques/Students-DataBase/issues/new" target="_blank">Report a bug or request for new feature</a>
</td></tr>
<?php 
	}
	
	private function end_page() {
?>
</table>
</body>
</html>
<?php
	}
}
?>
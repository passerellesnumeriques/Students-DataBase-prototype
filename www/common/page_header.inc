<!DOCTYPE HTML>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8"/>
	<title>PN Students DataBase</title>
	<link rel="stylesheet" href="/static/application/style.css"/>
	<script type="text/javascript" src="/static/common/utils.js"></script>
	</head>
<body>
<table width=100% height=100% cellspacing=0 cellpadding=0>
<?php global $page_embedded; if (!$page_embedded) {?>
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
<?php }?>
<tr><td valign=top class='page set_height'>
<div style='width:100%;height:100%;overflow:auto'>
<!DOCTYPE HTML>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=UTF-8"/>
	<title>PN Students DataBase</title>
	<link rel="stylesheet" href="/static/application/style.css"/>
	<link rel="stylesheet" href="/static/application/datalist.css"/>
	<script type="text/javascript" src="/static/application/utils.js"></script>
	<script type="text/javascript" src="/static/application/datalist.js"></script>
</head>
<body>
<table width=100% height=100% cellspacing=0 cellpadding=0>
<?php global $page_embedded; if (!$page_embedded) {?>
<tr class='header'><td style='padding:2px'>
<a href="/dynamic/application/page/home"><img src='/static/application/logo.png' height=28px border=0 style=';margin-left:0px;margin-bottom:-10px;margin-top:-10px;vertical-align:middle'/></a>
TODO: put a menu here
<div style='float:right;border-left:1px solid #808080;padding-left:2px'>
<?php 
global $app;
echo $app->people->first_name." ".$app->people->last_name." ";
?>
<img src='/static/application/settings.png' style='vertical-align:bottom'/>
<a href="/dynamic/application/page/logout" style='text-decoration:none;color:black'><img src='/static/application/logout.gif' style='border:0px;vertical-align:bottom'/><?php locale("application","Logout")?></a>
<?php if (Locale::$language <> "en") {?>
<a href="?set_language=en"><img src="/static/application/lang/en.gif" style="border:0px;vertical-align:bottom"/></a>
<?php } if (Locale::$language <> "fr") {?>
<a href="?set_language=fr"><img src="/static/application/lang/fr.jpg" style="border:0px;vertical-align:bottom"/></a>
<?php }?>
</div>
</td></tr>
<?php }?>
<tr><td valign=top class='page set_height'>
<div style='width:100%;height:100%;overflow:auto'>
<?php
$message = null;
if (isset($_POST["domain"]) && isset($_POST["username"]) && isset($_POST["password"])) {
	global $app;
	setcookie("domain",$_POST["domain"],time()+30*24*60*60,"/dynamic/application/page/enter");
	setcookie("username",$_POST["username"],time()+30*24*60*60,"/dynamic/application/page/enter");
	if ($app->user_management->login($_POST["domain"], $_POST["username"], $_POST["password"])) {
		header("Location: enter");
		die();
	} else
		$message = get_locale("application","Invalid username or password");
}
?>
<!DOCTYPE html>
<html>
<head>
	<title>PN Students DataBase</title>
	<link rel="stylesheet" href="/static/application/style.css"/>
</head>
<body onload="window.top.pn_loading_end();">
<form method="post" action="<?php if (isset($_GET["page"])) echo "?page=".$_GET["page"]; ?>" style="width:100%;height:100%" onsubmit="var f=document.getElementById('frame_loading');if(f)f.contentWindow.stop_loading = true; window.top.pn_loading_start(); return true;">
<table style="width:100%;height:100%"><tr><td valign=middle align=center>
<?php
if (isset($_GET["from"])) {
	switch ($_GET["from"]) {
	case "inactivity":
		locale("application","Logout_because_inactivity");
		break;
	}
}
if ($message <> null) {
	echo "<img src='/static/common/images/error.png' style='vertical-align:bottom'/> <span style='color:red'>".$message."</span>";
}
?>
<table style='border:1px solid #22bbea'>
	<tr align=left><td rowspan=6 style='padding: 5px'>
		<img src='/static/common/images/logo.png'/>
	</td></tr>
	<tr align=left>
		<td><?php locale("user_management","Domain")?></td>
		<td>
			<select name="domain">
				<?php
				$domains = include("domains");
				foreach ($domains as $domain=>$descr)
					echo "<option value=\"".$domain."\"".(@$_COOKIE["domain"]==$domain ? " selected" : "").">".$domain."</option>";
				?>
			</select>
		</td>
	</tr>
	<tr align=left>
		<td><?php locale("user_management","Username")?></td>
		<td><input type="text" size=30 maxlength=100 name="username" value='<?php if (isset($_COOKIE["username"])) echo $_COOKIE["username"];?>'/></td>
	</tr>
	<tr align=left>
		<td><?php locale("user_management","Password")?></td>
		<td><input type="password" size=30 maxlength=100 name="password"/></td>
	</tr>
	<tr align=left>
		<td colspan=2 align=center>
			<button type="submit"><?php locale("user_management","Login")?></button>
		</td>
	</tr>
	<tr align=left>
		<td colspan=2 align=center>
			<?php
			foreach (Locale::$known as $lang)
				echo "<a href='?set_language=".$lang."'><img src='/static/common/flags/".$lang.".png' style='border:0px;vertical-align:middle'/></a> ";
			?>
		</td>
	</tr>
	<tr align=left>
		<td colspan=3 align=center>
			<a href="/dynamic/development/page/tools">Development tools</a>
		</td>
	</tr>
</table>
<br/>
<span id='application_loading'><span><?php locale("application","Loading application")?></span> <iframe frameBorder=0 style='width:200px;height:15px;border:1px solid black;overflow:hidden;vertical-align:bottom' scrolling='no' src='/static/application/load.php?delay=100' id='frame_loading'></iframe><span id='application_loading_text' style='font-size:8pt'></span></span>
</td></tr></table>
</form>
</body>
</html>
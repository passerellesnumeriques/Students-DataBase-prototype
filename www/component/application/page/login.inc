<?php
if (isset($_POST["domain"]) && isset($_POST["username"]) && isset($_POST["password"])) {
	global $app;
	if ($app->user_management->login($_POST["domain"], $_POST["username"], $_POST["password"])) {
		header("Location: enter");
		setcookie("domain",$app->user_management->domain,time()+30*24*60*60,"/dynamic/application/page/enter");
		setcookie("username",$app->user_management->username,time()+30*24*60*60,"/dynamic/application/page/enter");
		die();
	}
}
?>
<!DOCTYPE html>
<html>
<head>
	<title>PN Students DataBase</title>
	<link rel="stylesheet" href="/static/application/style.css"/>
</head>
<body>
<form method="post" action="<?php if (isset($_GET["page"])) echo "?page=".$_GET["page"]; ?>" style="width:100%;height:100%">
<table style="width:100%;height:100%"><tr><td valign=middle align=center>
<table style='border:1px solid #22bbea'>
	<tr align=left><td rowspan=6 style='padding: 5px'>
		<img src='/static/common/images/logo.png'/>
	</td></tr>
	<tr align=left>
		<td><?php locale("user_management","Domain")?></td>
		<td>
			<select name="domain">
				<option value="PNC"<?php if (@$_COOKIE["domain"]=="PNC") echo " selected"?>>PNC</option>
				<option value="PNF"<?php if (@$_COOKIE["domain"]=="PNF") echo " selected"?>>PNF</option>
				<option value="PNP"<?php if (@$_COOKIE["domain"]=="PNP") echo " selected"?>>PNP</option>
				<option value="PNV"<?php if (@$_COOKIE["domain"]=="PNV") echo " selected"?>>PNV</option>
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
				echo "<a href='?set_language=".$lang."en'><img src='/static/common/flags/".$lang.".png' style='border:0px;vertical-align:middle'/></a> ";
			?>
		</td>
	</tr>
	<tr align=left>
		<td colspan=3 align=center>
			<a href="/dynamic/development/page/tools">Development tools</a>
		</td>
	</tr>
</table>
</td></tr></table>
</form>
</body>
</html>
<?php
if (isset($_POST["domain"]) && isset($_POST["username"]) && isset($_POST["password"])) {
	global $app;
	if ($app->user_management->login($_POST["domain"], $_POST["username"], $_POST["password"])) {
		header("Location: enter".(isset($_GET["page"])?"?page=".$_GET["page"]:""));
		die();
	}
}
?>
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<form method="post" action="<?php if (isset($_GET["page"])) echo "?page=".$_GET["page"]; ?>">
<table>
	<tr>
		<td><?php locale("user_management","Domain")?></td>
		<td>
			<select name="domain">
				<option value="PNC">PNC</option>
				<option value="PNF">PNF</option>
				<option value="PNP">PNP</option>
				<option value="PNV">PNV</option>
			</select>
		</td>
	</tr>
	<tr>
		<td><?php locale("user_management","Username")?></td>
		<td><input type="text" size=30 maxlength=100 name="username"/></td>
	</tr>
	<tr>
		<td><?php locale("user_management","Password")?></td>
		<td><input type="password" size=30 maxlength=100 name="password"/></td>
	</tr>
	<tr>
		<td colspan=2 align=center>
			<button type="submit"><?php locale("user_management","Login")?></button>
		</td>
	</tr>
</table>
</form>
<a href="?set_language=en"><img src="/static/application/lang/en.gif" style="border:0px;"/></a>
<a href="?set_language=fr"><img src="/static/application/lang/fr.jpg" style="border:0px;"/></a>
</body>
</html>
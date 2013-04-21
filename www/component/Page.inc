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
	
	public function generate($component, $path) {
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
		echo $content;
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
	<script type="text/javascript" src="/static/common/js/utils.js"></script>
	<script type="text/javascript" src="/static/common/js/pn.js"></script>
	<script type="text/javascript" src="/static/common/js/databaselock.js"></script>
	<?php
	foreach ($this->javascripts as $js) echo "<script type='text/javascript' src='".$js[0]."'>".$js[1]."</script>";
	foreach ($this->stylesheets as $css) echo "<link rel='stylesheet' href='".$css."'/>";
?>
</head>
<body<?php if ($this->onload <> null) echo " onload=\"".$this->onload."\"";?>>
<?php
	}

	private function end_page() {
		PNApplication::print_errors();
?>
</body>
</html>
<?php
	}
}
?>
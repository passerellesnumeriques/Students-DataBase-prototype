<?php
function page_shutdown_catch($page) {
	if ($page->done) return;
	$msg = "The page '".$page->path."' on component '".$page->component."' didn't finish correctly.";
	$error = error_get_last();
	if ($error <> null)
		$msg.= " Last error was in ".$error["file"]." line ".$error["line"].": ".$error["message"];
	PNApplication::error($msg);
	PNApplication::print_errors();
	$content = ob_get_clean();
	echo $content;
}

class Page {

	private $javascripts = array();
	private $stylesheets = array();
	private $onload = "";
	public $component;
	public $path;
	public $done = false;
	private $generate_id = 0;

	public function add_javascript($path, $onload = "") {
		if (!in_array($path, $this->javascripts))
			array_push($this->javascripts, array($path,$onload));
	}
	public function add_stylesheet($path) {
		if (!in_array($path, $this->stylesheets))
			array_push($this->stylesheets, $path);
	}
	public function onload($script) { $this->onload .= $script; }
	
	public function generate_id() {
		return str_replace("/", "_", str_replace(".", "_", $this->path))."_".($this->generate_id++);
	}

	private $flushed = false;
	public function generate($component, $path) {
		$this->component = $component->name;
		$this->path = $path;
		$file = "component/".$component->name."/page/".$path.".php";
		ob_start();
		register_shutdown_function("page_shutdown_catch", $this);
		try {
			include $file;
		} catch (Exception $e) {
			PNApplication::error($e->getMessage());
		}
		$content = ob_get_clean();
		if (!$this->flushed)
			$this->start_page();
		echo $content;
		$this->end_page();
		$this->done = true;
	}

	public function flush() {
		if (!$this->flushed) {
			$this->flushed = true;
			$this->start_page();
		}
		$content = ob_get_clean();
		echo $content;
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
	<script type="text/javascript" src="/static/common/js/ajax.js"></script>
	<script type="text/javascript" src="/static/common/js/locale.js"></script>
	<script type="text/javascript" src="/static/common/js/configuration.js"></script>
	<script type="text/javascript" src="/static/common/js/common_dialogs.js"></script>
	<script type="text/javascript" src="/static/application/databaselock.js"></script>
	<?php
	foreach ($this->javascripts as $js) {
		echo "<script type='text/javascript' src='".$js[0]."' onload='javascript_loaded(\"".$js[0]."\");".$js[1]."'>";
		echo "</script>";
	}
	foreach ($this->stylesheets as $css) echo "<link rel='stylesheet' href='".$css."'/>";
?>
</head>
<body<?php if (strlen($this->onload) > 0) echo " onload=\"".$this->onload."\"";?>>
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
<?php
class SubPageHeader {

	public function __construct(&$page, $icon_32px, $title) {
		$this->icon_32px = $icon_32px;
		$this->title = $title;
		$this->page = &$page;
		$this->id = $this->page->component."__".$this->page->path;
		$this->id = str_replace("/","_", str_replace(".", "_", $this->id));
	}

	private $page;
	private $icon_32px;
	private $title;
	private $header;
	private $id;

	public function add_header($html) {
		$this->header .= "<div>".$html."</div>";
	}

	public function generate() {
		$this->page->add_javascript("/static/common/js/horizontal_menu/horizontal_menu.js");
		$this->page->add_stylesheet("/static/common/js/horizontal_menu/horizontal_menu.css");
		$this->page->onload("new horizontal_menu('sub_page_menu');");
?>
<div class='sub_page_header' id='<?php echo $this->id?>_header'>
	<table><tr>
	<td>
		<div class='sub_page_header_title'>
			<img src='<?php echo $this->icon_32px?>' style='vertical-align:bottom'/>
			<?php echo $this->title?>
		</div>
	</td><td style='vertical-align:bottom'>
		<div class='horizontal_menu' id='sub_page_menu'>
			<?php echo $this->header?>
			<div><div class='button'><img src='/static/application/more.png'/><?php locale("application","More")?></div></div>
		</div>
	</td>
	</tr></table>
</div>
<?php
	}

	public function start_scrollable_content() {
		echo "<div id='".$this->id."_content' style='width:100%;overflow:auto;position:relative'>";
	}

	public function end_scrollable_content() {
		echo "</div>";
?>
<script type='text/javascript'>
function <?php echo $this->id?>_resize_content() {
	var content = document.getElementById('<?php echo $this->id?>_content');
	var header = document.getElementById('<?php echo $this->id?>_header');
	content.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
<?php echo $this->id?>_resize_content();
listenEvent(window,'resize',<?php echo $this->id?>_resize_content);
</script>
<?php
	}

	public function start_fill_content() {
		echo "<div id='".$this->id."_content' style='width:100%;position:relative'>";
	}

	public function end_fill_content() {
		echo "</div>";
?>
<script type='text/javascript'>
function <?php echo $this->id?>_resize_content() {
	var content = document.getElementById('<?php echo $this->id?>_content');
	var header = document.getElementById('<?php echo $this->id?>_header');
	content.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
<?php echo $this->id?>_resize_content();
addLayoutEvent(document.body,<?php echo $this->id?>_resize_content);
</script>
<?php
	}

}
?>
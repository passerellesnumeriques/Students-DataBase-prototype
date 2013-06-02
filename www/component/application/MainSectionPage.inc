<?php
class MainSectionPage {

	public function __construct(&$page, $icon_32px_url, $title, $default_sub_page_url) {
		$this->page = &$page;
		$this->icon_32px_url = $icon_32px_url;
		$this->title = $title;
		$this->default_sub_page_url = $default_sub_page_url;
		$this->id = $this->page->component."__".$this->page->path;
		$this->id = str_replace("/","_", str_replace(".", "_", $this->id));
		$page->add_javascript("/static/common/js/horizontal_menu/horizontal_menu.js");
		$page->add_stylesheet("/static/common/js/horizontal_menu/horizontal_menu.css");
		$page->onload("new horizontal_menu('".$this->id."_horizontal_menu');");
	}

	private $page;
	private $icon_32px_url;
	private $title;
	private $default_sub_page_url;
	private $id;
	private $menu_items = array();

	public function add_menu_link($icon, $text, $url) {
		$html = "<span class='page_header_item' onclick=\"document.getElementById('".$this->id."_content').src='".$url."';\">";
		if ($icon <> null)
			$html .= "<img src='".$icon."'/> ";
		$html .= $text;
		$html .= "</span> ";
		$this->add_menu_content($html);
	}

	public function add_menu_content($html) {
		array_push($this->menu_items, $html);
	}

	public function generate() {
?>
<div class='page_header' id='<?php echo $this->id?>_header'>
	<table><tr>
	<td>
		<div class='page_header_title'>
			<img src='<?php echo $this->icon_32px_url?>'/>
			<?php echo $this->title?>
		</div>
	</td><td>
		<div class='page_header_content'>
			<div class='horizontal_menu' id='<?php echo $this->id?>_horizontal_menu'>
				<?php
				foreach ($this->menu_items as $item) {
					echo "<div>";
					echo $item;
					echo "</div>";
				}
				?>
				<div style='cursor:pointer'><img src='/static/application/more.png'/><?php locale("application","More")?></div>
			</div>
		</div>
	</td>
	</tr></table>
</div>
<iframe class='page_content' frameborder=0 src="<?php echo $this->default_sub_page_url?>" style='width:100%;margin:0px;padding:0px;border:0px' id='<?php echo $this->id?>_content' name='<?php echo $this->id?>_content'></iframe>
<script type='text/javascript'>
function <?php echo $this->id?>_resize_frame() {
	var frame = document.getElementById('<?php echo $this->id?>_content');
	var header = document.getElementById('<?php echo $this->id?>_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
<?php echo $this->id?>_resize_frame();
addLayoutEvent(document.body,<?php echo $this->id?>_resize_frame);
</script>
<?php
	}

}
?>
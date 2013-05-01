<?php 
class MainSectionPage {
	
	public function __construct(&$page, $icon_32px_url, $title, $default_sub_page_url) {
		$this->page = &$page;
		$this->icon_32px_url = $icon_32px_url;
		$this->title = $title;
		$this->default_sub_page_url = $default_sub_page_url;
		$this->id = $this->page->component."__".$this->page->path;
		$his->id = str_replace("/","_", str_replace(".", "_", $this->id));
	}
	
	private $page;
	private $icon_32px_url;
	private $title;
	private $default_sub_page_url;
	private $id;
	private $menu_items = array();
	
	public function add_menu_link($icon, $text, $url) {
		$html = "<a href=\"".$url."\" target=\"".$this->id."_content\">";
		if ($icon <> null)
			$html .= "<img src='".$icon."'/> ";
		$html .= $text;
		$html .= "</a> ";
		$this->add_menu_content($html);
	}

	public function add_menu_content($html) {
		array_push($this->menu_items, $html);
	}
	
	public function generate() {
?>
<div class='page_header' id='<?php echo $this->id?>_header'>
	<div class='page_header_title'>
		<img src='<?php echo $this->icon_32px_url?>'/>
		<?php echo $this->title?>
	</div>
	<div class='page_header_content'>
		<?php foreach ($this->menu_items as $item) echo $item;?>
	</div>
</div>
<iframe class='page_content' frameborder=0 src="<?php echo $this->default_sub_page_url?>" style='width:100%;margin:0px;padding:0px;border:0px' id='<?php echo $this->id?>_content' name='<?php echo $this->id?>_content'></iframe>
<script type='text/javascript'>
function <?php echo $this->id?>_resize_frame() {
	var frame = document.getElementById('<?php echo $this->id?>_content');
	var header = document.getElementById('<?php echo $this->id?>_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
<?php echo $this->id?>_resize_frame();
listenEvent(window,'resize',<?php echo $this->id?>_resize_frame);
</script>
<?php		
	}
	
}
?>
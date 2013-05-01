<?php 
class SubPageHeader {
	
	public function __construct($icon_32px, $title) {
		$this->icon_32px = $icon_32px;
		$this->title = $title;
	}
	
	private $icon_32px;
	private $title;
	private $content;
	
	public function start_section() {
		$this->content .= "<span class='sub_page_header_section''>";
	}
	public function add_content($html) {
		$this->content .= $html;
	}
	public function end_section() {
		$this->content .= "</span>";
	}
	
	public function generate() {
?>
<div class='sub_page_header'>
	<img src='<?php echo $this->icon_32px?>' style='vertical-align:bottom'/>
	<?php echo $this->title?>
	<?php echo $this->content?>
</div>
<?php
	}
	
}
?>
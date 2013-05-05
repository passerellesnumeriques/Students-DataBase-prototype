<?php
class SubPageHeader {

	public function __construct(&$page, $icon_32px, $title) {
		$this->icon_32px = $icon_32px;
		$this->title = $title;
		$this->page = &$page;
	}

	private $page;
	private $icon_32px;
	private $title;
	private $header;

	public function add_header($html) {
		$this->header .= "<div>".$html."</div>";
	}

	public function generate() {
		$this->page->add_javascript("/static/common/js/component/horizontal_menu.js");
		$this->page->add_stylesheet("/static/common/js/component/horizontal_menu.css");
		$this->page->onload("new horizontal_menu('sub_page_menu');");
?>
<div class='sub_page_header'>
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

}
?>
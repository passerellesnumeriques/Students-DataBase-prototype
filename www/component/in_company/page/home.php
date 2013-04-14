<div class='page_header' id='in_company_header'>
	<div class='page_header_title'>
		<img src='/static/in_company/in_company_32.png'/>
		<?php locale("In Company")?>
	</div>
	<div class='page_header_content'>
		TODO menu
	</div>
</div>
<iframe class='page_content' frameborder=0 src="todo" style='width:100%;margin:0px;padding:0px;border:0px' id='in_company_content'></iframe>
<script type='text/javascript'>
function in_company_resize_frame() {
	var frame = document.getElementById('in_company_content');
	var header = document.getElementById('in_company_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
in_company_resize_frame();
listenEvent(window,'resize',in_company_resize_frame);
</script>
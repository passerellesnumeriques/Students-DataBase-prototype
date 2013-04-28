<div class='page_header' id='academic_header'>
	<div class='page_header_title'>
		<img src='/static/academic/academic_32.png'/>
		<?php locale("Academic")?>
	</div>
	<div class='page_header_content'>
		<a href="/dynamic/curriculum/page/curricula" target="academic_content">Curricula</a>
	</div>
</div>
<iframe class='page_content' frameborder=0 src="/dynamic/curriculum/page/curricula" style='width:100%;margin:0px;padding:0px;border:0px' id='academic_content' name='academic_content'></iframe>
<script type='text/javascript'>
function academic_resize_frame() {
	var frame = document.getElementById('academic_content');
	var header = document.getElementById('academic_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
academic_resize_frame();
listenEvent(window,'resize',academic_resize_frame);
</script>
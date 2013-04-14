<div class='page_header' id='selection_header'>
	<div class='page_header_title'>
		<img src='/static/selection/selection_32.png'/>
		<?php locale("Selection")?>
	</div>
	<div class='page_header_content'>
		TODO menu
	</div>
</div>
<iframe class='page_content' frameborder=0 src="todo" style='width:100%;margin:0px;padding:0px;border:0px' id='selection_content'></iframe>
<script type='text/javascript'>
function selection_resize_frame() {
	var frame = document.getElementById('selection_content');
	var header = document.getElementById('selection_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
selection_resize_frame();
listenEvent(window,'resize',selection_resize_frame);
</script>
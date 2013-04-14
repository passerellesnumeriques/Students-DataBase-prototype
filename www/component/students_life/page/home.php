<div class='page_header' id='students_life_header'>
	<div class='page_header_title'>
		<img src='/static/students_life/students_life_32.png'/>
		<?php locale("Students' Life")?>
	</div>
	<div class='page_header_content'>
		TODO menu
	</div>
</div>
<iframe class='page_content' frameborder=0 src="todo" style='width:100%;margin:0px;padding:0px;border:0px' id='students_life_content'></iframe>
<script type='text/javascript'>
function students_life_resize_frame() {
	var frame = document.getElementById('students_life_content');
	var header = document.getElementById('students_life_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
students_life_resize_frame();
listenEvent(window,'resize',students_life_resize_frame);
</script>
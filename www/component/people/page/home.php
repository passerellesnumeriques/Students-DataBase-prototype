<div class='page_header' id='user_management_header'>
	<div class='page_header_title'>
		<img src='/static/people/people_32.png'/>
		<?php locale("People")?>
	</div>
	<div class='page_header_content'>
		TODO menu
	</div>
</div>
<iframe class='page_content' frameborder=0 src="list" style='width:100%;margin:0px;padding:0px;border:0px' id='user_management_content'></iframe>
<script type='text/javascript'>
function user_management_resize_frame() {
	var frame = document.getElementById('user_management_content');
	var header = document.getElementById('user_management_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
user_management_resize_frame();
listenEvent(window,'resize',user_management_resize_frame);
</script>
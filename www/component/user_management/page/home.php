<div class='page_header' id='user_management_header'>
	<div class='page_header_title'>
		<img src='/static/user_management/user_management_32.png'/>
		<?php locale("User Management")?>
	</div>
	<div class='page_header_content'>
		<a href="users" target="user_management_content">
			<img src='/static/user_management/user_list.png'/> <?php locale("Users list")?>
		</a>
		<a href="roles" target="user_management_content">
			<img src='/static/user_management/role.png'/> <?php locale("Roles")?>
		</a>
	</div>
</div>
<iframe class='page_content' frameborder=0 src="users" style='width:100%;margin:0px;padding:0px;border:0px' id='user_management_content' name='user_management_content'></iframe>
<script type='text/javascript'>
function user_management_resize_frame() {
	var frame = document.getElementById('user_management_content');
	var header = document.getElementById('user_management_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
user_management_resize_frame();
listenEvent(window,'resize',user_management_resize_frame);
</script>
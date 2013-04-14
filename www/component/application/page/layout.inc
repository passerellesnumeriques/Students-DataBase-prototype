<div id='application_header'>
	<table cellspacing=0 cellpadding=0 width=100%>
	<tr>
		<td width=100% style='padding-left:50px' align=center>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/selection/page/home';"><img src='/static/selection/selection_16.png'/><?php locale("selection","Selection")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/academic/page/home';"><img src='/static/academic/academic_16.png'/><?php locale("academic","Academic")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/students_life/page/home';"><img src='/static/students_life/students_life_16.png'/><?php locale("students_life","Students' Life")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/in_company/page/home';"><img src='/static/in_company/in_company_16.png'/><?php locale("in_company","In Company")?></span>
			<span class='application_menu_item' onclick='pn.context_menu("application_more_sub_menu", this);'><img src='/static/application/more.png'/>More</span>
			<div id='application_more_sub_menu' class='context_menu'>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/user_management/page/home';"><img src='/static/user_management/user_management_16.png'/><?php locale("user_management","User Management")?></div>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/people/page/home';"><img src='/static/people/people_16.png'/><?php locale("people","People")?></div>
			</div>
		</td>
		<td nowrap="nowrap" class='application_header_section'>
			<?php 
			global $app;
			echo $app->people->first_name." ".$app->people->last_name." ";
			?>
			<img src='/static/common/images/settings.png' style='vertical-align:bottom'/>
			<a href="/dynamic/application/page/logout" style='text-decoration:none;color:black'><img src='/static/common/images/logout.gif' style='border:0px;vertical-align:bottom'/><?php locale("application","Logout")?></a>
		</td><td nowrap="nowrap" class='application_header_section' title="<?php locale("application","Database")?>">
			<img src='/static/application/database.png' style='vertical-align:bottom'/>
			<select style='padding:0px;margin:0px' onchange="location.href='/dynamic/application/service/set_domain?domain='+this.value;">
			<?php
			foreach (preg_split("/,/", file_get_contents("domains",true)) as $domain)
				echo "<option value='".$domain."'".($domain == PNApplication::$instance->current_domain ? " selected" : "").">".$domain."</option>"; 
			?>
			</select>
		</td><td nowrap="nowrap" class='application_header_section' title="Language">
			<?php
			foreach (Locale::$known as $lang)
				if (Locale::$language <> $lang)
					echo "<a href='?set_language=".$lang."'><img src='/static/common/flags/".$lang.".png' style='border:0px;vertical-align:middle'/></a> ";
			?>
		</td>
	</tr>
	</table>
</div>
<iframe frameborder=0 width=100% id='application_content' src='/dynamic/application/page/home'></iframe>
<div id='application_footer'>
Copyright (c) Passerelles numériques
-
<a href="https://github.com/passerellesnumeriques/Students-DataBase/issues/new" target="_blank">Report a bug or request for new feature</a>
</div>
<a href="/dynamic/application/page/enter" style='position:fixed;top:-2px;left:2px'>
	<img src='/static/common/images/logo.png' height=32px border=0 style='vertical-align:top'/>
</a>
<script type='text/javascript'>
function application_resize_frame() {
	var frame = document.getElementById('application_content');
	var header = document.getElementById('application_header');
	var footer = document.getElementById('application_footer');
	frame.style.height = (getWindowHeight()-header.scrollHeight-footer.scrollHeight-1)+"px";
}
application_resize_frame();
listenEvent(window,'resize',application_resize_frame);
</script>
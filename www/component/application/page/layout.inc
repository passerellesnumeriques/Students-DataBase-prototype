<?php
$this->add_javascript("/static/common/js/horizontal_menu/horizontal_menu.js");
$this->add_stylesheet("/static/common/js/horizontal_menu/horizontal_menu.css");
$this->add_javascript("/static/common/js/context_menu/context_menu.js");
$this->add_stylesheet("/static/common/js/context_menu/context_menu.css");
$this->add_javascript("/static/common/js/animation.js");
$this->onload("new horizontal_menu('application_menu');application_resize_frame();var e = document.getElementById('application_hide');animation.fadeOut(e,500,function(){e.parentNode.removeChild(e);});");
?>
<div id='application_hide' style='position:fixed;width:100%;height:100%;top:0px;left:0px;z-index:100;background-color:white'>
<table width=100% height=100%>
<tr><td align=center valign=middle>
<img src='/static/common/images/wait_32.png'/>
</td></tr>
</table>
</div>
<div id='application_header'>
	<table cellspacing=0 cellpadding=0 width=100%>
	<tr>
		<td width=100% style='padding-left:50px' align=center>
			<div class='horizontal_menu' id='application_menu'>
				<div class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/selection/page/home';"><img src='/static/selection/selection_16.png'/><?php locale("selection","Selection")?></div>
				<div class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/students_life/page/home';"><img src='/static/students_life/students_life_16.png'/><?php locale("students_life","Students' Life")?></div>
				<div class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/academic/page/home';"><img src='/static/academic/academic_16.png'/><?php locale("academic","Academic")?></div>
				<div class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/in_company/page/home';"><img src='/static/in_company/in_company_16.png'/><?php locale("in_company","In Company")?></div>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/user_management/page/home';"><img src='/static/user_management/user_management_16.png'/><?php locale("user_management","User Management")?></div>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/people/page/home';"><img src='/static/people/people_16.png'/><?php locale("people","People")?></div>
				<div class='application_menu_item'><img src='/static/application/more.png'/><?php locale("application","More")?></div>
			</div>
			<!--
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/selection/page/home';"><img src='/static/selection/selection_16.png'/><?php locale("selection","Selection")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/academic/page/home';"><img src='/static/academic/academic_16.png'/><?php locale("academic","Academic")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/students_life/page/home';"><img src='/static/students_life/students_life_16.png'/><?php locale("students_life","Students' Life")?></span>
			<span class='application_menu_item' onclick="document.getElementById('application_content').src='/dynamic/in_company/page/home';"><img src='/static/in_company/in_company_16.png'/><?php locale("in_company","In Company")?></span>
			<span class='application_menu_item' onclick='new context_menu("application_more_sub_menu").showBelowElement(this);'><img src='/static/application/more.png'/>More</span>
			<div id='application_more_sub_menu' class='context_menu'>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/user_management/page/home';"><img src='/static/user_management/user_management_16.png'/><?php locale("user_management","User Management")?></div>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/people/page/home';"><img src='/static/people/people_16.png'/><?php locale("people","People")?></div>
			</div>
			<span class='application_menu_item'><img src='/static/common/images/search.png'/><form action="/dynamic/application/page/search" method="get" target="application_content" style='display:inline;'><input name="q" type='text' size=15 style='margin:0px;padding:0px;border:1px solid #8080FF;font-size:11px' autocomplete="off"/></form></span>
			 -->
		</td>
		<td nowrap="nowrap" class='application_header_section'>
			<img src='/static/common/images/search.png' style='vertical-align:bottom'/><form action="/dynamic/application/page/search" method="get" target="application_content" style='display:inline;'><input name="q" type='text' size=15 style='margin:0px;padding:0px;border:1px solid #8080FF;font-size:11px' autocomplete="off"/></form>
		</td>
		<td nowrap="nowrap" class='application_header_section' style='cursor:pointer' onclick="new context_menu('user_settings_menu').showBelowElement(this);">
			<?php echo PNApplication::$instance->people->first_name." ".PNApplication::$instance->people->last_name;?>
			<img src='/static/common/images/settings.png' style='vertical-align:bottom'/>
			<div id='user_settings_menu' class='context_menu'>
				<div class='context_menu_item' onclick="document.getElementById('application_content').src='/dynamic/people/page/profile?people=<?php echo PNApplication::$instance->people->user_people_id?>';">
					<img src='/static/people/profile_16.png'/>
					<?php locale("people","Profile")?>
				</div>
				<div class='context_menu_item' onclick="location='/dynamic/application/page/logout';">
					<img src='/static/common/images/logout.gif'/>
					<?php locale("application","Logout")?>
				</div>
				<hr style='width:95%;height:1px;color:black;background-color:black;border:0px;'/>
				<div style='padding:2px'>
					Language:
					<?php
					foreach (Locale::$known as $lang)
						if (Locale::$language <> $lang)
							echo "<a href='?set_language=".$lang."'><img src='/static/common/flags/".$lang.".png' style='border:0px;vertical-align:middle'/></a> ";
					?>
				</div>
			</div>
		</td><td nowrap="nowrap" class='application_header_section' title="<?php locale("application","Database")?>">
			<img src='/static/application/database.png' style='vertical-align:bottom'/>
			<select style='padding:0px;margin:0px' onchange="location.href='/dynamic/application/service/set_domain?format=xml&domain='+this.value;">
			<?php
			foreach (preg_split("/,/", file_get_contents("domains",true)) as $domain)
				echo "<option value='".$domain."'".($domain == PNApplication::$instance->current_domain ? " selected" : "").">".$domain."</option>";
			?>
			</select>
		</td>
	</tr>
	</table>
</div>
<iframe frameborder=0 id='application_content' name='application_content' src='/dynamic/application/page/home'></iframe>
<div id='application_right_panel'>
<div id='right_panel_open' style='visibility:hidden;position:absolute'>
	<div style='background-color:#D0D0FF;padding:2px;'>
		<img src='/static/common/images/arrow_right_10.gif' style='vertical-align:middle;cursor:pointer' onclick='application_hide_right_panel()'/>
		TODO: who's online
	</div>
	<div style='overflow-y:auto'>
	</div>
</div>
<div id='right_panel_close' style='visibility:hidden;position:absolute;background-color:#D0D0FF'>
	<img src='/static/common/images/arrow_left_10.gif' style='vertical-align:middle;cursor:pointer' onclick='application_show_right_panel()'/>
</div>
</div>
<div id='application_footer'>
Copyright (c) 2013<?php
$now = date("Y");
if ($now <> "2013") echo "-".$now;
?> - Passerelles numériques
-
<a href="https://github.com/passerellesnumeriques/Students-DataBase/issues/new" target="_blank">Report a bug or request for new feature</a>
<span id='application_loading' style='position:fixed;bottom:1px;right:2px;z-index:5;'><span><?php locale("application","Loading application")?></span> <iframe style='width:100px;height:10px;border:1px solid black;overflow:hidden;vertical-align:bottom' scrolling='no' src='/static/application/load.php?delay=2000'></iframe><span id='application_loading_text'></span></span>
<span style='position:fixed;bottom:1px;right:3px;z-index:5;visibility:hidden;' id='inactivity_status'>
	<?php locale("application","Inactivity")?>: <span id='inactivity_time'></span>
</span>
</div>
<a href="/dynamic/application/page/home" target="application_content" style='position:fixed;top:-2px;left:2px'>
	<img src='/static/common/images/logo.png' height=32px border=0 style='vertical-align:top'/>
</a>
<script type='text/javascript'>
function application_resize_frame() {
	var frame = document.getElementById('application_content');
	var header = document.getElementById('application_header');
	var footer = document.getElementById('application_footer');
	var right = document.getElementById('application_right_panel');
	var right_open = document.getElementById('right_panel_open');
	var right_close = document.getElementById('right_panel_close');
	frame.style.height = right.style.height = (getWindowHeight()-header.scrollHeight-footer.scrollHeight-1)+"px";
	if (right.data) {
		frame.style.width = (getWindowWidth()-10)+"px";
		right.style.position = "fixed";
		right.style.right = "0px";
		right.style.top = (header.scrollHeight)+"px";
		right.style.width = "10px";
		right_close.style.visibility = 'visible';
		right_close.style.position = 'static';
		right_close.style.width = right.clientWidth + "px";
		right_close.style.height = right.clientHeight + "px";
		right_open.style.visibility = 'hidden';
		right_open.style.position = 'absolute';
	} else {
		frame.style.width = (getWindowWidth()-200)+"px";
		right.style.position = "fixed";
		right.style.right = "0px";
		right.style.top = (header.scrollHeight)+"px";
		right.style.width = "200px";
		right_open.style.visibility = 'visible';
		right_open.style.position = 'static';
		right_open.style.width = right.clientWidth + "px";
		right_open.style.height = right.clientHeight + "px";
		right_close.style.visibility = 'hidden';
		right_close.style.position = 'absolute';
	}
}
function application_hide_right_panel() {
	var right = document.getElementById('application_right_panel');
	right.data = true;
	application_resize_frame();
}
function application_show_right_panel() {
	var right = document.getElementById('application_right_panel');
	right.data = false;
	application_resize_frame();
}
document.getElementById('application_right_panel').data = true; // Hide it as it is not yet implemented
application_resize_frame();
addLayoutEvent(document.body,application_resize_frame);
</script>
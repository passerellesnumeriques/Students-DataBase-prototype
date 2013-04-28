<?php 
$plugin = @$_GET["plugin"];
$page = @$_GET["page"];
$people = @$_GET["people"];

if ($plugin == null) $plugin = "people";
if ($page == null) $page = "profile_".$plugin;
$pages = @PNApplication::$instance->components[$plugin]->get_profile_pages($people);
if ($pages == null || !isset($pages[$page]))
	die("Unknow profile page '".$page."' in '".$plugin."'");
$page = $pages[$page][1];

$full_name = null;
if ($people <> null) {
	$res = DataBase::$conn->execute("SELECT first_name,last_name FROM People WHERE id=".$people);
	if ($res) $res = DataBase::$conn->next_row($res);
	if ($res) $full_name = $res[0]." ".$res[1];
}
?>
<div class='page_header' id='profile_header'>
	<div class='page_header_title'>
		<img src='/static/people/profile_32.png'/>
		<?php if ($full_name <> null) echo $full_name; else locale("Profile")?>
	</div>
	<div class='page_header_content'>
<?php 
	foreach (PNApplication::$instance->components as $cname=>$c) {
		$cl = new ReflectionClass($c);
		try { if ($cl->getMethod("get_profile_pages") == null) continue; }
		catch (Exception $e) { continue; }
		$pages = @$c->get_profile_pages($people);
		if ($pages <> null) {
			foreach ($pages as $page_id=>$p) {
				echo "<span class='page_menu_item'>";
				echo "<a href=\"".$p[1]."\" target='profile_content'>";
				echo $p[0];
				echo "</a>";
				echo "</span>";
			}
		}
	}
?>
	</div>
</div>
<iframe class='page_content' frameborder=0 src="<?php echo $page?>" style='width:100%;margin:0px;padding:0px;border:0px' id='profile_content' name='profile_content'></iframe>
<script type='text/javascript'>
function profile_resize_frame() {
	var frame = document.getElementById('profile_content');
	var header = document.getElementById('profile_header');
	frame.style.height = (getWindowHeight()-header.offsetHeight)+"px";
}
profile_resize_frame();
listenEvent(window,'resize',profile_resize_frame);
</script>
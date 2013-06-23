<?php
// handle change of campaign
if (isset($_GET["set_campaign"])) $component->campaign_id = $_GET["set_campaign"];

// header of the page with menu
require_once("component/application/MainSectionPage.inc");
$p = new MainSectionPage($this, '/static/selection/selection_32.png', get_locale("Selection"), "dashboard");
$html = get_locale("Campaign").": <select onchange='set_selection_campaign(this);'>";
$html .= "<option value='0'></option>";
if (PNApplication::$instance->user_management->has_right("edit_selection"))
	$html .= "<option value='new'>".htmlentities(get_locale("Create new campaign"),ENT_COMPAT,"UTF-8")."</option>";
foreach ($component->get_campaigns() as $c) {
	$html .= "<option value='".$c["id"]."'".($component->campaign_id == $c["id"] ? " selected='selected'":"").">";
	$html .= htmlentities($c["name"],ENT_COMPAT,"UTF-8");
	$html .= "</option>";
}
$html .= "</select>";
$p->add_menu_content($html);
$p->add_menu_link("/static/selection/dashboard_16.png", get_locale("Dashboard"), "dashboard");
$p->add_menu_link("/static/selection/information_session_16.png", get_locale("Information Sessions"), "information_sessions");
$p->add_menu_link("/static/selection/application_16.png", get_locale("Applications"), "applications");
$p->add_menu_link("/static/selection/written_exam_16.png", get_locale("Written Exams"), "written_exams");
$p->add_menu_link("/static/selection/interview_16.png", get_locale("Interviews"), "interviews");
$p->add_menu_link("/static/selection/social_investigation_16.png", get_locale("Social Investigations"), "social_investigations");
$p->generate();
?>
<script type='text/javascript'>
function set_selection_campaign(select) {
	if (select.value == 'new') {
		input_dialog('/static/common/images/add.png', "<?php locale("Create new campaign")?>", "<?php locale("common","Name")?>", "", 100, function(name){
			if (name.length == 0)
				return "<?php locale("common", "Cannot be empty")?>";
			return null;
		},function(name){
			ajax.post_parse_result("/dynamic/selection/service/create_campaign",{name:name},function(result){
				if (result && result.id) {
					var u = new URL(location.href);
					u.params["set_campaign"] = result.id;
					location.href = u.toString();
				}
			});
		});
	} else {
		var u = new URL(location.href);
		u.params["set_campaign"] = select.value;
		location.href = u.toString();
	}
}
</script>
<?php
if ($component->campaign_id == null) die(get_locale("Please select a selection campaign"));

require_once("common/SQLQuery.inc");
$campaign = SQLQuery::create()->select("SelectionCampaign")->where("id", $component->campaign_id)->execute_single_row();

$this->add_javascript("/static/common/js/collapsable_section/collapsable_section.js");
$this->add_stylesheet("/static/common/js/collapsable_section/collapsable_section.css");

$this->add_javascript("/static/selection/selection_widget.js");
$this->add_stylesheet("/static/selection/style.css");

require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this, "/static/selection/dashboard_32.png", get_locale("Dashboard"));
$header->generate();
?>
<table width=100%>
<tr><td align=left valign=top>
<div id='widget_general_status' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("General Status")?></div>
	<div class='collapsable_section_content' id='general_status_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_information_sessions' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Information Sessions")?></div>
	<div class='collapsable_section_content' id='information_sessions_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_applications' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Applications")?></div>
	<div class='collapsable_section_content' id='applications_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_written_exams' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Written Exams")?></div>
	<div class='collapsable_section_content' id='written_exams_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_interviews' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Interviews")?></div>
	<div class='collapsable_section_content' id='interviews_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_social_investigations' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Social Investigations")?></div>
	<div class='collapsable_section_content' id='social_investigations_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<div id='widget_result' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Result")?></div>
	<div class='collapsable_section_content' id='result_content'><img src='/static/common/images/loading.gif'/></div>
</div>
</td><td align=left valign=top>
<div id='widget_calendar' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Calendar")?></div>
	<div class='collapsable_section_content' id='calendar_content'><img src='/static/common/images/loading.gif'/></div>
</div>
</td></tr>
</table>
<script type='text/javascript'>
new collapsable_section('widget_general_status');
new collapsable_section('widget_information_sessions');
new collapsable_section('widget_applications');
new collapsable_section('widget_written_exams');
new collapsable_section('widget_interviews');
new collapsable_section('widget_social_investigations');
new collapsable_section('widget_result');
new collapsable_section('widget_calendar');

new selection_widget('general_status');
new selection_widget('information_sessions');
new selection_widget('applications');
new selection_widget('written_exams');
new selection_widget('interviews');
new selection_widget('social_investigations');
new selection_widget('result');
new selection_widget('calendar');
</script>
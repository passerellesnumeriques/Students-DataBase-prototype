<?php
if ($component->campaign_id == null) die(get_locale("Please select a selection campaign"));

require_once("common/SQLQuery.inc");
$campaign = SQLQuery::create()->select("SelectionCampaign")->where("id", $component->campaign_id)->execute_single_row();

$this->add_javascript("/static/common/js/collapsable_section/collapsable_section.js");
$this->add_stylesheet("/static/common/js/collapsable_section/collapsable_section.css");

$this->add_javascript("/static/selection/selection_widget.js");
$this->add_stylesheet("/static/selection/style.css");

require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this, "/static/selection/information_session_32.png", get_locale("Information Sessions"));
$header->generate();
?>
<div id='widget_information_sessions' class='selection_dashboard_widget'>
	<div class='collapsable_section_header'><?php locale("Information Sessions")?></div>
	<div class='collapsable_section_content' id='information_sessions_content'><img src='/static/common/images/loading.gif'/></div>
</div>
<script type='text/javascript'>
new collapsable_section('widget_information_sessions');
new selection_widget('information_sessions');
</script>
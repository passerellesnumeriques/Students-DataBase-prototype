<?php
/* @var $component selection */
if ($component->campaign_id == null) die(get_locale("Please select a selection campaign"));

require_once("common/SQLQuery.inc");
$nb_sessions = $component->select_information_sessions()->count()->execute_single_value();
$nb_sessions_no_schedule = $component->select_information_sessions_without_schedule()->count()->execute_single_value();
$nb_sessions_no_staff = $component->select_information_sessions_with_schedule_but_without_staff()->count()->execute_single_value();

echo $nb_sessions." ".get_locale($nb_sessions > 1 ? "information sessions planned" : "information session planned")."<br/>";
if ($nb_sessions_no_schedule == 0 && $nb_sessions_no_staff == 0)
	echo "<span style='color:green;margin-left:15px'>".get_locale("All have a schedule and assigned staff")."</span><br/>";
else {
	if ($nb_sessions_no_schedule > 0)
		echo "<span style='color:red;margin-left:15px'>".$nb_sessions_no_schedule." ".get_locale("don't have a schedule yet")."</span><br/>";
	else
		echo "<span style='color:green;margin-left:15px'>".get_locale("All have a schedule")."</span><br/>";
	if ($nb_sessions_no_staff > 0)
		echo "<span style='color:red;margin-left:15px'>".$nb_sessions_no_staff." ".get_locale("don't have any staff assigned")."</span><br/>";
	else
		echo "<span style='color:green;margin-left:15px'>".get_locale("All schedules sessions have a staff assigned")."</span><br/>";
}
?>
<?php
/* @var $component selection */
if ($component->campaign_id == null) die(get_locale("Please select a selection campaign"));

require_once("common/SQLQuery.inc");
$nb_applicants = $component->select_applicants()->count()->execute_single_value();

echo $nb_applicants." ".get_locale($nb_applicants > 1 ? "applicants" : "applicant");
?>
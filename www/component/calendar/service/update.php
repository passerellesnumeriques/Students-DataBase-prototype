<?php
if (!isset($_GET["id"])) { PNApplication::error("Missing calendar id"); return; }
$id = $_GET["id"];

require_once("common/SQLQuery.inc");
$calendar = SQLQuery::create()->select("Calendar")->where("id",$id)->execute_single_row();
if ($calendar == null) { PNApplication::error("Invalid calendar"); return; }

// check rights
if (!PNApplication::$instance->calendar->can_write($id, $calendar)) {
	PNApplication::error("Access Denied");
	return;
}

if ($calendar["type"] == "internet") {
	if (time()-intval($calendar["last_modification"]) > 5*60) {
		// update from internet
		require_once("update_internet_calendar.inc");
		update_internet_calendar($id, $calendar["data"], $calendar["last_modification"]);
	}
}
echo "true";
?>
<?php
if (!isset($_GET["id"])) { PNApplication::error("Missing calendar id"); return; }
$id = $_GET["id"];

require_once("common/SQLQuery.inc");
$calendar = SQLQuery::create()->select("Calendar")->where("id",$id)->execute_single_row();
if ($calendar == null) { PNApplication::error("Invalid calendar"); return; }

// check this calendar is owned by the current user
$owned = SQLQuery::create()->select("UserCalendar")->where("calendar",$id)->where("username",PNApplication::$instance->user_management->username)->execute_single_row();
if ($owned == null) { PNApplication::error("Access denied"); return; }

if ($calendar["type"] == "internet") {
	$c = curl_init($calendar["data"]);
	curl_setopt($c, CURLOPT_HEADER, FALSE);
	curl_setopt($c, CURLOPT_RETURNTRANSFER, TRUE);
	curl_setopt($c, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($c, CURLOPT_CONNECTTIMEOUT, 10);
	curl_setopt($c, CURLOPT_TIMEOUT, 20);
	$result = curl_exec($c);
	if ($result === FALSE)
		PNApplication::error(curl_error($c));
	curl_close($c);
	if ($result) {
		if (substr($result, 0, 15) == "BEGIN:VCALENDAR") {
			// VCalendar format
			include "vcalendar.inc";
			$cal = parseVCal($result);
			echo "[";
			$first = true;
			foreach ($cal->events as $ev) {
				if ($ev->start == null) continue;
				if ($ev->end == null) continue;
				if ($ev->uid == null) continue;

				if ($first) $first = false; else echo ",";
				echo "{";
				echo "uid:".json_encode($ev->uid);
				echo ",start:".json_encode($ev->start);
				echo ",end:".json_encode($ev->end);
				echo ",modified:".json_encode($ev->last_modified);
				echo ",title:".json_encode($ev->summary);
				echo ",description:".json_encode($ev->description);
				echo ",status:".json_encode($ev->status);
				echo "}";
			}
			echo "]";
		}
	}
	return;
}

PNApplication::error("Invalid calendar: unknown type ".$calendar["type"]);

?>
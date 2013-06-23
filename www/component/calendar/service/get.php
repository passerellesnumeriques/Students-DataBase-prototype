<?php
if (!isset($_GET["id"])) { PNApplication::error("Missing calendar id"); return; }
$id = $_GET["id"];

require_once("common/SQLQuery.inc");
$calendar = SQLQuery::create()->select("Calendar")->where("id",$id)->execute_single_row();
if ($calendar == null) { PNApplication::error("Invalid calendar"); return; }

// check this calendar is accessible by the current user
if (!PNApplication::$instance->calendar->can_read($id, $calendar)) {
	PNApplication::error("Access Denied");
	return;
}

echo "{";
if ($calendar["type"] == "internet") {
	if (time()-intval($calendar["last_modification"]) > 5*60) {
		// need to update from internet
		echo "action:{url:".json_encode("/dynamic/calendar/service/update?id=".$id).",message:".json_encode(get_locale("calendar","Updating calendar from internet",array("name"=>$calendar["name"])))."},";
	}
}

function encode_date_time($date, $time) {
	$d = array();
	$i = strpos($date, "-");
	$d["year"] = substr($date, 0, $i);
	$date = substr($date, $i+1);
	$i = strpos($date, "-");
	$d["month"] = substr($date, 0, $i);
	$d["day"] = substr($date, $i+1);
	if ($time !== null) {
		$i = strpos($time, ":");
		$d["hour"] = substr($time, 0, $i);
		$time = substr($time, $i+1);
		$i = strpos($time, ":");
		$d["minute"] = substr($time, 0, $i);
		$d["second"] = substr($time, $i+1);
	}
	return json_encode($d);
}

// get events
$events = SQLQuery::create()->select("CalendarEvent")->where("calendar", $id)->execute();
echo "events:[";
$first = true;
foreach ($events as $ev) {
	if ($first) $first = false; else echo ",";
	echo "{";
	echo "id:".json_encode($ev["id"]);
	echo ",uid:".json_encode($ev["uid"]);
	echo ",start:".encode_date_time($ev["start_date"],$ev["start_time"]);
	echo ",end:".encode_date_time($ev["end_date"],$ev["end_time"]);
	echo ",modified:".json_encode($ev["last_modified"]);
	echo ",title:".json_encode($ev["title"]);
	echo ",description:".json_encode($ev["description"]);
	echo ",location:".json_encode($ev["location"]);
	echo ",organizer:".json_encode($ev["organizer"]);
	echo ",attendees:".$ev["attendees"];
	if ($ev["freq"] <> null)
		echo ",freq:[".$ev["freq"]."]";
	echo "}";
}
echo "]}";
?>
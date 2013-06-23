<?php
function update_internet_calendar($id, $url = null, $last_modification = null) {
	require_once("common/SQLQuery.inc");
	if ($url === null || $last_modification === null) {
		$r = SQLQuery::create()->select("Calendar")->field("data")->field("last_modification")->where("id", $id)->execute_single_row();
		$url = $r["data"];
		$last_modification = intval($r["last_modification"]);
	}

	$now = time();
	$c = curl_init($url);
	curl_setopt($c, CURLOPT_HEADER, FALSE);
	curl_setopt($c, CURLOPT_RETURNTRANSFER, TRUE);
	curl_setopt($c, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($c, CURLOPT_CONNECTTIMEOUT, 30);
	curl_setopt($c, CURLOPT_TIMEOUT, 100);
	if ($last_modification > 0)
		curl_setopt($c, CURLOPT_HTTPHEADER, array('If-Modified-Since: '.gmdate('D, d M Y H:i:s \G\M\T', $last_modification)));
	set_time_limit(150);
	$result = curl_exec($c);
	if ($last_modification > 0 && curl_getinfo($c, CURLINFO_HTTP_CODE) == 304)
		return; // no recent update
	if ($result === FALSE)
		PNApplication::error(curl_error($c));
	curl_close($c);
	if ($result) {
		// update modification
		SQLQuery::update("Calendar", array("last_modification"=>$now), array("id"=>$id));
		// retrieve current events on this calendar
		$events = SQLQuery::create()->select("CalendarEvent")->where("calendar", $id)->execute();

		if (substr($result, 0, 15) == "BEGIN:VCALENDAR") {
			// VCalendar format
			include "vcalendar.inc";
			$cal = parseVCal($result);
			/* @var $ev VCalendar_Event */
			foreach ($cal->events as $ev) {
				if ($ev->start == null) continue;
				if ($ev->end == null) continue;
				if ($ev->uid == null) continue;
				// look for same event already in database
				$found = null;
				for ($i = 0; $i < count($events); $i++) {
					$e = $events[$i];
					if ($e["uid"] == $ev->uid) {
						$found = $e;
						array_splice($events, $i, 1); // remove it
						break;
					}
				}

				if ($found == null) {
					// this is a new event
					$a = array(
						"calendar"=>$id,
						"uid"=>$ev->uid,
						"start_date"=>$ev->start["year"]."-".$ev->start["month"]."-".$ev->start["day"],
						"start_time"=>(isset($ev->start["hour"]) ? $ev->start["hour"].":".$ev->start["minute"].":".$ev->start["second"] : null),
						"end_date"=>$ev->end["year"]."-".$ev->end["month"]."-".$ev->end["day"],
						"end_time"=>(isset($ev->end["hour"]) ? $ev->end["hour"].":".$ev->end["minute"].":".$ev->end["second"] : null),
						"last_modified"=>gmmktime($ev->last_modified["hour"], $ev->last_modified["minute"], $ev->last_modified["second"], $ev->last_modified["month"], $ev->last_modified["day"], $ev->last_modified["year"]),
						"title"=>$ev->summary,
						"description"=>$ev->description,
						"location"=>$ev->location,
						"organizer"=>$ev->organizer,
						"attendees"=>json_encode($ev->attendees),
					);
					if ($ev->recur <> null) {
						$a["freq"] = json_encode($ev->recur->freq);
						$a["freq"] .= ",".$ev->recur->interval;
						$a["freq"] .= ",".json_encode($ev->recur->valid_months);
						$a["freq"] .= ",".json_encode($ev->recur->valid_hours);
						$a["freq"] .= ",".json_encode($ev->recur->valid_week_days);
						$a["freq"] .= ",".json_encode($ev->recur->valid_month_days);
						$a["freq"] .= ",".json_encode($ev->recur->valid_year_days);
						$a["freq"] .= ",".json_encode($ev->recur->valid_weeks);
						$a["freq"] .= ",".json_encode($ev->recur->until <> null ? $ev->recur->until["year"]."-".$ev->recur->until["month"]."-".$ev->recur->until["day"] : null);
						$a["freq"] .= ",".json_encode($ev->recur->until <> null && isset($ev->recur->until["hour"]) ? $ev->recur->until["hour"].":".$ev->recur->until["minute"].":".$ev->recur->until["second"] : null);
						$a["freq"] .= ",".json_encode($ev->recur->count);
						$a["freq"] .= ",".json_encode($ev->recur->wkst);
					}
					SQLQuery::insert("CalendarEvent", $a);
				} else {
					$modif = gmmktime($ev->last_modified["hour"], $ev->last_modified["minute"], $ev->last_modified["second"], $ev->last_modified["month"], $ev->last_modified["day"], $ev->last_modified["year"]);
					if ($modif <> $found["last_modified"]) {
						// update event in database
						$a = array(
							"start_date"=>$ev->start["year"]."-".$ev->start["month"]."-".$ev->start["day"],
							"start_time"=>(isset($ev->start["hour"]) ? $ev->start["hour"].":".$ev->start["minute"].":".$ev->start["second"] : null),
							"end_date"=>$ev->end["year"]."-".$ev->end["month"]."-".$ev->end["day"],
							"end_time"=>(isset($ev->end["hour"]) ? $ev->end["hour"].":".$ev->end["minute"].":".$ev->end["second"] : null),
							"last_modified"=>$modif,
							"title"=>$ev->summary,
							"description"=>$ev->description,
							"location"=>$ev->location,
							"organizer"=>$ev->organizer,
							"attendees"=>json_encode($ev->attendees),
						);
						if ($ev->recur <> null) {
							$a["freq"] = json_encode($ev->recur->freq);
							$a["freq"] .= ",".$ev->recur->interval;
							$a["freq"] .= ",".json_encode($ev->recur->valid_months);
							$a["freq"] .= ",".json_encode($ev->recur->valid_hours);
							$a["freq"] .= ",".json_encode($ev->recur->valid_week_days);
							$a["freq"] .= ",".json_encode($ev->recur->valid_month_days);
							$a["freq"] .= ",".json_encode($ev->recur->valid_year_days);
							$a["freq"] .= ",".json_encode($ev->recur->valid_weeks);
							$a["freq"] .= ",".json_encode($ev->recur->until <> null ? $ev->recur->until["year"]."-".$ev->recur->until["month"]."-".$ev->recur->until["day"] : null);
							$a["freq"] .= ",".json_encode($ev->recur->until <> null && isset($ev->recur->until["hour"]) ? $ev->recur->until["hour"].":".$ev->recur->until["minute"].":".$ev->recur->until["second"] : null);
							$a["freq"] .= ",".json_encode($ev->recur->count);
							$a["freq"] .= ",".json_encode($ev->recur->wkst);
						}
						SQLQuery::update("CalendarEvent", $a, array("id"=>$found["id"]));
					}
				}
			}
			// remove the remaining events in the database
			if (count($events) > 0)
				SQLQuery::remove_rows("CalendarEvents", $events);
		}
	}

}
?>
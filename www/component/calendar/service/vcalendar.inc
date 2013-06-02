<?php
class VCalendar {

	public $timezones = array();
	public $events = array();

}
class VCalendar_TimeZone {
	public $id = "";
	public $offset = 0;
}
class VCalendar_Event {
	public $start = null;
	public $end = null;
	public $uid = null;
	public $last_modified = null;
	public $description = "";
	public $summary = "";
	public $status = null;
}

function parse_VCal_Date($s) {
	$d = array();
	$d["year"] = intval(substr($s, 0, 4));
	$d["month"] = intval(substr($s, 4, 2));
	$d["day"] = intval(substr($s, 6, 2));
	$i = strpos($s, "T");
	if ($i !== FALSE) {
		$d["hour"] = intval(substr($s, $i+1, 2));
		$d["minute"] = intval(substr($s, $i+3, 2));
		$d["second"] = intval(substr($s, $i+5, 2));
	}
	return $d;
}
function _check_date(&$d) {
	while ($d["second"] < 0) {
		$d["second"] += 60;
		$d["minute"]--;
	}
	while ($d["second"] > 59) {
		$d["second"] -= 60;
		$d["minute"]++;
	}
	while ($d["minute"] < 0) {
		$d["minute"] += 60;
		$d["hour"]--;
	}
	while ($d["minute"] > 59) {
		$d["minute"] -= 60;
		$d["hour"]++;
	}
	while ($d["hour"] < 0) {
		$d["hour"] += 24;
		$d["day"]--;
	}
	while ($d["hour"] > 23) {
		$d["hour"] -= 24;
		$d["day"]++;
	}
	while ($d["month"] <= 0) {
		$d["year"]--;
		$d["month"] += 12;
	}
	while ($d["month"] > 12) {
		$d["year"]++;
		$d["month"] -= 12;
	}
	while ($d["day"] <= 0) {
		if ($d["month"] > 1) {
			$d["month"]--;
			$d["days"] = cal_days_in_month(CAL_GREGORIAN, $d["month"], $d["year"])+$d["days"];
			continue;
		}
		$d["month"] = 12;
		$d["year"]--;
		$d["days"] = cal_days_in_month(CAL_GREGORIAN, $d["month"], $d["year"])+$d["days"];
	}
	while ($d["day"] > cal_days_in_month(CAL_GREGORIAN, $d["month"], $d["year"])) {
		$d["day"] -= cal_days_in_month(CAL_GREGORIAN, $d["month"], $d["year"]);
		$d["month"]++;
		if ($d["month"] > 12) { $d["month"] = 1; $d["year"]++; }
	}
}
function VCalendar_Date($l, $cal) {
	$date = $l[2];
	if (substr($date,strlen($date)-1) == "Z")
		$d = parse_VCal_Date(substr($date,0,strlen($date)-1));
	else if (isset($l[1]["TZID"])) {
		$d = parse_VCal_Date($date);
		foreach ($cal->timezones as $tz) {
			if ($tz->id == $l[1]["TZID"]) {
				$d["minute"] += $tz->offset;
				_check_date($d);
				break;
			}
		}
	} else {
		$d = parse_VCal_Date($date);
	}
	return $d;
}

function startWith($line, $start) {
	if (substr($line, 0, strlen($start)) == $start) return true;
	return false;
}

function parseVCal($s) {
	$cal = new VCalendar();
	$lines = explode("\n",$s);

	// merge splitted lines
	for ($i = 0; $i < count($lines); $i++) {
		if (substr($lines[$i],0,1) == " ") {
			// this is the continuation of previous line
			$lines[$i-1] .= substr($lines[$i],1);
			$i--;
			$lines[$i] = trim($lines[$i]);
			array_splice($lines, $i+1, 1);
			continue;
		}
		$lines[$i] = trim($lines[$i]);
	}

	// split lines into keyword, attributes, value
	$vlines = array();
	for ($i = 0; $i < count($lines); $i++) {
		$j = strpos($lines[$i], ":");
		$k = strpos($lines[$i], ";");
		$vline = array("",array(),null);
		if ($k > 0 && $k < $j) {
			// KEYWORD;ATTRIBUTES:VALUE
			$vline[0] = substr($lines[$i],0,$k);
			$vline[2] = substr($lines[$i],$j+1);
			$s = substr($lines[$i],$k+1,$j-$k-1);
			$attrs = explode(";",$s);
			foreach ($attrs as $attr) {
				$j = strpos($attr, "=");
				if ($j <= 0)
					$vline[1][$attr] = "";
				else
					$vline[1][substr($attr,0,$j)] = substr($attr,$j+1);
			}
		} else {
			// KEYWORD:VALUE
			$vline[0] = substr($lines[$i],0,$j);
			$vline[2] = substr($lines[$i],$j+1);
		}
		array_push($vlines, $vline);
	}

	for ($i = 0; $i < count($vlines); $i++) {
		if ($vlines[$i][0] == "BEGIN" && $vlines[$i][2] == "VCALENDAR")
			$i = _VCALENDAR($vlines, $i+1, $cal);
	}
	return $cal;
}
function _VCALENDAR($lines, $i, &$cal) {
	while ($i < count($lines)) {
		if ($lines[$i][0] == "END" && $lines[$i][2] == "VCALENDAR")
			return $i;
		if ($lines[$i][0] == "BEGIN") {
			if ($lines[$i][2] == "VTIMEZONE")
				$i = _VTIMEZONE($lines, $i, $cal);
			else if ($lines[$i][2] == "VEVENT")
				$i = _VEVENT($lines, $i, $cal);
		}
		$i++;
	}
	return $i;
}
function _VTIMEZONE($lines, $i, &$cal) {
	$tz = new VCalendar_TimeZone();
	while ($i < count($lines)) {
		if ($lines[$i][0] == "END" && $lines[$i][2] == "VTIMEZONE")
			break;
		if ($lines[$i][0] == "TZID")
			$tz->id = $lines[$i][2];
		if ($lines[$i][0] == "TZOFFSETFROM") {
			$sign = substr($lines[$i][2], 0, 1);
			if ($sign == '+')
				$tz->offset = intval(substr($lines[$i][2],1));
			else if ($sign == '-')
				$tz->offset = -intval(substr($lines[$i][2],1));
			else
				$tz->offset = intval($lines[$i][2]);
			// convert into minutes
			$tz->offset = ($tz->offset/100)*60+($tz->offset%100);
		}
		$i++;
	}
	array_push($cal->timezones, $tz);
	return $i;
}
function _VEVENT($lines, $i, &$cal) {
	$ev = new VCalendar_Event();
	while ($i < count($lines)) {
		if ($lines[$i][0] == "END" && $lines[$i][2] == "VEVENT")
			break;
		if ($lines[$i][0] == "DTSTART")
			$ev->start = VCalendar_Date($lines[$i], $cal);
		if ($lines[$i][0] == "DTEND")
			$ev->end = VCalendar_Date($lines[$i], $cal);
		if ($lines[$i][0] == "LAST-MODIFIED")
			$ev->last_modified = VCalendar_Date($lines[$i], $cal);
		if ($lines[$i][0] == "UID")
			$ev->uid = $lines[$i][2];
		if ($lines[$i][0] == "DESCRIPTION")
			$ev->description = $lines[$i][2];
		if ($lines[$i][0] == "SUMMARY")
			$ev->summary = $lines[$i][2];
		if ($lines[$i][0] == "STATUS")
			$ev->status = $lines[$i][2];
		$i++;
	}
	array_push($cal->events, $ev);
	return $i;
}
?>
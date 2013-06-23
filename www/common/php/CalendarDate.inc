<?php
class CalendarDate {
	
	private $timestamp;
	private $year, $month, $day;
	private $hour, $minute, $second;
	//private $week_num;
	private $week_day;
	private $year_day;
	
	public function __construct($time = null) {
		if ($time == null) $time = time();
		$this->timestamp = $time;
		$this->update_from_timestamp();
	}
	
	private function update_from_timestamp() {
		$d = getdate($this->timestamp);
		$this->year = $d["year"];
		$this->month = $d["mon"];
		$this->day = $d["mday"];
		$this->hour = $d["hours"];
		$this->minute = $d["minutes"];
		$this->second = $d["seconds"];
		$this->week_day = $d["wday"];
		$this->year_day = $d["yday"];
	}
	
	public function set_date($year, $month, $day) {
		$this->timestamp = mktime($this->hour, $this->minute, $this->second, $month, $day, $year);
		$this->update_from_timestamp();
	}
	public function set_time($hour, $minute, $second) {
		$this->timestamp = mktime($hour, $minute, $second, $this->month, $this->day, $this->year);
		$this->update_from_timestamp();
	}
	
	public function get_timestamp() { return $this->timestamp; }
	public function get_year() { return $this->year; }
	public function get_month() { return $this->month; }
	public function get_day() { return $this->day; }
	public function get_hour() { return $this->hour; }
	public function get_minute() { return $this->minute; }
	public function get_second() { return $this->second; }
	public function get_week_day() { return $this->week_day; }
	public function get_year_day() { return $this->year_day; }
	
	public function add_seconds($seconds) {
		$this->timestamp += $seconds;
		$this->update_from_timestamp();
	}
	public function add_minutes($minutes) {
		$this->timestamp += $minutes*60;
		$this->update_from_timestamp();
	}
	public function add_hours($hours) {
		$this->timestamp += $hours*60*60;
		$this->update_from_timestamp();
	}
	public function add_days($days) {
		$this->timestamp += $days*24*60*60;
		$this->update_from_timestamp();
	}
	public function add_months($months) {
		$this->timestamp = mktime($this->hour, $this->minute, $this->second, $this->month + $months, $this->day, $this->year);
		$this->update_from_timestamp();
	}
	public function add_years($years) {
		$this->timestamp = mktime($this->hour, $this->minute, $this->second, $this->month, $this->day, $this->year + $years);
		$this->update_from_timestamp();
	}
	
} 
?>
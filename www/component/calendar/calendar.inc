<?php
class calendar extends Component {

	protected function is_page_allowed($path) {
		return true;
	}
	protected function is_service_allowed($path) {
		return true;
	}

	public function can_read($calendar_id, $calendar = null) {
		require_once("common/SQLQuery.inc");
		// if owner, access allowed
		$owned = SQLQuery::create()->select("UserCalendar")->where("calendar",$id)->where("username",PNApplication::$instance->user_management->username)->execute_single_row();
		if ($owned <> null) return true;
		// check read_rights
		if ($calendar == null) {
			$calendar = SQLQuery::create()->select("Calendar")->where("id",$id)->execute_single_row();
			if ($calendar == null) return false;
		}
		if ($calendar["read_rights"] == null) return true;
		$rights = json_decode($calendar["read_rights"]);
		foreach ($rights as $name=>$value)
			if (PNApplication::$instance->user_management->has_right($name, $value))
				return true;
		return false;
	}
	public function can_write($calendar_id, $calendar = null) {
		require_once("common/SQLQuery.inc");
		// if owner, access allowed
		$owned = SQLQuery::create()->select("UserCalendar")->where("calendar",$id)->where("username",PNApplication::$instance->user_management->username)->execute_single_row();
		if ($owned <> null) return true;
		// check read_rights
		if ($calendar == null) {
			$calendar = SQLQuery::create()->select("Calendar")->where("id",$id)->execute_single_row();
			if ($calendar == null) return false;
		}
		if ($calendar["write_rights"] == null) return false; // if no specific rights specified, only the owner can write
		$rights = json_decode($calendar["write_rights"]);
		foreach ($rights as $name=>$value)
			if (PNApplication::$instance->user_management->has_right($name, $value))
				return true;
		return false;
	}

	public function create_calendar($name, $owner, $read_rights, $write_rights) {
		require_once("common/SQLQuery.inc");
		$id = SQLQuery::insert("Calendar", array("type"=>"internal", "name"=>$name, "owner"=>$owner, "read_rights"=>json_encode($read_rights), "write_rights"=>json_encode($write_rights)));
		if ($owner <> null)
			SQLQuery::insert("UserCalendar", array("username"=>$owner, "calendar"=>$id));
		return $id;
	}

}
?>
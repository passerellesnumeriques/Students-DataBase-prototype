<?php
class geography extends Component {

	private static $countries = null;

	public function getCountries() {
		if (geography::$countries == null) {
			geography::$countries = array();
			$res = DataBase::execute("SELECT * FROM geography.country");
			while (($r = DataBase::next_row($res)) <> null) {
				$c = new Country($r["code"], $r["sovereign"], utf8_encode($r["name"]), intval($r["divisions"]));
				array_push(geography::$countries, $c);
			}
		}
		return geography::$countries;
	}

	public function is_page_allowed($path) {
		return true; // TODO
	}
	public function is_service_allowed($path) {
		return true; // TODO
	}

	public function service($path) {
		if ($path == "get_geonames") {
			// TODO security
			include "component/geography/service/get_geonames.php";
			return;
		}
		parent::service($path);
	}

}

class Country {

	private $code;
	private $sovereign;
	private $name;
	private $divisions;

	public function __construct($code, $sovereign = null, $name = null, $divisions = null) {
		$this->code = $code;
		$this->sovereign = $sovereign;
		$this->name = $name;
		$this->divisions = $divisions;
	}

	public function getCode() { return $this->code; }
	public function getName() { $this->_load(); return $this->name; }
	public function getSovereign() { $this->_load(); return $this->sovereign; }
	public function getDivisions() { $this->_load(); return $this->divisions; }

	private function _load() {
		if ($this->divisions !== null) return;
		$res = DataBase::execute("SELECT * FROM geography.country WHERE code='".DataBase::escape_string($this->code)."'");
		$r = DataBase::next_row($res);
		$this->sovereign = $r["sovereign"];
		$this->name = $r["name"];
		$this->divisions = intval($r["timezone"]);
	}

}
?>
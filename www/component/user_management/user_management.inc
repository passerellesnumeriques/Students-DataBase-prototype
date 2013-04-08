<?php 
class user_management extends Component {
	
	public $domain = null;
	public $username = null;
	
	public function login($domain, $username, $password) {
		if ($this->username <> null) return false;
		if (!$this->app->authentication->authenticate($domain, $username, $password)) return false;
		$this->domain = $domain;
		$this->username = $username;
		require_once("common/SQLQuery.inc");
		$people = SQLQuery::create()->select("Users")->field("people")->where("domain",$domain)->where("username",$username)->execute_single_value();
		if ($people <> null)
			$this->app->people->logged($people);
		return true;
	}
	
	public function logout() {
		session_destroy();
	}
	
	private $rights = null;
	public function has_right($right_name, $value = true) {
		if ($rights == null) {
			require_once("common/SQLQuery.inc");
			$q = new SQLQuery();
			$roles = $q->select("UserRole")->field("role_id")->where("domain",$this->domain)->where("username",$this->username)->execute_single_field();
			$user_rights = $q->select("UserRights")->fields("right","value")->where("domain",$this->domain)->where("username",$this->username)->execute();
			$rights = array();
			foreach ($user_rights as $r)
				$rights[$r["right"]] = $this->get_right_value($r["value"]);
			$role_rights = $q->select("RoleRights")->fields("right","value")->where_in("role_id", $roles)->execute();
			foreach ($role_rights as $r) {
				if (!isset($rights[$r["right"]]))
					$rights[$r["rights"]] = $this->get_right_value($r["value"]);
				else
					$rights[$r["rights"]] = $this->get_higher_right($rights[$r["rights"]], $this->get_right_value($r["value"]));
			}
			$this->rights = $rights;
		}
		if (!isset($this->rights[$right_name])) return false;
		return $this->rights[$right_name] === $value;
	}
	private function get_right_value($s) {
		if ($s == "true") return true;
		if ($s == "false") return false;
		if ($s == "null") return null;
		return $s;
	}
	private function get_higher_right($v1, $v2) {
		if (is_bool($v1)) return $v1 | $v2;
		return $v1; // TODO
	}
	
	public function populate_model(&$model) {
		$model->addTable("Users")
			->addString("domain")
			->addString("username")
			->addForeignKey("people", "People")
			->addIndex("domain","username");
		$model->addData("User","Domain","Users", "domain");
		$model->addData("User","Username","Users", "username");
		$model->addTable("UserRights")
			->addString("domain", 100)
			->addString("username", 100)
			->addString("right", 100)
			->addString("value")
			->addIndex("domain","username")
			->addLink("Users", array("domain"=>"domain","username"=>"username"), false);
		$model->addTable("Role")
			->addPrimaryKey("id")
			->addString("name");
		$model->addData("User","Role","Role", "name");
		$model->addTable("UserRole")
			->addString("domain", 100)
			->addString("username", 100)
			->addForeignKey("role_id", "Role")
			->addIndex("domain","username")
			->addLink("Users", array("domain"=>"domain","username"=>"username"), false);
		$model->addTable("RoleRights")
			->addForeignKey("role_id", "Role")
			->addString("right", 100)
			->addString("value")
			->addIndex("role_id");
	}
	public function dependencies() { return array("people"); }
	
	protected function is_page_allowed($path) {
		// TODO
		return true;
	}
	
}
?>
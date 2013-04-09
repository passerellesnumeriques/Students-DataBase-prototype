<?php 
require_once("common/Event.inc");
class user_management extends Component {
	
	/** @var string domain of the user logged in */
	public $domain = null;
	/** @var string username of the user logged in */
	public $username = null;
	
	/** @var Event raised when a user logged in */
	public $logged_in;
	/** @var Event raised when the user logout */
	public $logged_out;
	
	function init() {
		$this->logged_in = new Event();
		$this->logged_out = new Event();
	}
	
	public function login($domain, $username, $password) {
		if ($this->username <> null) return false;
		$auth = $this->app->authentication->get_authentication_system($domain);
		if ($auth == null) return false;
		if (!$auth->authenticate($username, $password)) return false;
		$this->domain = $domain;
		$this->username = $username;
		$this->logged_in->fire();
		setcookie("domain",$this->domain,time()+30*24*60*60,"/dynamic/application/page/login");
		setcookie("username",$this->username,time()+30*24*60*60,"/dynamic/application/page/login");
		return true;
	}
	
	public function logout() {
		$this->logged_out->fire();
		setcookie(session_name(), "", time()-10000, "/dynamic/");
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
	
	protected function is_page_allowed($path) {
		// TODO
		return true;
	}
	
}
?>
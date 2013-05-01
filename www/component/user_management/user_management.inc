<?php
require_once("common/Event.inc");
class user_management extends Component {

	/** @var string domain of the user logged in */
	public $domain = null;
	/** @var string username of the user logged in */
	public $username = null;
	/** @var string authentication token returned by the authentication system */
	public $auth_token = null;

	/** @var Event raised when a user logged in */
	public $logged_in;
	/** @var Event raised when the user logout */
	public $logged_out;

	function init() {
		$this->logged_in = new Event();
		$this->logged_out = new Event();
	}

	public function login($domain, $username, $password) {
		if ($this->username <> null) $this->logout();
		$auth = $this->app->authentication->get_authentication_system($domain);
		if ($auth == null) return false;
		$this->auth_token = $auth->authenticate($username, $password);
		if ($this->auth_token === null) return false;
		$this->domain = $domain;
		$this->username = $username;
		$this->logged_in->fire();
		return true;
	}

	public function logout() {
		$this->logged_out->fire();
		setcookie(session_name(), "", time()-10000, "/dynamic/");
		session_destroy();
	}

	private $rights = array();
	private $is_admin = array();
	public function has_right($right_name, $value = true) {
		if (!isset($this->rights[PNApplication::$instance->current_domain])) {
			// retrieve all possible rights
			$all_rights = array();
			foreach ($this->app->components as $c) {
				foreach ($c->get_readable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
				foreach ($c->get_writable_rights() as $cat) foreach ($cat->rights as $r) $all_rights[$r->name] = $r;
			}

			require_once("common/SQLQuery.inc");
			// get roles of the user
			$roles = SQLQuery::create()->bypass_security()->select("UserRole")->field("role_id")->where("domain",$this->domain)->where("username",$this->username)->execute_single_field();
			// check if the user is an administrator
			foreach ($roles as $role_id)
				if ($role_id == 0) { $this->is_admin[PNApplication::$instance->current_domain] = true; break; }
			if (!isset($this->is_admin[PNApplication::$instance->current_domain])) {
				// get rights directly attached to the user
				$user_rights = SQLQuery::create()->bypass_security()->select("UserRights")->field("right")->field("value")->where("domain",$this->domain)->where("username",$this->username)->execute();
				$rights = array();
				if ($user_rights)
				foreach ($user_rights as $r)
					$rights[$r["right"]] = $all_rights[$r["right"]]->parse_value($r["value"]);
				// merge with rights attached to the roles of the user
				if (count($roles) > 0)
					$role_rights = SQLQuery::create()->bypass_security()->select("RoleRights")->field("right")->field("value")->where_in("role_id", $roles)->execute();
				else $role_rights = null;
				if ($role_rights)
				foreach ($role_rights as $r) {
					if (!isset($rights[$r["right"]]))
						$rights[$r["right"]] = $all_rights[$r["right"]]->parse_value($r["value"]);
					else
						$rights[$r["right"]] = $all_rights[$r["right"]]->get_higher_value($rights[$r["right"]], user_management::parse_value($r["value"]));
				}
				// check all rights exist, and compute implications
				user_management::compute_rights_implications($rights, $all_rights);

				// if we are not on the local domain, remove all writable rights, to make everything read-only
				if (PNApplication::$instance->current_domain != file_get_contents("local_domain")) {
					foreach (PNApplication::$instance->components as $cname=>$c)
						foreach ($c->get_writable_rights() as $rname=>$rtype)
							unset($rights[$rname]);
				}
				$this->rights[PNApplication::$instance->current_domain] = $rights;
			} else {
				// in case this is an admin, only put names of rights
				$is_local = PNApplication::$instance->current_domain == file_get_contents("local_domain");
				$rights = array();
				foreach (PNApplication::$instance->components as $cname=>$c) {
					foreach ($c->get_readable_rights() as $rname=>$rtype)
						array_push($rights, $rname);
					if ($is_local)
						foreach ($c->get_writable_rights() as $rname=>$rtype)
							array_push($rights, $rname);
				}
				$this->rights[PNApplication::$instance->current_domain] = $rights;
			}
		}
		if (isset($this->is_admin[PNApplication::$instance->current_domain])) return in_array($right_name, $this->rights[PNApplication::$instance->current_domain]);
		if (!isset($this->rights[PNApplication::$instance->current_domain][$right_name])) return false;
		return $this->rights[PNApplication::$instance->current_domain][$right_name] === $value;
	}
	public function compute_rights_implications(&$rights, &$all_rights) {
		$changed = false;
		do {
			$changed = false;
			foreach ($rights as $name=>$value) {
				$right = @$all_rights[$name];
				if ($right === null) {
					PNApplication::error("Unknown right '".$name."'");
					unset($rights[$name]);
					continue;
				}
				foreach ($right->implies as $if_value=>$implications)
					if ($value == $if_value)
					foreach ($implications as $rname=>$rvalue) {
						if (isset($rights[$rname])) {
							$higher = $all_rights[$rname]->get_higher_value($rights[$name], $rvalue);
							if ($rights[$rname] !== $higher) {
								$rights[$rname] = $higher;
								$changed = true;
							}
						} else {
							$rights[$rname] = $rvalue;
							$changed = true;
						}
					}
			}
		} while ($changed);
	}

	protected function is_page_allowed($path) {
		if ($path == "home") return true;
		if ($path == "users")
			return $this->has_right("consult_user_list");
		if ($path == "user_rights")
			return $this->has_right("consult_user_rights");
		if ($path == "role_rights")
			return $this->has_right("manage_roles");
		if ($path == "roles")
			return $this->has_right("manage_roles");
		return false;
	}
	protected function is_service_allowed($path) {
		if ($path == "save_user_rights")
			return $this->has_right("edit_user_rights");
		if ($path == "save_role_rights")
			return $this->has_right("manage_roles");
		if ($path == "create_role" || $path == "remove_role")
			return $this->has_right("manage_roles");
		return false;
	}

	public function get_readable_rights() {
		return array(
			new AccessRightCategory("user_management","User Management",array(
				new BooleanRight("consult_user_list", "See the list of users"),
				new BooleanRight("consult_user_roles", "See roles of a user"),
				new BooleanRight("consult_user_rights", "See rights of a user",array(true=>array("consult_user_roles"=>true))),
			))
		);
	}
	public function get_writable_rights() {
		return array(
			new AccessRightCategory("user_management","User Management",array(
				new BooleanRight("manage_roles", "Manage roles (create/remove/edit)"),
				new BooleanRight("assign_role", "Assign/Unassign roles to users",array(true=>array("consult_user_list"=>true,"consult_user_role"=>true))),
				new BooleanRight("edit_user_rights", "Assign/Unassign rights to users",array(true=>array("consult_user_list"=>true,"consult_user_rights"=>true))),
			))
		);
	}

	public function get_profile_pages(&$people) {
		$domain = @$_GET["domain"];
		$username = @$_GET["username"];
		if ($domain <> null && $people == null) {
			$people = $this->app->people->get_people_from_user($domain, $username);
		} else if ($domain == null && $people <> null) {
			$this->app->people->get_user_from_people($people, $domain, $username);
		}
		return array(
			"user_rights"=>array("/static/user_management/access_list.png", get_locale("user_management","User Rights"),"/dynamic/user_management/page/user_rights?domain=".$domain."&username=".$username),
		);
	}

}

class AccessRightCategory {
	public $locale_namespace;
	public $locale_name;
	public $rights;
	public function __construct($locale_namespace, $category_name, $rights) {
		$this->locale_namespace = $locale_namespace;
		$this->locale_name = $category_name;
		$this->rights = $rights;
	}
}
abstract class Right {
	public $name;
	public $locale;
	public $implies;
	public function __construct($name, $locale, $implies = array()) {
		$this->name = $name;
		$this->locale = $locale;
		$this->implies = $implies;
	}
	public abstract function parse_value($value);
	public abstract function get_higher_value($v1, $v2);
	public abstract function is_highest($value);
}
class BooleanRight extends Right {
	public function parse_value($value) {
		return $value === true || $value === "1" || $value === "true" || $value === "on";
	}
	public function get_higher_value($v1, $v2) {
		return $v1 || $v2;
	}
	public function is_highest($value) {
		return $value;
	}
}
?>
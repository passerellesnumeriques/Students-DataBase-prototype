<?php
require_once("AuthenticationSystem.inc"); 
class FakeAuthenticationSystem extends AuthenticationSystem {
	
	public function authenticate($username, $password) {
		return true;
	}
	
	public function get_user_list() {
		return array(
			"test"
		);
	}
	
}
?>
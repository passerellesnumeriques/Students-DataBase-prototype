<?php 
class authentication extends Component {

	public function get_authentication_system($domain) {
		// TODO
		require_once("FakeAuthenticationSystem.inc");
		return new FakeAuthenticationSystem();
	}
	
}
?>
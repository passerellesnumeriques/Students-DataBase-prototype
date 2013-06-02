<?php
class authentication extends Component {

	public function get_authentication_system($domain) {
		require_once("FakeAuthenticationSystem.inc");
		require_once("URLAuthenticationSystem.inc");
		$local = file_get_contents("local_domain");
		switch ($domain) {
			case "PNP":
				if ($local == "PNP") return new FakeAuthenticationSystem();
				return new URLAuthenticationSystem("http://pncebu.dyndns.org:8000/authentication_system.php/");
			default:
				return new FakeAuthenticationSystem();
		}
	}

}
?>
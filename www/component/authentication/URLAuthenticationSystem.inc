<?php
require_once("AuthenticationSystem.inc");
class URLAuthenticationSystem extends AuthenticationSystem {

	public function __construct($base_url) {
		$this->base_url = $base_url;
	}

	private $base_url;

	public function authenticate($username, $password) {
		$c = curl_init($this->base_url."authenticate");
		curl_setopt($c, CURLOPT_COOKIESESSION, TRUE);
		curl_setopt($c, CURLOPT_HEADER, FALSE);
		curl_setopt($c, CURLOPT_POST, TRUE);
		curl_setopt($c, CURLOPT_RETURNTRANSFER, TRUE);
		curl_setopt($c, CURLOPT_POSTFIELDS, array("username"=>$username,"password"=>$password));
		$result = curl_exec($c);
		curl_close($c);
		$i = strpos($result, "\n");
		if ($i === FALSE) return null;
		return substr($result, $i+1);
	}

}
?>
<?php 
abstract class AuthenticationSystem {
	
	/**
	 * @return boolean
	 * @param string $username
	 * @param string $password
	 */
	public abstract function authenticate($username, $password);
	
	/**
	 * @return array of usernames, or NULL if this functionality is not available
	 */
	public function get_user_list() { return null; }
	
	/**
	 * @param string $username
	 * @param string $password
	 * @return boolean true if successful, false if it fails, null if the functionality is not available
	 */
	public function create_user($username, $password) { return null; }
	
	/**
	 * @param string $username
	 * @return boolean true if successful, false if it fails, null if the functionality is not available
	 */
	public function remove_user($username) { return null; }

	/**
	 * @param string $username
	 * @param string $new_username
	 * @return boolean true if successful, false if it fails, null if the functionality is not available
	 */
	public function rename_user($username, $new_username) { return null; }
	
}
?>
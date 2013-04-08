<?php 
require_once("DataBaseSystem.inc");
class DataBaseSystem_MySQL extends DataBaseSystem {
	
	public function connect($server, $username, $password, $db_name) {
		$this->conn = mysql_pconnect($server, $username, $password);
		if ($this->conn === false)
			$this->conn = mysql_connect($this->server, $this->username, $this->password);
		if ($this->conn === false)
			return DataBaseSystem::ERR_CANNOT_CONNECT_TO_SERVER;
		if (!mysql_select_db($db_name, $this->conn))
			return DataBaseSystem::ERR_DB_DOES_NOT_EXIST;
		return DataBaseSystem::ERR_OK;
	}
	
	private $conn = null;
	
	protected function execute_sql($sql_query) {
		return mysql_query($sql_query, $this->conn);
	}
	
	public function next_row($query_result) {
		return mysql_fetch_array($query_result);
	}
	
	public function get_insert_id() {
		return mysql_insert_id($this->conn);
	}
	
	public function get_last_error_number() {
		return mysql_errno($this->conn);
	}
	
	public function get_last_error_message() {
		return mysql_error($this->conn);
	}
	
	public function escape_string($str) {
		return mysql_real_escape_string($str);
	} 
	
}
?>
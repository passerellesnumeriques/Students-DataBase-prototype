<?php 
abstract class DataBaseSystem {

	const ERR_OK = 0;
	const ERR_CANNOT_CONNECT_TO_SERVER = 1;
	const ERR_INVALID_CREDENTIALS = 2;
	const ERR_DB_DOES_NOT_EXIST = 3;
	public abstract function connect($server, $username, $password, $db_name);
	
	public function execute($sql_query, $report_errors = true) {
		$res = $this->execute_sql($sql_query);
		if ($res === false && $report_errors) {
			$err = $this->get_last_error_number();
			if ($err <> 0)
				PNApplication::error("DataBase Error #".$err.": ".$this->get_last_error_message()." [".$sql_query."]");
		}
		return $res;
	}

	protected abstract function execute_sql($query);
	public abstract function next_row($query_result);
	public abstract function get_insert_id();
	public abstract function get_last_error_number();
	public abstract function get_last_error_message();
	
	public abstract function escape_string($str);

}
?>
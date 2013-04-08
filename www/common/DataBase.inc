<?php 
mysql_pconnect("localhost", "root", "");
mysql_select_db("students");

class DataBase {
	
	public static function query($sql, $log_error = true) {
		$res = mysql_query($sql);
		if (mysql_errno() <> 0 && $log_error)
			array_push(PNApplication::$errors, "MySQL Error #".mysql_errno().": ".mysql_error()."<br/>Query:<br/>".$sql);
		return $res;
	} 
	public static function fetch_array($res) {
		return mysql_fetch_array($res);
	}
	
	public static function update_model(&$model) {
		foreach ($model->tables as $table)
			DataBase::update_table($table);
	}
	private static function update_table(&$table) {
		$res = DataBase::query("DESCRIBE `".$table->name."`", false);
		if ($res === FALSE) { DataBase::create_table($table); return; }
		// TODO update if necessary
	}
	private static function create_table(&$table) {
		$sql = "CREATE TABLE `".$table->name."` (";
		$first = true;
		foreach ($table->columns as $col) {
			if ($first) $first = false; else $sql .= ", ";
			$sql .= $col->get_sql();
		}
		$pk = $table->getPrimaryKey();
		if ($pk <> null)
			$sql .= ", PRIMARY KEY(`".$pk->name."`)";
		$sql .= ")";
		DataBase::query($sql);
	}
	
}
?>
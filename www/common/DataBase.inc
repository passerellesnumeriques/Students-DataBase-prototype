<?php 
class DataBase {
	
	public static $conn;
	
}
// TODO make it based on configuration
require_once("DataBaseSystem_MySQL.inc");
DataBase::$conn = new DataBaseSystem_MySQL();
DataBase::$conn->connect("localhost", "root", "", "students"); 
?>
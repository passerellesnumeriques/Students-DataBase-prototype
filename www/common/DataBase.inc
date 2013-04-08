<?php 
/** This class only aims at giving easy access to the DataBase connection */
class DataBase {
	
	/** @var DataBaseSystem */
	public static $conn;
	
}
// TODO make it based on configuration
require_once("DataBaseSystem_MySQL.inc");
DataBase::$conn = new DataBaseSystem_MySQL();
DataBase::$conn->connect("localhost", "root", "", "students"); 
?>
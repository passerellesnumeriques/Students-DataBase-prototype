<?php
abstract class Component {
	
	public function __construct($name) {
		$this->name = $name;
	}
	
	public $name;
	protected $access_rights_by_page = array();
	protected $access_rights_by_service = array();
	
	public function page($path) {
		// TODO check rights
		include "component/".$this->name."/page/".$path.".php";
	}
	
	public function service($path) {
		// TODO check rights
		include "component/".$this->name."/service/".$path.".php";
	}
	
}
?>

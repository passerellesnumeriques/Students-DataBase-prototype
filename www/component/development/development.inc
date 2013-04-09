<?php 
class development extends Component {
	
	/** Override page to allow any page, without any check */
	public function page($path) {
		include "page/".$path.".php";
	}
	
	
}
?>
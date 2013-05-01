<?php 
class data_model extends Component {
	
	protected function is_page_allowed($path) {
		// TODO
		return false;
	}
	protected function is_service_allowed($path) {
		// access to model already contains security checks
		return true;
	}
	
	public function generate_field_type($col) {
		if ($col instanceof \datamodel\ColumnString) return 'string:'.$col->max_length;
		if ($col instanceof \datamodel\ColumnInteger) return 'integer';
		if ($col instanceof \datamodel\ColumnEnum) {
			$type = 'enum[';
			$first = true;
			foreach ($col->values as $v) {
				if ($first) $first = false; else $type .= ",";
				$type .= json_encode($v);
			}
			$type .= "]";
			return $type;
		}
		if ($col instanceof \datamodel\ColumnDate) return "date";
		return 'unknown';
	}
	
}
?>
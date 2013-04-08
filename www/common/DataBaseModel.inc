<?php 

/** This class provides the methods to update the DataBase, based on the current data model.
 * This will be used mainly during development, so each time a change is made in the data model,
 * the change are reflected automatically in the DataBase.
 * For instance, a column is added, or the type of a data changed... 
 */
class DataBaseModel {
	
	public static function update_model(&$model) {
		foreach ($model->tables as $table)
			DataBaseModel::update_table($table);
	}
	private static function update_table(&$table) {
		$res = DataBase::$conn->execute("DESCRIBE `".$table->name."`", false);
		if ($res === FALSE) { DataBaseModel::create_table($table); return; }
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
		DataBase::$conn->execute($sql);
	}
	
}
?>
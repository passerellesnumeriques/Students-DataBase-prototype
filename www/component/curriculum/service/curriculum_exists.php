<?php 
$name = $_POST["name"];
$res = DataBase::$conn->execute("SELECT id FROM Curriculum WHERE name='".DataBase::$conn->escape_string($name)."'");
$result = (!$res || !DataBase::$conn->next_row($res) ? "false" : "true");
PNApplication::print_json_result($result);
?>
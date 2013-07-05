<?php
DataBase::execute("INSERT INTO `geography`.`country_division` (`country`,`level`,`name`,`english_name`) VALUE ('".$_POST["country"]."',".$_POST["level"].",'".DataBase::escape_string(utf8_decode($_POST["local"]))."','".DataBase::escape_string(utf8_decode($_POST["english"]))."')");
echo !PNApplication::has_errors();
?>
<?php 
PNApplication::$instance->current_domain = $_GET["domain"];
header("Location: /");
?>
<?php 
PNApplication::$instance->current_domain = $_GET["domain"];
header("Location: /dynamic/application/page/enter");
?>
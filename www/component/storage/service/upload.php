<?php 
$fn = (isset($_SERVER['HTTP_X_FILENAME']) ? $_SERVER['HTTP_X_FILENAME'] : false);
if ($fn) {
	// AJAX call
	// TODO get content from file_get_contents('php://input')
} else {
	// form submit
	$files = $_FILES['fileselect'];
	foreach ($files['error'] as $id => $err) {
		if ($err == UPLOAD_ERR_OK) {
			$fn = $files['name'][$id];
			// TODO move_uploaded_file($files['tmp_name'][$id], ...);
		}
	}
}
?>
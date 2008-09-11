<?php

	require_once 'Directory.Controller.Main.php';

	header('Content-type: text/xml');
 	header("Connection: close");
 	header("Expires: -1"); 

	$directoryController = new DirectoryController($_REQUEST);
	$output = $directoryController->render();	

	echo $output;

?>

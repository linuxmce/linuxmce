<?php
	if (isset($_GET["setCurrentRoom"])) {
		$currentRoom = $_GET["setCurrentRoom"];
		$currentRoom = intval($currentRoom);
		setcookie("currentRoom","" . $currentRoom);
	}
	if (isset($_GET["setCurrentUser"])) {
		$currentUser= $_GET["setCurrentUser"];
		$currentUser = intval($currentUser);
		setcookie("currentUser","" . $currentUser);
	}
	
	header("Location: iOrbiter.php");
?>
 <?php
/*
	Copyright 2008 - Peer Oliver Schmidt
	GPLv2 Licensed
*/
 	include_once("lib.inc.php");
	error_reporting(E_ALL);
	connectDB();
	$currentUser = 1;
	$currentScreen = 1;
	$currentRoom = 3; 
	$PK_File = 0;
	$Details = "";
	$SortedBy = "";
	$mediaType = "";
	$Subtype = "";
	global $currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;
	if (isset($_GET["room"])) {
		$currentRoom = $_GET["room"];
		$currentRoom = intval($currentRoom);
	}
	if (isset($_GET["screen"])) {
		$currentScreen = $_GET["screen"];
		$currentScreen = intval($currentScreen);
	}
	if (isset($_GET["user"])) {
		$currentUser = $_GET["user"];
	}
	if (isset($_GET["Details"])) {
		$Details = $_GET["Details"];
	}
	if (isset($_GET["SortedBy"])) {
		$SortedBy = $_GET["SortedBy"];
	}
	
	if (isset($_GET["Subtype"])) {
		$Subtype = $_GET["Subtype"];
	}
	if (isset($_GET["mediaType"])) {
		$mediaType = $_GET["mediaType"];
	}

	$currentEntertainArea = getMyValue($link,"SELECT PK_EntertainArea FROM EntertainArea WHERE FK_Room = $currentRoom");
	$heading = getMyValue($link,"SELECT Description FROM Room WHERE PK_Room = $currentRoom") . " - " . getMyValue($link,"SELECT UserName FROM Users WHERE PK_Users = $currentUser");
	
	if ($Details != "") {
		buildFileListFromAttribute($mediaLink,$Details,$mediaType);
	}
	if ($SortedBy != "") {
		buildFileListFromSortedBy($mediaLink, $SortedBy,$mediaType);
	}
	If ($Subtype != "") {
		buildFileListFromSubtype($mediaLink, $Subtype, $mediaType);
	}
?>
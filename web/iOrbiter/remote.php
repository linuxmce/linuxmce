<?php
/*
	Copyright 2008 - Peer Oliver Schmidt
	GPLv2 Licensed
*/
	/*
	 * This file is used to display the detail page for the supplied media file
	 * identified by PK_File
	 */
 	include_once("lib.inc.php");
	error_reporting(E_ALL);
	connectDB();
/*	$currentUser = 1;
	$currentScreen = 1;
	$currentRoom = 3; 
*/	$PK_File = 0;
	$remoteType='';
	global $pk_file,$currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;
	if (isset($_COOKIE["currentUser"])) {	$currentUser = $_COOKIE["currentUser"]; }
	if (isset($_COOKIE["currentRoom"])) {	$currentRoom = $_COOKIE["currentRoom"]; }

	if (isset($_GET["room"])) {
		$currentRoom = $_GET["room"];
		$currentRoom = intval($currentRoom);
		$currentEntertainArea = getEntertainArea($link,$currentRoom);
	}
	if (isset($_GET["screen"])) {
		$currentScreen = $_GET["screen"];
		$currentScreen = intval($currentScreen);
	}
	if (isset($_GET["user"])) {
		$currentUser = $_GET["user"];
	}
	if (isset($_GET["pk_file"])) {
		$PK_File = $_GET["pk_file"];
	}
	if (isset($_GET["type"])) {
		$remoteType = $_GET["type"]; // Type of Remote, either AV or TV atm.
	}
	print "<ul>\n";
	
	if ($PK_File <> 0) {
		print "<li class='group'>Media Information</li>\n";
		print "<li>Playing $currentRoom $currentEntertainArea</li>\n";
		print "<li>pk file $PK_File</li>\n";
		getPicture($mediaLink,$PK_File);
	}		
	print "";
	if ($remoteType == 'AV') {
		print "<li class='group'>Control</li>\n";
		print "<li onClick='tellemCommand(\"play\",$PK_File);'>Play/Pause</li>\n";
		print "<li onClick='tellemCommand(\"skip\",$PK_File);'>Skip Forward</li>\n";
		print "<li onClick='tellemCommand(\"skip\",$PK_File);'>Skip Backward</li>\n";
	}
	print "</ul>\n";
?>
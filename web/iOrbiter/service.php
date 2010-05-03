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
	$PK_File = 0;
*/	global $pk_file,$currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;
	if (isset($_COOKIE["currentUser"])) {	$currentUser = $_COOKIE["currentUser"]; }
	if (isset($_COOKIE["currentRoom"])) {	$currentRoom = $_COOKIE["currentRoom"]; }
	$remotetype = "";
	

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
	if (isset($_GET["remotetype"])) {
	        $remoteType = $_GET["remotetype"];
        fi

	if (isset($_GET["command"])) {
		$command = $_GET["command"];
	} else {
		$command = 'play';
	}
	if ($command == 'play') {
          playFile($mediaLink,$PK_File);
        } elseif ($command == 'stop') {
          stopFile($currentRoom);
        } elseif ($command == 'skipfwd') {
          skipfwd($currentRoom);
        } elseif ($command == 'skipback') {
          skipback($currentRoom);
        }
?>
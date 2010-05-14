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
	

	if (isset($_POST["room"])) {
		$currentRoom = $_POST["room"];
		$currentRoom = intval($currentRoom);
		$currentEntertainArea = getEntertainArea($link,$currentRoom);
	}
	if (isset($_POST["screen"])) {
		$currentScreen = $_POST["screen"];
		$currentScreen = intval($currentScreen);
	}
	if (isset($_POST["user"])) {
		$currentUser = $_POST["user"];
	}
	if (isset($_POST["pk_file"])) {
		$PK_File = $_POST["pk_file"];
	}	
	if (isset($_POST["remotetype"])) {
	        $remoteType = $_POST["remotetype"];
        }

	if (isset($_POST["command"])) {
		$command = $_POST["command"];
	} else {
		$command = 'play';
	}
	$file=fopen("/tmp/test.txt","w+");
	fwrite($file,"command= $command");
	fclose($file);
	if ($command == 'play') {
          playFile($mediaLink,$PK_File);
        } elseif ($command == 'stop') {
          sendPlayerCommand($currentRoom,44);  // MH_Stop_Media          
        } elseif ($command == 'skipfwd') {
//          sendPlayerCommand($currentRoom,63);  // Skip_Fwd / 
            sendPlayerCommand($currentRoom,65,array(array("5", "+1"))); // Jump Position in Play list
        } elseif ($command == 'skipback') {
            sendPlayerCommand($currentRoom,65,array(array("5", "-1"))); // Jump Position in Play list
//          sendPlayerCommand($currentRoom,64);  // Skip_Back
        } elseif ($command == 'louder') {
          sendCommand(-106,89);  // Vol+
        } elseif ($command == 'quieter') {
          sendCommand(-106,90);  // Vol-
        } elseif ($command == 'mute') {
          sendCommand(-106,97);  // Mute
        } elseif ($command == 'brighter') {
          sendCommand(-104,223);  // BrightnessUp
        } elseif ($command == 'darker') {
          sendCommand(-104,224);  // BrightnessDown
        }
?>

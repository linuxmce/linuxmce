<?php
include("lib.inc.php");
error_reporting(E_ALL);
connectDB();
global $currentRoom, $currentUser;
if (isset($_COOKIE["currentUser"])) {	$currentUser = $_COOKIE["currentUser"]; }
if (isset($_COOKIE["currentRoom"])) {	$currentRoom = $_COOKIE["currentRoom"]; }
if (isset($_GET["currentRoom"])) {
	$currentRoom = $_GET["currentRoom"];
	$_COOKIE["currentRoom"] = $currentRoom;
}

print getMediaStatus($currentRoom);
?>
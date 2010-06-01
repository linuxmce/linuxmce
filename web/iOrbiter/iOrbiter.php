<?php
/*
	Copyright 2008 - Peer Oliver Schmidt
	GPLv2 Licensed
*/
/*
	iOrbiter provides web based access to the LinuxMCE installation.
	It does do so not based on transferring big graphics, but mostly text items.
	
	Most of the scenarios of the regular UI1 orbiter are used in this
	Orbiter as well.
 */
	include_once("lib.inc.php");
	error_reporting(E_ALL);
	connectDB();
	global $currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;

	if (isset($_COOKIE["currentUser"])) {	
		$currentUser = $_COOKIE["currentUser"]; 
	} else {
		$currentUser = getMyValue($link,"SELECT PK_Users FROM Users ORDER BY PK_Users Limit 1;");	
		setcookie("currentUser","" . $currentUser);		
	}
	if (isset($_COOKIE["currentRoom"])) {	
		$currentRoom = $_COOKIE["currentRoom"]; 
	} else {
		$currentRoom = getMyValue($link,"SELECT PK_Room FROM Room ORDER BY PK_ROOM Limit 1;");	
		setcookie("currentRoom","" . $currentRoom);
	}
	$currentScreen = 1;
	$media = True;
	
	if (isset($_GET["screen"])) {
		$currentScreen = $_GET["screen"];
		$currentScreen = intval($currentScreen);
	}
	$currentEntertainArea = getMyValue($link,"SELECT PK_EntertainArea FROM EntertainArea WHERE FK_Room = $currentRoom");
	if (intval($currentEntertainArea)==0) {
		$media = False;
	} 
	$room = getMyValue($link,"SELECT Description FROM Room WHERE PK_Room = $currentRoom");
	$user = getMyValue($link,"SELECT UserName FROM Users WHERE PK_Users = $currentUser");
	$heading =  $room . " - " . $user;
	
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LinuxMCE</title>
		<meta http-equiv="Content-Type" content="text/xml; charset=UTF-8" />
		<meta name="viewport" content="width=320; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;"/>
<style type="text/css" media="screen">@import "iui/iui.css";</style>
<style type="text/css" media="screen">@import "iOrbiter.css";</style>
<script type="application/x-javascript" src="iui/iui.js"></script>
<script type="application/x-javascript" src="tellem.js"></script>
</head>
<body onLoad="setInterval('tellemXineStatus(<?php print $currentRoom; ?>)', 5000);" onclick="console.log('Hello', event.target);">
	<div class="toolbar">
		<h1 id="status" ></h1>
		<a id="backButton" class="button"></a>
		
<?php
		print "<a id='homeButton' class='button' href='iOrbiter.php?currentRoom=$currentRoom&currentUser=$currentUser' target='_self'>Home</a>\n";
		print "<a class='button2' href='#Locations'>$room</a>\n";
		print "<a class='button' href='#All-Users'>$user</a>\n";
?>

	</div>
	<ul id="home" title="Home" selected="true">
		<li><a href="#Lighting-Scenarios">Lighting Scenarios</a></li>
		<li><a href="#Media-Scenarios">Media Scenarios</a></li>
		<li><a href="#Climate-Scenarios">Climate Scenarios</a></li>
		<li><a href="#Communication-Scenarios">Communication Scenarios</a></li>
		<li><a href="#Security-Scenarios">Security Scenarios</a></li>
		<li><a href="#Power">Power</a></li>
		<li><a href="#AdvancedOptions">Advanced</a></li>
		<li><a href="#linuxmce-links">LinuxMCE Links</a></li>
	</ul>
	<ul id="linuxmce-links" title="Links">
		<li><a href="/" target="_self">Web Admin</a></li>
		<li><a href="/lmce-admin/weborbiter" target="_self">Weborbiter</a></li>
		<li><a href="http://www.linuxmce.org/" target="_self">Website</a></li>
		<li><a href="http://forum.linuxmce.org/" target="_self">Forum</a></li>
		<li><a href="http://wiki.linuxmce.org/" target="_self">Wiki</a></li>
	</ul>
<?php
	// Lighting
	doDesignObjVariations(1345,$link);
	// Media
	// Only show the media sub menu, if we are in an Entertainment Area.
	if ($media) {
		doDesignObjVariations(1346,$link, $currentUser, $currentRoom);	
	} else {
		print "<ul id='Media-Scenarios'>\n";
		print "<li>No media devices in this room</li>\n";
		print "</ul>\n";
	}

	// Climate
	doDesignObjVariations(1347,$link);
	// Telecom
	doDesignObjVariations(1349,$link);
	// Security
	doDesignObjVariations(1348,$link);
	// Rooms / Location
	doDesignObjVariations(2918,$link);
	// Users
	doDesignObjVariations(1903,$link);
	// Power
	doDesignObjVariations(3332,$link);
	// Advanced
	doDesignObjVariations(2022,$link);
	//
	If (isset($_GET["CommandGroup"])) {
		$commandGroup = $_GET["CommandGroup"];
		doCommandGroup($link,$commandGroup);
	} 
	If (isset($_GET["CommandGroup_D"])) {
		$commandGroup = $_GET["CommandGroup_D"];
		doCommandGroup_D($link,$commandGroup);
	} 
	
		

	// We get the main Screens object
	$query = "SELECT FK_DesignObj FROM Screen_DesignObj Where FK_Screen = $currentScreen AND $UI AND $SKIN";
//	print "<p>Getting screen $currentScreen.</p>\n";
	$mainDesignObj = getMyValue($link,$query);
//	print "<pre>mainDesignObj $mainDesignObj</pre>\n";
	// And we get the standard variation of the main menu first.
	
//	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = NULL); // First we use the standard variation.
//	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = 1); // After that we add everything from the Normal 4:3 Variation
	
//  print "<p>End Of Table</p>\n";
//  print "</body></html>\n";
	

	mysql_close($mediaLink);
	mysql_close($link);
		
?>
</body>
</html>

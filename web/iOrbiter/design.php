<?php
/*
	Copyright 2008 - Peer Oliver Schmidt
	GPLv2 Licensed
*/
	error_reporting(E_ALL);
	include("lib.inc.php");
	connectdb();
	global $UI, $SKIN;
	global $PK_File, $currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;
	$mainDesignObj = NULL;
	if (isset($_COOKIE["currentUser"])) {	$currentUser = $_COOKIE["currentUser"]; }
	if (isset($_COOKIE["currentRoom"])) {	$currentRoom = $_COOKIE["currentRoom"]; }
	If (isset($_GET["CommandGroup"])) {
		$commandGroup = $_GET["CommandGroup"];
		doCommandGroup($link,$commandGroup);
	} 
	If (isset($_GET["CommandGroup_D"])) {
		$commandGroup = $_GET["CommandGroup_D"];
		doCommandGroup_D($link,$commandGroup);
	} 
	if (isset($_GET["PK_File"])) {
		$PK_File = $_GET["PK_File"];
	}	

		

	// We get the main Screens object
	$query = "SELECT FK_DesignObj FROM Screen_DesignObj Where FK_Screen = $currentScreen AND $UI AND $SKIN";
	if ($currentScreen != 1) {
		print "<h2>Gxetting screen $currentScreen.</h2>\n";
		$mainDesignObj = getMyValue($link,$query);
		print "<pre>mainDesignObj $mainDesignObj</pre>\n";
		
	} else {
		// We just want to go back to the main screen.
		print "<h2 onload='refresh;'></h2>";		
	}
	// And we get the standard variation of the main menu first.	
	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = NULL); // First we use the standard variation.
	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = 1); // After that we add everything from the Normal 4:3 Variation
	
  print "<p>End Of Table</p>\n";
  print "</body></html>\n";
	

	mysql_close($mediaLink);
	mysql_close($link);
		
?>

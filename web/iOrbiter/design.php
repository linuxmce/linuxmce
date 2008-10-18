
<?php
	error_reporting(E_ALL);
	include("lib.inc.php");
	connectdb();
	global $UI, $SKIN;
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
	print "<h2>Getting screen $currentScreen.</h2>\n";
	$mainDesignObj = getMyValue($link,$query);
	print "<pre>mainDesignObj $mainDesignObj</pre>\n";
	// And we get the standard variation of the main menu first.
	
	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = NULL); // First we use the standard variation.
	orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea,$UI = 1); // After that we add everything from the Normal 4:3 Variation
	
  print "<p>End Of Table</p>\n";
  print "</body></html>\n";
	

	mysql_close($mediaLink);
	mysql_close($link);
		
?>

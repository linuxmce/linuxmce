 <?php
 	include_once("lib.inc.php");
	error_reporting(E_ALL);
	connectDB();
	$currentUser = 1;
	$currentScreen = 1;
	$currentRoom = 3; 
	$PK_File = 0;
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
	if (isset($_GET["PK_File"])) {
		$PK_File = $_GET["PK_File"];
	}	

	$currentEntertainArea = getMyValue($link,"SELECT PK_EntertainArea FROM EntertainArea WHERE FK_Room = $currentRoom");
	$heading = getMyValue($link,"SELECT Description FROM Room WHERE PK_Room = $currentRoom") . " - " . getMyValue($link,"SELECT UserName FROM Users WHERE PK_Users = $currentUser");
	
	buildAttributeList($mediaLink,$PK_File);
?>

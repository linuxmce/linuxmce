<?
function setupWebOrbiter($output,$dbADO) {

	$command = $_REQUEST['command'];

	// Get the list of rooms
	if ($command == 'GetRooms') {
		$query = "SELECT PK_Room, Description FROM Room WHERE 1";
		$rooms = $dbADO->Execute($query);
		echo $rooms;
	}

	// Get the list of users
	if ($command == 'GetUsers') {
		$query = "SELECT PK_Users, UserName FROM Users WHERE 1";
		$users = $dbADO->Execute($query);
		echo $users;
	}

	// Get the list of skins
	if ($command == 'GetSkins') {
		$query = "SELECT PK_Skin, Description FROM Skin WHERE 1";
		$skins = $dbADO->Execute($query);
		echo $skins;
	}

	// Get the available UIs
	if ($command == 'GetUIs') {
		$query = "SELECT PK_UI, Description FROM UI WHERE 1";
		$uis = $dbADO->Execute($query);
		echo $uis;
	}

	// Get the available languages
	if ($command == 'GetLanguages') {
		$query = "SELECT PK_Language, Description FROM Language WHERE 1";
		$language = $dbADO->Execute($query);
		echo $language;
	}

	// Generate the orbiter
	if ($command == 'GenerateOrbiter') {
		// Check if the resolution is already in our DB
		$res = $_REQUEST['resolution'];
		$resolution = split('x', $res);
		$resX = $resolution[0];
		$resY = $resolution[1];
		//echo $resX;
		//echo $resY;

		$query = "SELECT PK_Size, Description FROM Size WHERE Width=".$resX." AND Height=".$resY;
		$size = $dbADO->Execute($query);
		// Check the existence of the current resolution in the DB and insert it if it doesn't exist
		// then select the one we just added

		// Grab the rest of the parameters to create the new orbiter
		// default language
		// default user
		// default room
		// default skin
		// default UI

		// Create the orbiter, return the PK_Key for the new orbiter, reload router, and start generating
		}
	}

	if ($command == 'UpdateOrbiter') {
		// Update the orbiter with the new settings, reload router, and start regenerating
	}

	if ($command == 'GenerationStatus') {
		// Get the PK_Key of the orbiter to check
		// Return the percentage of generation
	}
}
?>

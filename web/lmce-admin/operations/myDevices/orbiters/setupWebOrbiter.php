<?
function setupWebOrbiter($output,$dbADO) {

	$command = $_REQUEST['command'];
	$json = new StdClass();
	$jason->data = array();
	$jason->success = false;
	$jason->error = '';
	$jason->count = 0;
	
	switch ($command) {
	case "GetRooms": // Get the list of rooms
		$query = "SELECT PK_Room, Description FROM Room";
		lookup($query, $dbADO, $json);
	break;
	case "GetUsers": // Get the list of users
		$query = "SELECT PK_Users, UserName FROM Users";
		lookup($query, $dbADO, $json);
	break;
	case "GetSkins": // Get the list of skins
		$query = "SELECT PK_Skin, Description FROM Skin";
		lookup($query, $dbADO, $json);
	break;
	case "GetUIs": // Get the available UIs
		$query = "SELECT PK_UI, Description FROM UI";
		lookup($query, $dbADO, $json);
	break;
	case "GetLanguages": // Get the available languages
		$query = "SELECT PK_Language, Description FROM Language";
		lookup($query, $dbADO, $json);
	break;
	

	case "GenerateOrbiter":	// Generate the orbiter
		// Check if the resolution is already in our DB
		$res = $_REQUEST['resolution'];
		$resolution = split('x', $res);
		$resX = $resolution[0];
		$resY = $resolution[1];

		$query = "SELECT PK_Size, Description FROM Size WHERE Width=? AND Height=?";
		if ($size = $dbADO->GetRow($query,array($resX,$resY))){
			//there are results. grab the first one and return the ID
			$json->success = true;
			$json->count = 1;
			$json->data[] = $size;
		} else { 
			//no records returned, we need to add this resolution
			$query = "INSERT into Size(Description,Width,Height) values(?,?,?)";
			if ($dbADO->Execute($query,array($res,$resX,$resY))) {
				//now grab the new row
				$query = "SELECT PK_Size, Description FROM Size WHERE PK_Size=?";
				if ($row=$dbADO->GetRow($query,array($dbADO->Insert_ID()))){
					$json->success = true;
					$json->count = 1;
					$json->data[] = $row;
				}
			}
		}
		// Check the existence of the current resolution in the DB and insert it if it doesn't exist
		// then select the one we just added

		// Grab the rest of the parameters to create the new orbiter
		// default language
		// default user
		// default room
		// default skin
		// default UI

		// Create the orbiter, return the PK_Key for the new orbiter, reload router, and start generating
		//}
	case "UpdateOrbiter": // Update the orbiter with the new settings, reload router, and start regenerating
	break;
	
	case "GenerationStatus":
		// Get the PK_Key of the orbiter to check
		// Return the percentage of generation
	break;

	}
	echo json_encode($json);

}

function lookup($query, $dbADO, $json) {
	if ($recordset = $dbADO->Execute($query)) {
		$json->success = true;
		$json->count = $recordset->RecordCount();;
		while ($row=$recordset->FetchRow()) {
			$json->data[] = $row;
		}
	} else {
		$json->error = mysql_error();
	}
}

?>

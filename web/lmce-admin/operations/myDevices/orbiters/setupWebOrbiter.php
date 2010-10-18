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
		// Check the existence of the current resolution in the DB and insert it if it doesn't exist
		// Check if the resolution is already in our DB
		$res = $_REQUEST['resolution'];
		$resolution = split('x', $res);
		$resX = $resolution[0];
		$resY = $resolution[1];
		$Size = 0; // placeholder for whatever "Size" table row we end up with below
		
		$query = "SELECT PK_Size, Description FROM Size WHERE Width=? AND Height=?";
		if ($size = $dbADO->GetRow($query,array($resX,$resY))){
			//there are results. grab the first one and return the ID
			$json->success = true;
			$json->count = 1;
			$json->data[] = $size;
			$Size = $size;
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
					$Size = $row;
				}
			}
		}
		
		// then select the one we just added
		
		// Grab the rest of the parameters to create the new orbiter
		// DeviceTemplate - 1748 web device, 1749 proxy orbiter
		// Description
		// default size - 25
		// default language - 26
		// default user - 3
		// default room - FK_Room
		// default skin - 24
		// default UI - 104
		// width - 100
		// height - 101

		// Create the orbiter, return the PK_Key for the new orbiter, reload router, and start generating
		//$orbiterID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);
		updateOrbiter($orbiterID,$deviceData);
		
	case "UpdateOrbiter": // Update the orbiter with the new settings, reload router, and start regenerating
		updateOrbiter($orbiterID,$deviceData);
	break;
	
	case "GenerationStatus":
		// Get the PK_Key of the orbiter to check
		$orbiterID = $_REQUEST['orbiter-id-param'];
		// Return the percentage of generation
		if ($orbiterData=$dbADO->GetRow('SELECT PK_Orbiter,RegenInProgress,RegenStatus,RegenPercent from Orbiter WHERE PK_Orbiter=?',$orbiterID)) {
			$json->success = true;
			$json->count = 1;
			$json->data[] = $orbiterData;
		}
	break;

	}
	echo json_encode($json);

}

function updateOrbiter($orbiterID,$deviceData) {
	// Update the orbiter with the new settings, reload router, and start regenerating
	// $deviceData is an array of device data to update. id=>value
	
	// Full regen code from the orbiter wizard page. reloads router.
	/*
	$dbADO->Execute('UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter=?',$orbiterID); 
	$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$orbiterID);
	$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$orbiterID.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$orbiterID.' 21 "-r" 24 1';
	exec($commandToSend);
	//*/
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

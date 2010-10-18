<?
function setupWebOrbiter($output,$dbADO) {

	$command = $_REQUEST['command'];
	$installationID = (int)@$_SESSION['installationID'];
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
		
		$query = "SELECT PK_Size, Description FROM Size WHERE Width=? AND Height=?";
		if ($size = $dbADO->GetRow($query,array($resX,$resY))){
			//we have an existing Size row
			$SizeRow = $size;
		} else { 
			//no records returned, we need to add this resolution
			//
			// First, determine the aspect ratio. For now, we ignore aspect ratios which can't be
			// determined by the resolution.
			$rest16_9 = abs($resY - ($resX / 16 * 9));
			$rest4_3 = abs($resY - ($resX / 4 * 3));
			if ($rest4_3 < $rest16_9) {
				// We have a 4:3 screen
				$scaleX = intval($resX / 2.84444444444);
				$scaleY = intval($resY / 1.6);
			} else {
				// We assume a 16:9 screen
				$scaleX = intval($resX / 2.13333333333);
				$scaleY = intval($resY / 1.6);
			}
			$query = "INSERT into Size(Description,Width,Height,ScaleX,ScaleY) values(?,?,?,?,?)";
			if ($dbADO->Execute($query,array($res,$resX,$resY,$scaleX,$scaleY))) {
				//now grab the new row
				$query = "SELECT PK_Size, Description FROM Size WHERE PK_Size=?";
				if ($row=$dbADO->GetRow($query,array($dbADO->Insert_ID()))){
					$SizeRow = $row;
				}
			}
		}
		
		// then select the one we just added
		
		// Grab the rest of the parameters to create the new orbiter
		// DeviceTemplate - 1748 web device, 1749 proxy orbiter
		// Description
		// default room - FK_Room
		
		//The rest are DeviceData values
		// default size - 25
		// default language - 26
		// default user - 3
		// default skin - 24
		// default UI - 104
		// width - 100
		// height - 101

		/* Tentative JSON structure for config data:
		 * {
		 *   description : "string",
		 *   defaultRoom : "int",
		 *   deviceData : {[
		 *       id : "string"
		 *     ],[
		 *       id : "string"
		 *     ...
		 *   ]}
		 * }
		 */
		
		// Create the orbiter, return the PK_Key for the new orbiter, reload router, and start generating
		//$orbiterID=createDevice('1748',$installationID,0,NULL,$dbADO);
		// Web orbiter, need to make sure it's related proxy orbiter uses a unique port number
		//$out=exec_batch_command("/usr/pluto/bin/configure_proxyorbiter.pl -d $orbiterID",0);
		updateOrbiter($orbiterID,$deviceData);
	break;
	case "UpdateOrbiter": // Update the orbiter with the new settings, reload router, and start regenerating
		updateOrbiter($orbiterID,$deviceData);
	break;
	
	case "GenerationStatus":
		// Get the PK_Key of the orbiter to check
		$orbiterID = $_REQUEST['orbiter'];
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

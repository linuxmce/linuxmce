<?php
function debug($text) {
	// temporary debug logging function
	return;
	global $file;
	if (!$file) $file = fopen ('/tmp/orbiter.log', 'a');
	fwrite ($file, $text . "\n");
}
function setupWebOrbiter($output,$dbADO) {
	$command = $_REQUEST['command'];
	$json = new StdClass();
	$jason->data = array();
	$jason->success = false;
	$jason->error = '';
	$jason->count = 0;
	
	$query = "SELECT * from Installation limit 1";
	if ($row=$dbADO->GetRow($query)){
		$ConfigRow = $row;
	}
	$InstallationID = $ConfigRow['PK_Installation'];
	switch ($command) {
	case "GetRooms": // Get the list of rooms
		$query = "SELECT PK_Room, Description FROM Room where FK_Installation='$InstallationID'";
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
	case "GetOrbiters": // Get a list of orbiters
		$query = "SELECT d.PK_Device AS device,d.Description FROM Device d INNER JOIN Device_DeviceData dd ON dd.FK_Device=d.PK_Device WHERE FK_Installation=$InstallationID AND FK_DeviceTemplate=1749 AND FK_DeviceData=119";
		if ($_REQUEST['orbiter']) $query .= " AND PK_Device=" . escapeshellcmd($_REQUEST['orbiter']);
		$query .= " ORDER BY IK_DeviceData ASC";
		lookup($query,$dbADO,$json);
	break;
	case "GetDeviceData": // Get all device data for a given device
		$query = "SELECT dd.FK_DeviceData as DeviceData,dd.IK_DeviceData as Value FROM Device d INNER JOIN Device_DeviceData dd ON dd.FK_Device=d.PK_Device WHERE PK_Device=?";
		if ($recordset = $dbADO->Execute($query,array($_REQUEST['orbiter']))) {
			$json->success = true;
			$json->count = $recordset->RecordCount();;
			while ($row=$recordset->FetchRow()) {
				$json->data[$row['DeviceData']] = $row['Value'];
			}
			$query = "SELECT FK_Room from Device where PK_Device=?";
			if ($room = $dbADO->GetRow($query,array($_REQUEST['orbiter']))){
				$json->data['Room'] = $room['FK_Room'];
			}
		} else {
			$json->error = mysql_error();
		}
	break;
	case "GenerateOrbiter":	// Generate the orbiter
		// Check the existence of the current resolution in the DB and insert it if it doesn't exist
		// Check if the resolution is already in our DB
		$res = $_REQUEST['resolution'];
		$resolution = split('x', $res);
		$resX = $resolution[0];
		$resY = $resolution[1];
		
		$sizeRow = getSize($resX,$resY,$dbADO);

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
		
		$configData = json_decode(stripslashes($_REQUEST['config']));
		debug(json_encode($configData));
		// php classes cannot have numeric members, need to convert to a hash
		foreach ($configData->deviceData as $key => $value) {
			$data[$key] = $value;
		}
		$data['25'] = $SizeRow['PK_Size'];
		$data['100'] = $SizeRow['Width'];
		$data['101'] = $SizeRow['Height'];
		$configData->deviceData=$data;
		/* JSON structure for config data:
		 * {
		 *   description : "string",
		 *   defaultRoom : "int",
		 *   deviceData : {
		 *       id : "string",
		 *       id : "string"
		 *     ...
		 *   }
		 * }
		 */
		debug(json_encode($configData));
		// Create the orbiter, return the PK_Key for the new orbiter, reload router, and start generating
		$orbiterID=createDevice('1748',$installationID,0,NULL,$dbADO);
		$orbiterID++; // the proxy orbiter is actually created as a child of 1748
		debug("orbiter created: $orbiterID");
		// Web orbiter, need to make sure it's related proxy orbiter uses a unique port number
		$out=exec_batch_command("/usr/pluto/bin/configure_proxyorbiter.pl -d $orbiterID",0);
		debug("configure_proxyorbiter output: $out");
		updateOrbiter($dbADO, $json, $orbiterID,$configData);
		$json->success = true;
		$json->orbiter=$orbiterID;
	break;
	case "ReloadRouter": // Needed to manually reload the router to get a brand new orbiter to generate then fire the regen command
		exec("/usr/pluto/bin/MessageSend localhost -o 0 -1000 7 6", $out, $return);
		$json->success = true;
	break;
	case "FirstGeneration": // Needed to manually call this after the router reload to successfully generate a brand new orbiter
		$orbiterID = escapeshellcmd($_REQUEST['orbiter']);
		$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$orbiterID.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$orbiterID.' 21 "-r"';
        	exec($commandToSend);
		$json->success = true;
	break;
	case "UpdateOrbiter": // Update the orbiter with the new settings, reload router, and start regenerating
		$orbiterID = $_REQUEST['orbiter'];
		$configData = json_decode($_REQUEST['config']);
		$res = $_REQUEST['resolution'];
		$resolution = split('x', $res);
		$resX = $resolution[0];
		$resY = $resolution[1];
                $sizeRow = getSize($resX,$resY,$dbADO);
		$configData->deviceData['25'] = $SizeRow['PK_Size'];
		$configData->deviceData['100'] = $SizeRow['Width'];
		$configData->deviceData['101'] = $SizeRow['Height'];
		updateOrbiter($dbADO, $json, $orbiterID,$configData);
	break;
	
	case "GenerationStatus":
		// Get the PK_Key of the orbiter to check
		$orbiterID = escapeshellcmd($_REQUEST['orbiter']);
		/*
		694 - command GetOrbiterStatus
		9 - destination device
		2 - orbiter id
		returns:
		9 - If a regen is in progress, this is a status.
		48 - If a regen is in progress, this is a percentage
		5 - The status: O=OK to load, 
						N=New, skin generated, need router reset, 
						n=new, no skins at all, 
						R=Regenerating skin now, 
						r=Regenerating skin for new orbiter,
						U=Unknown, 
						D=Device is not an orbiter
		*/
		if ($data = dce_command(9,1,694,array(2=>$orbiterID))) {
		// Return the percentage of generation
			$json->success = true;
			$json->count = 1;
			$json->data[] = $data;
		} else {
			$json->error = "Unable to execute command";
		}
	break;

	}
	echo json_encode($json);

}

function updateOrbiter($dbADO, $json, $orbiterID,$configData) {
	// Update the orbiter with the new settings, reload router, and start regenerating
	$data = array();
	$data['2'] = $orbiterID;
	$data['163'] = $configData->Description;
	$data['57'] = $configData->defaultRoom;
	$tempData = array();
	foreach ($configData->deviceData as $key => $value) {
		$tempData[] = "$key|$value";
	}
	$data['109'] = implode("|",$tempData);
	// $deviceData is an array of device data to update. id=>value
	
	// Full regen code from the orbiter wizard page.
	debug(json_encode($data));
	dce_command(4,1,957,$data);
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

function dce_command($destination,$type,$command,$deviceData) {
	$cmd = "/usr/pluto/bin/MessageSend localhost -o 0 $destination $type $command";
	foreach ($deviceData as $key => $value) {
		$cmd .= " $key \"$value\"";
	}
	debug($cmd);
	exec($cmd, $out, $return);
	debug(json_encode($out));
	if ($out[0] != "0:UNHANDLED") { // MessageSend does not give any exit value so this will always pass even if the command fails
		foreach ($out as $item) {
			$pieces = explode(":",$item);
			$data[$pieces[0]] = $pieces[1];
		}
		return $data;
	} else {
		return false;
	}
}

function getSize ($resX,$resY,$dbADO) {
	$query = "SELECT PK_Size, Description, Width, Height FROM Size WHERE Width=? AND Height=?";
	if ($size = $dbADO->GetRow($query,array($resX,$resY))){
		//we have an existing Size row
		return $size;
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
			$query = "SELECT PK_Size, Description, Width, Height FROM Size WHERE PK_Size=?";
			if ($row=$dbADO->GetRow($query,array($dbADO->Insert_ID()))){
				return $row;
			}
		}
	}
}

?>

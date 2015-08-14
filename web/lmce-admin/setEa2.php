<?php
/*
 setEa - temp utility to set the ea for a new Qorbiter
 */
ini_set("display_errors", "off");
$deviceID = -1;
$label = "QOrbiter " . $deviceID;
//initialization area
if (isset($_GET["d"])) {
	$deviceID = $_GET['d'];
	if ($deviceID == "") {
		echo "No Device set";
		die("No Device Set");
	}
} else {
	die("Please specify the device ID with d=xxxx");

}

if (isset($_GET['label'])) {
	$label = mysql_real_escape_string($_GET['label']);
} else {
	die("No description");
}

/*
 Need to get children devices and do the same as the parent.
 */
echo "Starting for deviceID::" . $deviceID . "<br>";
$deviceName = mysql_real_escape_string($_GET['label']);
$mediaPlayerID = $deviceID + 1;
$mobileRoom = -1;
$mobileEa = -1;
$server = "localhost";
$mysqlUser = "root";
$mysqlPass = "";
$conn = mysql_connect($server, $mysqlUser, $mysqlPass);
mysql_select_db("pluto_main", $conn);
$dupe;

if ($conn) {
	$installSql = "SELECT * FROM `Installation` ";
	$iRes = mysql_query($installSql, $conn) or die(mysql_error($conn));
	$inst = "";

	while ($row = mysql_fetch_array($iRes)) {
		$inst = $row['PK_Installation'];
	}

	$installation = $inst;
	echo "Installation is " . $installation . "<br>Device: " . $label . " <br> ID::" . $deviceID . "<br>";

	if ($installation == "") {
		die("Cant Find Installation");
	}
	//check for duplicate ea, and obtain existing
	if(checkIfDupe($conn)){
		echo "checked duplicates<br>";
	}

	if (setParentDescription($conn)) {
		echo "<br>Set Device Parent Name/Description<br>";
	}

	if (setupMobileRoom($conn)) {
		echo "Mobile Room Setup complete<br>";
	}

	if (fixIntEa($conn)) {
		echo "EntertainArea set <br>";
	}

	if (fixRoomEa($conn)) {
		echo "Checked EAs <br>";
	}

	if (precheckDeviceEntertainArea($conn)) {
		echo "checked device_entArea<br>";
	}

	echo "finished";
}

function checkIfDupe($conn) {
	global $deviceID;
	global $mediaPlayerID;
	
	 $GLOBALS['deviceName']=mysql_real_escape_string($_GET['label']);
	 $deviceName= $GLOBALS['deviceName'];
	

echo "checking dupe for $deviceName<br>";
	$sql = "SELECT * from Device_EntertainArea where FK_Device= $mediaPlayerID ;";
	echo "$sql<br>";
	$result = mysql_query($sql);
	$existingEa = NULL;
	if (mysql_numrows($result) == 0) {
		echo "no device found <br>";
		return true;
	} else {
		echo "Found device matching criteria <br>";
		while ($row = mysql_fetch_array($result)) {
			$existingEa = $row['FK_EntertainArea'];
			echo "Existing EntertainArea::$existingEa<br>";
		}

		if ($existingEa == NULL) {
			die("Logical Error in finding existing ea in device_entertainarea");
		}

		$query = "SELECT * from EntertainArea WHERE PK_EntertainArea=$existingEa";
		$result = mysql_query($query);

		if (mysql_num_rows($result == 0)) {
			die("Missing Poper EA all together!");
		} else {
			while ($row = mysql_fetch_array($result)) {
				
				if($row['Description'] == $deviceName){
					echo "nothing wrong with duplicate check<br> ";
					return true;
				} else {
					if($deviceName=="localhost"){
						
						echo "<h2>Detected 'localhost' for device name, leaving existing device name ".$row['Description']." in place</h2><br>";
					
					$GLOBALS['deviceName']=$row['Description'];
					} else {
						
					$GLOBALS['deviceName']=$row['Description'];
						echo "Device name is being updated to  ".$deviceName;
						return true; 
					}
				}	
			}
		}

	}

}

function setParentDescription($conn) {
	global $deviceID;
	 $deviceName =$GLOBALS['deviceName'];
	
echo "Setting Parent device to ".$GLOBALS['deviceName'];


	if ($deviceName == "QOrbiter-Generic" || $deviceName=="localhost"){
			return true;
	}
	

	echo "Setting parent device description label to " . $GLOBALS['deviceName']."<br>";
	$sql = "UPDATE Device SET Description='" . $GLOBALS['deviceName'] . "' WHERE PK_Device='" . $deviceID . "';";
	$result = mysql_query($sql, $conn);
	return true;
}

function setEntertainArea($connect) {
	global $mediaPlayerID;
	global $mobileRoom;
	$deviceN = $GLOBALS['deviceName'];
	
	$d = mysql_real_escape_string($_GET['label']);
	echo "Creating EA " . $GLOBALS['deviceName'] . " in room: " . $mobileRoom . "<br>";

	$sql = "INSERT INTO EntertainArea (FK_Room, Only1Stream,Description,Private,FK_FloorplanObjectType) Values ($mobileRoom,0,'$deviceN',0,52);";
	echo "<br>$sql<br>";
	mysql_query($sql, $connect);
	$id = mysql_insert_id($connect);
	echo "last insert id $id<br>";

	$ea = $id;

	if (is_null($ea)) {
		echo "invalid EA ";
		die("cannot continue");
	} else {
		echo "valid ea $ea <br>";
	}

	$sql2 = "SELECT * FROM `Device_EntertainArea` WHERE `FK_Device` =" . $mediaPlayerID . " LIMIT 0, 30 ";
	$result2 = mysql_query($sql2, $connect) or die(mysql_error($connect));
	$row = mysql_fetch_array($result2);

	if (mysql_num_rows() == 0) {
		echo "Device not present in Device_EntertainArea, adding to table";
		$sql3 = "INSERT INTO `pluto_main`.`Device_EntertainArea` (`FK_Device`, `FK_EntertainArea`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES ( $mediaPlayerID ,  $ea , NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
		echo "<br>$sql3<br>";
		$result3 = mysql_query($sql3, $connect) or die(mysql_error($connect));
	}
}

function setupMobileRoom($conn) {
	global $mobileRoom;
	echo "<br>Connection Found, Starting<br>";

	$roomSql = "SELECT * FROM `Room` WHERE `Description` like 'Mobile' LIMIT 0, 30 ";
	echo "Checking for existing mobile orbiter room <br>";
	$result = mysql_query($roomSql, $conn) or die(mysql_error($conn));
	$cnt = mysql_num_rows($result);

	if ($cnt === 0) {

		$iRoomSql = "INSERT INTO `pluto_main`.`Room` (`PK_Room`, `FK_Installation`, `FK_RoomType`, `Description`, `FK_Icon`, `ManuallyConfigureEA`, `HideFromOrbiter`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, " . $installation . ", '9', 'Mobile', NULL, '1', '1', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
		$result2 = mysql_query($iRoomSql, $conn) or die(mysql_error($conn));
		$lastId = mysql_insert_id($conn);

		$chkSql = $roomSql = "SELECT * FROM `Room` WHERE `PK_Room` = " . $lastId;
		$result3 = mysql_query($chkSql, $conn) or die(mysql_error($conn));
		while ($row = mysql_fetch_array($result3)) {
			if ($row['PK_Room']) {
				$mobileRoom = $row['PK_Room'];
				echo "Mobile QOrbiters Room is " . $mobileRoom;
			}
		}
		echo "Not Found, so we've added it. Setting up entertain area now<br>";
	} else {
		while ($row = mysql_fetch_array($result)) {
			if ($row['PK_Room']) {
				$mobileRoom = $row['PK_Room'];
				echo "Mobile QOrbiters Room is " . $mobileRoom . "<br>";
			}
		}

	}

	return true;
}

function fixIntEa($conn) {
	global $mobileRoom;
	global $deviceName; // = mysql_real_escape_string($_GET['label']);

	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;

	echo "<br><b>Checking for incorrect int ea settings for device id::" . $mp . "</b> in room " . $mobileRoom . "<br>";
	//first check that we dont have duplicate entries in the entertain area table
	$sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '" . $mp . "' LIMIT 0, 30 ";
	$result = mysql_query($sql, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result) == 0) {
		echo "No int EntertainArea for " . $GLOBALS['deviceName'] . " to fix<br>";
		return true;
	} else {
		while ($row = mysql_fetch_array($result)) {
			echo "Found int EntertainArea. Incorrect setting  for device " . $deviceName . " in " . $row['PK_EntertainArea'] . "<br>";
			$correctionSql = "UPDATE EntertainArea SET Description='" . $GLOBALS['deviceName'] . "', FK_Room='" . $mobileRoom . "'  WHERE PK_EntertainArea = '" . $row['PK_EntertainArea'] . "'";
			$correctionResult = mysql_query($correctionSql, $conn);
			echo "Setting Corrected.<br>";
			return true;
		}
	}

}

function fixRoomEa($conn) {
	global $mobileRoom;
	$deviceName; // = mysql_real_escape_string($_GET['label']);
	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;
	echo "<br>Checking named EntertainAreas for correct Settings for device " . $GLOBALS['deviceName'] . "<br>";

	$sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '" . $GLOBALS['deviceName'] . "' LIMIT 0, 30 ";
	$result = mysql_query($sql, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result) == 0) {
		echo "No EntertainArea present, will create one for device " . $GLOBALS['deviceName'] . "<br>";
		setEntertainArea($conn);
		return true;
	} else {
		echo "Existing EntertainArea found, will validate ...";
		while ($row = mysql_fetch_array($result)) {
			echo " . ";
			$correctionSql = "UPDATE EntertainArea SET Description = " . $GLOBALS['deviceName'] . " , FK_Room=" . $mobileRoom . "  WHERE PK_EntertainArea = " . $row["PK_EntertainArea"] . ";";
			$correctionResult = mysql_query($corrctionSql, $conn);

		}
		echo " complete <br>";
		return true;
	}
}

function precheckDeviceEntertainArea($conn) {
	global $deviceName; // = mysql_real_escape_string($_GET['label']);
	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;

	echo "<br>Checking Device_EntertainArea to validate qMediaPlayer Settings for QOrbiter on device::" . $GLOBALS['deviceName'] . "<br>";

	$sql2 = "SELECT * FROM `Device_EntertainArea` WHERE `FK_Device` =" . $mp . " LIMIT 0, 30 ";
	$result2 = mysql_query($sql2, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result2) == 0) {
		echo "parent device not set, continuing.<br>";
		$sql = "SELECT * FROM `Device_EntertainArea` WHERE `FK_Device` =" . $mediaPlayerID . " LIMIT 0, 30 ";
		$result = mysql_query($sql, $conn) or die(mysql_error($conn));

		if (mysql_num_rows($result) == 0) {
			echo "Missing Entry, needs to be added for " . $GLOBALS['deviceName'] . "<br> ";

			$check = "SELECT * FROM EntertainArea WHERE Description LIKE '" . $_GET['label'] . "' ";
			$checkResult = mysql_query($check, $conn) or die(mysql_error($conn));			$ea;

			echo mysql_num_rows($checkResult);

			while ($checkRow = mysql_fetch_array($checkResult)) {
				$ea = $checkRow['PK_EntertainArea'];

			}

			if (is_null($ea)) {
				echo "invalid EA <br>";
				return true;
			} else {
				echo "EA::" . $ea . "<br>";
			}

			$sql3 = "INSERT INTO `pluto_main`.`Device_EntertainArea` (`FK_Device`, `FK_EntertainArea`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (" . $mediaPlayerID . ", " . $ea . ", NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
			$result3 = mysql_query($sql3, $conn) or die(mysql_error($conn));

			return true;
		} else {
			echo "device in table<br>";
			while ($row = mysql_fetch_array($result)) {
				$cleanupSql = "UPDATE Device_EntertainArea SET FK_Device= " . $mediaPlayerID . " , WHERE PK_EntertainArea = " . $row["PK_EntertainArea"] . ";";
				$result = mysql_query($cleanupSql, $conn);
				return true;
			}
		}

	}

	return true;

}

function updateEntertainArea($connect, $device, $location) {
	echo "Updating device to existing EA <br>";

	global $deviceName; // = mysql_real_escape_string($_GET['label']);
	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;

	if ($mobileEa == -1) {
		echo "invalid ea, exiting updateEntertainArea()";
		return;
	}
	echo "updating...<br>";
	$updatesql = "UPDATE `pluto_main`.`Device_EntertainArea` SET `FK_EntertainArea` = " . $mobileEa . " WHERE `Device_EntertainArea`.`FK_Device` = " . $mediaPlayerID;
	$result = mysql_query($updatesql, $connect) or die(mysql_error($conn));
	echo "updated. <br>";
}
  ?>

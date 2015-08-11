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

if($deviceName=="QOrbiter-Generic")
die("not changing");

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

function setParentDescription($conn) {
	global $deviceID;
	$deviceName = mysql_real_escape_string($_GET['label']);
	
if($deviceName=="QOrbiter-Generic")
return true;

	echo "Setting parent device description label to " . $deviceName;
	$sql = "UPDATE Device SET Description='" . $deviceName . "' WHERE PK_Device='" . $deviceID . "';";
	$result = mysql_query($sql, $conn);
	return true;
}

function setEntertainArea($connect) {
	global $mediaPlayerID;
	global $mobileRoom;
	$d = mysql_real_escape_string($_GET['label']);
	echo "Creating EA " . $d . " in room: " . $mobileRoom . "<br>";

	$sql="INSERT INTO EntertainArea (FK_Room, Only1Stream,Description,Private,FK_FloorplanObjectType) Values ($mobileRoom,0,'$d',0,52);";
	echo "<br>$sql<br>";
	mysql_query($sql, $connect);
	$id = mysql_insert_id($connect);
	echo "last insert id $id<br>";
	
	$ea=$id;
	
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
	$deviceName = mysql_real_escape_string($_GET['label']);

	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;

	echo "<br><b>Checking for incorrect int ea settings for device id::" . $mp . "</b> in room " . $mobileRoom . "<br>";
	//first check that we dont have duplicate entries in the entertain area table
	$sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '" . $mp . "' LIMIT 0, 30 ";
	$result = mysql_query($sql, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result) == 0) {
		echo "No int EntertainArea for " . $deviceName . " to fix<br>";
		return true;
	} else {
		while ($row = mysql_fetch_array($result)) {
			echo "Found int EntertainArea. Incorrect setting  for device " . $deviceName . " in " . $row['PK_EntertainArea'] . "<br>";
			$correctionSql = "UPDATE EntertainArea SET Description='" . $deviceName . "', FK_Room='" . $mobileRoom . "'  WHERE PK_EntertainArea = '" . $row['PK_EntertainArea'] . "'";
			$correctionResult = mysql_query($correctionSql, $conn);
			echo "Setting Corrected.<br>";
			return true;
		}
	}

}

function fixRoomEa($conn) {
	global $mobileRoom;
	$deviceName = mysql_real_escape_string($_GET['label']);
	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;
	echo "<br>Checking named EntertainAreas for correct Settings for device " . $deviceName . "<br>";

	$sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '" . $deviceName . "' LIMIT 0, 30 ";
	$result = mysql_query($sql, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result) == 0) {
		echo "No EntertainArea present, will create one for device " . $deviceName . "<br>";
		setEntertainArea($conn);
		return true;
	} else {
		echo "Existing EntertainArea found, will validate ...";
		while ($row = mysql_fetch_array($result)) {
			echo " . ";
			$correctionSql = "UPDATE EntertainArea SET Description = " . $deviceName . " , FK_Room=" . $mobileRoom . "  WHERE PK_EntertainArea = " . $row["PK_EntertainArea"] . ";";
			$correctionResult = mysql_query($corrctionSql, $conn);

		}
		echo " complete <br>";
		return true;
	}
}

function precheckDeviceEntertainArea($conn) {
	$deviceName = mysql_real_escape_string($_GET['label']);
	$mp = $_GET['d'];
	$mediaPlayerID = $mp + 1;

	echo "<br>Checking Device_EntertainArea to validate qMediaPlayer Settings for QOrbiter on device::" . $deviceName . "<br>";

	$sql2 = "SELECT * FROM `Device_EntertainArea` WHERE `FK_Device` =" . $mp . " LIMIT 0, 30 ";
	$result2 = mysql_query($sql2, $conn) or die(mysql_error($conn));

	if (mysql_num_rows($result2) == 0) {
		echo "parent device not set, continuing.<br>";
		$sql = "SELECT * FROM `Device_EntertainArea` WHERE `FK_Device` =" . $mediaPlayerID . " LIMIT 0, 30 ";
		$result = mysql_query($sql, $conn) or die(mysql_error($conn));

		if (mysql_num_rows($result) == 0) {
			echo "Missing Entry, needs to be added for " . $deviceName . "<br> ";

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

	$deviceName = mysql_real_escape_string($_GET['label']);
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

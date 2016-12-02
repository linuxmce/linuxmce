<?php

/*
 * Qrbitergenerator.php. Its sole function is to prepare a blob
 * configuration data for orbiter to consume on startup with key data to interact
 * with the system
 */

//initialization area
if (isset($_GET["d"])) {
	$deviceID = $_GET['d'];
} else if (isset($_GET["m"])) {
	echo getMountPounts();
	exit ;
} else if (isset($_GET["c"])) {
	echo getCommandParameters($_GET["c"]);
	exit ;
} else if (isset($_GET["id"])) {
	ini_set('display_errors', 1);
	error_reporting(E_ALL);
	include "operations/mediaBrowser/mediaStreamer.php";
	sendMedia($_GET["id"]);
	exit ;
} else {
	die("Please specify the device ID with d=xxxx");
}

$server = "localhost";
$mysqlUser = "root";
$mysqlPass = "";
$orbiterConfPath = "/var/tmp/" . $deviceID . "conf.xml";

$conn = mysqli_connect($server, $mysqlUser, $mysqlPass);
mysqli_select_db($conn, "pluto_main");

//installation variable area
$userArray = array();
$floorplanArray = array();
$roomArray = array();
$entertainAreaArray = array();
$lightingScenarioArray = array();
$mediaScenarioArray = Array();
$climateScenarioArray = Array();
$telecomScenarioArray = Array();
$securitySecnarioArray = Array();

// array relating to grids and other data
$mediaSortArray = Array();

//main loop
if ($conn) {
	//echo "Connected, Proceeding with OrbiterGen for device: ".$deviceID."<br>";
} else {

	//echo "could not connect";
}

$doc = new DOMDocument();
$doc -> formatOutput = true;
$orbiterData = $doc -> createElement("Orbiter" . $deviceID);
$doc -> appendChild($orbiterData);

setupConfFile($deviceID, $conn, $orbiterConfPath);
getFileFormats($conn, $doc, $orbiterData);
getMediaSubTypes($conn, $doc, $orbiterData);
if (getDefaults($conn, $doc, $orbiterData)) {
	//	echo ".....done<br>";
}

if ($userCheck = getUserArray($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if ($roomCheck = getRooms($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if ($genreCheck = getGenres($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if ($attribCheck = getSortingViews($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getFloorplans($conn, $doc, $orbiterData)) {
	//      echo "....done!<br>";
}
if (getLightingScenarios($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getMediaScenarios($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getClimateScenarios($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getTelecomScenarios($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getSecurityScenarios($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getFloorPlanDevices($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (getKeys($conn, $doc, $orbiterData)) {
	//	echo "....done!<br>";
}

if (mysqli_close($conn)) {
	//echo "DB closed";
}

//echo $doc->saveXML();
$doc -> save($orbiterConfPath);
fclose($orbiterConfPath);
//end main loop
echo file_get_contents($orbiterConfPath);

//function area-----------------------------------------------------------------------------------

//get floorplan information
function getKeys($conn, $doc, $orbiterData) {

	$keysElement = $doc -> createElement("Keys");
	$orbiterData -> appendChild($keysElement);

	$t = $doc -> createElement("Key");
	$attrib2 = $t -> setAttribute("id", "ca");
	$keysElement -> appendChild($t);
	$c = $doc -> createCDATASection(  str_replace("\n", "\r\n", file_get_contents("/etc/pluto/certs/CA/cacert.pem")) );
	$t -> appendChild($c);

	$t = $doc -> createElement("Key");
	$attrib2 = $t -> setAttribute("id", "clientp");
	$keysElement -> appendChild($t);
	$c = $doc -> createCDATASection( str_replace("\n", "\r\n", file_get_contents("/etc/pluto/certs/dce-client.crt")) );
	$t -> appendChild($c);

	$t = $doc -> createElement("Key");
	$attrib2 = $t -> setAttribute("id", "client");
	$keysElement -> appendChild($t);
	$c = $doc -> createCDATASection( str_replace("\n", "\r\n", file_get_contents("/etc/pluto/certs/dce-client.key.pem")) );
	$t -> appendChild($c);

	return true;
}

//get floorplan information
function getFloorplans($conn, $doc, $orbiterData) {
	$sql = "SELECT * FROM `Floorplan` ";

	//	echo "Setting Floorplan...";

	$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

	$floorplanElement = $doc -> createElement("Floorplans");
	$orbiterData -> appendChild($floorplanElement);

	while ($row = mysqli_fetch_array($result)) {
		$t = $doc -> createElement("Floorplan");

		$attrib2 = $t -> setAttribute("Page", $row['Page']);
		$attrib2 = $t -> setAttribute("Description", $row['Description']);
		$attrib2 = $t -> setAttribute("Icon", $row['FK_Icon']);
		$attrib2 = $t -> setAttribute("Installation", $row['FK_Installation']);
		$floorplanElement -> appendChild($t);
	}

	return true;
}

function getFileFormats($conn, $doc, $orbiterData) {
	if (mysqli_select_db($conn, "pluto_media")) {

		$sql = "SELECT * FROM `FileFormat` ";

		//	echo "Setting FileFormats Sort List Up...";

		$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

		$genreElement = $doc -> createElement("FileFormats");
		$orbiterData -> appendChild($genreElement);

		while ($row = mysqli_fetch_array($result)) {
			$t = $doc -> createElement("FileFormat");
			$att1 = $t -> setAttribute("Name", $row['Description']);
			$att1 = $t -> setAttribute("PK_FileFormat", $row['PK_FileFormat']);
			$genreElement -> appendChild($t);
		}

		if (mysqli_select_db($conn, "pluto_main")) {
			return true;
		}
	}
}

//-----media subtypes------------------------------

function getMediaSubTypes($conn, $doc, $orbiterData) {
	if (mysqli_select_db($conn, "pluto_media")) {

		$sql = "SELECT * FROM `MediaSubType` ";

		//	echo "Setting Media SubTypes Sort List Up...";

		$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

		$genreElement = $doc -> createElement("MediaSubTypes");
		$orbiterData -> appendChild($genreElement);

		while ($row = mysqli_fetch_array($result)) {
			$t = $doc -> createElement("MediaSubType");
			$att1 = $t -> setAttribute("Name", $row['Description']);
			$att1 = $t -> setAttribute("PK_MediaSubType", $row['PK_MediaSubType']);
			$genreElement -> appendChild($t);
		}

		if (mysqli_select_db($conn, "pluto_main")) {
			return true;
		}
	}
}

//prepares a userlist
function getUserArray($conn, $doc, $orbiterData) {
	//	echo ("Looking up Users now..");
	$sql = "SELECT  `PK_Users` ,  `UserName` ,  `HasMailbox` ,  `AccessGeneralMailbox` , 
	 `Extension` ,  `FirstName` ,  `LastName` ,  `Nickname` ,  `FK_UserMode` 
	FROM  `Users` 
	LIMIT 0 , 30";

	$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

	$uElement = $doc -> createElement("Users");
	$orbiterData -> appendChild($uElement);

	while ($row = mysqli_fetch_array($result)) {
		$b = $doc -> createElement($row['UserName']);
		$attrib1 = $b -> setAttribute("PK_User", $row['PK_Users']);
		$attrib2 = $b -> setAttribute("HasMailbox", $row['HasMailbox']);
		$attrib2 = $b -> setAttribute("UserMode", $row['FK_UserMode']);
		$attrib2 = $b -> setAttribute("FirstName", $row['FirstName']);
		$attrib2 = $b -> setAttribute("LastName", $row['LastName']);
		$attrib2 = $b -> setAttribute("NickName", $row['Nickname']);
		$attrib2 = $b -> setAttribute("AccessGeneralMailbox", $row['AccessGeneralMailbox']);
		$uElement -> appendChild($b);
	}

	return true;

}

//Configuration File Checking-----------------------------------------------------------------------
function setupConfFile($deviceID, $conn, $orbiterConfPath) {
	//	echo "Looking for ".$orbiterConfPath."...";
	if (fopen($orbiterConfPath, "w+")) {
		//echo "found existing conf, overwriting!<br>";

	} else {
		//echo "Config does not exist, creating: <br>";
		fopen($orbiterConfPath, "w+") or die(print_r(error_get_last()));

		return true;
	}
}

//getdefaults------------------------------------------------------------------------------------
function getDefaults($conn, $doc, $orbiterData) {
	//echo "Finding default user and room...";
	$dElement = $doc -> createElement("Default");

	$defaultUserSql = "Select * from Device_DeviceData LEFT JOIN Users on 
Device_DeviceData.IK_DeviceData = Users.PK_Users
 where FK_DeviceData = 3 and FK_Device =" . $_GET['d'];
	$result = mysqli_query($conn, $defaultUserSql) or die(mysqli_error($conn));
	if ($row = mysqli_fetch_array($result)) {
		$attrib1 = $dElement -> setAttribute("PK_User", $row['IK_DeviceData']);
		$attrib3 = $dElement -> setAttribute("sPK_User", $row['UserName']);

	}

	//$defaultLocationSql = "Select * from Device_DeviceData where FK_DeviceData = 27 and FK_Device =".$_GET['d'];
	$defaultLocationSql = "Select * from Device where PK_Device =" . $_GET['d'];
	$result = mysqli_query($conn, $defaultLocationSql) or die(mysqli_error($conn));
	if ($row = mysqli_fetch_array($result)) {

		$subResult = mysqli_query($conn, "SELECT * from EntertainArea where FK_Room = '" . $row['FK_Room'] . "'");
		$row2 = mysqli_fetch_array($subResult);

		$attrib2 = $dElement -> setAttribute("DefaultLocation", $row2['Description']);
		$attrib2 = $dElement -> setAttribute("DefaultEA", $row2['PK_EntertainArea']);
		$attrib2 = $dElement -> setAttribute("DefaultRoom", $row['FK_Room']);
		$room = $row['FK_Room'];

	}
	$orbiterData -> appendChild($dElement);
	return true;

}

//get rooms--------------------------------------------------------------------------------------
function getRooms($conn, $doc, $orbiterData) {
	//echo ("Looking up Rooms now.. ");

	$sql = "SELECT Room.*,EntertainArea.PK_EntertainArea, EntertainArea.Description as EA, Icon.MainFileName, Icon.SelectedFileName, Icon.AltFileNames, Icon.BackgroundFileName FROM `Room`
	        Left Join EntertainArea On Room.PK_Room = EntertainArea.FK_Room
                Left Join Icon On FK_Icon = PK_Icon ";

	$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

	$rElement = $doc -> createElement("Rooms");
	$orbiterData -> appendChild($rElement);

	while ($row = mysqli_fetch_array($result)) {
		$desc = $row['Description'];
		$r = $doc -> createElement("node");

		$att1 = $r -> setAttribute("PK_Room", $row['PK_Room']);
		$att1 = $r -> setAttribute("FK_RoomType", $row['FK_RoomType']);
		$att2 = $r -> setAttribute("Description", $row['Description']);
		$att2 = $r -> setAttribute("EA", $row['EA']);
		$attrib2 = $r -> setAttribute("ManuallyConfigureEA", $row['ManuallyConfigureEA']);
		$attrib2 = $r -> setAttribute("HideFromOrbiter", $row['HideFromOrbiter']);
		$attrib2 = $r -> setAttribute("PK_EntertainArea", $row['PK_EntertainArea']);
		$attrib2 = $r -> setAttribute("FK_FloorplanObjectType", $row['FK_FloorplanObjectType']);
		$attrib2 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);

		getIcons($r, $row);

		$rElement -> appendChild($r);
	}
	return true;
}

// regular get icons function returning all names of the associated icons
function getIcons($r, $row) {
	$att1 = $r -> setAttribute("MainFileName", $row['MainFileName']);
	$att1 = $r -> setAttribute("SelectedFileName", $row['SelectedFileName']);
	$att1 = $r -> setAttribute("AltFileNames", $row['AltFileNames']);
	$att1 = $r -> setAttribute("BackgroundFileName", $row['BackgroundFileName']);
}

//get lighting scenarios===========================================-=------------------------------------
function getLightingScenarios($conn, $doc, $orbiterData) {
	//	echo "Starting Lighting Scenarios";
	$locationArray = array();
	$locationSql = "SELECT Room.PK_Room, Room.Description as RoomDesc, EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = EntertainArea.FK_Room and EntertainArea.Private = 0";

	$lightingScenarioElement = $doc -> createElement("LightingScenarios");
	$orbiterData -> appendChild($lightingScenarioElement);

	$roomResult = mysqli_query($conn, $locationSql);
	while ($row = mysqli_fetch_array($roomResult)) {
		$lightlocationitem = $doc -> createElement("Room");
		$locationAttribute1 = $lightlocationitem -> setAttribute("int", $row['PK_Room']);
		$locationAttribute2 = $lightlocationitem -> setAttribute("EAstring", $row['Description']);
		$locationAttribute3 = $lightlocationitem -> setAttribute("EntertainArea", $row['PK_EntertainArea']);
		$locationAttribute4 = $lightlocationitem -> setAttribute("Room", $row['RoomDesc']);

		$lightingScenarioElement -> appendChild($lightlocationitem);

		$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled,Icon.MainFileName, Icon.SelectedFileName, Icon.AltFileNames, Icon.BackgroundFileName  from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     LEFT Join Icon On CommandGroup.FK_Icon = PK_Icon
     where CommandGroup.FK_Array = 1 and Room.PK_Room=" . $row['PK_Room'] . " and CommandGroup.Disabled = 0 
     Order By CommandGroup_Room.Sort";

		$result = mysqli_query($conn, $autoGeneratedSql);

		while ($row = mysqli_fetch_array($result)) {

			$r = $doc -> createElement("LightingButton");

			$att1 = $r -> setAttribute("FK_Room", $row['FK_Room']);
			$att1 = $r -> setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
			$att2 = $r -> setAttribute("Description", $row['Description']);
			$attrib2 = $r -> setAttribute("eaDescription", $row['RoomDesc']);

			$attrib2 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);
			getIcons($r, $row);
			$lightlocationitem -> appendChild($r);
		}

	}
	$eaScenariosSql = "SELECT  CommandGroup_EntertainArea.FK_CommandGroup, 
	CommandGroup_EntertainArea.FK_EntertainArea,           
	EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	EntertainArea.Description as eaDescription, EntertainArea.Private, 	CommandGroup_EntertainArea.Sort, 
	CommandGroup.FK_Array, 	CommandGroup.Hint,	CommandGroup.Description
	from CommandGroup_EntertainArea 
	Left Join  EntertainArea on
	CommandGroup_EntertainArea.FK_EntertainArea = EntertainArea.PK_EntertainArea
	left join CommandGroup on
   	CommandGroup.Hint = EntertainArea.Description
	where CommandGroup.FK_Array = 1";

	return true;
}

//------------------===========================--------------------------------genres------------------
function getGenres($conn, $doc, $orbiterData) {
	if (mysqli_select_db($conn, "pluto_media")) {

		$sql = "SELECT * FROM `Attribute` WHERE FK_AttributeType = 8";

		//	echo "Setting Genre List Up...";

		$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

		$genreElement = $doc -> createElement("GenreList");
		$orbiterData -> appendChild($genreElement);

		while ($row = mysqli_fetch_array($result)) {
			$t = $doc -> createElement("GenreNode");
			$att1 = $t -> setAttribute("Name", $row['Name']);
			$att1 = $t -> setAttribute("FK_AttributeType", $row['FK_AttributeType']);
			$attrib2 = $t -> setAttribute("PK_Attribute", $row['PK_Attribute']);
			$genreElement -> appendChild($t);
		}

		if (mysqli_select_db($conn, "pluto_main")) {
			return true;
		}
	}
}

//-----attributes for sort----------------------------------------------------------
function getSortingViews($conn, $doc, $orbiterData) {
	if (mysqli_select_db($conn, "pluto_media")) {

		$sql = "SELECT * FROM `AttributeType` ";

		//	echo "Setting Attribute Sort List Up...";

		$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

		$genreElement = $doc -> createElement("AttributeList");
		$orbiterData -> appendChild($genreElement);

		while ($row = mysqli_fetch_array($result)) {
			$t = $doc -> createElement("Attribute");
			$att1 = $t -> setAttribute("Name", $row['Description']);
			$att1 = $t -> setAttribute("PK_AttributeType", $row['PK_AttributeType']);
			$genreElement -> appendChild($t);
		}

		if (mysqli_select_db($conn, "pluto_main")) {
			return true;
		}
	}
}

//Media scenarios====================================----------------------------------------------------
function getMediaScenarios($conn, $doc, $orbiterData) {
	//	echo "Starting Media Scenarios";
	$locationArray = array();
	$locationSql = "SELECT EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM  EntertainArea where EntertainArea.Private = 0";

	$mediaScenarioElement = $doc -> createElement("MediaScenarios");

	$orbiterData -> appendChild($mediaScenarioElement);

	$roomResult = mysqli_query($conn, $locationSql);
	while ($row = mysqli_fetch_array($roomResult)) {
		$mediaLocationItem = $doc -> createElement("Room");
		$locationAttribute1 = $mediaLocationItem -> setAttribute("int", $row['FK_Room']);
		$locationAttribute2 = $mediaLocationItem -> setAttribute("EAstring", $row['Description']);
		$locationAttribute3 = $mediaLocationItem -> setAttribute("EntertainArea", $row['PK_EntertainArea']);
		$locationAttribute4 = $mediaLocationItem -> setAttribute("Room", $row['RoomDesc']);

		$mediaScenarioElement -> appendChild($mediaLocationItem);

		$autoGeneratedSql = "SELECT  CommandGroup_EntertainArea.FK_CommandGroup, CommandGroup_EntertainArea.FK_EntertainArea, CommandGroup_EntertainArea.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, 
	 CommandGroup.Disabled from CommandGroup_EntertainArea 
	 LEFT Join
     CommandGroup on CommandGroup_EntertainArea.FK_CommandGroup = CommandGroup.PK_CommandGroup      
     where CommandGroup.FK_Array =5 and CommandGroup.Disabled = 0 and FK_EntertainArea = " . $row['PK_EntertainArea'] . " Order By CommandGroup_EntertainArea.Sort";

		$result = mysqli_query($conn, $autoGeneratedSql);

		while ($row = mysqli_fetch_array($result)) {

			$r = $doc -> createElement("MediaButton");

			$att1 = $r -> setAttribute("FK_Room", $row['FK_Room']);
			$att1 = $r -> setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
			$att2 = $r -> setAttribute("Description", $row['Description']);
			$attrib2 = $r -> setAttribute("eaDescription", $row['RoomDesc']);

			$attrib2 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);
			$mediaLocationItem -> appendChild($r);
		}

	}
	$eaScenariosSql = "SELECT  CommandGroup_EntertainArea.FK_CommandGroup, 
	CommandGroup_EntertainArea.FK_EntertainArea,           
	EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	EntertainArea.Description as eaDescription, EntertainArea.Private, 	CommandGroup_EntertainArea.Sort, 
	CommandGroup.FK_Array, 	CommandGroup.Hint,	CommandGroup.Description
	from CommandGroup_EntertainArea 
	Left Join  EntertainArea on
	CommandGroup_EntertainArea.FK_EntertainArea = EntertainArea.PK_EntertainArea
	left join CommandGroup on
   	CommandGroup.Hint = EntertainArea.Description
	where CommandGroup.FK_Array = 1";

	return true;
}

//climate scenarios================================================--------------------------------------
function getClimateScenarios($conn, $doc, $orbiterData) {
	//	echo "Starting Climate Scenarios";
	$locationArray = array();
	$locationSql = "SELECT Room.PK_Room, Room.Description as RoomDesc, EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = EntertainArea.FK_Room and EntertainArea.Private = 0";

	$climateScenarioElement = $doc -> createElement("ClimateScenarios");
	$orbiterData -> appendChild($climateScenarioElement);

	$climateresult = mysqli_query($conn, $locationSql);
	while ($row = mysqli_fetch_array($climateresult)) {
		$climateLocationItem = $doc -> createElement("ClimateRoom");
		$locationAttribute1 = $climateLocationItem -> setAttribute("int", $row['PK_Room']);
		$locationAttribute2 = $climateLocationItem -> setAttribute("EAstring", $row['Description']);
		$locationAttribute3 = $climateLocationItem -> setAttribute("EntertainArea", $row['PK_EntertainArea']);
		$locationAttribute4 = $climateLocationItem -> setAttribute("Room", $row['RoomDesc']);

		$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 2 and Room.PK_Room=" . $row['PK_Room'];

		$result = mysqli_query($conn, $autoGeneratedSql);

		while ($row = mysqli_fetch_array($result)) {

			$r = $doc -> createElement("ClimateButton");
			$att1 = $r -> setAttribute("FK_Room", $row['FK_Room']);
			$att2 = $r -> setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
			$att3 = $r -> setAttribute("Description", $row['Description']);
			$attrib4 = $r -> setAttribute("eaDescription", $row['RoomDesc']);
			$attrib5 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);
			$attrib6 = $r -> setAttribute("Sort", $row['Sort']);
			$climateLocationItem -> appendChild($r);
		}
		$climateScenarioElement -> appendChild($climateLocationItem);
	}
	$eaScenariosSql = "SELECT  CommandGroup_EntertainArea.FK_CommandGroup, 
	CommandGroup_EntertainArea.FK_EntertainArea,           
	EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	EntertainArea.Description as eaDescription, EntertainArea.Private, 	CommandGroup_EntertainArea.Sort, 
	CommandGroup.FK_Array, 	CommandGroup.Hint,	CommandGroup.Description
	from CommandGroup_EntertainArea 
	Left Join  EntertainArea on
	CommandGroup_EntertainArea.FK_EntertainArea = EntertainArea.PK_EntertainArea
	left join CommandGroup on
   	CommandGroup.Hint = EntertainArea.Description
	where CommandGroup.FK_Array = 1";

	return true;
}

//telecom scenarios========--------------------------------------------------------------------------
function getTelecomScenarios($conn, $doc, $orbiterData) {
	//	echo "Starting Telecom Scenarios";
	$locationArray = array();
	$locationSql = "SELECT Room.PK_Room, Room.Description as RoomDesc,
	 EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	 EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = 
	 EntertainArea.FK_Room and EntertainArea.Private = 0";

	$telecomScenarioElement = $doc -> createElement("TelecomScenarios");
	$orbiterData -> appendChild($telecomScenarioElement);

	$result = mysqli_query($conn, $locationSql);

	while ($row = mysqli_fetch_array($result)) {
		$telecomLocationItem = $doc -> createElement("TelecomRoom");
		$locationAttribute1 = $telecomLocationItem -> setAttribute("int", $row['PK_Room']);
		$locationAttribute2 = $telecomLocationItem -> setAttribute("EAstring", $row['Description']);
		$locationAttribute3 = $telecomLocationItem -> setAttribute("EntertainArea", $row['PK_EntertainArea']);
		$locationAttribute4 = $telecomLocationItem -> setAttribute("Room", $row['RoomDesc']);

		$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 4 and Room.PK_Room=" . $row['PK_Room'];

		$result2 = mysqli_query($conn, $autoGeneratedSql);
		while ($row = mysqli_fetch_array($result2)) {
			$r = $doc -> createElement("TelecomButton");
			$att1 = $r -> setAttribute("FK_Room", $row['PK_Room']);
			$att2 = $r -> setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
			$att3 = $r -> setAttribute("Description", $row['Description']);
			$attrib4 = $r -> setAttribute("eaDescription", $row['RoomDesc']);
			$attrib5 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);
			$attrib6 = $r -> setAttribute("Sort", $row['Sort']);
			$telecomLocationItem -> appendChild($r);
		}
		$telecomScenarioElement -> appendChild($telecomLocationItem);
	}

	return true;
}

//security scenarios===============================-----------------------------------------------------
function getSecurityScenarios($conn, $doc, $orbiterData) {
	//	echo "Starting Security Scenarios";
	$locationArray = array();
	$locationSql = "SELECT Room.PK_Room, Room.Description as RoomDesc,
	 EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	 EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = 
	 EntertainArea.FK_Room and EntertainArea.Private = 0";

	$securityScenarioElement = $doc -> createElement("SecurityScenarios");
	$orbiterData -> appendChild($securityScenarioElement);

	$result = mysqli_query($conn, $locationSql);

	while ($row = mysqli_fetch_array($result)) {
		$securityLocationItem = $doc -> createElement("SecurityRoom");
		$locationAttribute1 = $securityLocationItem -> setAttribute("int", $row['PK_Room']);
		$locationAttribute2 = $securityLocationItem -> setAttribute("EAstring", $row['Description']);
		$locationAttribute3 = $securityLocationItem -> setAttribute("EntertainArea", $row['PK_EntertainArea']);
		$locationAttribute4 = $securityLocationItem -> setAttribute("Room", $row['RoomDesc']);

		$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 3 and Room.PK_Room=" . $row['PK_Room'];

		$result2 = mysqli_query($conn, $autoGeneratedSql);
		while ($row = mysqli_fetch_array($result2)) {
			$r = $doc -> createElement("SecurityButton");
			$att1 = $r -> setAttribute("FK_Room", $row['PK_Room']);
			$att2 = $r -> setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
			$att3 = $r -> setAttribute("Description", $row['Description']);
			$attrib4 = $r -> setAttribute("eaDescription", $row['RoomDesc']);
			$attrib5 = $r -> setAttribute("FloorplanInfo", $row['FloorplanInfo']);
			$attrib6 = $r -> setAttribute("Sort", $row['Sort']);
			$securityLocationItem -> appendChild($r);
		}
		$securityScenarioElement -> appendChild($securityLocationItem);
	}

	return true;
}

function getFloorPlanDevices($conn, $doc, $orbiterData) {
	//	echo "Looking for  floorplan devices"."<br>";

	$floorplanDeviceElement = $doc -> createElement("FloorplanDevices");
	//setup xml elements for reading
	$orbiterData -> appendChild($floorplanDeviceElement);

	//looks for devices with floorplan data
	//selects all the devices with floorplan info thanks possy
	$sql = "select PK_Device, Device.Description, Device.FK_Room,
        ddd1.IK_DeviceData as FloorplanInfo,
        ddd2.IK_DeviceData as FloorplanObjectType,
        fopt.FK_FloorplanType as FloorplanType
        From Device

        Join Device_DeviceData as ddd1 on Device.PK_Device = ddd1.FK_Device
        Join Device_DeviceData as ddd2 on Device.PK_Device = ddd2.FK_Device
        Join FloorplanObjectType as fopt on ddd2.IK_DeviceData = fopt.PK_FloorplanObjectType

        Where ddd1.FK_DeviceData = 10 and ddd2.FK_DeviceData = 11 ORDER BY FK_Room;  ";

	$result = mysqli_query($conn, $sql);
	$i = 0;
	while ($row = mysqli_fetch_array($result)) {

		$fp = $doc -> createElement("FPDevice");
		$att1 = $fp -> setAttribute("Name", $row['Description']);
		$att2 = $fp -> setAttribute("Device", $row['PK_Device']);
		$att3 = $fp -> setAttribute("Position", $row['FloorplanInfo']);
		$att4 = $fp -> setAttribute("Type", $row['FloorplanObjectType']);
		$att5 = $fp -> setAttribute("fpType", $row['FloorplanType']);
		$att6 = $fp -> setAttribute("Room", $row['FK_Room']);
		$floorplanDeviceElement -> appendChild($fp);

		$i++;
	}

	
	$sql = "select PK_EntertainArea, Description, FloorplanInfo, FK_FloorplanObjectType 
		       From EntertainArea where FloorplanInfo IS NOT NULL AND FK_FloorplanObjectType IS NOT NULL";
	$result = mysqli_query($conn, $sql);
	$i = 0;
	while ($row = mysqli_fetch_array($result)) {
		$fp = $doc -> createElement("FPDevice");
		$att1 = $fp -> setAttribute("Name", $row['Description']);
		$att2 = $fp -> setAttribute("Device", $row['PK_EntertainArea']);
		$att3 = $fp -> setAttribute("Position", $row['FloorplanInfo']);
		$att4 = $fp -> setAttribute("Type", $row['FK_FloorplanObjectType']);
		$att5 = $fp -> setAttribute("fpType", 5);		
		$floorplanDeviceElement -> appendChild($fp);
		$i++;
	}

//		echo $i." Devices with floorplan data";
	return true;
}

function getMountPounts() {
	$server = "localhost";
	$mysqlUser = "root";
	$mysqlPass = "";

	$conn = mysqli_connect($server, $mysqlUser, $mysqlPass);
	if ($conn) {
		//	echo "Connection!";
	}

	if (mysqli_select_db($conn, "pluto_main")) {
		//	echo "found db";
	}

	$query = 'select *from Device where FK_DeviceTemplate=-202 OR FK_DeviceTemplate=11 OR FK_DeviceTemplate=1850 OR FK_DeviceTemplate=1790 OR FK_DeviceTemplate=1768  OR FK_DeviceTemplate=1769;';
	$deviceArray = array();

	$res = mysqli_query($conn, mysqli_real_escape_string($query)) or die(mysqli_error($conn));
	$i = 0;
	while ($row = mysqli_fetch_array($res)) {
		$deviceArray[] = array("Description" => $row['Description'], "Device" => $row["PK_Device"]);
	}

	$jsonReturn = json_encode($deviceArray);
	return $jsonReturn;
}

function getCommandParameters($commandNumber) {
	$cmd = $commandNumber;

	if (!is_numeric($cmd)) {
		return;
	}

	$server = "localhost";
	$mysqlUser = "root";
	$mysqlPass = "";

	$conn = mysqli_connect($server, $mysqlUser, $mysqlPass);
	if ($conn) {
		//      echo "Connection!";
	}

	if (mysqli_select_db($conn, "pluto_main")) {
		//      echo "found db";
	}

	$sql = "SELECT Command_CommandParameter.FK_Command, Command_CommandParameter.FK_CommandParameter, Command_CommandParameter.Description as cptype , CommandParameter.PK_CommandParameter, CommandParameter.Description, CommandParameter.FK_ParameterType, ParameterType.Description as ParamDesc FROM Command_CommandParameter LEFT JOIN CommandParameter " . "ON CommandParameter.PK_CommandParameter = Command_CommandParameter.FK_CommandParameter LEFT JOIN ParameterType  ON " . "CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType WHERE Command_CommandParameter.FK_Command = " . $cmd . " LIMIT 0, 30 ";

	$paramArray = array();
	$paramArray["device"] = array("device" => $_GET["idx"]);
	$res = mysqli_query($conn, mysqli_real_escape_string($sql)) or die(mysqli_error($conn));
	$p1Array = array();
	while ($row = mysqli_fetch_array($res)) {

		$p1Array[] = array("type" => $row["ParamDesc"], "Command" => $row["FK_Command"], "CommandParameter" => $row["FK_CommandParameter"], "CommandHint" => $row["cptype"], "CommandDescription" => $row["Description"]);
	}
	$paramArray["params"] = $p1Array;
	return json_encode($paramArray);

}

function sendMedia($fkFile) {
		if(isset($_GET['pre'])){
		 return	prepMedia($fkFile);
		}

	$steam = NULL;
	$path_parts = pathinfo($fkFile);
	$data = "/tmp/" . $path_parts['filename'] . ".mp4";
	switch ($path_parts['extension']) {
		case 'mp3' :
		case 'aac' :
		case 'ogg' :
		case 'flac' :
			$stream = new VideoStream($fkFile);
			break;

		case 'mkv' :
			$stream = new VideoStream($data);
			break;
		case 'mpeg' :
		case "avi" :
		$stream = new VideoStream($fkFile);
			break;

		case 'mp4' :
		case 'm4v' :
			$stream = new VideoStream($fkFile);
			break;

		default :
			
			break;
	}
	$stream.start();

	/*
	 session_start();
	 $_SESSION['file']=$fkFile;
	 $size=filesize($fkFile);
	 $length=$size;
	 $start=0;
	 $end=$size-1;
	 header("Content-Disposition:inline; filename=".$fkFile."");
	 header("Content-length:".$size."");
	 header("Content-Transfer-Encoding: binary");
	 header("Content-Type: audio/mpeg");
	 header("Accept-Ranges:0-".$length);
	 header("Content-Range: bytes".$start."-".$end/$size."");
	 echo file_get_contents($fkFile);
	 */

}

function prepMedia($fkFile){
	echo "Preprocessing file $fkFile";
	
	$path_parts = pathinfo($fkFile);
	$data = "/tmp/" . $path_parts['filename'] . ".mp4";
	switch ($path_parts['extension']) {
		case 'mp3' :
		case 'aac' :
		case 'ogg' :
		case 'flac' :
			echo "Audio, No processing";
			//$stream = new AudioStream($fkFile); 
			return "Done";
			break;

		case 'mkv' :
		echo "MKV, Attempting Remux<br>";
			
		if(!file_exists($data)){
			sleep(1);
			 shell_exec("/var/www/lmce-admin/./remux.sh -f '".$fkFile."' > /dev/null 2>/dev/null ");
			 
		}	
			return "Done";
			break;
		case 'mpeg' :
		case "avi" :
			echo "AVI/MPEG, No processing, too much cpu required.";
				return "Done";
			break;
		case 'mp4' :
		case 'm4v' :
			echo "Mp4, mv4, not processing. May experience audio problems.";
			return "Done";
			break;

		default :
			return "Done";
			break;
	}
	
}

function StartsWith($Haystack, $Needle) {
	// Recommended version, using strpos
	return strpos($Haystack, $Needle) === 0;
}
?>
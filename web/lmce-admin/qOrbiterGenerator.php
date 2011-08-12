<?php

//initialization area
$deviceID = $_GET['d'];
$server = "localhost";
$mysqlUser = "root";
$mysqlPass = "";
$orbiterConfPath = "/var/tmp/".$deviceID."conf.xml";

$conn = mysql_connect($server, $mysqlUser, $mysqlPass);
mysql_select_db("pluto_main", $conn);

//installation variable area
$userArray = array();
$roomArray = array();
$entertainAreaArray = array();
$lightingScenarioArray = array();
$mediaScenarioArray=Array();
$climateScenarioArray=Array();
$telecomScenarioArray=Array();
$securitySecnarioArray=Array();

// array relating to grids and other data
$mediaSortArray=Array();


//main loop
if ($conn)
{
echo "Connected, Proceeding with OrbiterGen for device: ".$deviceID."<br>";
}
else 
{
	
echo "could not connect";
}


		$doc = new DOMDocument();
		$doc->formatOutput = true;
		$orbiterData = $doc->createElement("Orbiter".$deviceID);
		$doc->appendChild($orbiterData);

setupConfFile($deviceID, $conn, $orbiterConfPath);
getFileFormats($conn, $doc, $orbiterData);
getMediaSubTypes($conn, $doc, $orbiterData);
if (getDefaults($conn, $doc, $orbiterData))
{
	echo ".....done<br>";
}

if ($userCheck = getUserArray($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if ($roomCheck = getRooms($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if ($genreCheck = getGenres($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if ($attribCheck = getSortingViews($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if (getLightingScenarios($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if (getMediaScenarios($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if (getClimateScenarios($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if (getTelecomScenarios($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}

if (getSecurityScenarios($conn, $doc, $orbiterData))
{
	echo "....done!<br>";
}
	
if (mysql_close($conn))
{
echo "DB closed";
}


echo $doc->saveXML();
$doc->save($orbiterConfPath);
fclose($orbiterConfPath);
//end main loop


//function area-----------------------------------------------------------------------------------

function getFileFormats($conn, $doc, $orbiterData)
{
if (mysql_select_db("pluto_media", $conn))
	{
	
	$sql ="SELECT * FROM `FileFormat` ";
	
	echo "Setting FileFormats Sort List Up...";
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
		
	$genreElement = $doc->createElement("FileFormats");	
	$orbiterData->appendChild($genreElement);
	
	while ($row = mysql_fetch_array($result))
		{
		$t = $doc->createElement("FileFormat");			
		$att1= $t->setAttribute("Name", $row['Description']);
		$att1= $t->setAttribute("PK_FileFormat", $row['PK_FileFormat']);						
		$genreElement->appendChild($t);		
		}		
		
		if(mysql_select_db("pluto_main", $conn))
		{
	return true;
		}
	}
}
//-----media subtypes------------------------------

function getMediaSubTypes($conn, $doc, $orbiterData)
{
if (mysql_select_db("pluto_media", $conn))
	{
	
	$sql ="SELECT * FROM `MediaSubType` ";
	
	echo "Setting Media SubTypes Sort List Up...";
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
		
	$genreElement = $doc->createElement("MediaSubTypes");	
	$orbiterData->appendChild($genreElement);
	
	while ($row = mysql_fetch_array($result))
		{
		$t = $doc->createElement("MediaSubType");			
		$att1= $t->setAttribute("Name", $row['Description']);
		$att1= $t->setAttribute("PK_MediaSubType", $row['PK_MediaSubType']);						
		$genreElement->appendChild($t);		
		}		
		
		if(mysql_select_db("pluto_main", $conn))
		{
	return true;
		}
	}
}

//prepares a userlist
function getUserArray($conn, $doc, $orbiterData)
{
	echo ("Looking up Users now..");	
	$sql = "SELECT  `PK_Users` ,  `UserName` ,  `HasMailbox` ,  `AccessGeneralMailbox` , 
	 `Extension` ,  `FirstName` ,  `LastName` ,  `Nickname` ,  `FK_UserMode` 
	FROM  `Users` 
	LIMIT 0 , 30"; 
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
	
	$uElement = $doc->createElement("Users");
	$orbiterData->appendChild($uElement);
	
	while ($row = mysql_fetch_array($result))
	{
		$b = $doc->createElement($row['UserName']);		
		$attrib1= $b->setAttribute("PK_User", $row['PK_Users']);
		$attrib2= $b->setAttribute("HasMailbox", $row['HasMailBox']);
		$attrib2= $b->setAttribute("UserMode", $row['FK_UserMode']);
		$attrib2= $b->setAttribute("FirstName", $row['FirstName']);
		$attrib2= $b->setAttribute("LastName", $row['LastName']);
		$attrib2= $b->setAttribute("NickName", $row['NickName']);
		$attrib2= $b->setAttribute("AccessGeneralMailbox", $row['AccessGeneralMailbox']);				
		$uElement->appendChild($b);	
	}
			
	return true;			
	
}

//Configuration File Checking-----------------------------------------------------------------------
function setupConfFile($deviceID,$conn,$orbiterConfPath)
{
	echo "Looking for ".$orbiterConfPath."...";
	if (fopen($orbiterConfPath, "w+"))	{
		echo "found existing conf, overwriting!<br>";
		
	}
	else 
	{
		echo "Config does not exist, creating: <br>";
		fopen($orbiterConfPath, "w+") or die(print_r(error_get_last()));
		
		return true;
	}
}

//getdefaults------------------------------------------------------------------------------------
function getDefaults($conn, $doc, $orbiterData)
{
echo "Finding default user and room...";
$dElement = $doc->createElement("Default");
$orbiterData->appendChild($dElement);

$defaultUserSql = "Select * from Device_DeviceData LEFT JOIN Users on 
Device_DeviceData.IK_DeviceData = Users.PK_Users
 where FK_DeviceData = 3 and FK_Device =".$_GET['d'];
$result = mysql_query($defaultUserSql,$conn) or die(mysql_error($conn));
	while ($row = mysql_fetch_array($result))
	{			
		$attrib1= $dElement->setAttribute("PK_User", $row['IK_DeviceData']);
		$attrib3= $dElement->setAttribute("sPK_User", $row['UserName'] );
		
	}
	
$defaultLocationSql = "Select * from Device_DeviceData where FK_DeviceData = 27 and FK_Device =".$_GET['d'];
$result = mysql_query($defaultLocationSql,$conn) or die(mysql_error($conn));
	while ($row = mysql_fetch_array($result))
	{			
		
		$subResult = mysql_query("SELECT * from EntertainArea where Description = '".$row['IK_DeviceData']."'", $conn);
		$row2 = mysql_fetch_array($subResult);
		
		$attrib2= $dElement->setAttribute("DefaultLocation", $row['IK_DeviceData']);
		$attrib2= $dElement->setAttribute("DefaultEA", $row2['PK_EntertainArea']);
		$attrib2= $dElement->setAttribute("DefaultRoom", $row2['FK_Room']);
		
	}
	
			
	return true;

}

//get rooms--------------------------------------------------------------------------------------
function getRooms($conn, $doc, $orbiterData)
{	
echo ("Looking up Rooms now.. ");	

	$sql = "SELECT * FROM `Room` ";
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
	
	$rElement = $doc->createElement("Rooms");
	$orbiterData->appendChild($rElement);
	
	while ($row = mysql_fetch_array($result))
	{				
		$desc = $row['PK_Description'];
		$r = $doc->createElement("node");	
		
		$att1= $r->setAttribute("PK_Room", $row['PK_Room']);
		$att1= $r->setAttribute("FK_RoomType", $row['FK_RoomType']);
		$att2= $r->setAttribute("Description", $row['Description']);
		$attrib2= $r->setAttribute("ManuallyConfigureEA", $row['ManuallyConfigureEA']);
		$attrib2= $r->setAttribute("HideFromOrbiter", $row['HideFromOrbiter']);
		$attrib2= $r->setAttribute("PK_EntertainArea", $row['PK_EntertainArea']);
		$attrib2= $r->setAttribute("FK_FloorplanObjectType", $row['FK_FloorplanObjectType']);
		$attrib2= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$rElement->appendChild($r);		
	}	
				return true;
}

//get lighting scenarios===========================================-=------------------------------------
function getLightingScenarios($conn, $doc, $orbiterData)
{
	echo "Starting Lighting Scenarios";
	$locationArray = array();
	$locationSql= "SELECT Room.PK_Room, Room.Description as RoomDesc, EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = EntertainArea.FK_Room and EntertainArea.Private = 0";
	
	$lightingScenarioElement = $doc->createElement("LightingScenarios");
	$orbiterData->appendChild($lightingScenarioElement);
	
	
	$roomResult = mysql_query($locationSql);	
	while ($row = mysql_fetch_array($roomResult))
	{			
		$lightlocationitem = $doc->createElement("Room");
		$locationAttribute1 = $lightlocationitem->setAttribute("int" ,$row['PK_Room']);
		$locationAttribute2 = $lightlocationitem->setAttribute("EAstring" ,$row['Description']);
		$locationAttribute3 = $lightlocationitem->setAttribute("EntertainArea" ,$row['PK_EntertainArea']);
		$locationAttribute4 = $lightlocationitem->setAttribute("Room" ,$row['RoomDesc']);
		
		$lightingScenarioElement->appendChild($lightlocationitem);
	
	$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 1 and Room.PK_Room=".$row['PK_Room']." and CommandGroup.Disabled = 0 ";
	
	$result=mysql_query($autoGeneratedSql);
	
	while ($row = mysql_fetch_array($result))
	{				
		
		$r = $doc->createElement("LightingButton");	
		
		$att1= $r->setAttribute("FK_Room", $row['FK_Room']);
		$att1= $r->setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
		$att2= $r->setAttribute("Description", $row['Description']);
		$attrib2= $r->setAttribute("eaDescription", $row['RoomDesc']);
		
		$attrib2= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$lightlocationitem->appendChild($r);		
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
function getGenres($conn, $doc, $orbiterData)
{	
	if (mysql_select_db("pluto_media", $conn))
	{
	
	$sql ="SELECT * FROM `Attribute` WHERE FK_AttributeType = 8";
	
	echo "Setting Genre List Up...";
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
		
	$genreElement = $doc->createElement("GenreList");	
	$orbiterData->appendChild($genreElement);
	
	while ($row = mysql_fetch_array($result))
		{
		$t = $doc->createElement("GenreNode");			
		$att1= $t->setAttribute("Name", $row['Name']);
		$att1= $t->setAttribute("FK_AttributeType", $row['FK_AttributeType']);		
		$attrib2= $t->setAttribute("PK_Attribute", $row['PK_Attribute']);			
		$genreElement->appendChild($t);		
		}		
		
		if(mysql_select_db("pluto_main", $conn))
		{
	return true;
		}
	}
}
//-----attributes for sort----------------------------------------------------------
function getSortingViews($conn, $doc, $orbiterData)
{
if (mysql_select_db("pluto_media", $conn))
	{
	
	$sql ="SELECT * FROM `AttributeType` ";
	
	echo "Setting Attribute Sort List Up...";
	
	$result = mysql_query($sql,$conn) or die(mysql_error($conn));
		
	$genreElement = $doc->createElement("AttributeList");	
	$orbiterData->appendChild($genreElement);
	
	while ($row = mysql_fetch_array($result))
		{
		$t = $doc->createElement("Attribute");			
		$att1= $t->setAttribute("Name", $row['Description']);
		$att1= $t->setAttribute("PK_AttributeType", $row['PK_AttributeType']);						
		$genreElement->appendChild($t);		
		}		
		
		if(mysql_select_db("pluto_main", $conn))
		{
	return true;
		}
	}
}

//Media scenarios====================================----------------------------------------------------
function getMediaScenarios($conn, $doc, $orbiterData)
{
	echo "Starting Media Scenarios";
	$locationArray = array();
	$locationSql= "SELECT EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM  EntertainArea where EntertainArea.Private = 0";
	
	$mediaScenarioElement = $doc->createElement("MediaScenarios");
	
	$orbiterData->appendChild($mediaScenarioElement);
	
	
	$roomResult = mysql_query($locationSql);	
	while ($row = mysql_fetch_array($roomResult))
	{			
		$mediaLocationItem = $doc->createElement("Room");
		$locationAttribute1 = $mediaLocationItem->setAttribute("int" ,$row['FK_Room']);
		$locationAttribute2 = $mediaLocationItem->setAttribute("EAstring" ,$row['Description']);
		$locationAttribute3 = $mediaLocationItem->setAttribute("EntertainArea" ,$row['PK_EntertainArea']);
		$locationAttribute4 = $mediaLocationItem->setAttribute("Room" ,$row['RoomDesc']);
		
		$mediaScenarioElement->appendChild($mediaLocationItem);
	
	$autoGeneratedSql = "SELECT  CommandGroup_EntertainArea.FK_CommandGroup, CommandGroup_EntertainArea.FK_EntertainArea, CommandGroup_EntertainArea.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, 
	 CommandGroup.Disabled from CommandGroup_EntertainArea 
	 LEFT Join
     CommandGroup on CommandGroup_EntertainArea.FK_CommandGroup = CommandGroup.PK_CommandGroup      
     where CommandGroup.FK_Array =5 and CommandGroup.Disabled = 0 ";
	
	$result=mysql_query($autoGeneratedSql);
	
	while ($row = mysql_fetch_array($result))
	{				
		
		$r = $doc->createElement("MediaButton");	
		
		$att1= $r->setAttribute("FK_Room", $row['FK_Room']);
		$att1= $r->setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
		$att2= $r->setAttribute("Description", $row['Description']);
		$attrib2= $r->setAttribute("eaDescription", $row['RoomDesc']);
		
		$attrib2= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$mediaLocationItem->appendChild($r);		
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
function getClimateScenarios($conn, $doc, $orbiterData)
{
	echo "Starting Climate Scenarios";
	$locationArray = array();
	$locationSql= "SELECT Room.PK_Room, Room.Description as RoomDesc, EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = EntertainArea.FK_Room and EntertainArea.Private = 0";
	
	$climateScenarioElement = $doc->createElement("ClimateScenarios");	
	$orbiterData->appendChild($climateScenarioElement);
	
	
	$climateresult = mysql_query($locationSql);	
	while ($row = mysql_fetch_array($climateresult))
	{			
		$climateLocationItem = $doc->createElement("ClimateRoom");
		$locationAttribute1 = $climateLocationItem->setAttribute("int" ,$row['PK_Room']);
		$locationAttribute2 = $climateLocationItem->setAttribute("EAstring" ,$row['Description']);
		$locationAttribute3 = $climateLocationItem->setAttribute("EntertainArea" ,$row['PK_EntertainArea']);
		$locationAttribute4 = $climateLocationItem->setAttribute("Room" ,$row['RoomDesc']);		
		
	
	$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 2 and Room.PK_Room=".$row['PK_Room'];
	
	$result=mysql_query($autoGeneratedSql);
	
	while ($row = mysql_fetch_array($result))
	{				
		
		$r = $doc->createElement("ClimateButton");			
		$att1= $r->setAttribute("FK_Room", $row['FK_Room']);
		$att2= $r->setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
		$att3= $r->setAttribute("Description", $row['Description']);
		$attrib4= $r->setAttribute("eaDescription", $row['RoomDesc']);		
		$attrib5= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$attrib6= $r->setAttribute("Sort", $row['Sort']);		
		$climateLocationItem->appendChild($r);		
		}
			$climateScenarioElement->appendChild($climateLocationItem);
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
function getTelecomScenarios($conn, $doc, $orbiterData)
{
	echo "Starting Telecom Scenarios";
	$locationArray = array();
	$locationSql= "SELECT Room.PK_Room, Room.Description as RoomDesc,
	 EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	 EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = 
	 EntertainArea.FK_Room and EntertainArea.Private = 0";
	
	$telecomScenarioElement = $doc->createElement("TelecomScenarios");	
	$orbiterData->appendChild($telecomScenarioElement);	
	
	$result = mysql_query($locationSql);	
	
	while ($row = mysql_fetch_array($result))
	{			
		$telecomLocationItem = $doc->createElement("TelecomRoom");
		$locationAttribute1 = $telecomLocationItem->setAttribute("int" ,$row['PK_Room']);
		$locationAttribute2 = $telecomLocationItem->setAttribute("EAstring" ,$row['Description']);
		$locationAttribute3 = $telecomLocationItem->setAttribute("EntertainArea" ,$row['PK_EntertainArea']);
		$locationAttribute4 = $telecomLocationItem->setAttribute("Room" ,$row['RoomDesc']);				
	
	$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 4 and Room.PK_Room=".$row['PK_Room'];
	
	$result2=mysql_query($autoGeneratedSql);	
	while ($row = mysql_fetch_array($result2))
	{						
		$r = $doc->createElement("TelecomButton");			
		$att1= $r->setAttribute("FK_Room", $row['PK_Room']);
		$att2= $r->setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
		$att3= $r->setAttribute("Description", $row['Description']);
		$attrib4= $r->setAttribute("eaDescription", $row['RoomDesc']);		
		$attrib5= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$attrib6= $r->setAttribute("Sort", $row['Sort']);		
		$telecomLocationItem->appendChild($r);		
		}
			$telecomScenarioElement->appendChild($telecomLocationItem);
	}
	
	return true;
}

//security scenarios===============================-----------------------------------------------------
function getSecurityScenarios($conn, $doc, $orbiterData)
{
	echo "Starting Security Scenarios";
	$locationArray = array();
	$locationSql= "SELECT Room.PK_Room, Room.Description as RoomDesc,
	 EntertainArea.PK_EntertainArea, EntertainArea.FK_Room, 
	 EntertainArea.Description FROM Room LEFT JOIN EntertainArea on Room.PK_Room = 
	 EntertainArea.FK_Room and EntertainArea.Private = 0";
	
	$securityScenarioElement = $doc->createElement("SecurityScenarios");	
	$orbiterData->appendChild($securityScenarioElement);	
	
	$result = mysql_query($locationSql);	
	
	while ($row = mysql_fetch_array($result))
	{			
		$securityLocationItem = $doc->createElement("SecurityRoom");
		$locationAttribute1 = $securityLocationItem->setAttribute("int" ,$row['PK_Room']);
		$locationAttribute2 = $securityLocationItem->setAttribute("EAstring" ,$row['Description']);
		$locationAttribute3 = $securityLocationItem->setAttribute("EntertainArea" ,$row['PK_EntertainArea']);
		$locationAttribute4 = $securityLocationItem->setAttribute("Room" ,$row['RoomDesc']);				
	
	$autoGeneratedSql = "SELECT  CommandGroup_Room.FK_CommandGroup, CommandGroup_Room.FK_Room, CommandGroup_Room.Sort, CommandGroup.Description, 
	CommandGroup.AlwaysShow, Room.Description as RoomDesc, Room.PK_Room,
	 CommandGroup.Disabled from CommandGroup_Room 
	 LEFT Join
     CommandGroup on CommandGroup_Room.FK_CommandGroup = CommandGroup.PK_CommandGroup
     LEFT Join 
     Room on CommandGroup_Room.FK_Room = Room.PK_Room 
     where CommandGroup.FK_Array = 3 and Room.PK_Room=".$row['PK_Room'];
	
	$result2=mysql_query($autoGeneratedSql);	
	while ($row = mysql_fetch_array($result2))
	{						
		$r = $doc->createElement("SecurityButton");			
		$att1= $r->setAttribute("FK_Room", $row['PK_Room']);
		$att2= $r->setAttribute("FK_CommandGroup", $row['FK_CommandGroup']);
		$att3= $r->setAttribute("Description", $row['Description']);
		$attrib4= $r->setAttribute("eaDescription", $row['RoomDesc']);		
		$attrib5= $r->setAttribute("FloorplanInfo", $row['FloorplanInfo']);		
		$attrib6= $r->setAttribute("Sort", $row['Sort']);		
		$securityLocationItem->appendChild($r);		
		}
			$securityScenarioElement->appendChild($securityLocationItem);
	}
	
	return true;
}

?>
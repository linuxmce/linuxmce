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

if (getLightingScenarios($conn, $doc, $orbiterData))
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

//get lighting scenarios
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
		
		$orbiterData->appendChild($lightlocationitem);
	
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

function getSortingViews($conn, $doc, $orbiterData)
{
	
}

?>
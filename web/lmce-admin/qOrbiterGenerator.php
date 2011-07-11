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

$userCheck = getUserArray($conn, $doc, $orbiterData);
$roomCheck = getRooms($conn, $doc, $orbiterData);


	
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
				
}

?>
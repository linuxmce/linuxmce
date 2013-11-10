  <?php
  /*
  setEa - temp utility to set the ea for a new Qorbiter
  */

  //initialization area
  if (isset($_GET["d"])) {
    $deviceID = $_GET['d'];
  } else {
    die("Please specify the device ID with d=xxxx");
  }


 
  $mobileRoom=-1;
  $server = "localhost";
  $mysqlUser = "root";
  $mysqlPass = "";
  $conn = mysql_connect($server, $mysqlUser, $mysqlPass);
  mysql_select_db("pluto_main", $conn);
  $dupe;

  if($conn)
  {

$installSql = "SELECT * FROM `Installation` ";
$iRes=mysql_query($installSql, $conn) or die(mysql_error($conn));
$inst="";
while($row=mysql_fetch_array($iRes)){
$inst=$row['PK_Installation'];
//print_r($row);
}

$installation=$inst;
echo "Installation is ".$installation."<br>";
if($installation==""){
return;
}
      echo "Connection Found, Starting<br>";

	$roomSql = "SELECT * FROM `Room` WHERE `Description` like 'Mobile QOrbiters' LIMIT 0, 30 ";
	echo "Checking for existing mobile orbiter room <br>";
	$result = mysql_query($roomSql,  $conn) or die (mysql_error($conn));
	$cnt = mysql_num_rows($result);
	
	if($cnt===0){

	$iRoomSql ="INSERT INTO `pluto_main`.`Room` (`PK_Room`, `FK_Installation`, `FK_RoomType`, `Description`, `FK_Icon`, `ManuallyConfigureEA`, `HideFromOrbiter`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, ".$installation.", '13', 'Mobile QOrbiters', NULL, '0', '0', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
	$result2=mysql_query($iRoomSql,  $conn) or die (mysql_error($conn));

	$lastId = mysql_insert_id($conn);

	$chkSql=$roomSql = "SELECT * FROM 'Room' WHERE PK_Room = '".$lastId."'";

	$result3 = mysql_query($chkSql, $conn) or die(mysql_error($conn));
	
	 while ($row = mysql_fetch_array($result3)){
          if($row['PK_Room']){
          $mobileRoom = $row['PK_Room'];
          }
        }

echo "Not Found, so we've added it. Setting up entertain area now<br>";	

}
else {
   while ($row = mysql_fetch_array($result)){
          if($row['PK_Room']){
          $mobileRoom = $row['PK_Room'];
          }
        }
	echo "Found, Setting up entertain area now <br>";

	}

      $dupe = checkForDupe($conn, $deviceID);
	if($dupe)
	{
	echo "Duplicate, will set to ea for device<br>";
	}
	else
	{
	echo "No Matching Ea found, will set to new EA<br>";
	setEntertainArea($conn, $deviceID, $mobileRoom);
	}

}

  function checkForDupe($connect, $device){
    $status = false;
    $sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '".$device."' LIMIT 0, 30 ";
    echo "<b>Checking for duplicate Ea for device " . $device. "</b><br>";
    $result = mysql_query($sql, $connect) or die(mysql_error($connect));
    $i= 0;
  while ($row = mysql_fetch_array($result))
	{
	  if($row['Description']){
	   $status = true;
	  }
	}

return $status;
}

function setEntertainArea($connect, $device, $location){
echo "Setting device ".$device." to EA: ".$location."<br>";
$sql = "INSERT INTO `pluto_main`.`EntertainArea` (`PK_EntertainArea`, `FK_Room`, `Only1Stream`, `Description`, `Private`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, ".$location.", '0',".$device." , '0', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
$result = mysql_query($sql, $connect) or die(mysql_error($connect));
$id=mysql_insert_id($conn);
echo $id;
}

?>

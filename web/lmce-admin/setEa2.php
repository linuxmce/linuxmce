  <?php
  /*
  setEa - temp utility to set the ea for a new Qorbiter
  */

  //initialization area
  if (isset($_GET["d"])) {
    $deviceID = $_GET['d'];
   $room = $_GET['r'];

  } else {
    die("Please specify the device ID with d=xxxx");
  }

  if (isset($_GET["r"])) {
     $room = $_GET['r'];
  } else {
    die("Please specify the room ID with r=xxxx");
  }


  $server = "localhost";
  $mysqlUser = "root";
  $mysqlPass = "";
  $conn = mysql_connect($server, $mysqlUser, $mysqlPass);
  mysql_select_db("pluto_main", $conn);
  $dupe;

  if($conn)
  {
      echo "Connection Found, Starting<br>";
      $dupe = checkForDupe($conn, $deviceID);
	if($dupe)
	{
	echo "Duplicate, will set to ea for device<br>";
	}
	else
	{
	echo "No Matching Ea found, will set to new EA<br>";
	setEntertainArea($conn, $deviceID, $room);
	}
  }

  function checkForDupe($connect, $device){
$status = false;
    $sql = "SELECT * FROM `EntertainArea` WHERE `Description` LIKE '".$deviceID."' LIMIT 0, 30 ";
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
$sql = "INSERT INTO `pluto_main`.`EntertainArea` (`PK_EntertainArea`, `FK_Room`, `Only1Stream`, `Description`, `Private`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_mod`, `psc_restrict`) VALUES (NULL, ".$location.", '1',".$device." , '0', NULL, NULL, NULL, NULL, NULL, '0', CURRENT_TIMESTAMP, NULL);";
$result = mysql_query($sql, $connect) or die(mysql_error($connect));
$id=mysql_insert_id($conn);
echo $id;
}



?>
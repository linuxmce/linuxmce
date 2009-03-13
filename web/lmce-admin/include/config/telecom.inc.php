<?
	// pluto Asterisk
	$dbPlutoTelecomType = "MySQL";
	$dbPlutoTelecomUser = "root";
	$dbPlutoTelecomPass = "";
	$dbPlutoTelecomServer = "localhost";
	$dbPlutoTelecomDatabase = "pluto_telecom";

	
  	$telecomADO = &ADONewConnection('mysql');
  	$telecomADO->NConnect($dbPlutoTelecomServer,urlencode($dbPlutoTelecomUser),urlencode($dbPlutoTelecomPass),urlencode($dbPlutoTelecomDatabase)); 
?>

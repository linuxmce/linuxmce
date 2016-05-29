<?php
	// pluto Asterisk
	$dbPlutoTelecomType = "mysqli";
	$dbPlutoTelecomUser = "root";
	$dbPlutoTelecomPass = "";
	$dbPlutoTelecomServer = "localhost";
	$dbPlutoTelecomDatabase = "pluto_telecom";

  	$telecomADO = &ADONewConnection($dbPlutoTelecomType);
  	$telecomADO->NConnect($dbPlutoTelecomServer,urlencode($dbPlutoTelecomUser),urlencode($dbPlutoTelecomPass),urlencode($dbPlutoTelecomDatabase)); 
?>

<?php
	// pluto Asterisk
	$dbEIBType = "mysqli";
	$dbEIBUser = "eib";
	$dbEIBPass = "";
	$dbEIBServer = "localhost";
	$dbEIBDatabase = "eib";

  	//$eibDSN = $dbEIBType.'://'.urlencode('aa'.$dbEIBUser).':'.urlencode($dbEIBPass).'@'.$dbEIBServer.'/'.urlencode($dbEIBDatabase);
  	//$eibADO = &ADONewConnection($eibDSN);

	$eibADO = &ADONewConnection($dbEIBType);
	$eibADO->NConnect($dbEIBServer,urlencode($dbEIBUser),urlencode($dbEIBPass),urlencode($dbEIBDatabase));

  	if(!$eibADO){
  		die('EIB database not available. Please install the EIB package.');
  	}
?>

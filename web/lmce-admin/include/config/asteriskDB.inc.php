<?php
	// pluto Asterisk
	$dbPlutoAsteriskType = "mysqli";
	$dbPlutoAsteriskUser = "root";
	$dbPlutoAsteriskPass = "";
	$dbPlutoAsteriskServer = "localhost";
	$dbPlutoAsteriskDatabase = "asterisk";

	$asteriskADO = &ADONewConnection($dbPlutoAsteriskType);
	$asteriskADO->NConnect($dbPlutoAsteriskServer,urlencode($dbPlutoAsteriskUser),urlencode($dbPlutoAsteriskPass),urlencode($dbPlutoAsteriskDatabase)); 
?>

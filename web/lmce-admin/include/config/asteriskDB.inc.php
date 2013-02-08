<?
	// pluto Asterisk
	$dbPlutoAsteriskType = "MySQL";
	$dbPlutoAsteriskUser = "root";
	$dbPlutoAsteriskPass = "";
	$dbPlutoAsteriskServer = "localhost";
	$dbPlutoAsteriskDatabase = "asterisk";

	

	$asteriskADO = &ADONewConnection('mysql');
	$asteriskADO->NConnect($dbPlutoAsteriskServer,urlencode($dbPlutoAsteriskUser),urlencode($dbPlutoAsteriskPass),urlencode($dbPlutoAsteriskDatabase)); 
?>

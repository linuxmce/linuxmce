<?php

/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com
	GPLv2 Licensed
	TVDB Data Grabber
	TvDB.com data grabber for television shows from the web admin, both for single and multiple files 
	
	checktvdb.php		- 	initialization
	tvdbUtils.php 		-	common include
	tvdbseries.php		-	Episode confirmation page
	tvdbAttributes.php	- 	Displays inserted attributes and screenshot
	
	
 */


include('tvdbUtils.php');
	
$fileID=$_GET['fileID'];
error_reporting(0);
	// tvdb api key 
	
function initialSetup($fileID,$mediadbADO,$dbADO, $output) {

searchSeries($fileID,$mediadbADO,$dbADO, $output) ;
	
}


?>
<?php
/*
	Copyright 2008,2009 - Peer Oliver Schmidt
	GPLv2 Licensed
*/

	error_reporting(E_ALL);
	
//	connectToDCERouter();

	global $possyDeviceFromID;
	global $limit;
	global $currentMediaPlayer;
	$possyDeviceFromID = 1;
//	$possyDeviceFromID = 45; // Proxy Orbiter
//	$limit = " Limit 0,200";
	$limit = "";

	include_once("libMessageSend.php");


	function lightinroom() {
	// Returns an array of light devices in the current room
		global $currentRoom, $link;
		$deviceCategoryLights = 73;
		$lights = getDeviceForDeviceCategoryInRoom($link, $deviceCategoryLights);		
		return $lights;
	}
	
	function getMediaStatus($room) {
	// Checks the media players (Xine and MPlayer) if they have anything playing right now
		global $currentMediaPlayer;
		$remote = "";
		$status ="";
		$ipAddress = getMDIP($room);
		if ($status == "") { $status = XineMplayerStatus($ipAddress,12000); }
		if ($status == "") { $status = XineMplayerStatus($ipAddress,12010); }
		if (pvrInstalled() == "VDR") {
			if ($status == "") { $status = vdrStatus($ipAddress);  }
		} else if (pvrInstalled() == "MythTV") {
			if ($status == "") { $status = mythTVStatus($ipAddress);  }
		}
						

		if ($status <> "") {
			$remote = "remote.php?type=$currentMediaPlayer";
			$status = "<a href='$remote' title='$status'>$status</a>";
		}
// $status .= "IP $ipAddress - Room $room<br>";
		return $status;		
	}
	
	function pvrInstalled() {
		// Returns the installed PVR based on the installed plugin
		// 36 for MythTV Plugin
		// 1704 for VDR Plugin
		// If an id ten tea installs both, VDR wins :P
		global $link;
		$query = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = 36;";
		if (getMyValue($link,$query) != "") {
			$pvrInstalled = "MythTV";
		}
		$query = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = 1704;";
		if (getMyValue($link,$query) != "") {
			$pvrInstalled = "VDR";
		}
		return $pvrInstalled;
	}
	
	function mythTVStatus($ipAddress) {
		global $currentMediaPlayer;
		$port = 10001;
		$status = "";
		$output = "";
		$exitCode = 0;
		$errorNumber = 0;
		$errorString = "";
		$timeout = 3;
		// We check if a mythfrontend process is running. If it is, we assume, MythTV is running
		if (($ipAddress == "192.168.80.1") or ($ipAddress == "127.0.0.1")) {
			// Checking if mythtv is running on the core
			$retVal = exec("ps ax|grep mythfrontend|grep -v grep",&$output,&$exitCode);
			if ($exitCode == 0) {
				$currentMediaPlayer = "TV";
				$status = "TV";
			}
		} else {
			// Checking a mythfrontend running on an MD, we check for an open port at 10001
			$oldStatus = error_reporting(E_ERROR);
			$fp = fsockopen($ipAddress, $port, &$errorNumber, &$errorString, $timeout);
			error_reporting($oldStatus);
			if ($errorString <> "") {
				return "";
			}
			$currentMediaPlayer = "TV";
			$status="TV";
		}
		return $status;
	}	


	function vdrStatus($ipAddress) {
		global $currentMediaPlayer;
		$port = 2001;
		$status = "";
		$output = "";
		$exitCode = 0;
		$errorNumber = 0;
		$errorString = "";
		$timeout = 3;
		if (($ipAddress == "192.168.80.1") or ($ipAddress == "127.0.0.1")) {
			// Checking a VDR running on the core, we check for a running vdr-sxfe process
			$retVal = exec("ps ax|grep vdr-sxfe|grep -v grep",&$output,&$exitCode);
			if ($exitCode == 0) {
				$currentMediaPlayer = "TV";
				$status = "TV";
			}
		} else {
			// Checking a VDR running on an MD, we check for an open SVDRP connection
			$oldStatus = error_reporting(E_ERROR);
			$fp = fsockopen($ipAddress, $port, &$errorNumber, &$errorString, $timeout);
			error_reporting($oldStatus);
			if ($errorString <> "") {
				return "";
			}
			$currentMediaPlayer = "TV";
			$status="TV";
		}
		return $status;
	}

	function xineMplayerStatus($ipAddress,$port) {
	// Returns a string containing information about the currently playing media in Xine or MPlayer
		$timeout = 5;

		global $link, $mediaLink, $currentMediaPlayer;
		$errorNumber = 0;
		$errorString = "";
		$oldStatus = error_reporting(E_ERROR);
		$fp = fsockopen($ipAddress, $port, &$errorNumber, &$errorString, $timeout);
		error_reporting($oldStatus);
		if ($errorString <> "") {
			return "";
		}
		stream_set_timeout($fp,$timeout);
		$printed = 0;
		if (($fp) and ($printed == 0)){
			while ((!feof($fp)) And ($printed==0)) { //    && (!$info['timed_out'])) {
				if ($errorNumber != 0) {
					print "Error $errorNumber - $errorString";
					break;
				}
				$data = fgets($fp);
				if (strlen($data) > 4) {
					$tabelle = explode(",",$data);
					$attribute = "";
					$attributeSQL="SELECT Name FROM pluto_media.Attribute Join pluto_media.File_Attribute ON FK_Attribute = PK_Attribute";
					$attributeSQL .= " WHERE FK_File = " . $tabelle[7];
					$attributeSQL .= " AND FK_AttributeType = 13"; // Title
					$attribute = GetMyValue($link,$attributeSQL);
					if ($tabelle[0] == "0") {
						$status='Pausing ';
					} else {
						$status='Playing ';
					}
					if ($attribute == "") {
						$status .= $tabelle[8];
					} else {
						$status .= $attribute;
					}
					// print "<br>Geschwindigkeit: " . $tabelle[0];
					// print "<br>Position: " . $tabelle[1];
					//print "<br>Gesamt: " . $tabelle[2];
					// print "<br>unbekannt: " . $tabelle[3];
					//print "<br>unbekannt: " . $tabelle[4];
					// print "<br>unbekannt: " . $tabelle[5];
					// print "<br>unbekannt: " . $tabelle[6];
					// print "<br>PK_File: " . $tabelle[7];
					// print "<br>Path: " . $tabelle[8];
					//      if ($tabelle[7]+1 > 1) {
					//        print "<br>Wir haben jetzt eine PK_File Info";
					//      }                      
					// print fgets($fp);
					//      ob_flush();
					fclose($fp);
					if ($port == 12000) {
						$currentMediaPlayer = "AV-Xine";	
					} elseif ($port == 12001) {
						$currentMediaPlayer = "AV-Mplayer";
					} else {
						$currentMediaPlayer = "AV-unknown";
					}
					return "$status";
				}
				$printed = 1;
				fclose($fp);
				return "";
			}
		} else {
			return "";
		}
	}


	function getMD($room = 0) {
		// Return the device id of the MD in the current room
		global $currentRoom,$link;
		if ($room == 0) {
			$room = $currentRoom;
		}
		$MD = getDeviceForTemplateInRoom(7);
		if ($MD == "") {
			$MD = getDeviceForTemplateInRoom(28);
		}
//		$queryMD="SELECT PK_Device FROM Device Where FK_Room = $room AND FK_DeviceTemplate in (7,28);";
//		$MD = getMyValue($link,$queryMD);
		return $MD;
	}
	
	function getMDIP($room = 0) {
		// Return the IP address of the MD in the current room
		global $currentRoom,$link;
		if ($room == 0) {
			$room = $currentRoom;
		}
		$queryMDIP="SELECT IPaddress FROM Device Where FK_Room = $room AND FK_DeviceTemplate in (7,28);";
		$MDIP = getMyValue($link,$queryMDIP);
		return $MDIP;
	}
	
	function getCurrentMediaDevice($room = 0) {
		// Return the media device PK_Device of the currently playing device.
		global $currentMediaPlayer, $link;
		$listOfDevices=getMediaDevices($room);
		if ($currentMediaPlayer == "AV-Xine") {
			$currentMediaDevice = getDeviceForTemplateInRoom($link,5); // Get Xine Player Device
		} elseif ($currentMediaPlayer == "AV-Mplayer") {
			$currentMediaDevice = getDeviceForTemplateInRoom($link,1901); // Get Mplayer Device
		} elseif ($currentMediaPlayer == "AV-unknown") {
			$currentMediaDevice = getDeviceForTemplateInRoom($link,58);  // We assume a SqueezeBox
		} elseif ($currentMediaPlayer == "TV") {
			if (pvrInstalled() == "VDR") {
				$currentMediaDevice = getDeviceForTemplateInRoom($link,1705);  // We assume a VDR device
			} else if (pvrInstalled() == "MythTV") {
				$currentMediaDevice = getDeviceForTemplateInRoom($link,35); // The MythTV player device
			}
		} else {
			$currentMediaDevice = getDeviceForTemplate($link,2);
		}
		return $currentMediaDevice;
	}

	function getMediaDevices($room = 0) {
		// Return a list of Media playback devices in the current room
		// At the moment we support
		// 5 = Xine Player
		// 1901 = MPlayer
		// 58 = SqueezeBox
		// 1705 = VDR
		
		global $currentRoom,$link;
		if ($room == 0) {
			$room = $currentRoom;
		}
		$queryMediaDevices="SELECT PK_Device,FK_Device_Template FROM Device Where FK_Room = $room AND FK_DeviceTemplate in (5,1901,58,1705);";
		$mediaDevices = getMyArray($link,$queryMediaDevices);
		return $mediaDevices;
	}					

	function sendPlayerCommand($room, $command, $parameter=0) {
		global $possyDeviceFromID;
		global $currentMediaPlayer;
		
		// The media plugin needs to receive its messages from a device in the current room.
		$possyDeviceFromID = getCurrentMD();
		// $status = getMediaStatus($room);		
		$socket = commStart("dcerouter",3450,$possyDeviceFromID);
		// myMessageSend($socket,$deviceFromID,$deviceToID (10 == media plugin),$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID,$parameter2Content);		
		$file = fopen("/tmp/test.txt","w");
		fwrite($file,"start\n");
		// $destinationDevice = getCurrentMediaDevice($room);

		$destinationDevice = getCurrentMediaDevice($room); // getDeviceForTemplate(2);
		fwrite($file, "from device: $possyDeviceFromID\n");
		fwrite($file, "destination device: $destinationDevice\n");
		fwrite($file,"room: $room currentmp: $currentMediaPlayer myMessageSend($socket,$possyDeviceFromID,x-$destinationDevice-x,1,$command,". implode($parameter).");");
		fclose($file);
//		myMessageSend($socket,$possyDeviceFromID,getCurrentMediaDevice($room),1,$command); // ,13,'"' . $filePath . '"', 45, $currentEntertainArea);
		myMessageSend($socket,$possyDeviceFromID,$destinationDevice,1,$command,$parameter); // ,13,'"' . $filePath . '"', 45, $currentEntertainArea);
		commEnd($socket);
	}

	function sendCommand($destination, $command, $parameter=0) {
		global $link;
		// We send the messages from the MD/Hybrid device, instead of our usual
		// device. Let's see, if lighting etc works.
		$deviceFromID = getMD();
		        
		$socket = commStart("dcerouter",3450,$deviceFromID);
		if (! is_array($parameter)) {
			myMessageSend($socket,$deviceFromID,getDestinationDevice($link,$destination),1,$command); // ,13,'"' . $filePath . '"', 45, $currentEntertainArea);
		} else { 
			myMessageSend($socket,$deviceFromID,getDestinationDevice($link,$destination),1,$command,$parameter);		
		}
		commEnd($socket);
	}

	
	function playFile($mediaLink, $pk_file) {
		global $currentRoom,$link,$currentEntertainArea,$possyDeviceFromID;
/*		print "entertain: $currentEntertainArea\n";
		print "current room: $currentRoom\n";*/
		$currentEntertainArea = getEntertainArea($link,$currentRoom);
		// Get the path of the file to play, and call messagesend
		// to play it in the current entertainment area
		$queryFilePath = "SELECT Concat(Path,'/',Filename) From File Where PK_File = " . $pk_file . ";";
		
		$filePath = getMyValue($mediaLink, $queryFilePath);
/*		print "<li>$filePath</li>"; */ 
		// commStart($server, $port, $deviceIDFrom)
		$socket = commStart("dcerouter",3450,$possyDeviceFromID);
		// myMessageSend($socket,$deviceFromID,$deviceToID (10 == media plugin),$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID,$parameter2Content);		
		myMessageSend($socket,$possyDeviceFromID,10,1,43,13,'"' . $filePath . '"', 45, $currentEntertainArea);
		commEnd($socket);
	}	
	
	function getEntertainArea($link, $room) {
		$query = "SELECT PK_EntertainArea From EntertainArea Where FK_Room = $room";
		$return = getMyValue($link,$query);
		return $return;
	}
	

	function DoParameter29($mediaLink, $parameterValue, $commandGroup) {
		// DoParameter29 is called, when ever a file listing needs to
		// be presented.
		if ($commandGroup == 77) {
			buildFileList($mediaLink, $parameterValue);
		} elseif ($commandGroup == 41 or $commandGroup == 43 or $commandGroup == 44) // Playlist, Pictures and Docs
		{	
			buildFileList($mediaLink, $parameterValue);
		} elseif ($commandGroup == 39 or $commandGroup = 40) // Audio and Video
		{			
	//		buildMediaSubTypes($mediaLink, $parameterValue);
			buildSortByList($mediaLink, $parameterValue);
	//		buildFileList($mediaLink, $parameterValue);
		} else
		{
			die("<li>No file listing for commandGroup $commandGroup setup</li>");
		}
	}

	function datagrid($datagrid) {
		if ($datagrid == 61) {
			$array = getRecentCalls();
		}
	}

	function getPicture($mediaLink, $PK_File) {
		// Get a link to the picture associated with the supplied
		// file.	
		$path = "/lmce-admin/mediapics/";
		if ( ! file_exists("/var/www/" . $path) ) {
			$path = "/pluto-admin/mediapics/";
		}
		$fileName = "";
		$summary = "";
		$query = "SELECT FK_Picture From Picture_File Where FK_File = $PK_File";
		$fileName = getMyValue($mediaLink,$query);
		// Next get the summary of the supplied file
		$query = "SELECT Text From LongAttribute WHERE FK_AttributeType = 37 AND FK_File = $PK_File";
		$summary = getMyValue($mediaLink,$query);
		if ($fileName != "") {
			print "<li>"; // style='height: 320px;'>";
			print "<img style='float:left;' src='$path" . $fileName . "_tn.jpg' height='320px'>";			
			print "<p>$summary</p>";
			print "</li>\n";
			print "<li style='clear:left;'></li>\n";
		} else if ($summary != "") {
			print "<li>$summary";
			print "</li>\n";
		}
	}

	function orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea, $PK_UI = NULL, $isParent = false) {
		// Get the description for the current variation, and the variation designobj containing all other
		// objects of the page.
		$mainDesignObjVariation = NULL;
		$title = NULL;
		if (Is_Null($PK_UI)) {
			$UI = "FK_UI IS NULL";
			$header = "Standard Variation";
		} else {
			$UI = "FK_UI = $PK_UI";
			$header = getMyValue($link,"Select Description From UI Where PK_UI = $PK_UI");
		}
		// print "<h1>$header</h1>";
		$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation Where FK_DesignObj = $mainDesignObj AND $UI";
		if (!is_null($mainDesignObj) and !is_null($UI)) {
			$mainDesignObjVariation = getMyValue($link,$query);
		}
		//print "<pre>$UI mainDesignObjVariation $mainDesignObjVariation</pre>\n";
		//print "<pre>mainDesignObj $mainDesignObj</pre>\n";
		// If we can't find the main DesignObjVariation, we use the supplied designobj direct.
		if (is_null($mainDesignObjVariation) or $isParent) {
			print "<pre>Setting variation = $mainDesignObj</pre>";
			$mainDesignObjVariation = $mainDesignObj;
		}
		// Try to get a title for the current screen
		$query = "SELECT Screen.Description From Screen Where Screen.PK_Screen = (Select FK_Screen From Screen_DesignObj Where FK_DesignObj = $mainDesignObj);";
		//print "<pre>$query</pre>";
		if (!is_null($mainDesignObj)) {
			$title = getMyValue($link,$query);
		} 
		print "<ul id='$title'>\n";
		// Each of the objects on the main menu needs to be fetched.
		$query = "SELECT FK_DesignObj_Child FROM DesignObjVariation_DesignObj D Where FK_DesignObjVariation_Parent = $mainDesignObjVariation";
		//print "<pre>$query</pre>";
		if (!is_null($mainDesignObjVariation)) {
			$array = getMyArray($link,$query);
			// print_r($array);
			// Get all screens objects for the current variation.
			orbiterTakeCareOfObjects($link, $array, $currentRoom, $currentEntertainArea,$UI);
		}
		print "</ul>\n";
	}
	
	function doPower() {
                global $currentScreen,$currentEntertainArea, $currentUser, $currentRoom;                
                $url = "design.php?currentRoom=$currentRoom&currentUser=$currentUser&CommandGroup_D";                                 
		print "<ul id='Power'>\n";
		print "<li><a href='" . $url . "=7717'>Display On</a></li>\n";
		print "<li><a href='" . $url . "=9803'>Power On</a></li>\n";
		print "<li><a href='" . $url . "=7718'>Media Off</a></li>\n";
		print "<li><a href='" . $url . "=7725'>Display Off</a></li>\n";
		print "<li><a href='" . $url . "=7721'>Power Off</a></li>\n";
	} 

	function doDesignObjVariations($designObjVariation, $link) {
		global $currentScreen,$currentEntertainArea, $currentUser, $currentRoom;
		$url = "design.php?currentRoom=$currentRoom&currentUser=$currentUser&CommandGroup";
		$target = "";  // Set to self to start over. -> For example after room and user selection.
		// How do I get to the CommandGroup_Room containing the list of lights?
		$query = "SELECT Value FROM DesignObjVariation_DesignObjParameter ";
		$query .= "Where FK_DesignObjVariation = $designObjVariation AND FK_DesignObjParameter = 11";
		$PK_Array = getMyValue($link,$query);
		// $PK_Array = 1;
		// print "<ul><li>PK Array $PK_Array</li>\n";
		if ($designObjVariation == 3332) {
			doPower();
		} elseif (! Isset($PK_Array)) {
			
			print "<li><a href='$designObjVariation'>DesignObjVariation $designObjVariation</a></li>\n";
			orbiterDoVariation($link, $designObjVariation,$currentRoom,$currentEntertainArea,$isParent = true);
			// print "<ul id='Power'>\n";
			// print "<li>not yet</li>\n";
		} else {
			$query = "SELECT Description FROM Array Where PK_Array = $PK_Array";
			$buttonDescription = getMyValue($link,$query);
			$buttonDescription = preg_replace("/ /","-",$buttonDescription);
			print "<ul id='$buttonDescription'>\n";
			// $buttonDescription = "Lighting Scenario";
			If (in_array($PK_Array,array(1,2,3,4))) {
				$query = "SELECT PK_CommandGroup, Description FROM CommandGroup ";
				$query .= " JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup = PK_CommandGroup ";
				$query .= " WHERE FK_Array = $PK_Array AND FK_Room = $currentRoom";
			} elseif ($PK_Array == 5) {
				// Media Scenarios are not room based, but EntertainmentArea based
				$query = "SELECT PK_CommandGroup, Description FROM CommandGroup ";
				$query .= "JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup = PK_CommandGroup ";
				$query .= "WHERE FK_Array = $PK_Array AND CommandGroup_EntertainArea.FK_EntertainArea = $currentEntertainArea ";
				$query .= "ORDER BY Sort";
			} elseif ($PK_Array == 13) {
				// List of users that are not hidden from the Orbiter
				$target = " target='_self'";
				$url = "setEnvironment.php?setCurrentUser";
				$query = "SELECT PK_Users, UserName FROM Users ";
				$query .= "Where HideFromOrbiter = 0";
			} elseif ($PK_Array == 14) {
				// List of rooms that are not hidden from the Orbiter
				$target = " target='_self'";
				$url = "setEnvironment.php?setCurrentRoom";
				$query = "SELECT PK_Room, Description FROM Room ";
				$query .= "Where HideFromOrbiter = 0";
			} elseif ($PK_Array == 10) {
				// List of Media Directors
				$query = "SELECT PK_Room, concat('Reset ',Description) As Description From Room ";
				$query .= "Where HideFromOrbiter = 0";
		
			} else {
				die("<p>Unknown PK_Array $PK_Array</p>\n");
			}
			$buttonArray = getMyArray($link,$query);
			foreach($buttonArray as $buttonDetail) {
				print "<li><a href='$url=$buttonDetail[0]' $target>$buttonDetail[1]</a></li>\n";
			}
		}
		print "</ul>";
	}

	function orbiterTakeCareOfObjects($link, $array, $currentRoom, $currentEntertainArea,$UI) {
		// This function takes all the objects in the array
		// and displays interesting stuff about it. Creates links
		// shows headings, etc.
		global $currentUser;
		foreach($array as $arrayEntry) {
			$designObject = $arrayEntry[0];
			$query = "SELECT FK_DesignObjType FROM DesignObj Where PK_DesignObj = $designObject";
			$designObjType = getMyValue($link,$query);
			print "<p>DesignObjType = $designObjType</p>";
			If ($designObjType == 3) {
				// Array
				$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation WHERE FK_DesignObj = $designObject AND $UI";
				$designObjVariation = getMyValue($link,$query);
				if (! is_null($designObjVariation)) {
					doDesignObjVariations($designObjVariation,$link);
				}
				// If we are working on a specific UI, we need to include the Standard variation as well
				if (is_null($designObjVariation) or $UI != "FK_UI IS NULL") {
					$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation WHERE FK_DesignObj = $designObject AND FK_UI IS NULL";
					$designObjVariation = getMyValue($link,$query);
					doDesignObjVariations($designObjVariation,$link);
				}
			} else {
				// Other design types (than Array)
				$query = "SELECT Description FROM DesignObj WHERE PK_DesignObj = $designObject";
				$description = getMyValue($link,$query);
				$query = "SELECT FK_DesignObjCategory FROM DesignObj WHERE PK_DesignObj = $designObject";
				$designObjCategory = getMyValue($link,$query);
				// Get the DesignObjVariation for the current UI
				$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation WHERE FK_DesignObj = $designObject AND $UI";
				$designObjVariation = getMyValue($link,$query);

				// If there is not DesignObjVariation for the currentUI, use the variation w/o a FK_UI
				if ($UI != "FK_UI IS NULL") {
					$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation WHERE FK_DesignObj = $designObject AND FK_UI Is NULL";
					$designObjVariation = getMyValue($link,$query);
				} 
				print "<h3>$designObjType - $designObject - $description</h3>";
				print "<h4>DesignObjCategory $designObjCategory</h4>\n";
				print "<p>DesignObjVariation $designObjVariation</p>";
				// Get the commandgroup that gets executed, when someone presses a button or selects an object in a datagrid.
				$query = "SELECT FK_CommandGroup_D_OnActivate FROM DesignObjVariation D Where PK_DesignObjVariation = $designObjVariation";
				$commandGroup_D_OnActivate = getMyValue($link,$query);
				if (is_null($commandGroup_D_OnActivate) or $commandGroup_D_OnActivate == "" or $commandGroup_D_OnActivate == 0) {
//					$query = "SELECT FK_CommandGroup
				}
				print "<a href='design.php?currentUser=$currentUser&currentRoom=$currentRoom&CommandGroup_D=$commandGroup_D_OnActivate'>$commandGroup_D_OnActivate</a>\n";
			} 
			print "</p><hr></hr>\n";
		}
	

	}

	function listMyArray($link, $query, $label, $groupByField=-1, $addAnEntry=-1, $refURL = '',$withUL = True) {
		// listMyArrays displays the contents of a supplied array in a way, the UI can be interpreted.
		// $link is the mysql link
		// $query contains the query to be executed
		// $label contains the prefix for ID to be used
		// $groupByField can be set to a field which to group by all the links
		// $addAnEntry can contain an array, allowing additional entries be stored into the linked list
		// $refURL contains the URL to be executed with the IDs of the MySQL array
		// $withUL can be set to false to disable the printing of the UL tag, for example because other
		//	stuff had already set the UL tag.
		global $currentRoom, $currentUser;
		$array = getMyArray($link, $query);
		$oldGroup = "";

		if ($withUL) {
			print "<ul>\n";
		}
		if ($addAnEntry <> -1) {
			print "<li class='group'>Commands</li>\n";
			if (is_array($addAnEntry[0])) {
				foreach ($addAnEntry as $addAnEntrySingle) {
					print "<li id='$label"." $addAnEntrySingle[0]" . "'><a href='$addAnEntrySingle[2]&currentUser$currentUser&currentRoom=$currentRoom'>$addAnEntrySingle[1]</a></li>\n";
				}
			}
			print "<li id='$label"." $addAnEntry[0]" . "'><a href='$addAnEntry[2]'>$addAnEntry[1]</a></li>\n";
		}
		foreach($array as $member) {
			if ($groupByField >= 0) {
				if ($oldGroup <> $member[$groupByField]) {
					print "<li class='group'>$member[$groupByField]</li>";
					$oldGroup = $member[$groupByField];
				}
			}
			if ($refURL == '') {
				$ref = $member[2];
			} else {
				$ref = $refURL . "$label=$member[0]";
			}
			print "<li id='$label"."_$member[0]" . "'><a href='$ref'>$member[1]</a></li>\n";
		}
		if ($withUL) {
			print "</ul>\n";
		}
	}
	
	function doGenres($mediaLink, $pk_mediatype = 5) {
		// Display a UL of genres for the supplied mediatype
		$query = "Select Distinct Attribute.Name, Attribute.Name From File_Attribute Join File On File.PK_File = File_Attribute.FK_File";
		$query .= " Join Attribute On File_Attribute.FK_Attribute = Attribute.PK_Attribute";
		$query .= " WHERE File.EK_MediaType = $pk_mediatype AND Attribute.FK_AttributeType = 8";
		$query .= " ORDER BY Name";
		listMyArray($mediaLink, $query, "Genre");
	}

	function buildSpeedDialList($link) {
		// Get a list of speed dial scenarios for the current room
		global $currentRoom, $currentUser;
		$query = "SELECT CommandGroup.Description,FK_CommandParameter,IK_CommandParameter FROM CommandGroup";
		$query .= " JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup";
		$query .= " JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command";
		$query .= " JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup";
		$query .= " WHERE AutoGeneratedDate=0";
		$query .= " AND FK_Array=4";   // ARRAY_Communication_Scenarios_CONST
		$query .= " AND FK_Room=" . $currentRoom;
		$query .= " ORDER BY PK_CommandGroup,FK_CommandParameter";
		listMyArray($link,$query,"Speed Dial",-1,-1,"speeddial.php?currentUser=$currentUser&currentRoom=$currentRoom&");
	}

	function buildVoiceMailList() {
		// Scan the voicemail dir of the current user, and return list of new and saved msgs.
		global $currentUser, $currentRoom;		
		global $link;
		$extension = getMyValue($link,"SELECT Extension From Users WHERE PK_Users = $currentUser");
		$VOICEMAIL_EVENT = "/usr/pluto/bin/SendVoiceMailEvent.sh";
		$VOICEMAIL_URL_PARAM = "/usr/pluto/bin/Voicemail_URL_Parm.php";
		$VOICEMAIL_URL = "/recordings/misc/audio.php?recording=";
		$voicemailUnread = "/var/spool/asterisk/voicemail/default/$extension";
		$voicemailSaved = $voicemailUnread . "/Old";
	}

	function buildRecentCallList($link) {
		global $currentUser, $currentRoom;
		// Get a list of recent calls - We don't distinguish between users.
		$query = "SELECT src, dst, calldate, billsec, channel FROM asteriskcdrdb.cdr";
		$query .= " ORDER BY calldate DESC LIMIT 0,20";
		listMyArray($link,$query,"Recent Calls",-1,-1,"call.php?currentUser=$currentUser&currentRoom=$currentRoom&");
	}

	function buildSortByList($mediaLink, $pk_mediatype) {
		global $currentUser, $currentRoom;
		// Get a list of Attributes to Sort by
		$query = "SELECT Distinct PK_AttributeType, Description FROM MediaType_AttributeType ";
		$query .= "JOIN AttributeType ON PK_AttributeType = FK_AttributeType ";
		$query .= "where MediaSortOption is not null ";
		if ($pk_mediatype == 5) {
			$query .= "and EK_MediaType in (3,5) ";
		} else {
			$query .= "and EK_MediaType = $pk_mediatype ";
		}
		listMyArray($mediaLink,$query,"SortedBy",-1,-1,"findmore.php?currentUser=$currentUser&currentRoom=$currentRoom&mediaType=$pk_mediatype&");
	}

	function buildMediaSubTypes($mediaLink, $pk_mediatype = 5) {
		global $currentUser, $currentRoom;
		// Display a list of available media subtypes for the
		// provided mediatype.
		$query = "SELECT PK_MediaSubType, Description FROM MediaSubType ";
		$query .= "JOIN MediaType_MediaSubType ON FK_MediaSubType = PK_MediaSubType ";
		if ($pk_mediatype == 5) {
			$query .= "AND EK_MediaType IN (3,5)";
		} else {
			$query .= "AND EK_MediaType = $pk_mediatype ";
		}
		listMyArray($mediaLink, $query, "Subtype",-1,-1,"findmore.php?currentUser=$currentUser&currentRoom=$currentRoom&");
	}

	
	function buildFileList($mediaLink, $pk_mediatype, $commandGroup = -1) {
		global $currentUser, $currentRoom,$limit;
		// Build a list of files that have the supplied media type. For Video we add MediaType 3 to it.
		// the query should return
		// 1) The ID
		// 2) The name to display
		// 3) The URL to call excluding the ID 
		// 4) The Group-By string.
		if ($pk_mediatype == 5) // Video -> sort by filename		
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType in (3,5) AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= $limit;
		} elseif ($pk_mediatype == 4) // Audio -> Sort by artist
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType = $pk_mediatype AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= $limit;
		} elseif ($pk_mediatype == 7) // Audio -> Sort by artist
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType = $pk_mediatype AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= $limit;
		} elseif ($pk_mediatype == 21) // Playlist
		{	
			$query = "SELECT PK_Playlist,Name as Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_Playlist=',PK_Playlist) as Target,Substr(Name,1,1) ";
			$query .= "FROM pluto_media.Playlist ";
			$query .= "WHERE EK_User IS NULL Or EK_User = 0 ";
			$query .= "ORDER By Name ";
			$query .= $limit;
		}
		else
		{
			$query = "SELECT 0,'Media Type $pk_mediatype is currently unsupported' as Filename, '' As Target, ' ' As Heading";
		}
		listMyArray($mediaLink,$query,"File",3);
	}

	function buildFileListFromSortedBy($mediaLink, $pk_sortedby,$pk_media = 5) {
		global $currentUser, $currentRoom, $limit;
		// Build a list of files that have the supplied attribute.
		// Get all attributes belonging to a specific media file.
		$query = "SELECT Distinct PK_Attribute, Attribute.Name, AttributeType.Description FROM Attribute ";
		$query .= "JOIN File_Attribute ON FK_Attribute = PK_Attribute ";
		$query .= "JOIN AttributeType ON PK_AttributeType = FK_AttributeType ";
		$query .= "JOIN File ON PK_File = FK_File ";
		$query .= "WHERE FK_AttributeType = $pk_sortedby And Name <> \"*\" And Name <> \"-\" ";
		$query .= "And IsDirectory = 0 And Missing = 0 AND EK_Users_Private Is Null ";
		if ($pk_media == 5 or $pk_media == 3) {
			$query .= "AND EK_MediaType in (3,5) ";
		} else 
		{
			$query .= "AND EK_MediaType = $pk_media ";
		}
		$query .= "Order By Attribute.Name ";
		$query .= $limit;
		
		
		listMyArray($mediaLink,$query,"Details",2,array($pk_sortedby,"Play All","play-all.php"),"findmore.php?currentUser=$currentUser&currentRoom=$currentRoom&mediaType=$pk_media&");
	}

	function buildFileListFromAttribute($mediaLink, $pk_attribute) {
		global $currentUser, $currentRoom, $limit;
		// Build a list of files that have the supplied attribute.
		$query = "SELECT Distinct PK_File,Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
		$query .= "FROM pluto_media.File ";
		$query .= "JOIN pluto_media.File_Attribute ON PK_File = FK_File ";
		$query .= "WHERE EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 And FK_Attribute = $pk_attribute ";
		$query .= "ORDER By Filename ";
		$query .= $limit;
		listMyArray($mediaLink,$query,"File",3,array($pk_attribute,"Play All","play-all.php?where=FK_Attribute&content=$pk_attribute"));
	}

	function buildFileListFromSubtype($mediaLink, $pk_mediatype) {
		global $currentUser, $currentRoom, $limit;
		// Build a list of files that have the supplied attribute.
		$query = "SELECT PK_File,Filename,concat('detail.php?currentUser=$currentUser&currentRoom=$currentRoom&PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
		$query .= "FROM pluto_media.File ";
		$query .= "JOIN pluto_media.File_Attribute ON PK_File = FK_File ";
		$query .= "WHERE EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 And FK_Attribute = $pk_attribute ";
		$query .= "ORDER By Filename ";
		$query .= $limit;
		listMyArray($mediaLink,$query,"File",3);
	}

	function buildAttributeList($mediaLink, $pk_file, $withUL = True) {
		// Get all attributes belonging to a specific media file.
		global $currentUser, $currentRoom;
		$query = "SELECT Distinct PK_Attribute, Attribute.Name, AttributeType.Description FROM Attribute ";
		$query .= "JOIN File_Attribute ON FK_Attribute = PK_Attribute ";
		$query .= "JOIN AttributeType ON PK_AttributeType = FK_AttributeType ";
		$query .= "WHERE File_Attribute.FK_File = $pk_file And Name <> \"*\" ";
		$query .= "Order By (Description != \"Title\"),(Description != \"Performer\"), Description";
	//	print "Query: $query";
		
		listMyArray($mediaLink,$query,"Details",2,array($pk_file,"Play","play.php?currentUser=$currentUser&currentRoom=$currentRoom&pk_file=$pk_file"),"findmore.php?currentUser=$currentUser&currentRoom=$currentRoom&",$withUL = $withUL);
	}
	
	function getMyValue($link,$query) {
		$returnValue = array(1);
		// Expects a query which returns a single field, and returns such field as a single variable
		if (!($result = mysql_query($query,$link))) {
			print "<p>Query without result: " . $query . "</p>\n";
		} else {
			$returnValue = mysql_fetch_array($result);
		}
		return $returnValue[0];
	}

	function getMyArray($link,$query) {
		// Returns the result set as an array
		$returnValue = array();
		if (!($result = mysql_query($query,$link))) {
			print "<p>Query without result: $query</p>\n";
		}
		while ($row = mysql_fetch_array($result)) {
			array_push($returnValue,$row);
		}
		return $returnValue;
	}


	function doCommandGroup($link,$commandGroup) {
		global $current, $mediaLink, $possyDeviceFromID;
		$messageType = 1;   // 1 = COMMAND, 2 = EVENT
		
		$commandGroupDescription = getMyValue($link,"SELECT Description FROM CommandGroup WHERE PK_CommandGroup = $commandGroup");
		// print "<h1>CommandGroup $commandGroup - $commandGroupDescription</h1>\n";
		$query = "SELECT PK_CommandGroup_Command,FK_Command, FK_Device FROM CommandGroup_Command Where FK_CommandGroup = $commandGroup Order By OrderNum";
		// print "<p>query: $query</p>\n";
//		print "<ul>$commandGroupDescription</ul>\n";
		$commands = getMyArray($link,$query);
		$socket = commStart("dcerouter",3450,$possyDeviceFromID);
		
		foreach($commands as $commandDetail) {
			$messageToSend = "";
			$destinationDeviceDescription = "";
			$deviceToID = getDestinationDevice($link,$commandDetail[2]);
			$messageID = $commandDetail[1];
			
			$commandDescription = getMyValue($link,"SELECT Description FROM Command Where PK_Command = $messageID");
//			print "<h2>Destination: $deviceToID - $destinationDeviceDescription</h2>\n";
//			print "<h3>Command: $messageID - $commandDescription</h3>\n";

			$commandGroupCommand = $commandDetail[0];
			$parameters = getMyArray($link,"SELECT FK_CommandParameter, IK_CommandParameter FROM CommandGroup_Command_CommandParameter C Where FK_CommandGroup_Command = $commandGroupCommand");
			$messageToSend = "$deviceToID $messageType $messageID";

			// This loop is needed for commands with special needs, like the file listing for media playback.
			foreach($parameters as $parameter) {
				
				$parameterType = $parameter[0];
				$parameterValue = $parameter[1];
				$parameterDescription = getMyValue($link,"SELECT Description FROM CommandParameter C WHERE PK_CommandParameter = $parameterType");

				$messageToSend = $messageToSend . " $parameterType \"$parameterValue\"";

				if ($messageID == 741) {
					// Goto Screen
					if ($parameterType == 159) {
						// PK_Screen
						$currentScreen = $parameterValue;
					}
				} elseif ($messageID == 401) {
					if ($parameterType == 29) {
						DoParameter29($mediaLink, $parameterValue, $commandGroup);
					}
				} else {
					print "<li>messageID: $messageID</li>\n";
				}
				
	//			print "<p>Parameter: $parameterType - $parameterDescription - $parameterValue</p>\n";

			}
			
			//  41 = TV
			// 43 = Execute Playlist
			// 44 = House to sleep
			// 193 = Light Off
			// 184 = Light On
			// 
			// We send every message for the time being, and no longer limit us
			// to known messages.
//			$sendMessageArray = array(1 => 41, 43, 44, 193, 184);
//			$found = array_search($messageID, $sendMessageArray);
//			if ( ! $found) {
				// Handled elsewhere.
//			} else {
				myMessageSend($socket, $possyDeviceFromID,$deviceToID,1,$messageID, $parameters);
//			}
		}
		commEnd($socket);
		// fclose($socket);
	}
	
function getDestinationDevice($link, $destinationDevice) {
	if( $destinationDevice >= 0) {
		$destinationDeviceDescription = getMyValue($link,"SELECT Description From Device Where PK_Device = $destinationDevice");
	} elseif ($destinationDevice == -100) {
		$destinationDevice = getDeviceForTemplate($link,1); // DCERouterDevice
	} elseif ($destinationDevice == -101) {
		$destinationDevice = getDeviceForTemplate($link,12); // Orbiter Plug-In
	} elseif ($destinationDevice == -102) {
		$destinationDevice = getDeviceForTemplate($link,9); // DataGrid Plug-In
	} elseif ($destinationDevice == -103) {
		$destinationDevice = getDeviceForTemplate($link,27); // GeneralInfoPlugin
	} elseif ($destinationDevice == -104) {
		$destinationDevice = getDeviceForTemplate($link,32); // LightingPlugin
	} elseif ($destinationDevice == -105) {
		$destinationDevice = getDeviceForTemplate($link,31); // ClimatePlugin
	} elseif ($destinationDevice == -106) {
		$destinationDevice = getDeviceForTemplate($link,2); // MediaPlugin
	} elseif ($destinationDevice == -107) {
		$destinationDevice = getDeviceForTemplate($link,34); // TelecomPlugin
	} elseif ($destinationDevice == -108) {
		$destinationDevice = getDeviceForTemplate($link,33); // SecurityPlugin
	} elseif ($destinationDevice == -109) {
		$destinationDevice = getDeviceForTemplate($link,52); // EventPlugin
	} elseif ($destinationDevice == -110) {
		$destinationDevice = getDeviceForTemplate($link,39); // InfraRedPlugin
	} elseif ($destinationDevice == -111) {
		$destinationDevice = getDeviceForTemplate($link,1809); // Plug And Play Interface
	} elseif ($destinationDevice == -150) {
		$destinationDevice = getDeviceForTemplateInRoom($link,26); // Local App Server
	} elseif ($destinationDevice == -151) {
		$destinationDevice = getDeviceForTemplateInRoom($link,28); // Local Media Director
	} elseif ($destinationDevice == -152) {
		$destinationDevice = getDeviceForTemplateInRoom($link,5); // Local Media Player
	} elseif ($destinationDevice == -200) {
		$destinationDevice = getDeviceForTemplateInRoom($link,28); // Media Director
	} elseif ($destinationDevice == -300) {
	// TODO - what is the current Orbiter for the iOrbiter
	//	$destinationDevice = getCurrentOrbiter();
	}
	return $destinationDevice;
}	

function getDeviceForTemplate($link, $deviceTemplate) {
	$query = "SELECT PK_Device FROM Device D Where FK_DeviceTemplate = $deviceTemplate";
	return getMyValue($link,$query);
}	

function getDeviceForTemplateInRoom($link, $deviceTemplate) {
	global $currentRoom;
	$query = "SELECT Distinct PK_Device FROM Device D Where FK_DeviceTemplate = $deviceTemplate and FK_Room = $currentRoom";
	// There can be more than one device for a given template. For example multiple lights
	$value = getMyArray($link,$query);	
	if (count($value) < 2) {
		return $value[0][0];
	} else { 
		return $value;
	}
//	return getMyValue($link,$query);
}	

function getDeviceForDeviceCategory($link, $deviceCategory) {
	$query = "SELECT PK_Device FROM Device D Where FK_DeviceTemplate IN (Select PK_DeviceTemplate FROM DeviceTemplate Where FK_DeviceCategory = $deviceCategory)";
	$value = getMyArray($link,$query);
	$value = getMyArray($link,$query);	
	if (count($value) == 1) {
		return $value[0];
	} else { 
		return $value;
	}
}	

function getDeviceForDeviceCategoryInRoom($link, $deviceCategory) {
	global $currentRoom;
	$query = "SELECT PK_Device FROM Device D Where FK_DeviceTemplate IN (Select PK_DeviceTemplate FROM DeviceTemplate Where FK_DeviceCategory = $deviceCategory) AND FK_Room = $currentRoom";
	$value = getMyArray($link,$query);	
	if (count($value) == 1) {
		return $value[0];
	} else { 
		return $value;
	}
}	
	
function getCurrentMD() {
	global $link;
//	return getMD();
 	return getDeviceForTemplateInRoom($link, 28);
} 

function getCurrentEA() {
	global $link, $currentRoom;
	
	$query = "SELECT PK_EntertainArea From EntertainArea WHERE FK_Room = $currentRoom";
	return getMyValue($link,$query);
} 



	function doCommandGroup_D($link,$commandGroup) {
		// Get all associated commands and their parameters from the supplied commandgroup_D
		// $commandGroupDescription = getMyValue($link,"SELECT Description FROM CommandGroup_D WHERE PK_CommandGroup_D = $commandGroup");
		global $currentScreen, $currentRoom, $possyDeviceFromID;
		$commandGroupDescription = "empty";
		// print "<h1>CommandGroup_D $commandGroup - $commandGroupDescription</h1>\n";
		$query = "SELECT PK_CommandGroup_D" . "_Command,FK_Command, FK_DeviceTemplate FROM CommandGroup_D_Command Where FK_CommandGroup_D = $commandGroup Order By OrderNum";
		// print "<p>query: $query</p>\n";                
		$socket = commStart("dcerouter",3450,$possyDeviceFromID);
		
		$commands = getMyArray($link,$query);
		$commandDetail = array();
		
		foreach($commands as $commandDetail) {
			$destinationDeviceDescription = "";
			
			$destinationDevice = getDestinationDevice($link,$commandDetail[2]);
			
			$command = $commandDetail[1];
			
			$commandDescription = getMyValue($link,"SELECT Description FROM Command Where PK_Command = $command");
			// print "<h2>Destination: $destinationDevice - $destinationDeviceDescription</h2>\n";
			// print "<h3>Command: $command - $commandDescription</h3>\n";

			$commandGroupCommand = $commandDetail[0];
			$parameters = getMyArray($link,"SELECT FK_CommandParameter, IK_CommandParameter FROM CommandGroup_D_Command_CommandParameter C Where FK_CommandGroup_D"."_Command = $commandGroupCommand");
			$messageSendParameter = array();
			foreach($parameters as $parameter) {
				
				$parameterType = $parameter[0];
				$parameterValue = $parameter[1];
				$parameterDescription = getMyValue($link,"SELECT Description FROM CommandParameter C WHERE PK_CommandParameter = $parameterType");

				if ($command == 741) {
					// Goto Screen
					if ($parameterType == 159) {
						// PK_Screen
						$currentScreen = $parameterValue;
					}
				}
				if ($command == 401) {
					if ($parameterType == 29) {
						DoParameter29($mediaLink, $parameterValue,$commandGroup);
					}
				}
				// Maintain replacement variables
				if ($parameterValue == "<%=MD%>") {
					$parameterValue = getCurrentMD();
					$parameter[1] = $parameterValue;
				} else if ($parameterValue == "<%=E%>") {
					$parameterValue = getCurrentEA();
					$parameter[1] = $parameterValue;
				}

				if ($parameterValue != "") {
					array_push($messageSendParameter, array($parameterType, $parameterValue));
				} else {
//					array_push($messageSendParameter, array($parameterType,"0"));
				}
				// print "<p>xyCommand: $command - Parameter: $parameterType - $parameterDescription - $parameterValue</p>\n";

			}
			// print "<p>messageSendParamter: ";
			// print "<p>xDestinationDevice: $destinationDevice - $query </p>";
			myMessageSend($socket, $possyDeviceFromID, $destinationDevice,1,$command,$messageSendParameter);
			// print_r ($messageSendParameter);
		}
		commEnd($socket);
	}
	

function connectdb() {
	global $link, $mediaLink, $currentScreen, $currentEntertainArea, $UI, $SKIN;

	$user = "root";
	$password = "";
	$link = mysql_connect("127.0.0.1",$user,$password);
	mysql_set_charset('utf8',$link); 
	
	mysql_select_db("pluto_main",$link);

	$mediaLink = mysql_connect("127.0.0.1",$user,$password,true);
	mysql_set_charset('utf8',$mediaLink); 
	
	mysql_select_db("pluto_media",$mediaLink);

	$currentRoom = 1;
	$currentEntertainArea = 1;
	$currentScreen = 1;

	$UI = "FK_UI IS NULL";
	$SKIN = "FK_SKIN is NULL";
	// Check if we got called with some command to be executed
}

?>
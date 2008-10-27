<?php
/*
	Copyright 2008 - Peer Oliver Schmidt
	GPLv2 Licensed
*/
	function DoParameter29($mediaLink, $parameterValue, $commandGroup) {
		// DoParameter29 is called, when ever a file listing needs to
		// be presented.
		if ($commandGroup == 41 or $commandGroup == 43 or $commandGroup == 44) // Playlist, Pictures and Docs
		{	
			buildFileList($mediaLink, $parameterValue);
		} elseif ($commandGroup == 39 or $commandGroup = 40) // Audio and Video
		{			
	//		buildMediaSubTypes($mediaLink, $parameterValue);
			buildSortByList($mediaLink, $parameterValue);
	//		buildFileList($mediaLink, $parameterValue);
		} else
		{
			die("<p>No file listing for commandGroup $commandGroup setup</p>");
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
		$fileName = "";
		$summary = "";
		$query = "SELECT FK_Picture From Picture_File Where FK_File = $PK_File";
		$fileName = getMyValue($mediaLink,$query);
		// Next get the summary of the supplied file
		$query = "SELECT Text From LongAttribute WHERE FK_AttributeType = 37 AND FK_File = $PK_File";
		$summary = getMyValue($mediaLink,$query);
		if ($fileName != "") {
			print "<li>"; // style='height: 320px;'>";
			print "<img style='float:left;' src='/pluto-admin/mediapics/" . $fileName . "_tn.jpg' height='320px'>";			
			print "<p>$summary</p>";
			print "</li>\n";
			print "<li style='clear:left;'></li>\n";
		} else if ($summary != "") {
			print "<li>$summary";
			print "</li>\n";
		}
	}

	function orbiterDoVariation($link, $mainDesignObj, $currentRoom, $currentEntertainArea, $PK_UI = NULL) {
		// Get the description for the current variation, and the variation designobj containing all other
		// objects of the page.
		if (Is_Null($PK_UI)) {
			$UI = "FK_UI IS NULL";
			$header = "Standard Variation";
		} else {
			$UI = "FK_UI = $PK_UI";
			$header = getMyValue($link,"Select Description From UI Where PK_UI = $PK_UI");
		}
		print "<h1>$header</h1>";
		$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation Where FK_DesignObj = $mainDesignObj AND $UI";
		$mainDesignObjVariation = getMyValue($link,$query);
		print "<pre>$UI mainDesignObjVariation $mainDesignObjVariation</pre>\n";
		// Each of the objects on the main menu needs to be fetched.
		$query = "SELECT FK_DesignObj_Child FROM DesignObjVariation_DesignObj D Where FK_DesignObjVariation_Parent = $mainDesignObjVariation";
		$array = getMyArray($link,$query);
		// Get all screens objects for the current variation.
		orbiterTakeCareOfObjects($link, $array, $currentRoom, $currentEntertainArea,$UI);
	}

	function doDesignObjVariations($designObjVariation,$link) {
		global $currentRoom, $currentScreen, $currentEntertainArea;
		$url = "design.php?CommandGroup";
		$target = "";  // Set to self to start over. -> For example after room and user selection.
		// How do I get to the CommandGroup_Room containing the list of lights?
		$query = "SELECT Value FROM DesignObjVariation_DesignObjParameter ";
		$query .= "Where FK_DesignObjVariation = $designObjVariation AND FK_DesignObjParameter = 11";
		$PK_Array = getMyValue($link,$query);
		// $PK_Array = 1;
		$query = "SELECT Description FROM Array Where PK_Array = $PK_Array";
		$buttonDescription = getMyValue($link,$query);
		$buttonDescription = ereg_replace(" ","-",$buttonDescription);
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
			$query .= "WHERE FK_Array = $PK_Array AND CommandGroup_EntertainArea.FK_EntertainArea = $currentEntertainArea";
		} elseif ($PK_Array == 13) {
			// List of users that are not hidden from the Orbiter
			$target = " target='_self'";
			$url = "iOrbiter.php?currentUser";
			$query = "SELECT PK_Users, UserName FROM Users ";
			$query .= "Where HideFromOrbiter = 0";
		} elseif ($PK_Array == 14) {
			// List of rooms that are not hidden from the Orbiter
			$target = " target='_self'";
			$url = "iOrbiter.php?currentRoom";
			$query = "SELECT PK_Room, Description FROM Room ";
			$query .= "Where HideFromOrbiter = 0";
		} else {
			die("<p>Unknown PK_Array $PK_Array</p>\n");
		}
		$buttonArray = getMyArray($link,$query);
		foreach($buttonArray as $buttonDetail) {
			print "<li><a href='$url=$buttonDetail[0]' $target>$buttonDetail[1]</a></li>\n";
		}
		print "</ul>";
	}

	function orbiterTakeCareOfObjects($link, $array, $currentRoom, $currentEntertainArea,$UI) {
		// This function takes all the objects in the array
		// and displays interesting stuff about it. Creates links
		// shows headings, etc.
		foreach($array as $arrayEntry) {
			$designObject = $arrayEntry[0];
			$query = "SELECT FK_DesignObjType FROM DesignObj Where PK_DesignObj = $designObject";
			$designObjType = getMyValue($link,$query);
			print "<p>DesignObjType = $designObjType</p>";
			If ($designObjType == 3) {
				// Array
				$query = "SELECT PK_DesignObjVariation FROM DesignObjVariation WHERE FK_DesignObj = $designObject AND $UI";
				$designObjVariation = getMyValue($link,$query);
				doDesignObjVariations($designObjVariation,$link);
				// If we are working on a specific UI, we need to include the Standard variation as well
				if ($UI != "FK_UI IS NULL") {
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
				print "<a href='design.php?CommandGroup_D=$commandGroup_D_OnActivate'>$commandGroup_D_OnActivate</a>\n";
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
		$array = getMyArray($link, $query);
		$oldGroup = "";

		if ($withUL) {
			print "<ul>\n";
		}
		if ($addAnEntry <> -1) {
			print "<li class='group'>Commands</li>\n";
			if (is_array($addAnEntry[0])) {
				foreach ($addAnEntry as $addAnEntrySingle) {
					print "<li id='$label"." $addAnEntrySingle[0]" . "'><a href='$addAnEntrySingle[2]'>$addAnEntrySingle[1]</a></li>\n";
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
		global $currentRoom;
		$query = "SELECT CommandGroup.Description,FK_CommandParameter,IK_CommandParameter FROM CommandGroup";
		$query .= " JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup";
		$query .= " JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command";
		$query .= " JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup";
		$query .= " WHERE AutoGeneratedDate=0";
		$query .= " AND FK_Array=4";   // ARRAY_Communication_Scenarios_CONST
		$query .= " AND FK_Room=" . $currentRoom;
		$query .= " ORDER BY PK_CommandGroup,FK_CommandParameter";
		listMyArray($link,$query,"Speed Dial",-1,-1,"speeddial.php");
	}

	function buildVoiceMailList() {
		// Scan the voicemail dir of the current user, and return list of new and saved msgs.
		global $currentUser;
		global $link;
		$extension = getMyValue($link,"SELECT Extension From Users WHERE PK_Users = $currentUser");
		$VOICEMAIL_EVENT = "/usr/pluto/bin/SendVoiceMailEvent.sh";
		$VOICEMAIL_URL_PARAM = "/usr/pluto/bin/Voicemail_URL_Parm.php";
		$VOICEMAIL_URL = "/recordings/misc/audio.php?recording=";
		$voicemailUnread = "/var/spool/asterisk/voicemail/default/$extension";
		$voicemailSaved = $voicemailUnread . "/Old";
	}

	function buildRecentCallList($link) {
		// Get a list of recent calls - We don't distinguish between users.
		$query = "SELECT src, dst, calldate, billsec, channel FROM asteriskcdrdb.cdr";
		$query .= " ORDER BY calldate DESC LIMIT 0,20";
		listMyArray($link,$query,"Recent Calls",-1,-1,"call.php");
	}

	function buildSortByList($mediaLink, $pk_mediatype) {
		// Get a list of Attributes to Sort by
		$query = "SELECT Distinct PK_AttributeType, Description FROM MediaType_AttributeType ";
		$query .= "JOIN AttributeType ON PK_AttributeType = FK_AttributeType ";
		$query .= "where MediaSortOption is not null ";
		if ($pk_mediatype == 5) {
			$query .= "and EK_MediaType in (3,5) ";
		} else {
			$query .= "and EK_MediaType = $pk_mediatype ";
		}
		listMyArray($mediaLink,$query,"SortedBy",-1,-1,"findmore.php?mediaType=$pk_mediatype&");
	}

	function buildMediaSubTypes($mediaLink, $pk_mediatype = 5) {
		// Display a list of available media subtypes for the
		// provided mediatype.
		$query = "SELECT PK_MediaSubType, Description FROM MediaSubType ";
		$query .= "JOIN MediaType_MediaSubType ON FK_MediaSubType = PK_MediaSubType ";
		if ($pk_mediatype == 5) {
			$query .= "AND EK_MediaType IN (3,5)";
		} else {
			$query .= "AND EK_MediaType = $pk_mediatype ";
		}
		listMyArray($mediaLink, $query, "Subtype",-1,-1,"findmore.php?");
	}

	
	function buildFileList($mediaLink, $pk_mediatype, $commandGroup = -1) {
		// Build a list of files that have the supplied media type. For Video we add MediaType 3 to it.
		if ($pk_mediatype == 5) // Video -> sort by filename
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType in (3,5) AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= "LIMIT 0,200";
		} elseif ($pk_mediatype == 4) // Audio -> Sort by artist
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType = $pk_mediatype AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= "LIMIT 0,200";
		} elseif ($pk_mediatype == 7) // Audio -> Sort by artist
		{
			$query = "SELECT PK_File,Filename,concat('detail.php?PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
			$query .= "FROM pluto_media.File ";
			$query .= "WHERE EK_MediaType = $pk_mediatype AND EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 ";
			$query .= "ORDER By Filename ";
			$query .= "LIMIT 0,200";
		} else
		{
			$query = "SELECT 0,'Media Type $pk_mediatype is currently unsupported' as Filename, '' As Target, ' ' As Heading";
		}
		listMyArray($mediaLink,$query,"File",3);
	}

	function buildFileListFromSortedBy($mediaLink, $pk_sortedby,$pk_media = 5) {
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
		$query .= "LIMIT 0,100";
		
		
		listMyArray($mediaLink,$query,"Details",2,array($pk_sortedby,"Play All","play-all"),"findmore.php?mediaType=$pk_media&");
	}

	function buildFileListFromAttribute($mediaLink, $pk_attribute) {
		// Build a list of files that have the supplied attribute.
		$query = "SELECT Distinct PK_File,Filename,concat('detail.php?PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
		$query .= "FROM pluto_media.File ";
		$query .= "JOIN pluto_media.File_Attribute ON PK_File = FK_File ";
		$query .= "WHERE EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 And FK_Attribute = $pk_attribute ";
		$query .= "ORDER By Filename ";
		$query .= "LIMIT 0,200";
		listMyArray($mediaLink,$query,"File",3);
	}

	function buildFileListFromSubtype($mediaLink, $pk_mediatype) {
		// Build a list of files that have the supplied attribute.
		$query = "SELECT PK_File,Filename,concat('detail.php?PK_File=',PK_File) as Target,Substr(Filename,1,1) ";
		$query .= "FROM pluto_media.File ";
		$query .= "JOIN pluto_media.File_Attribute ON PK_File = FK_File ";
		$query .= "WHERE EK_Users_Private IS NULL And IsDirectory = 0 And Missing = 0 And FK_Attribute = $pk_attribute ";
		$query .= "ORDER By Filename ";
		$query .= "LIMIT 0,200";
		listMyArray($mediaLink,$query,"File",3);
	}

	function buildAttributeList($mediaLink, $pk_file, $withUL = True) {
		// Get all attributes belonging to a specific media file.
		$query = "SELECT PK_Attribute, Attribute.Name, AttributeType.Description FROM Attribute ";
		$query .= "JOIN File_Attribute ON FK_Attribute = PK_Attribute ";
		$query .= "JOIN AttributeType ON PK_AttributeType = FK_AttributeType ";
		$query .= "WHERE File_Attribute.FK_File = $pk_file And Name <> \"*\" ";
		$query .= "Order By (Description != \"Title\"),(Description != \"Performer\"), Description";
	//	print "Query: $query";
		
		listMyArray($mediaLink,$query,"Details",2,array($pk_file,"Play","play"),"findmore.php?",$withUL = $withUL);
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
		global $currentScreen, $mediaLink;
		$commandGroupDescription = getMyValue($link,"SELECT Description FROM CommandGroup WHERE PK_CommandGroup = $commandGroup");
		// print "<h1>CommandGroup $commandGroup - $commandGroupDescription</h1>\n";
		$query = "SELECT PK_CommandGroup" . "_Command,FK_Command, FK_Device FROM CommandGroup" . "_Command Where FK_CommandGroup = $commandGroup Order By OrderNum";
		// print "<p>query: $query</p>\n";
		$commands = getMyArray($link,$query);

		foreach($commands as $commandDetail) {
			$destinationDeviceDescription = "";
			$destinationDevice = $commandDetail[2];
			if( $destinationDevice >= 0) {
				$destinationDeviceDescription = getMyValue($link,"SELECT Description From Device Where PK_Device = $destinationDevice");
			} elseif ($destinationDevice = -300) {
				$destinationDeviceDescription = "Current Orbiter";
			}	
			$command = $commandDetail[1];
			
			$commandDescription = getMyValue($link,"SELECT Description FROM Command Where PK_Command = $command");
	//		print "<h2>Destination: $destinationDevice - $destinationDeviceDescription</h2>\n";
	//		print "<h3>Command: $command - $commandDescription</h3>\n";

			$commandGroupCommand = $commandDetail[0];
			$parameters = getMyArray($link,"SELECT FK_CommandParameter, IK_CommandParameter FROM CommandGroup_Command_CommandParameter C Where FK_CommandGroup"."_Command = $commandGroupCommand");
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
						DoParameter29($mediaLink, $parameterValue, $commandGroup);
					}
				}
	//			print "<p>Parameter: $parameterType - $parameterDescription - $parameterValue</p>\n";

			}
		}
	}

	function doCommandGroup_D($link,$commandGroup) {
		// Get all associated commands and their parameters from the supplied commandgroup_D
		// $commandGroupDescription = getMyValue($link,"SELECT Description FROM CommandGroup_D WHERE PK_CommandGroup_D = $commandGroup");
		global $currentScreen;
		$commandGroupDescription = "empty";
		// print "<h1>CommandGroup_D $commandGroup - $commandGroupDescription</h1>\n";
		$query = "SELECT PK_CommandGroup_D" . "_Command,FK_Command, FK_DeviceTemplate FROM CommandGroup_D_Command Where FK_CommandGroup_D = $commandGroup Order By OrderNum";
		// print "<p>query: $query</p>\n";
		$commands = getMyArray($link,$query);

		foreach($commands as $commandDetail) {
			$destinationDeviceDescription = "";
			$destinationDevice = $commandDetail[2];
			if( $destinationDevice >= 0) {
				$destinationDeviceDescription = getMyValue($link,"SELECT Description From Device Where PK_Device = $destinationDevice");
			} elseif ($destinationDevice = -300) {
				$destinationDeviceDescription = "Current Orbiter";
			}	
			$command = $commandDetail[1];
			
			$commandDescription = getMyValue($link,"SELECT Description FROM Command Where PK_Command = $command");
			// print "<h2>Destination: $destinationDevice - $destinationDeviceDescription</h2>\n";
			// print "<h3>Command: $command - $commandDescription</h3>\n";

			$commandGroupCommand = $commandDetail[0];
			$parameters = getMyArray($link,"SELECT FK_CommandParameter, IK_CommandParameter FROM CommandGroup_D_Command_CommandParameter C Where FK_CommandGroup_D"."_Command = $commandGroupCommand");
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
				print "<p>Parameter: $parameterType - $parameterDescription - $parameterValue</p>\n";

			}
		}
	}

function connectdb() {
	global $link, $mediaLink, $currentRoom, $currentUser, $currentScreen, $currentEntertainArea, $UI, $SKIN;

	$user = "root";
	$password = "";
	$link = mysql_connect("localhost",$user,$password);
	
	mysql_select_db("pluto_main",$link);

	$mediaLink = mysql_connect("localhost",$user,$password,true);
	
	mysql_select_db("pluto_media",$mediaLink);

	$currentRoom = 1;
	$currentEntertainArea = 1;
	$currentScreen = 1;

	$UI = "FK_UI IS NULL";
	$SKIN = "FK_SKIN is NULL";
	// Check if we got called with some command to be executed
}

function messageSend($deviceToId = 10, $deviceGroupToId = 0, $messageId = 43, $messagePriority = 1, $messageType = 1,
		$deviceCategoryTo = 0, $deviceTemplateTo = 0, $includeChildren = 0, $messageBroadcastLevel = 0,
		$messageRetry = 0, $relativeToSender = 0, $expectedResponse = 0, $deviceListTo = "") {

	// Reimplementation of the command line messagesend. This allows the orbiter to send messages to the DCERouter to
	// initiate activities.

/*	deviceToId = 10 # Media plugin
	deviceGroupToId = 0 # no specific device group.
	messageId = 43 # MH_Play_File (see Advanced / DCE / Commands for more)
	messagePriority = 1 # PRIORITY_LOW=0, PRIORITY_NORMAL=1, PRIORITY_HIGH=2, PRIORITY_URGENT=3
	messageType = 1      # MESSAGETYPE_COMMAND=1, MESSAGETYPE_EVENT=2, MESSAGETYPE_DATAPARM_CHANGE=3, MESSAGETYPE_REPLY=4, MESSAGETYPE_DATAPARM_REQUEST=5, MESSAGETYPE_LOG=6, MESSAGETYPE_SYSCOMMAND=7, MESSAGETYPE_REGISTER_INTERCEPTOR=8, MESSAGETYPE_MESSAGE_INTERCEPTED=9, MESSAGETYPE_EXEC_COMMAND_GROUP=10, MESSAGETYPE_START_PING=11, MESSAGETYPE_STOP_PING=12, MESSAGETYPE_PURGE_INTERCEPTORS=13, MESSAGETYPE_PENDING_TASKS=14
	deviceCategoryTo = 0 # no specific category
	deviceTemplateTo = 0 # no specific template
	includeChildren = 0 # we don't care about children
	messageBroadcastLevel = 0 # BL_None=0, BL_DirectSiblings=1, BL_SameComputer=2, BL_SameRoom=3, BL_SameHouse=4, BL_AllHouses=5
	messageRetry = 0 # MR_None=0, MR_Retry=1, MR_Persist=2
	relativeToSender = 0 # not relative to sender
	expectedResponse = 0 # delivery confirmation
	deviceListTo = "" # no specific list of devices to send stuff to.
*/	
	
	
//	$blobheader = pack("9LBLLBL0s",magicNumber, deviceFromId, deviceToId, deviceGroupToId, messageId, messagePriority, messageType, deviceCategoryTo, deviceTemplateTo, includeChildren, messageBroadcastLevel, messageRetry, relativeToSender, expectedResponse, deviceListTo)
	$blobheader = pack("9LBLLBL0s",magicNumber, deviceFromId, deviceToId, deviceGroupToId, messageId, messagePriority, messageType, deviceCategoryTo, deviceTemplateTo, includeChildren, messageBroadcastLevel, messageRetry, relativeToSender, expectedResponse, deviceListTo);
	
	
//	# SendHeader(blob,dcerouter)    
	
	$parameter1ID = 13; // filename
	$parameter1Content = "/home/public/data/samples/security/security.mpg";
	
	$parameter2ID = 45; // Entertainment Area
	$parameter2Content = "2";
	
	$blobbody = pack("LL" . strlen($parameter1Content) . "sL1sL",2,$parameter1ID,$parameter1Content,$parameter2ID,$parameter2Content,0);
	
	#SendBody(blob,dcerouter)
	
	$blobfooter = pack("L",6789);
	#SendFooter(blob,dcerouter)
	$blobsize = strlen($blobheader)+strlen($blobbody)+strlen($blobfooter);
	print "blobsize mit drei mal calcsize";
	print $blobsize;
	
	$bigblob = $blobheader . $blobbody . $blobfooter;
	print "blobsize mit bigblob";
	print strlen($bigblob);
	
	$bigblob = "78 10 1 43 13 \"/home/public/data/samples/security/security.mpg\"  45 2";
	$delimiter = " ";
//	$bigblob = $delimiter.join(sys.argv[1:]);
//	SendBigBlob($bigblob,$dcerouter);
	print "big blob:";
	print $bigblob;
}
?>
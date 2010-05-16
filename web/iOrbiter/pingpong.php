<?php

error_reporting(E_ALL); 
        

$possyDeviceFromID = 214; // an Orbiter device id, which only exists to receive messages.

// commStart, commEnd and myMessageSend are in libMessageSend.php
$socketCommand = commStart("dcerouter",3450,$possyDeviceFromID,"COMMAND");
$socketEvent = commStart("dcerouter",3450,$possyDeviceFromID,"EVENT",1);
// Register Message Receiptor
$destination = -1000; // DCE Router Device
$messageType = 8; // MessageType Register Interceptor
$paramType = 1; // 
$paramValue = 1; 
//$paramType = 1; // PARM_FROM=1, PARM_TO=2, PARM_TEMPLATE=3, PARM_CATEGORY=4, PARM_MESSAGE_TYPE=5, PARM_MESSAGE_ID=6
//$paramValue = 10 ; // Media Plug-Ins (8=MDs, 11=MediaPlayer Plugins,13=MediaPlayers,64=VirtualMediaDriector)
// myMessageSend($socket,$deviceFromID,$deviceToID,$messageType = 1,$messageID,$parameter1ID=0,$parameter1Content="",$parameter2ID=0,$parameter2Content="") {

$messageTypeRegister = 8;
$messageTypeEvent = 2;
$messageTypeCommand = 1;
$messageTypeClearCriteria = 13;

$criteriaTypeFrom = 1;
$criteriaTypeTo = 2;
$criteriaTypeCategory = 4;

// Clear all registrations
/* myMessageSend($socket,$possyDeviceFromID,$destination,$messageTypeClearCriteria,0,0,"");

$input = socket_read($socket, 1024);
print "clear register returned: $input \n";
*/

// Setup regstration to receive all Events from DCERouter (device 1)
// myMessageSend($socket,$possyDeviceFromID,$destination,$messageTypeRegister,0,5,$messageTypeEvent,$criteriaTypeFrom,1);

// Register an interceptor without detailing what we want
myMessageSend($socketEvent,$possyDeviceFromID,$destination,$messageTypeRegister,0);

$input = socket_read($socketEvent, 1024);
print "first return: $input \n";
print "PLAIN_TEXT\n";
socket_write($socketEvent, "PLAIN_TEXT\n",strlen("PLAIN_TEXT\n"));
$input = socket_read($socketEvent, 1024);
print "second return: $input \n";


while ($input = socket_read($socketCommand, 1024)) {
  print "Return: $input --EOL--\n";
  socket_write($socketCommand,"OK\n",3);
}
// myMessageSend($socket,$possyDeviceFromID,10,1,43,13,'"' . $filePath . '"', 45, $currentEntertainArea)
commEnd($socketEvent);
commEnd($socketCommand);
                                                                 


// included from libMessageSend.php

	error_reporting(E_ALL);

	function getSocket($server, $port) {
		// Get a socket connection to the router
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);	
		if ( ! $socket) {
			print "Error creating socket";
		}
		if (!	socket_connect($socket,$server,$port)) {
			print "Error connecting to server";
		}
		return $socket;
	}
	
	
	function sendSocket($socket, $message) {
		// Send message to the socket
		print $message;
		$result = socket_write($socket, $message, strlen($message));
		if ( ! $result) {
			$errCode = socket_last_error();
			socket_clear_error();
			print "Error writing to socket: " . $errCode .  " - " . socket_strerror($errCode) . "\n";
		}		
		return $result;
	}
	
	function commStart($server = "localhost",$port = 3450, $deviceFromID, $command = "EVENT", $debug = 0) { 
		$socket = getSocket($server, $port);
		$result = sendSocket($socket,$command . " " . $deviceFromID . "\n");
		
		$result = socket_read($socket,1024, PHP_NORMAL_READ);
		if ($debug == 1) { 
			print "Result: <<$result>> \n";
		}
		return $socket;
	}

	function myMessageSend($socket,$deviceFromID,$deviceToID,$messageType = 1,$messageID,$parameter1ID=0,$parameter1Content="",$parameter2ID=0,$parameter2Content="") {
		// Careful:
		//    parameter1 und parameter2 content must include " if it is a long string!

		$messageToSend = $deviceFromID . " " . $deviceToID . " " . $messageType . " " . $messageID;
		if (is_array($parameter1ID)) {
			foreach ($parameter1ID as $parameterPair) {
				$messageToSend .= " " . $parameterPair[0] . " " . $parameterPair[1];
			}
			unset($parameter1ID);
		} 
		if (isset($parameter1ID)) {
			if ($parameter1ID <> 0) {
				$messageToSend .=  " " . $parameter1ID . " " . $parameter1Content;
			} else {
//				print "No parameter 1\n";
			}
			if ($parameter2ID <> 0) {
				$messageToSend .= " " . $parameter2ID . " " . $parameter2Content;
			} else {
//				print "No parameter 2\n";
			}
		}
		$messageLength = strlen($messageToSend);
		$result = sendSocket($socket, "MESSAGET " . $messageLength . "\n");
		$result = sendSocket($socket, $messageToSend . "\n");
	}

	function commEnd($socket) { 
		socket_close($socket);
	}


?>

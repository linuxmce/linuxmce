<?php
/*

  pingpong.php
  run it from the commandline with php pingpong.php. It will register itself with a dcerouter 
  using the defined Orbiter ID. Make sure the Orbiter ID exists. After registering, it receives
  all messages for the Orbiter and displays these messages on the screen.

  Copyright 2010, Peer Oliver Schmidt
  Licence: GPL v3
*/
error_reporting(E_ALL); 
        

$possyDeviceFromID = 214; // an Orbiter device id, which only exists to receive messages.

// commStart, commEnd and myMessageSend are in libMessageSend.php
$socketCommand = commStart("dcerouter",3450,$possyDeviceFromID,"COMMAND",1);
$socketEvent = commStart("dcerouter",3450,$possyDeviceFromID,"EVENT",1);
// Register Message Receiptor
$destination = -1000; // DCE Router Device
$messageType = 8; // MessageType Register Interceptor
$paramType = 1; // 
$paramValue = 1; 
//$paramType = 1; // PARM_FROM=1, PARM_TO=2, PARM_TEMPLATE=3, PARM_CATEGORY=4, PARM_MESSAGE_TYPE=5, PARM_MESSAGE_ID=6
//$paramValue = 10 ; // Media Plug-Ins (8=MDs, 11=MediaPlayer Plugins,13=MediaPlayers,64=VirtualMediaDriector)
// myMessageSend($socket,$deviceFromID,$deviceToID,$messageType = 1,$messageID,$parameter1ID=0,$parameter1Content="",$parameter2ID=0,$parameter2Content="") {

$parameterMessageType = 5;
$messageTypeRegister = 8;
$messageTypeEvent = 2;
$messageTypeCommand = 1;
$messageTypeClearCriteria = 13;

$criteriaTypeFrom = 1;
$criteriaTypeTo = 2;
$criteriaTypeCategory = 4;
$criteriaTypeMessageID = 6;

$deviceMediaPlugin = 10;
$deviceDCERouter = 1;
$deviceOrbiterPlugin = 9;

$messageSetNowPlaying = 242;
$messageOrbiterRegistered = 255;

// Clear all registrations - does NOT return anything on the socketEvent
myMessageSend($socketEvent,$possyDeviceFromID,$destination,$messageTypeClearCriteria,0,0,"");

// Setup regstration to receive all Events from DCERouter (device 1)
// myMessageSend($socketEvent,$possyDeviceFromID,$destination,$messageTypeRegister,0,5,$messageTypeEvent,$criteriaTypeFrom,1);

// Setup regstration to receive all Events from DCERouter (device 1)
// myMessageSend($socketEvent,$possyDeviceFromID,$destination,$messageTypeRegister,0,$parameterMessageType,$messageTypeCommand,$criteriaTypeMessageID,$messageSetNowPlaying);

// Register an interceptor to receive a message whenever playback starts.

/* myMessageSend($socketEvent,$possyDeviceFromID,$destination,$messageTypeRegister,0,$parameterMessageType,$messageTypeCommand,$criteriaTypeMessageID,242);
$input = socket_read($socketEvent, 1024);
print "input $input\n";
*/

// For now, we are happy with PLAIN_TEXT messages
print "PLAIN_TEXT\n";
socket_write($socketEvent, "PLAIN_TEXT\n",strlen("PLAIN_TEXT\n"));
$input = socket_read($socketEvent, 1024);
print "second return: $input \n";

// Tell Orbiter plugin we are alive - does NOT return anything on the socketEvent
$messageSendParameter = array(array(8,"1"));  // Register ON
array_push($messageSendParameter, array(17,"")); // Current User
array_push($messageSendParameter, array(45,"6")); // Entertainment Area
array_push($messageSendParameter, array(57,"6")); // Current Room
 
myMessageSend($socketEvent,$possyDeviceFromID,$deviceOrbiterPlugin,$messageTypeCommand,$messageOrbiterRegistered,$messageSendParameter);
// After registering this devie with Orbiter-plugin, we automatically receive messages destined for an Orbiter. No need to intercept
// any more messages per se



while ($input = socket_read($socketCommand, 1024, PHP_NORMAL_READ)) {
  print $input . "\n";
  $arrayReturn = explode(" ",$input);  
  $length = intval($arrayReturn[1]) + 1;
  print "Reading $length bytes\n";
  $input = readSocketForDataLength($socketCommand,$length);
  digestMessage($input);
  socket_write($socketCommand,"OK\n",3);
  if ($length == 1) {
  	die();
  }
}

// myMessageSend($socket,$possyDeviceFromID,10,1,43,13,'"' . $filePath . '"', 45, $currentEntertainArea)
commEnd($socketEvent);
commEnd($socketCommand);

function digestMessage($input) {
  if (strlen($input) > 500) {
      $input = "only the first 200byte: " . substr($input,0,500);
  }
  print "Return: $input --EOL--\n";
}                                                                 

function readSocketForDataLength ($socket, $len)
{
    // This function was copied from the php.net site
    $offset = 0;
    $socketData = '';
   
    while ($offset < $len) {
        if (($data = @socket_read ($socket, $len-$offset, PHP_BINARY_READ)) === false) {
            return false;
        }
       
        $dataLen = strlen ($data);
        $offset += $dataLen;
        $socketData .= $data;
       
        if ($dataLen == 0) { break; }
    }

    return $socketData;
}



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
	
	
	function sendSocket($socket, $message, $debug = 0) {
		// Send message to the socket
		if ($debug == 1) {
			print $message;		
		}
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
				if (substr($parameterPair[1],0,1) != '"') {
				  $parameterPair[1] = '"' . $parameterPair[1] . '"';
				}
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
		$result = sendSocket($socket, "MESSAGET " . $messageLength . "\n",1);
		$result = sendSocket($socket, $messageToSend . "\n",1);
	}

	function commEnd($socket) { 
		socket_close($socket);
	}


?>

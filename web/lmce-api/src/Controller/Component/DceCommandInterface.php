<?php

namespace App\Controller\Component;

use Cake\Controller\Component;

class DceCommandExectorComponent extends Component
{
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
		$result = socket_write($socket, $message, strlen($message));
		if ( ! $result) {
			$errCode = socket_last_error();
			socket_clear_error();
			print "Error writing to socket: " . $errCode .  " - " . socket_strerror($errCode) . "\n";
		}		
		return $result;
	}
	
	function commStart($server = "localhost",$port = 3450, $deviceFromID, $command = "EVENT") { 
		$socket = getSocket($server, $port);
		$result = sendSocket($socket,$command . " " . $deviceFromID . "\n");
		
		$result = socket_read($socket,1024, PHP_NORMAL_READ);
		if ($command == "COMMAND") { 
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
				print "No parameter 2\n";
			}
		}
		$messageLength = strlen($messageToSend);
		$result = sendSocket($socket, "MESSAGET " . $messageLength . "\n");
		$result = sendSocket($socket, $messageToSend . "\n");
	}

	function commEnd($socket) { 
		socket_close($socket);
	}

	function examplePlaySecurityMessage() {
		// Just an example on how to call the messagesend function	
		global $possyDeviceFromID;
		$server = "dcerouter";
		$port = 3450;
		$timeout = 10;
		$errno = 0;
		$errstr = "";
		$deviceFromID = 51;
		$deviceToID = 10;
		$messageType = 1;
		$messageID = 43; # MH_Play_File
		$parameters = array();

		$parameter1ID = 13; // filename
		$parameter1Content = '"/home/public/data/samples/security/security.mpg"';

		$parameter2ID = 45; // Entertainment Area
		$parameter2Content = "2";

		// commStart($server, $port, $deviceIDFrom)
		$socket = commStart($server,$port,$possyDeviceFromID);		
		myMessageSend($socket,$possyDeviceFromID,$devceToID,$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID, $parameter2Content);
		commEnd($socket);
	}

}

?>
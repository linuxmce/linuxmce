<?php

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
		$result = socket_write($socket, $message, strlen($message));
		if ( ! $result) {
			$errCode = socket_last_error();
			socket_clear_error();
			print "Error writing to socket: " . $errCode .  " - " . socket_strerror($errCode) . "\n";
		}		
		return $result;
	}

	function myMessageSend($server = "localhost",$port = 3450,$deviceFromID,$deviceToID,$messageType = 1,$messageID,$parameter1ID,$parameter1Content,$parameter2ID,$parameter2Content) {
		// Careful:
		//    parameter1 und parameter2 content must include " if it is a long string!
		print "Server: $server\n";

		$socket = getSocket($server, $port);


		$result = sendSocket($socket,"EVENT " . $deviceFromID . "\n");
		$result = socket_read($socket,1024, PHP_NORMAL_READ);
		print "Result from Event: $result\n";		
		$messageToSend = $deviceFromID . " " . $deviceToID . " " . $messageType . " " . $messageID;
		if ($parameter1ID <> 0) {
			$messageToSend .=  " " . $parameter1ID . " " . $parameter1Content;
		} else {
			print "No parameter 1\n";
		}
		if ($parameter2ID <> 0) {
			$messageToSend .= " " . $parameter2ID . " " . $parameter2Content;
		} else {
			print "No parameter 2\n";
		}
		$messageLength = strlen($messageToSend);
		$result = sendSocket($socket, "MESSAGET " . $messageLength . "\n");
		$result = sendSocket($socket, $messageToSend . "\n");
		echo "Message to send: ";
		echo $messageToSend;
		print "\n";
		socket_close($socket);
	}

	function examplePlaySecurityMessage() {
		// Just an example on how to call the messagesend function	
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
		$noOfParameters = array_push($parameters,array(13,"\"/home/public/data/samples/security/security.mpg\""));
		$noOfParameters = array_push($parameters,array(45,2));

		$parameter1ID = 13; // filename
		$parameter1Content = '"/home/public/data/samples/security/security.mpg"';

		$parameter2ID = 45; // Entertainment Area
		$parameter2Content = "2";

		myMessageSend($server,$port,$deviceFromID,$deviceToID,$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID,$parameter2Content);
	}
?>

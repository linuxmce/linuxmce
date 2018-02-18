<?php

namespace App\Controller\Component;

use Cake\Controller\Component;

class DceCommandExecutorComponent extends Component
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
	
	public function commStart($server = "localhost",$port = 3450, $deviceFromID, $command = "EVENT") { 
		$socket = $this->getSocket($server, $port);
		$result = $this->sendSocket($socket,$command . " " . $deviceFromID . "\n");
		
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
		$result = $this->sendSocket($socket, "MESSAGET " . $messageLength . "\n");	
		$result = $this->sendSocket($socket, $messageToSend . "\n");
		//echo $messageToSend;
		
	}

	function commEnd($socket) { 
		socket_close($socket);
	}
	
	function sendLightOnCommand($device){
				
		$params = array(
		'msgid'=>192,
		'params'=>  array(
			array('97','""')
		)  
		);
		$response = $this->executeFormedCommand($device, $params);
		return $response;
	}
        
        function sendColorCommand($data){
                       
            $params=array(
                'msgid' => 1115,
                'params' => array(
                    array('76',  $data['color']['color']['brightness']*255),
                    array('286', $data['color']['color']['hue']),
                    array('287', $data['color']['color']['saturation']*255)
                )
            );
            
            $response = $this->executeFormedCommand($data['device'], $params);
           
            if($response=="OK"){
                  return array( 
                "h"=>$data['color']['color']['hue'],
                "s"=>$data['color']['color']['saturation'],
                "b"=>$data['color']['color']['brightness']
            );
            } else {
                return array( 
                "h"=>0,
                "s"=>0,
                "b"=>0
            );
            }    
        }
        
        function sendColorTempCommand($data){
            
            $colorTempValue =  $data['colorTemp']['colorTemperatureInKelvin'];
            $finalColorTempVal = ( ($colorTempValue - 1000) / (10000 - 1000) ) * (6500 - 2000) + 2000 ;
             $params=array(
                'msgid' => 1160,
                'params' => array(
                    array('48', $finalColorTempVal)
                )
            );
            
            $this->executeFormedCommand($data['device'], $params);           
             return  $data['colorTemp']['colorTemperatureInKelvin'];
          
        }
	
	function sendLightOffCommand($device){
		$params = array(
		"msgid"=>193,
		'params'=> array(
			array('97','""'),	
			array('98','""')
		)		
		);
		$response = $this->executeFormedCommand($device, $params);
		return $response;
	}
	
		function sendGenericOnCommand($device){
		$response = "";
			
		return $response;
	}
	
	function sendLightLevelCommand($device, $level){
		$params=array(
		'msgid'=>184,
		'params'=>array(
		array('76', $level)
		)
		);
		
		$response = $this->executeFormedCommand($device, $params);
		return $response;
	}
	
		function executeScenario($scenario){
		$params=array(
		'msgid'=>370,
		'params'=>array(
		array('28', $scenario)
		)
		);
		
			$server = "localhost";
		$port = 3450;
		$timeout = 10;
		$errno = 0;
		$errstr = "";
		$deviceFromID = 8;
		$deviceToID = -1000;
		$messageType = 1;
		$messageID = $params['msgid']; 
		$parameters = $params['params'];
		
		// commStart($server, $port, $deviceIDFrom)
		$socket = $this->commStart($server,$port,$deviceFromID);		
		$this->myMessageSend($socket, $deviceFromID, $deviceToID ,$messageType,$messageID,$parameters);
		$this->commEnd($socket);
		
		return "OK";
	
	}
	
	
	
	
	function executeFormedCommand($device, $params /*array*/){
		$server = "localhost";
		$port = 3450;
		$timeout = 10;
		$errno = 0;
		$errstr = "";
		$deviceFromID = 8;
		$deviceToID = $device;
		$messageType = 1;
		$messageID = $params['msgid']; 
		$parameters = $params['params'];
		
		// commStart($server, $port, $deviceIDFrom)
		$socket = $this->commStart($server,$port,$deviceFromID);		
		$this->myMessageSend($socket, $deviceFromID, $device ,$messageType,$messageID,$parameters);
		$this->commEnd($socket);
		
		return "OK";
	}
	

	function playMediaInRoom($room, $media){
			// Just an example on how to call the messagesend function	
		global $possyDeviceFromID;
		$server = "dcerouter";
		$port = 3450;
		$timeout = 10;
		$errno = 0;
		$errstr = "";
		$deviceFromID = 0;
		$deviceToID = 10;
		$messageType = 1;
		$messageID = 43; # MH_Play_File
		$parameters = array();

		$parameter1ID = 13; // filename
		$parameter1Content = '"'.$media.'"';
		print_r($parameter1Content);
		
		$parameter2ID = 45; // Entertainment Area
		$parameter2Content = $room;

		// commStart($server, $port, $deviceIDFrom)
		$socket = $this->commStart($server,$port,$deviceFromID);		
		$this->myMessageSend($socket,$deviceFromID,$deviceToID,$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID, $parameter2Content);
		$this->commEnd($socket);
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
		$socket = $this->commStart($server,$port,$possyDeviceFromID);		
		$this->myMessageSend($socket,$possyDeviceFromID,$devceToID,$messageType,$messageID,$parameter1ID,$parameter1Content,$parameter2ID, $parameter2Content);
		$this->commEnd($socket);
	}

}

?>
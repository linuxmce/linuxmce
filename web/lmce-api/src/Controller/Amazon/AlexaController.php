<?php
/**
 * CakePHP(tm) : Rapid Development Framework (https://cakephp.org)
 * Copyright (c) Cake Software Foundation, Inc. (https://cakefoundation.org)
 *
 * Licensed under The MIT License
 * For full copyright and license information, please see the LICENSE.txt
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright Copyright (c) Cake Software Foundation, Inc. (https://cakefoundation.org)
 * @link      https://cakephp.org CakePHP(tm) Project
 * @since     0.2.9
 * @license   https://opensource.org/licenses/mit-license.php MIT License
 */
namespace App\Controller\Amazon;

use App\Controller\Amazon\AppController;
use Cake\Event\Event;
use Cake\ORM\TableRegistry;
use Cake\I18n\Time;

/**
 * Application Controller
 *
 * Add your application-wide methods in the class below, your controllers
 * will inherit them.
 *
 * @link https://book.cakephp.org/3.0/en/controllers.html#the-app-controller
 */
class AlexaController extends AppController
{

   
    public function initialize()
    {
        parent::initialize();
		 $this->loadComponent('RequestHandler');
		$this->loadComponent('DceCommandExecutor');
    }
	
    public function discovery(){
		$DeviceList = TableRegistry::get('Devices');
		$devices = $DeviceList->alexaDeviceDiscovery();
		$answer = array(
			'event' => [
				'header' =>[
					'namespace'=>'Alexa.Discovery',
					'name' => 'Discover.Response',
					'payloadVersion' => '3',
					'messageId'=>'foo'
				],
				'payload' => array(
				'endpoints' => $devices[0])
				
			]
		);
		
		$this->set(compact('answer'));
		$this->set('_serialize', 'answer');		
	}
	
		public function powercommand(){
		//$this->request->allowMethod(['POST', 'post']);
		$DeviceList = TableRegistry::get('Devices');
			
			
		$deviceTarget = $this->request->params['?']['device'];
		$command = $this->request->params['?']['cmd'];
		
		$rep = "";
		$error="";	
	
			if( isset($deviceTarget) && isset($command) ){
				$error = "none";
				
			if($command=="on"){
				$rep = $this->DceCommandExecutor->sendLightOnCommand($deviceTarget);
			} else if ($command=="off"){
				$rep = $this->DceCommandExecutor->sendLightOffCommand($deviceTarget);
			}
				
			} else {
				$error = " parameter(s) missing";
				$rep= $this->request->params;
			}
			
		$now = Time::now();
		$now->format('e');	  
		$reply = array(
		'status' => $rep,
		'timeSent'=>$now,
		'error'=>$error
		);	
			
		$device = $DeviceList->get($deviceTarget, ['contain'=>['DeviceTemplate', 'Device_DeviceData'] ] );
			if($device->device_template->FK_DeviceCategory==73){
				$lightState = explode("/", $device->State);				
				$reply['bri'] = $lightState[1];
				$reply['lightState'] = $lightState[0];
			}
		
		$this->set(compact('reply'));
		$this->set('_serialize', 'reply');		
	}
	
	function brightnessCommand(){
		//$this->request->allowMethod(['POST', 'post']);
		$DeviceList = TableRegistry::get('Devices');
			
			
		$deviceTarget = $this->request->params['?']['device'];
		$mode = $this->request->params['?']['mode'];
		$val = $this->request->params['?']['val'];
		
		$rep = "";
		$error="";	
	
		$amount ="";
			if( isset($deviceTarget) && isset($val) ){
				$error = "none";
				
			if($mode=="AdjustBrightness"){
			 if($val > 0){
				 $amount=   "+".($val*1);
			 } else {
				  $amount = "-".($val*-1);
			 }
			} else{
				$amount = (string)$val;
			}
				
			$rep = $this->DceCommandExecutor->sendLightLevelCommand($deviceTarget, $amount );
				
			} else {
				$error = " parameter(s) missing";
				$rep= $this->request->params;
			}
		
			
		$now = Time::now();
		$now->format('e');	  
		$reply = array(
		'status' => $rep,
		'timeSent'=>$now,
		'error'=>$error
		);	
			
		$device = $DeviceList->get($deviceTarget, ['contain'=>['DeviceTemplate', 'Device_DeviceData'] ] );
		
			if($device->device_template->FK_DeviceCategory==73){
			
				$lightState = explode("/", $device->State);				
				$reply['bri'] = $lightState[1];
				$reply['lightState'] = $lightState[0];
			}
		
		$this->set(compact('reply'));
		$this->set('_serialize', 'reply');	
	}
	
	public function report(){
		$deviceTarget = $this->request->params['?']['device'];
		$error = "none";
		
		$now = Time::now();
		$now->format('e');
		
		$reply= array(
		'status' => "OK",
		'timeSent'=>$now,
		'error'=>$error
		);	
		if(isset($deviceTarget)){
				$DeviceList = TableRegistry::get('Devices');		
			
			$device = $DeviceList->get($deviceTarget, ['contain'=>['DeviceTemplate', 'Device_DeviceData'] ] );
			if($device->device_template->FK_DeviceCategory==73){
			
				$lightState = explode("/", $device->State);				
				$reply['bri'] = $lightState[1];
				$reply['lightState'] = $lightState[0];
			}
		} else {
			$error = "invalid device";
		}
		
		$this->set(compact('reply'));
		$this->set('_serialize', 'reply');	
	}
	
	public function executescenario(){
		
		$scenario = $this->request->params['?']['scene'];
		$error = "none";
		
		$now = Time::now();
		$now->format('e');
		$status ="";
	
		
		if(isset($scenario)){
			$status = $this->DceCommandExecutor->executeScenario($scenario);
		} else {
			$error = "no scenario set";
		}
		
		$reply= array(
		'status' => $status,
		'timeSent'=>$now,
		'error'=>$error
		);	
		
		$this->set(compact('reply'));
		$this->set('_serialize', 'reply');
	}
	
	public function findMovieByName(){
		$error = "none";		
		$now = Time::now();
		$now->format('e');
		$status ="";
		
		$movieName = $this->request->getQuery('name');
		$roomRequest = $this->request->getQuery('room');
		$room = $this->findRoomByName($roomRequest);
		
		$Linuxmcefile = TableRegistry::get('Linuxmcefile');
		$movieReply = $Linuxmcefile->getVideoByName($movieName);
		
		$reply= array(
		'status' => $status,
		'timeSent'=>$now,
		'error'=>$error
		);	
		
		$reply['movie']=$movieReply;
		$reply['room'] = array(
		'room'=>$roomRequest,
			'id'=>$room
		);
		
		if($reply['movie']['count']==0){
			$reply['status'] = 'error';
			$reply["error"] = 'movie not found';
		} 
		else if ($roomRequest==-1) 
		{			
			$reply['status'] = 'error';
			$reply['error'] = 'Incorrect room';
		} else {
		$stat = $this->playMediaInRoom($reply['movie']['movies'][0]['fileName'], $room );
			$reply['status'] = 'OK';
		}
		
		
		
		
		$this->set(compact('reply'));
		$this->set('_serialize', 'reply');
	}
	
	public function findRoomByName($roomName){
		$error = "none";		
		$now = Time::now();
		$now->format('e');
		$status ="";		
		
		$EA = TableRegistry::get('Entertainarea');		
	
		$targetEa = $EA->getEntertainAreaFromName($roomName);
		
		return $targetEa;
	}
	
	public function playMediaInRoom($media, $room){
		
		$status = $this->DceCommandExecutor->playMediaInRoom($room, $media);
		return $status;
	}
	
}

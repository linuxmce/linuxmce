<?php
App::uses('AppController', 'Controller');
/**
 * Devices Controller
 */
class StatusController extends AppController {
	public $components = array('RequestHandler');
	public $uses = array('Device', 'Room');
	function index(){
		
    $lights = $this->Device->find('all' ,
									array(
									'conditions' => array('DeviceTemplate.FK_DeviceCategory' => 73 )
									));
		
		$roomList = $this->Room->find('all');
		$systemStatus = array(
		'systemstate' =>"online",
	    'lights' => $lights,
			'rooms' =>$roomList,
			'avequipment'=>array(),
			'climate' => array(),
			'irrigation'=> array(),
			'cameras' => array(),
			'security'=>array()
		);
		$this->set(compact('systemStatus'));
		$this->set('_serialize', array('systemStatus'));
	}

}

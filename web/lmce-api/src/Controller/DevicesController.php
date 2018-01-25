<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Devices Controller
 *
 * @property \App\Model\Table\DevicesTable $Devices
 *
 * @method \App\Model\Entity\Device[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class DevicesController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $this->paginate = [
            'contain' => ['DeviceTemplate']
        ];
        $devices = $this->paginate($this->Devices);

        $this->set(compact('devices'));
    }

    /**
     * View method
     *
     * @param string|null $id Device id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $device = $this->Devices->get($id, [
            'contain' => ['DeviceTemplate', 'DeviceData', 'Device_DeviceData', 'Room']
        ]);

        $this->set('device', $device);
    }
	
	public function lights(){
		
		$dl = $this->Devices->find('all');
	    $lightList = $this->Devices->find('byDeviceCategory', ['category'=> "73"]);
		//$lightList = $this->Devices->getAlexaLightList();
			
		$this->set(compact('lightList'));
		$this->set('_serialize', 'lightList');
	}
	
		public function switches(){
		
		$lightList = $this->Devices->find()->contain(['DeviceTemplate','Room'] );
		
		$final = $lightList->matching('DeviceTemplate', function ($q){
						 return $q->where(['DeviceTemplate.FK_DeviceCategory' => "126"]);
					 });
			
		$this->set(compact('final'));
		$this->set('_serialize', 'final');
	}
	
		public function mediaDirectors(){
		
		$lightList = $this->Devices->find()->contain(['DeviceTemplate','Room'] );
		
		$final = $lightList->matching('DeviceTemplate', function ($q){
						 return $q->where(['DeviceTemplate.FK_DeviceCategory' => "8"]);
					 });
			
		$this->set(compact('final'));
		$this->set('_serialize', 'final');
	}
	
		public function climate(){
		
		$lightList = $this->Devices->find()->contain(['DeviceTemplate','Room'] );
		
		$final = $lightList->matching('DeviceTemplate', function ($q){
						 return $q->where(['DeviceTemplate.FK_DeviceCategory' => "72"]);
					 });
			
		$this->set(compact('final'));
		$this->set('_serialize', 'final');
	}
	

}

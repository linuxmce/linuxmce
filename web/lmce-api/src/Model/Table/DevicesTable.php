<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class DevicesTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Device');
	$this->primaryKey('PK_Device');
        $this->colorDeviceTemplates= array(2319,2257); 
        $this->tunableDeviceTemplates = array(2362,2257);
        $this->dimmableDeviceTemplates = array(2318);
        $this->excludedDevices = array(2358, 2359, 2361);
	
	
	$this->hasOne( 'Room',
				  [
					  'className'=>'Rooms',
					  'foreignKey' => 'PK_Room',
					  'bindingKey' => 'FK_Room'
				  ]
	);
	
	$this->belongsTo('DeviceTemplate',
				  [
					  'className' => 'Devicetemplates',
					  'foreignKey' => 'FK_DeviceTemplate',
					  'bindingKey' => 'PK_DeviceTemplate'
				  ]
	
	);
	
  $this->hasMany('DeviceData',
				  [
					  'className' => 'Devicedata',
					  'foreignKey' => 'FK_DeviceData',
					  'bindingKey' => 'FK_DeviceData',	
					  'targetForeignKey' => 'FK_DeviceTemplate',
					  'joinTable'=> 'Devicetemplatedevicedata'
				  ]);
	
	 $this->hasMany('Device_DeviceData',
				  [
					  'className' => 'Devicedevicedata',
					  'foreignKey' => 'FK_Device',
					  'bindingKey' => 'PK_Device',	
					  'targetForeignKey' => 'FK_DeviceData',
				  ]);
	
			$this->belongsToMany('Command',
				  [
					  'className'=>'Commands',
					  'joinTable' => 'Commandgroupcommand',
					  'foreignKey' => 'FK_Device',
					  'targetForeignKey' =>'FK_Command'
				  ]
	);
	
}
	
	
function findByDeviceCategory(Query $query, array $options){
	
	$cat = $options['category'];
	

	return $query
			->contain(['DeviceTemplate','Room', 'DeviceData', 'Device_DeviceData'] )
			->matching('DeviceTemplate', function ($q) use ($cat) {
						  return $q
							  ->contain('Manufacturer')
							  ->where(['DeviceTemplate.FK_DeviceCategory' => $cat]);
					 });
}
	
	function getAlexaLightList(){
				
		$devices = TableRegistry::get('Devices');
		$lightList = $this->find('byDeviceCategory', ['category'=> "73"]);
              
		
		$alexaObject = array();
		
		foreach($lightList as $light){
                    
                    if(in_array($light->device_template->PK_DeviceTemplate, $this->excludedDevices)){
                        break;
                    }
                    
			$appendArry = array(
				'endpointId' => (string) $light->PK_Device,
				'friendlyName' =>$light['Description'],
				'description' => $light->device_template->Description,
				'manufacturerName' => $light->device_template->manufacturer->Description,
				'displayCategories' => array('LIGHT'),
				'cookie' => array(
					'foo'=> 'data'
				),
				'capabilities'=> array(					
				[
					'type'=>'AlexaInterface',
					'interface' => 'Alexa.PowerController',
					'version' => '3',
					'properties' => array(
					'supported'=> [array(
					'name' => 'powerState'
					)],						
						'retrievable' => true						
					)
				],
					[
					'type'=>'AlexaInterface',
					'interface' => 'Alexa.BrightnessController',
					'version' => '3',
					'properties' => array(
					'supported'=> [array(
					'name' => 'brightness'
					)],						
						'retrievable' => true						
					)
				]	
				)				
			);
			
			if( in_array($light->device_template->PK_DeviceTemplate, $this->colorDeviceTemplates ) ){
				$appendArry['capabilities'][]= [
					'type'=>'AlexaInterface',
					'interface' => 'Alexa.ColorController',
					'version' => '3',
					'properties' => array(
					'supported'=> [array(
					'name' => 'color'
					)],						
						'retrievable' => true						
					)
				];
			} 
                        
                        if ( in_array($light->device_template->PK_DeviceTemplate, $this->tunableDeviceTemplates )  ){
                            
				$appendArry['capabilities'][]= [
					'type'=>'AlexaInterface',
					'interface' => 'Alexa.ColorTemperatureController',
					'version' => '3',
					'properties' => array(
					'supported'=> [array(
					'name' => 'colorTemperatureInKelvin'
					)],						
						'retrievable' => true						
					)
				];
			} 
                        
                    
				
			$alexaObject[] = $appendArry;
		}
		
		return $alexaObject;
	}
	
		function getAlexaTvList(){
				
		$devices = TableRegistry::get('Devices');
		$tvList = $this->find('byDeviceCategory', ['category'=> "8"]);
		
		$alexaObject = array();
		
		foreach($tvList as $tv){
			$appendArry = array(
				'endpointId' =>  (string) $tv->PK_Device,
				'friendlyName' =>$tv['Description'],
				'description' => $tv->device_template->Description,
				'manufacturerName' => $tv->device_template->manufacturer->Description,
				'displayCategories' => array('TV'),
				'cookie' => array('foo'=>'data'),
				'capabilities'=> array(
					
				[
					'type'=>'AlexaInterface',
					'interface' => 'Alexa.Speaker',
					'version' => '1.0',
					'properties' => array(
						'supported'=> array(						
							['name' => 'volume'],
							['name'=> 'muted']
					))],
					[
						'type'=>'AlexaInterface',
						'interface' => 'Alexa.ChannelController',
						'version' => '1.0',
						'properties' => array(
						'supported'=> [array(						
							'name' => 'channel',
					)]
						)]	,
					[
						'type'=>'AlexaInterface',
						'interface' => 'Alexa.PlaybackController',
						'version' => '1.0',
						'properties' => array(),
						'supportedOperations' => array('Play', 'Pause', 'Stop'),
					],		
					[
						'type'=>'AlexaInterface',
						'interface'=>'Alexa.InputController',
						'version'=>'1.0',
						'properties'=> array(
						'supported' => array(
							'name'=>'input'
						)
						)
					]
				)
				
			);
			$alexaObject[] = $appendArry;
		}
		
		return $alexaObject;
	}
	
		
	function alexaDeviceDiscovery(){
					
		$lightList = $this->getAlexaLightList();
		$mediaScenarioList = $this->getMediaScenarios();
		//$tvList = $this->getAlexaTvList();
		$alexaDeviceList = array( array_merge($lightList, $mediaScenarioList));
		
		return $alexaDeviceList;
	}
	
	function getMediaScenarios(){
		$ret = array();
		$entertainAreas = TableRegistry::get('Entertainarea')
			->find('all')
			->contain(['Commandgroup']);
			
		
		foreach($entertainAreas as $entertainArea){
			
		foreach($entertainArea->command_group as $CG){
				
			
		
			if($CG->Disabled==false){
				
			
				$appendArray = array(
			
			'endpointId' => $CG->PK_CommandGroup,
			'manufacturerName' => 'LinuxMCE',
			'friendlyName' => $CG->Description." in ".$entertainArea->Description,
			'description' => 'Linuxmce Media Scene',
			'displayCategories' => ['ACTIVITY TRIGGER'],
				'cookie' => array(
					"ismedia"=>"unknown"
				),
				'capabilities' => [
					array( 
					'type'=>'AlexaInterface',
						'interface'=>'Alexa.SceneController',
						'version'=>"3",
						'supportsDeactivation'=>true,
						'proactivelyReported'=>false						
					)
				]
			);			
			$ret[] = $appendArray;
			}
			
			}
		}
		
		
		return $ret;
			
			
	}
	
	function deviceDataForDevice($deviceId){
			
			
	}
        
        function handleDeviceReport($deviceTarget){
            $DeviceList = TableRegistry::get('Devices');		
            $device = $DeviceList->get($deviceTarget, ['contain'=>['DeviceTemplate', 'Device_DeviceData'] ] );
            $reply = array();
            $reply['error']="none";
            
			if($device->device_template->FK_DeviceCategory==73){
				
				$lightState = explode("/", $device->State);				
				$reply['bri'] = $lightState[1];
				$reply['lightState'] = $lightState[0];
				
				if( in_array($device->device_template->PK_DeviceTemplate, $this->colorDeviceTemplates)  ){
                                    $hueCurrentColor = json_decode($device->device__device_data[5]->IK_DeviceData, true);
                                    $hsl = $hueCurrentColor["color"]["currentColor"]["hsl"];
                                   
					$reply['color'] = array(
						"hsb"=>[
							"h"=>$hsl["h"],
							"s"=>$hsl["s"],
							"b"=>$hsl["l"]
						]						
					);
				} else if( in_array($device->device_template->PK_DeviceTemplate, $this->tunableDeviceTemplates) ){
                                    $hueCurrentColor = json_decode($device->device__device_data[5]->IK_DeviceData, true);
                                    $colorTemp = $hueCurrentColor["color"]["currentColorTemp"];
					$reply['colorTemperatureInKelvin'] = $colorTemp;
				}				
			}                        
	return $reply;	
        }
	
	
}



?>
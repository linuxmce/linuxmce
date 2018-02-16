<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class DevicedataTable extends Table{
	
public function initialize(array $config){
	$this->setTable('DeviceData');
	$this->primaryKey('PK_DeviceData');		
	
	$this->belongsToMany('Device', 
						 [
					  'className' => 'Devices',
					  'foreignKey' => 'FK_DeviceData',
					  'bindingKey' => 'PK_DeviceData',
					  'through' => 'Device_DeviceData',
					'targetForeignKey'=>'FK_Device'
				  ]);
	
						
}
	
}



?>
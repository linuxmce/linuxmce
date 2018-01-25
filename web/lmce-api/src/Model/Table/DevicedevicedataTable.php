<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class DevicedevicedataTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Device_DeviceData');
		
	$this->belongsTo('Device',
					[
						'className' =>'Devices',
						'bindingKey'=>'FK_Device',
						'foreignKey'=>'PK_Device'
					]
					);
	$this->belongsTo('DeviceData',
					[
						'className'=>'Devicedata',
						'bindingKey'=>'FK_DeviceData',
						'foreignKey'=>'PK_DeviceData'
					]);	
						
}
	
}



?>
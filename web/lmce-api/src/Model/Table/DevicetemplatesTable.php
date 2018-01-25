<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class DevicetemplatesTable extends Table{
	
public function initialize(array $config){
	$this->setTable('DeviceTemplate');
	$this->primaryKey('PK_DeviceTemplate');
	
	$this->belongsTo( 'Devicecategory',
				  [					  
					  'className' => 'Devicecategories',
					  'foreignKey' => 'FK_DeviceCategory',
					  'bindingKey' => 'PK_DeviceCategory'
				  ]
	);
	
	$this->hasOne('Manufacturer',
				  [
					  'className' => 'Manufacturers',
					  'foreignKey' => 'PK_Manufacturer',
					  'bindingKey' => 'FK_Manufacturer',
					  'joinType' => 'INNER'
				  ]
	
	);
	
			$this->belongsToMany('DeviceData',
				  [
					  'className' => 'Devicedata',
					  'foreignKey' => 'FK_DeviceTemplate',
					  'bindingKey' => 'PK_DeviceTemplate',
					  'targetForeignKey' => 'FK_DeviceData',
					  'joinTable' => 'DeviceTemplate_DeviceData'			  
				  ]);
}
	
}



?>
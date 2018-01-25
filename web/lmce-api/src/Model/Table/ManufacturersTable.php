<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class ManufacturersTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Manufacturer');
	$this->primaryKey('PK_Manufacturer');	
	
			$this->belongsToMany('Devices',
				  [
					  'joinTable' => 'Devicetemplates',
					  'foreignKey' => 'FK_Manufacturer',
					  'bindingKey' => 'PK_Manufacturer',
					  'targetForeignKey' =>'PK_DeviceTemplate',
					  'conditions'=> ['Device.FK_DeviceTemplate' => 'DeviceTemplate.PK_DeviceTemplate']					  
				  ]);
	
		$this->hasMany('DeviceTemplate',
				  [
					  'className' => 'Devicetemplates',
					  'foreignKey' => 'FK_Manufacturer',
					  'bindingKey' => 'PK_Manufacturer'
				  ]);
}
	
}
?>
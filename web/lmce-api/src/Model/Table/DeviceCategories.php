<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class Devicecategories extends Table{
	
public function initialize(array $config){
	$this->setTable('DeviceCategory');
	$this->primaryKey('PK_DeviceCategory');		
}
	
}



?>
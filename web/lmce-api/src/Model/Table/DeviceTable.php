<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class DeviceTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Device');
	$this->primaryKey('PK_Device');
}
	
}



?>
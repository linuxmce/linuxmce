<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class DevicesmanufacturersTable extends Table{
	
public function initialize(array $config){
	
	$this->belongsTo('Devices');
	$this->belongsTo('Devicetemplates');
	$this->belongsTo('Manufacturers');	
}	
	
}
?>
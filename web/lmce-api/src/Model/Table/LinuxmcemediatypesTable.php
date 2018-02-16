<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class LinuxmcemediatypesTable extends Table{
	
public function initialize(array $config){
	$this->setTable('MediaType');
	$this->primaryKey('PK_MediaType');	
	
				$this->hasMany( 'File',
				  [
					  'className'=>	  'Linuxmcefile',
					  'foreignKey' => 'EK_MediaType',
					  'bindingKey' => 'PK_MediaType',
					  'strategy' => 'select'
				  ]
	);
}
	
}
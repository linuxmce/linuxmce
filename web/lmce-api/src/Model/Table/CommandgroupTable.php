<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class CommandgroupTable extends Table{
	
public function initialize(array $config){
	$this->setTable('CommandGroup');
	$this->primaryKey('PK_CommandGroup');	
	
	$this->hasOne('Room',
				  [							 
					  'className' => 'Rooms',
					  'joinTable' => 'Commandgrouproom',
					  'bindingKey' => 'PK_CommandGroup',
					  'foreignKey' => 'PK_Room',
					  'targetForeignKey' =>'FK_Room'
				  ]);	

	}	
}


?>
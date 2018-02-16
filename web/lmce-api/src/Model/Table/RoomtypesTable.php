<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class RoomtypesTable extends Table{
	
public function initialize(array $config){
	$this->setTable('RoomType');
	$this->primaryKey('PK_RoomType');	
	
		$this->hasMany('Rooms',
				  [
					  'className' => 'Rooms',
					  'foreignKey' => 'FK_RoomType',
					  'bindingKey' => 'PK_RoomType'
				  ]
	
	);
	
}
	
}



?>
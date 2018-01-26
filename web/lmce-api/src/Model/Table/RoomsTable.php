<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class RoomsTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Room');
	$this->primaryKey('PK_Room');	
	
		$this->belongsTo('RoomType',
				  [
					  'className' => 'Roomtypes',
					  'foreignKey' => 'FK_RoomType',
					  'bindingKey' => 'PK_RoomType'
				  ]);
	
				$this->hasMany('EntertainArea',
				  [									  
					  'className' => 'Entertainarea',
					  'bindingKey' => 'PK_Room'	,
					  'foreignKey' => 'FK_Room'
				  ]);
	
		$this->belongsToMany('Commandgroup',
				  [									  
					  'through' => 'Commandgrouproom',
					  'bindingKey' => 'PK_Room'	,
					  'foreignKey' => 'FK_Room',
					  'targetForeignKey' => 'FK_CommandGroup'
				  ]);	
	
	
}
	
}



?>
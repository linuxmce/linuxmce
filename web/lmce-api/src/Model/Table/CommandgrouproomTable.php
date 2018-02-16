<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class CommandgrouproomTable extends Table{
	
public function initialize(array $config){
	$this->setTable('CommandGroup_Room');
	//$this->primaryKey('FK_CommandGroup');	
	
		$this->belongsTo('CommandGroup',
				  [	
					  'className' => 'Commandgroup',
					  'foreignKey' => 'PK_CommandGroup'
				  ]);	
	$this->belongsTo('Room',
				  [	
					  'className' => 'Rooms',
					  'foreignKey' => 'PK_Room'
				  ]);	
	}	
}


?>
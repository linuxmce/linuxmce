<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class CommandgroupcommandTable extends Table{
	
public function initialize(array $config){
	$this->setTable('CommandGroup_Command');
	$this->primaryKey('PK_CommandGroup_Command');
	
				$this->belongsTo('CommandGroup',
				  [									  
					 'className' =>'Commandgroup',
					 'foreignKey' =>'FK_CommandGroup'
				  ]);
	
	
			$this->belongsTo('Command',
				  [	
					  'className' => 'Commands',
					  'foreignKey' => 'FK_Command'
				  ]);
	
				$this->belongsTo('Device',
				  [	
					  'className' => 'Devices',
					  'foreignKey' => 'FK_Device'
				  ]);
	
			
	
	
}
}


?>
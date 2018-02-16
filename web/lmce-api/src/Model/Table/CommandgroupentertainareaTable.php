<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class CommandgroupentertainareaTable extends Table{
	
public function initialize(array $config){
	$this->setTable('CommandGroup_EntertainArea');
	
				$this->belongsTo('CommandGroup',
				  [									  
					 'className' =>'Commandgroup',
					  'foreignKey' =>'FK_CommandGroup'
				  ]);
	
	
			$this->belongsTo('EntertainArea',
				  [	
					  'className' => 'Entertainarea',
					  'foreignKey' => 'PK_EntertainArea'
				  ]);
	
}
}
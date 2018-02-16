<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
	
class CommandsTable extends Table{
	
public function initialize(array $config){
	$this->setTable('Command');
	$this->primaryKey('PK_Command');
	
	
	$this->belongsToMany('CommandGroup',
				  [
					  'className'=>'Commandgroup',
					  'joinTable' => 'Commandgroupcommand',
					  'foreignKey' => 'FK_Command',
					  'targetForeignKey' =>'FK_CommandGroup'
				  ]
	);
	
		$this->belongsToMany('Device',
				  [
					  'className'=>'Devices',
					  'joinTable' => 'Commandgroupcommand',
					  'foreignKey' => 'FK_Command',
					  'targetForeignKey' =>'FK_Device'
				  ]
	);
	}
}
	
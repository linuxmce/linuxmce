<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;
	
class EntertainareasTable extends Table {

public function initialize(array $config){
	$this->setTable('EntertainArea');
	$this->primaryKey('PK_EntertainArea');
	
	//	public $hasAndBelongsToMany = array(
//		'CommandGroup' => array(
//			'className' => 'CommandGroup',
//			'joinTable' => 'CommandGroup_Room',
//			'foreignKey' => 'FK_Room',
//			'associationForeignKey' => 'FK_CommandGroup',
//			'unique' => 'keepExisting',
//			'conditions' => '',
//			'fields' => '',
//			'order' => '',
//			'limit' => '',
//			'offset' => '',
//			'finderQuery' => '',
//		)
//	);
}
	
 function getEntertainAreaFromName($name){
	$targetEa =	$this->Model-find()
			->where(['Description LIKE' => $name]);	
	
	if( isset($targetEa) ){
		return $targetEa->PK_EntertainArea;
	}
	
	return -1;
}

}

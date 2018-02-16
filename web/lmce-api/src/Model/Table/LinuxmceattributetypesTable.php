<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmceattributetypesTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'AttributeType');	
	$this->setTable('AttributeType');
	$this->primaryKey('PK_AttributeType');
	
	
}
	
}
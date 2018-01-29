<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmcemediasubtypesTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'MediaSubType');	
	$this->setTable('MediaSubType');
	$this->primaryKey('PK_MediaSubType');
	
					$this->hasMany( 'File',
				  [
					  'className'=>	  'Linuxmcefile',
					  'foreignKey' => 'FK_MediaSubType',
					  'bindingKey' => 'PK_MediaSubType'
				  ]
	);
}
	
	
	

}
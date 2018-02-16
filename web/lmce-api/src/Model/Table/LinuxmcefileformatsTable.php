<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmcefileformatsTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'FileFormat');	
	$this->setTable('FileFormat');
	$this->primaryKey('PK_FileFormat');
	
					$this->hasMany( 'File',
				  [
					  'className'=>	  'Linuxmcefile',
					  'foreignKey' => 'FK_FileFormat',
					  'bindingKey' => 'PK_FileFormat'
				  ]
	);
}
	
	
	

}
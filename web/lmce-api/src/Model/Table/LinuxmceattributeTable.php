<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmceattributesTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'File');	
	$this->setTable('File');
	$this->primaryKey('PK_File');
}
	
	
	

}
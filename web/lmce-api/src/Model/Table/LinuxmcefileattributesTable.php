<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmcefileattributesTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'File_Attribute');	
	$this->setTable('File_Attribute');
	
		$this->belongsTo('File',
					[
						'className' =>'Linuxmcefile'
					]
					);
	$this->belongsTo('Attribute',
					[
						'className'=>'Linuxmceattributes',
						'bindingKey'=>'PK_Attribute'
					]);	
	
	
	
}
}
<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmceattributesTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'Attribute');	
	$this->setTable('Attribute');
	$this->primaryKey('PK_Attribute');
	
		$this->hasOne( 'AttributeType',
				  [
					  'className'=>'Linuxmceattributetypes',
					  'foreignKey' => 'PK_AttributeType',
					  'bindingKey' => 'FK_AttributeType'
				  ]	);
	
			 $this->belongsToMany('File',
				  [
					  'className' => 'Linuxmcefile',
					  'joinTable' => 'Linuxmcefileattributes',
					  'bindingKey' => 'PK_Attribute',
					  'foreignKey'=>'FK_Attribute',
					  'targetForeignKey' =>'FK_File'					  
				  ]);
	
}
	
	
	

}
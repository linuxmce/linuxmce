<?php
namespace App\Model\Table;
	
use App\Model\Table\PlutoMediaTable;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class LinuxmcefileTable extends PlutoMediaTable{
	
public function initialize(array $config){	
	parent::initialize($config);
    $this->table($this->connection()->config()['database'] . "." . 'File');	
	$this->setTable('File');
	$this->primaryKey('PK_File');
	
		$this->hasOne( 'FileFormat',
				  [
					  'className'=>'Linuxmcefileformats',
					  'foreignKey' => 'PK_FileFormat',
					  'bindingKey' => 'FK_FileFormat'
				  ]
	);
	
			$this->hasOne( 'MediaType',
				  [
					  'className'=>'Linuxmcemediatypes',
					  'foreignKey' => 'PK_MediaType',
					  'bindingKey' => 'EK_MediaType',
					  'strategy'=> 'select'
				  ]
	);
	
				$this->hasOne( 'MediaSubType',
				  [
					  'className'=>'Linuxmcemediasubtypes',
					  'foreignKey' => 'PK_MediaSubType',
					  'bindingKey' => 'EK_MediaSubType'
				  ]
	);
	
		 $this->belongsToMany('Attribute',
				  [
					  'className' => 'Linuxmceattributes',
					  'joinTable' => 'Linuxmcefileattributes',
					  'bindingKey' => 'PK_File',
					  'foreignKey'=>'FK_File',
					  'targetForeignKey' =>'FK_Attribute'					  
				  ]);
}
	
	
	function findByMediaType(Query $query, array $options){
		
		$mediaType = $options['mediaType'];		
			return $query
			->contain(['Attribute'])
			->where(['EK_MediaType = ' => $mediaType] );
	}
	
	
	function getVideoByName($name){
		
	$video = $this->find('byMediaType', ['mediaType' => 5]);
	
	$video->matching('Attribute', function($q) use ($name) {
		return $q->where(['Attribute.FK_AttributeType'=>13,  'Name LIKE ' =>$name.'%']);
	});
			
	$returnData = array();
	$returnData['count'] = $video->count();
	
		foreach($video as $title){
			
			$returnData['movies'][]= array(
			'title'=>$title['_matchingData']['Attribute']['Name'],
			'file'=>$title['PK_File'],
				'fileName'=>$title['Path']."/".$title['Filename']
			);
		}
		
	return $returnData;
		
	}
}
<?php
namespace App\Model\Table;
	
use Cake\ORM\Table;
use Cake\ORM\Query;	
use Cake\ORM\TableRegistry;
use Cake\Cache\Cache;

class PlutoMediaTable extends Table{
	
public function initialize(array $config){	
    //$this->table($this->connection()->config()['database'] . "." . 'pluto_media');		
}
public static function defaultConnectionName() {
        return 'pluto_media';
 }
	
}
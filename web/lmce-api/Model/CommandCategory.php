<?php
App::uses('AppModel', 'Model');
/**
 * CommandCategory Model
 
 */
class CommandCategory extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'CommandCategory';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_CommandCategory';


	// The Associations below have been created with all possible keys, those that are not needed can be removed

/**
 * belongsTo associations
 *
 * @var array
 */
 
 public $hasMany = array (
 "Command" => array (
 'className'=> 'Command',
	 'foreignKey' =>'FK_CommandCategory'
 )
 );
}

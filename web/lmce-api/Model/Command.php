<?php
App::uses('AppModel', 'Model');
/**
 * Command Model
 * 
 * @property CommandParameter $CommandParameter
 * @property DeviceCommandGroup $DeviceCommandGroup
 * @property Device $Device
 * @property Device $Device
 */
class Command extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'Command';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_Command';

/**
 * Validation rules
 *
 * @var array
 */
	public $validate = array(
		'Description' => array(
			'notBlank' => array(
				'rule' => array('notBlank'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'Category' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'AVCommand' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'Log' => array(
			'boolean' => array(
				'rule' => array('boolean'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
	);

	// The Associations below have been created with all possible keys, those that are not needed can be removed

/**
 * belongsTo associations
 *
 * @var array
 */

	public $belongsTo = array(
		'CommandCategory' => array (
		 'className'=>'CommandCategory',
		 'foreignKey' => 'FK_CommandCategory'
		)	
	);

/**
 * hasAndBelongsToMany associations
 *
 * @var array
 */
	public $hasAndBelongsToMany = array(
				
		'CommandParameter' => array(
			'className' => 'CommandParameter',
			'joinTable' => 'Command_CommandParameter',
			'foreignKey' => 'FK_Command',
			'associationForeignKey' => 'FK_CommandParameter',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'Device' => array(
			'className' => 'Device',
			'joinTable' => 'Device_Command',
			'foreignKey' => 'FK_Command',
			'associationForeignKey' => 'FK_Device',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		)
	);

}

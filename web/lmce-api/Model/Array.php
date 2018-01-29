<?php
App::uses('AppModel', 'Model');
/**
 * Array Model
 *
 * @property Psc $Psc
 */
class LmceArray extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'LmceArray';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_Array';

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
		'Define' => array(
			'notBlank' => array(
				'rule' => array('notBlank'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'CommandGroup' => array(
			'numeric' => array(
				'rule' => array('numeric'),
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
		'Psc' => array(
			'className' => 'Psc',
			'foreignKey' => 'psc_id',
			'conditions' => '',
			'fields' => '',
			'order' => ''
		)
	);
}

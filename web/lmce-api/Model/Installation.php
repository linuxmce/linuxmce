<?php
App::uses('AppModel', 'Model');
/**
 * Installation Model
 *
 * @property Household $Household
 * @property User $User
 */
class Installation extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'Installation';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_Installation';

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
		'isActive' => array(
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


/**
 * hasAndBelongsToMany associations
 *
 * @var array
 */
	public $hasAndBelongsToMany = array(
		
		'User' => array(
			'className' => 'User',
			'joinTable' => 'Installation_Users',
			'foreignKey' => 'FK_Installation',
			'associationForeignKey' => 'FK_Users',
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

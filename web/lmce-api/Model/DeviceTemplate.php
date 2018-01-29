<?php
App::uses('AppModel', 'Model');
/**
 * DeviceTemplate Model
 *
 * @property DeviceCommandGroup $DeviceCommandGroup
 * @property DeviceData $DeviceData
 * @property Event $Event
 * @property InfraredGroup $InfraredGroup
 * @property MediaType $MediaType
 */
class DeviceTemplate extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'DeviceTemplate';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_DeviceTemplate';

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
		'FK_DeviceCategory' => array(
			'numeric' => array(
				'rule' => array('numeric'),
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
		'ImplementsDCE' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'IsPlugAndPlay' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'IsIPBased' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'IsPlugIn' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'FK_Users_Maintainer' => array(
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

    public $hasMany = array(
        'Device' => array(
            'className' => 'Device',
            'foreignKey' => 'FK_DeviceTemplate',
            'conditions' => '',
            'order' => '',
            'limit' => '',
            'dependent' => ''
        )				
    );

	public $belongsTo = array(
	'DeviceCategory' => array(
	'className' => 'DeviceCategory',
	'foreignKey'=>'FK_DeviceCategory'		
	)
	);

/**
 * hasAndBelongsToMany associations
 *
 * @var array
 */
	public $hasAndBelongsToMany = array(
		
			'DeviceCommandGroup' => array(
			'className' => 'DeviceCommandGroup',
			'joinTable' => 'DeviceTemplate_DeviceCommandGroup',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_DeviceCommandGroup',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => ''
		),
		'DeviceCommandGroup' => array(
			'className' => 'DeviceCommandGroup',
			'joinTable' => 'DeviceTemplate_DeviceCommandGroup',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_DeviceCommandGroup',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'DeviceData' => array(
			'className' => 'DeviceData',
			'joinTable' => 'DeviceTemplate_DeviceData',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_DeviceData',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'Event' => array(
			'className' => 'Event',
			'joinTable' => 'DeviceTemplate_Event',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_Event',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'InfraredGroup' => array(
			'className' => 'InfraredGroup',
			'joinTable' => 'DeviceTemplate_InfraredGroup',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_InfraredGroup',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'MediaType' => array(
			'className' => 'MediaType',
			'joinTable' => 'DeviceTemplate_MediaType',
			'foreignKey' => 'FK_DeviceTemplate',
			'associationForeignKey' => 'FK_MediaType',
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

<?php
App::uses('AppModel', 'Model');
/**
 * Device Model
 *
 
 * @property DeviceCategory $DeviceCategory
 * @property DeviceCommandGroup $DeviceCommandGroup
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property DeviceTemplate $DeviceTemplate
 * @property Command $Command
 * @property CommandGroup $CommandGroup
 * @property DeviceData $DeviceData
 * @property DeviceGroup $DeviceGroup
 * @property EntertainArea $EntertainArea
 */
class Device extends AppModel {

public $_hidden = array('psc_user', 'FK_DesignObj');
	
	
/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'Device';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_Device';

/**
 * Validation rules
 *
 * @var array
 */
	public $validate = array(
		'Disabled' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
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
		'IPaddress' => array(
			'notBlank' => array(
				'rule' => array('notBlank'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'IgnoreOnOff' => array(
			'numeric' => array(
				'rule' => array('numeric'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'State' => array(
			'notBlank' => array(
				'rule' => array('notBlank'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'Status' => array(
			'notBlank' => array(
				'rule' => array('notBlank'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'IsNewDevice' => array(
			'boolean' => array(
				'rule' => array('boolean'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'ManuallyConfigureEA' => array(
			'boolean' => array(
				'rule' => array('boolean'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
		'Registered' => array(
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

/**
 * hasAndBelongsToMany associations
 *
 * @var array
 */
	public $hasAndBelongsToMany = array(

		'DeviceData' => array(
			'className' => 'DeviceData',
			'joinTable' => 'Device_DeviceData',
			'foreignKey' => 'FK_Device',
			'associationForeignKey' => 'FK_DeviceData',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		)
	);
	
	public $belongsTo = array(
	'DeviceTemplate'=> array (
	'className' => 'DeviceTemplate',
	'foreignKey' => 'FK_DeviceTemplate',
		'primaryKey'=>'FK_DeviceTemplate'
	)
	);
	
//	public function beforeFind($queryData){
//		
//		  if (empty($queryData['fields'])) {
//        $queryData['fields'] = array(
//            'Description',
//			'PK_Device',
//			'FK_DeviceTemplate',			
//            'Disabled',
//            'FK_Room',
//			'State',
//			'Status'
//        );
//    }
//	return $queryData;
//		
//	}

}

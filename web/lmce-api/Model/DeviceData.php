<?php
App::uses('AppModel', 'Model');
/**
 * DeviceData Model
 *
 * @property Psc $Psc
 * @property DHCPDevice $DHCPDevice
 * @property DeviceCategory $DeviceCategory
 * @property DeviceTemplate $DeviceTemplate
 * @property Device $Device
 */
class DeviceData extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'DeviceData';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_DeviceData';

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
		'FK_ParameterType' => array(
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

/**
 * hasAndBelongsToMany associations
 *
 * @var array
 */
	public $hasAndBelongsToMany = array(
		'DHCPDevice' => array(
			'className' => 'DHCPDevice',
			'joinTable' => 'DHCPDevice_DeviceData',
			'foreignKey' => 'device_data_id',
			'associationForeignKey' => 'd_h_c_p_device_id',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'DeviceCategory' => array(
			'className' => 'DeviceCategory',
			'joinTable' => 'DeviceCategory_DeviceData',
			'foreignKey' => 'device_data_id',
			'associationForeignKey' => 'device_category_id',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		),
		'DeviceTemplate' => array(
			'className' => 'DeviceTemplate',
			'joinTable' => 'DeviceTemplate_DeviceData',
			'foreignKey' => 'device_data_id',
			'associationForeignKey' => 'device_template_id',
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
			'joinTable' => 'Device_DeviceData',
			'foreignKey' => 'device_data_id',
			'associationForeignKey' => 'device_id',
			'unique' => 'keepExisting',
			'conditions' => '',
			'fields' => '',
			'order' => '',
			'limit' => '',
			'offset' => '',
			'finderQuery' => '',
		)
	);
	
	public function beforeFind($queryData){
		  if (empty($queryData['fields'])) {
        $queryData['fields'] = array(
            'Description',
            'FK_ParameterType',
            'FK_Room',
			'State',
			'Status'
        );
    }
	return $queryData;
		
	}
}

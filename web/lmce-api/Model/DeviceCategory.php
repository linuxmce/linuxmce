<?php
App::uses('AppModel', 'Model');
/**
 * DeviceCategory Model
 *
 * @property DeviceData $DeviceData
 * @property Event $Event
 */
class DeviceCategory extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'DeviceCategory';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'PK_DeviceCategory';

/**
 * Validation rules
 *
 * @var array
 */
	public $validate = array(
		'IsAV' => array(
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
		'DeviceData' => array(
			'className' => 'DeviceData',
			'joinTable' => 'DeviceCategory_DeviceData',
			'foreignKey' => 'device_category_id',
			'associationForeignKey' => 'device_data_id',
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
			'joinTable' => 'DeviceCategory_Event',
			'foreignKey' => 'device_category_id',
			'associationForeignKey' => 'event_id',
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

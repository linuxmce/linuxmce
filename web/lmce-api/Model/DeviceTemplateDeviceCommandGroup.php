<?php
App::uses('AppModel', 'Model');
/**
 * DeviceTemplateDeviceCommandGroup Model
 *
 *
 */
class DeviceTemplateDeviceCommandGroup extends AppModel {

/**
 * Use table
 *
 * @var mixed False or table name
 */
	public $useTable = 'DeviceTemplate_DeviceCommandGroup';

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'FK_DeviceTemplate';

/**
 * Validation rules
 *
 * @var array
 */
	public $validate = array(
		'FK_DeviceCommandGroup' => array(
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

}

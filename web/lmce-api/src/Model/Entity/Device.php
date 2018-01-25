<?php
namespace App\Model\Entity;

use Cake\ORM\Entity;

class Device extends Entity
{
    protected $_virtual = [
		'MACaddress',
		'FK_DesignObj',
		'FK_DeviceControlledVia',
		'IgnoreOnOff',
		'FK_Device_Routeto',
		'EK_Licensed_Device',
		'psc_id',
		'psc_batch',
		'psc_user',
		'psc_restrict',
		'psc_mod'
						  ];
}

?>
<?php
App::uses('AppModel', 'Model');

class CommandDevice extends AppModel {
	
    public $belongsTo = array(
        'Command', 'DeviceTemplate'
    );
}

?>
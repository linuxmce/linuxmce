<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/weborbiter.inc.php');

$deviceID=(int)@$_REQUEST['deviceID'];
$x=(int)@$_REQUEST['x'];
$y=(int)@$_REQUEST['y'];

if($deviceID==0){
	die('Orbiter not specified');
}


print get_orbiter_pic($deviceID,$dbADO,$x,$y);

function get_orbiter_pic($deviceID,$dbADO,$x,$y){
	$GLOBALS['headerImage']=1;
	return get_web_orbiter_screen($deviceID,$dbADO,1);
}
?>
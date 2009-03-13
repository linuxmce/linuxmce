<?php
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
$interval=(int)@$_REQUEST['interval'];
$interval=($interval<=0)?5:$interval;
header("Refresh: $interval;url={$_SERVER['PHP_SELF']}?{$_SERVER['QUERY_STRING']}"); 

session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');


$cameraID=(int)@$_REQUEST['cameraID'];
$installationID = (int)@$_SESSION['installationID'];

print get_video_frame($cameraID,$installationID,$dbADO);

function get_video_frame($cameraid,$installationID,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/viewCameras.lang.php');
	
	if($cameraid==0){
		return 'Camera not found.';
	}

	$securityDevicePlugin=getFieldsAsArray('Device','PK_Device',$dbADO,'WHERE FK_DeviceTemplate='.$GLOBALS['SecurityPlugin'].' AND FK_Installation='.$installationID);
	$securityPluginDD=getFieldsAsArray('Device_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.(int)@$securityDevicePlugin['PK_Device'][0].' AND FK_DeviceData='.$GLOBALS['Path']);

	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o -p "/var/www/pluto-admin/security_images" 0 '.$cameraid.' 1 84 19 0 20 "jpg" 23 0 31 0 60 100 61 100';
	$ret=exec_batch_command($cmd,1);
	$retArray=explode("\n",$ret);

	$msg=$TEXT_ERROR_CAMERA_SCREENSHOT_NOT_AVAILABLE_CONST;
	foreach ($retArray as $line) {
		if(ereg(':OK',$line)){
			exec('mv /var/www/pluto-admin/security_images/19.out /var/www/pluto-admin/security_images/'.$cameraid.'.jpg',$retArray,$retCode);
			$msg='<img src="security_images/'.$cameraid.'.jpg" alt="'.$TEXT_LOADING_CONST.'"/>';
		}
	}

	return $msg;
}
?>

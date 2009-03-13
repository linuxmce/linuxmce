<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/weborbiter.inc.php');

// retrieve variables
$userID=(int)@$_REQUEST['userID'];
$passMD5=@$_REQUEST['pass'];
$installationID=(int)@$_REQUEST['installationID'];
$deviceID=(int)@$_REQUEST['deviceID'];



// check if user is logged
if(is_valid_user($userID,$passMD5,$dbADO)){
	// get device data for device, to setup refresh and iframe use
	$ddArray=get_device_data($deviceID,$dbADO);
	
	// refresh is hardcoded to 5 seconds, if it's not set in device data
	$refresh=(isset($ddArray[$GLOBALS['RefreshInterval']]))?(int)$ddArray[$GLOBALS['RefreshInterval']]:5;
	
	$out=get_web_orbiter($installationID,$deviceID,$userID,$passMD5,$dbADO);
}else{
	$refresh=0;
	$out=login_form($dbADO,(int)@$_REQUEST['err']);
}

output_html($out,$dbADO,$refresh);
?>
<?php
session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$deviceID=(int)@$_REQUEST['deviceID'];
if($deviceID==0){
	die('ERROR: Device ID not specified.');
}

$tmpfile='device_'.$deviceID.'.txt';
$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h dcerouter -C '.$deviceID.' -e /tmp/'.$tmpfile;
$response=exec_batch_command($cmd,1);

if(file_exists('/tmp/'.$tmpfile)){
	header('Content-type: application/force-download');
	header('Content-Transfer-Encoding: Binary');
	header('Content-disposition: attachment; filename='.$tmpfile);
	readfile('/tmp/'.$tmpfile);
}else{
	die('ERROR: Temporary file not generated: <hr>'.nl2br($response));
}
?>
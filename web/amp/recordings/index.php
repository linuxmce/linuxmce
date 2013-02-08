<?php
// *********** autologin for pluto admin ******************
session_name('LMCE-admin');
session_start();

if(isset($_SESSION['Extension'])){
	unset($_SESSION['ari_user']);
	$_POST['username']=$_SESSION['Extension'];
	$_POST['password']=read_pass_from_file($_SESSION['Extension'],'/etc/asterisk/voicemail.conf');
}

function read_pass_from_file($username,$file){
	$lines = file($file);
	foreach ($lines as $key => $line) {

		unset($value);
		list($var,$value) = split('=>',$line);
		$var = trim($var);

		if ($var==$username && $value) {
			$buf = split(',',$value);
			return $buf[0];
		}
	}
	return false;
}
// *********** end autologin ******************

/**
 * @file
 * main
 */

include_once("includes/bootstrap.inc");
ari_page_header();
include_once("includes/common.inc");

handler();

ari_page_footer();


?>




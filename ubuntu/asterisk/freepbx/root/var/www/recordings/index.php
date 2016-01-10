<?php
// *********** autologin for pluto admin ******************
session_name('Web-admin');
session_start();


$extension=(int)@$_SESSION['Extension'];
if(isset($_SESSION['Extension'])){
	unset($_SESSION['ari_user']);
	$_POST['username']=$extension;
	$_POST['password']=read_pass_from_file($extension,'/etc/asterisk/voicemail.conf');
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
#session_start();
#var_dump($_SESSION);
include_once("includes/bootstrap.php");
ariPageHeader();
include_once("includes/common.php");

handler();

ariPageFooter();


?>




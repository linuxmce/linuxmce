<?php
  	
  	// language to fall back if a CONST does not exist in users language, or if the whole language file is missing
  	$GLOBALS['fallbacklang']='en';
  	
  	// temp to not break old php files not yet translated
  	$GLOBALS['lang']=$GLOBALS['fallbacklang'];
  	
  	// set language to user language if logged in, else set it to fallback language
  	if((isset($_SESSION['userLoggedIn'])) && ($_SESSION['userLoggedIn']== true)) {
  		if(isset($_SESSION['UserLanguage']))
  			$GLOBALS['userlang']=$_SESSION['UserLanguage'];  	
  		else
  			$GLOBALS['userlang']=$GLOBALS['fallbacklang'];	
  	} else {
  		$GLOBALS['userlang']=$GLOBALS['fallbacklang'];
  	}
  	$_SESSION['skin']=0;
  	
  	error_reporting(E_ALL ^ E_NOTICE);
	$GLOBALS['sqlCVS_Server']='schema.linuxmce.org';
  	$GLOBALS['globalConfigPath']='/var/www/lmce-admin/include/config/';
  	$GLOBALS['floorplansPath']='/var/www/lmce-admin/floorplans';
	$GLOBALS['contactPicturesPath']='/var/www/lmce-admin/contacts';
	$GLOBALS['usersPicsPath']='/var/www/lmce-admin/users/';
  	$GLOBALS['roomsPicsPath']='/var/www/lmce-admin/rooms/';
  	$GLOBALS['scenariosPicsPath']='/var/www/lmce-admin/scenarios/';
  	$GLOBALS['pluto.conf']='/etc/pluto.conf';
  	$GLOBALS['wap.conf']='/etc/wap.conf';
  	$GLOBALS['pluto_callerid.conf']='/etc/pluto-callerid.conf';
  	$GLOBALS['ErrorLog']='/var/log/pluto/lmce-admin.log';
  	$GLOBALS['databasesArray']=array('pluto_main_devel'=>'pluto_main','pluto_myth_devel'=>'pluto_myth');

  	// display loading time at the bottom of the page
  	$GLOBALS['display_loading_time']=0;

  	// set it to path with write permission to enable loading time logging orr leave blank to disable
  	$GLOBALS['save_loading_time']='/var/www/lmce-admin/cached/';

  	// log file for external calls
  	$GLOBALS['WebExecLogFile']='/var/log/pluto/webExecLog.log';

  	// if debug config exist, use it to override svn settings
	if(file_exists('include/config/config.debug.inc.php')){
		include('include/config/config.debug.inc.php');
	}

  	if($_SESSION['skin']==0){
		$_SESSION['cssFile']='include/styles/main.css';
	}else{
		$_SESSION['cssFile']='include/styles/main_red.css';
  	}
  	
  	include_once($GLOBALS['globalConfigPath'].'globalconfig.inc.php');
  	
  	$GLOBALS['sendErrorsTo']='INSERT_VALID_EMAIL_HERE';
	
	$directory="lmce-admin";
	$domain="http://localhost";
	
	// path where the media pics and orbiter backgrounds are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/lmce-admin/mediapics/';
	$GLOBALS['orbiterBackgroundPath']=$_SERVER['DOCUMENT_ROOT'].'/lmce-admin/orbiter_bg/';
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","LinuxMCE");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/");
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');
	
	//define("LANG","ro-RO"); // wtf ? :-)
	
	setlocale(LC_ALL, NULL); // NULL should use system (kunbuntu) settings ...

	//database
	require_once("database.inc.php");
	
	// hard coded values
	require_once("hardCoded.php");
	
?>

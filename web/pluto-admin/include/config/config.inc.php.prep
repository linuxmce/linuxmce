<?PHP
  	
  	// todo: set lang when translation is finished
  	$GLOBALS['lang']='en';
  	$_SESSION['skin']=0;
  	
  	error_reporting(E_ALL ^ E_NOTICE);
  	$GLOBALS['globalConfigPath']='/var/www/pluto-admin/include/config/';
  	$GLOBALS['floorplansPath']='/var/www/pluto-admin/floorplans';
  	$GLOBALS['usersPicsPath']='/var/www/pluto-admin/users/';
  	$GLOBALS['roomsPicsPath']='/var/www/pluto-admin/rooms/';
  	$GLOBALS['scenariosPicsPath']='/var/www/pluto-admin/scenarios/';
  	$GLOBALS['pluto.conf']='/etc/pluto.conf';
  	$GLOBALS['wap.conf']='/etc/wap.conf';
  	$GLOBALS['pluto_callerid.conf']='/etc/pluto-callerid.conf';
  	$GLOBALS['ErrorLog']='/var/log/pluto/pluto-admin.log';
  	$GLOBALS['databasesArray']=array('pluto_main_devel'=>'pluto_main','pluto_myth_devel'=>'pluto_myth');

  	// display loading time at the bottom of the page
  	$GLOBALS['display_loading_time']=0;

  	// set it to path with write permission to enable loading time logging orr leave blank to disable
  	$GLOBALS['save_loading_time']='/var/www/pluto-admin/cached/';

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
  	
	$GLOBALS['sendErrorsTo']='support@plutohome.com';
	
	$directory="/pluto-admin/";
	$domain="http://localhost";
	
	// path where the media pics and orbiter backgrounds are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/mediapics/';
	$GLOBALS['orbiterBackgroundPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/orbiter_bg/';
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","<-mkr_t_name_mixed->");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/");
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');
	
	
	
	define("LANG","ro-RO");
	
	
	setlocale(LC_ALL, LANG);

	//database
	require_once("database.inc.php");
	
	// hard coded values
	require_once("hardCoded.php");
	
?>

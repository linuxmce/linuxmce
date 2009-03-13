<?
function upnpServer($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/upnpServer.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	
	if($action=='form'){
	
		$ip=$_SERVER['REMOTE_ADDR'];
		
		// if IP is local, get the iframe with upnp server, else display error message
		$data=exec_batch_command('sudo -u root /usr/pluto/bin/Network_DisplaySettings.sh --all',1);
		$ret=parse_ini_str($data);

		$out.='<iframe src="http://'.$_SERVER['HTTP_HOST'].':3877/" style="width:98%;height:600px;"></iframe>';
		/*
		// config changed to allow external access
		if(isset($ret['INTERNAL_IP']) && same_network($ip,$ret['INTERNAL_IP'],$ret['INTERNAL_NETMASK'])){
			$out.='<iframe src="http://192.168.80.1:3877/" style="width:98%;height:600px;"></iframe>';
		}else{
			$out.='<span class="err">'.$TEXT_UPNPSERVER_NOT_AVAILABLE_CONST.'</span>';	
		}
		*/		
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=upnpServer&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		
		header('Location: index.php?section=upnpServer');
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_UPNPSERVER_CONST);
	$output->setNavigationMenu(array($TEXT_UPNPSERVER_CONST=>'index.php?section=upnpServer'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_UPNPSERVER_CONST);
	$output->output();
}
?>

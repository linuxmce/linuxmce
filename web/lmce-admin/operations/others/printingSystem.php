<?php
function printingSystem($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('printingSystem.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	
	if($action=='form'){
		exec("pgrep cupsd", $ignore, $return);
		if($return != 0){
			$out='<span class="err">'.translate('TEXT_PRINTING_SYSTEM_NOT_FOUND_CONST').'</span>';
		}else{
			$ip=$_SERVER['REMOTE_ADDR'];	
			$out.='<iframe src="https://'.$_SERVER['HTTP_HOST'].':631/" style="width:98%;height:600px;"></iframe>';
		}	
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=printingSystem&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}	

		
		header('Location: index.php?section=printingSystem');
		exit();
	}

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_PRINTING_SYSTEM_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_PRINTING_SYSTEM_CONST')=>'index.php?section=printingSystem'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_PRINTING_SYSTEM_CONST'));
	$output->output();
}
?>

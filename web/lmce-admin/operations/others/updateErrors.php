<?
function updateErrors($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/updateErrors.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	
		
	if($action=='form'){
		$out.='
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<form action="index.php" method="POST" name="updateErrors">
		<input type="hidden" name="section" value="updateErrors">
		<input type="hidden" name="action" value="add">
		
		<table width="600" cellspacing="0" cellpadding="0">';
		$computers=getDevicesArrayFromCategory($GLOBALS['rootComputerID'],$dbADO);
		
		foreach ($computers as $id=>$label) {
			$deviceDataText=getDeviceData($id,$GLOBALS['DD_updateErrors'],$dbADO);
			$model=getDeviceData($id,$GLOBALS['DD_model'],$dbADO);
			$lastUpdate=getDeviceData($id,$GLOBALS['DD_LastUpdate'],$dbADO);
			
			$errors=($deviceDataText!='')?$deviceDataText:$TEXT_NO_ERRORS_CONST;
			$out.='
			<tr  class="tablehead">
				<td><b>'.$label.' - '.$model.'</b></td>
				<td align="right"><b>Update # '.$lastUpdate.'</b></td>
			</tr>
			<tr>
				<td colspan="2">'.$errors.'</td>
			</tr>
			
			';
		}
		
		$out.='
		</table>
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=updateErrors&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_UPDATE_ERRORS_CONST);
	
	$output->setNavigationMenu(array($TEXT_UPDATE_ERRORS_CONST=>'index.php?section=updateErrors'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_UPDATE_ERRORS_CONST);
	$output->output();
}


?>
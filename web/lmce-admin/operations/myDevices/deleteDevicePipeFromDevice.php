<?php
function deleteDevicePipeFromDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteDevicePipeFromDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$deviceFromID = (int)$_REQUEST['deviceFromID'];
	$deviceToID = (int)$_REQUEST['deviceToID'];
	$pipe=((int)$_REQUEST['pipe']>0)?(int)$_REQUEST['pipe']:NULL;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		
			if ((int)$deviceFromID!=0 && $deviceToID!=0) {
				
				$deleteObjFromDevice = 'DELETE FROM Device_Device_Pipe WHERE FK_Device_From = ? and FK_Device_To = ? AND FK_Pipe=? ';
				$query = $dbADO->Execute($deleteObjFromDevice,array($deviceFromID,$deviceToID,$pipe));
				
				$out.="
				<script>
					alert('$TEXT_DEVICE_PIPE_REMOVED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {
				$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
			}		
			
		
	} else {
			$out = $TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST.'<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
	}
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
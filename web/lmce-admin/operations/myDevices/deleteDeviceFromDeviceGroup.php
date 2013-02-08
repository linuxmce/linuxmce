<?php
function deleteDeviceFromDeviceGroup($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteDeviceFromDeviceGroup.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$deviceID = (int)$_REQUEST['deviceID'];
	$deviceGroupID = (int)$_REQUEST['deviceGroupID'];
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		
			if ((int)$deviceID!=0 && $deviceGroupID!=0) {
				$deleteObjFromDevice = 'delete from Device_DeviceGroup where FK_Device = ? and FK_DeviceGroup = ?';
				$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$deviceGroupID));
				
				$out.="
				<script>
					alert('$TEXT_DEVICE_REMOVED_FROM_DEVICEGROUP_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {
				$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
			}		
			
		
	} else {
			$out = 'You are not allowed to do that!<a href="javascript:window.close();">Close</a>';
	}
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
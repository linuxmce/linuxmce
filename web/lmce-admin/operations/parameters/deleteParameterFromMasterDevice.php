<?php
function deleteParameterFromMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteParameterFromMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['deviceData'];
	$deviceID = (int)$_REQUEST['deviceID'];
		
	if ((int)$objID!=0 && $deviceID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceData where FK_DeviceTemplate = ? and FK_DeviceData = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$objID));
			
			$out.="
			<script>
				alert('$TEXT_PARAMETER_DELETED_FROM_DT_CONST');
			    opener.document.forms.{$from}.action.value='update';
				opener.document.forms.{$from}.lastAction.value='newDeviceData';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = $TEXT_NOTHING_TO_DELETE_CONST.'&nbsp;<a href="javascript:window.close();">Close</a>';
		}			
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function deleteObjectFromMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteObjectFromMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';	
	$deviceID = (int)$_REQUEST['deviceID'];
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0 && (int)$deviceID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DesignObj where FK_DeviceTemplate=? and FK_DesignObj=?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$objID));
			$out.="
			<script>
				alert('$TEXT_OBJECT_DELETED_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function deleteControlledViaCategoryFromMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteControlledViaCategoryFromMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceCategory_ControlledVia where PK_DeviceTemplate_Devicecategory_ControlledVia = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			$out.="
			<script>
				alert('$TEXT_CONTROLLED_VIA_CATEGORY_DELETED_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = $TEXT_NOTHING_TO_DELETE_CONST.'.&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
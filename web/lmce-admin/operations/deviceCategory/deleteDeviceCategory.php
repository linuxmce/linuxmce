<?php
function deleteDeviceCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteDeviceCategory.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);	
	$manufacturerID = (int)$_REQUEST['manufacturers'];
	$deviceCategSelected = (int)$_REQUEST['deviceCategSelected'];
	$deviceSelected = (int)$_REQUEST['deviceSelected'];
	$modelSelected = (int)$_REQUEST['model'];
	
	// check if is owner of category

	getDeviceCategoryChildsArray($deviceSelected,$dbADO);

	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceSelected;

	if ((int)$deviceSelected!=0) {
		$deleteObjFromDevice = 'delete from DeviceCategory where PK_DeviceCategory in ('.join(",",$GLOBALS['childsDeviceCategoryArray']).')';
		$query = $dbADO->_Execute($deleteObjFromDevice);
		$out.="
				<script>
					alert('$TEXT_DEVICE_CATEGORY_DELETED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
	} else {
		$out = $TEXT_NOTHING_TO_DELETE_CONST.'&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
	}

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function deletePipeFromDeviceTemplateControlledViaCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deletePipeFromDeviceTemplateControlledViaCategory.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
	$pipe= (int)$_REQUEST['pipe'];
	
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'DELETE FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe WHERE FK_DeviceTemplate_DeviceCategory_ControlledVia = ? AND FK_Pipe=?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID,$pipe));
			$out.="
			<script>
				alert('$TEXT_PIPE_DELETED_FROM_DC_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();				
				self.close();
				window.close();
			</script>
			";			
			$out .= '<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
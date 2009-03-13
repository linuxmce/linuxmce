<?php
function deletePipeFromDeviceTemplateControlledVia($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deletePipeFromDeviceTemplateControlledVia.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceTemplate_ControlledVia_Pipe  where FK_DeviceTemplate_DeviceTemplate_ControlledVia = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			$message=($dbADO->Affected_Rows()==1)?$TEXT_PIPE_DELETED_FROM_DT_CONST:$TEXT_PIPE_NOT_DELETED_FROM_DT_CONST;
			$out.="
			<script>
				alert('".$message." ');
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
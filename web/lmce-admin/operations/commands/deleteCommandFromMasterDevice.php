<?php
function deleteCommandFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$commandID = (int)$_REQUEST['commandID'];
	$commandGroupID = (int)$_REQUEST['commandGroupID'];
	
		
	if ($commandID!=0 && $commandGroupID!=0) {
			$deleteObjFromDevice = 'delete from DeviceCommandGroup_Command where FK_Command = ? and FK_DeviceCommandGroup= ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($commandID,$commandGroupID));
			$out.="
			<script>
				alert('$TEXT_PARAMETER_DELETED_CONST');
			    opener.document.forms.{$from}.action.value='update';
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
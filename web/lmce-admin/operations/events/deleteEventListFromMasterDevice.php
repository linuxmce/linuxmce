<?php
function deleteEventFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['Event'];
	$deviceID = (int)$_REQUEST['deviceID'];
		
	if ((int)$objID!=0 && $deviceID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_Event where FK_DeviceTemplate = ? and FK_Event = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$objID));
			$out.="
			<script>
				alert('Event list deleted from master device!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='newEventToMasterDevice';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}			
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function deleteEventParameterFromEvent($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$EventID = (int)$_REQUEST['EventID'];
	$eventParameterID = (int)$_REQUEST['eventParameterID'];
		
	if ($EventID!=0 && $eventParameterID!=0) {
			$deleteObjFromDevice = 'delete from Event_EventParameter where FK_EventParameter = ? and FK_Event = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($eventParameterID,$EventID));
			$out.="
			<script>
				alert('Event parameter deleted from event list!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='addEventParameterToEvent';
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
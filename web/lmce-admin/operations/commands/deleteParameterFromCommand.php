<?php
function deleteParameterFromCommand($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteParameterFromCommand.lang.php');

	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$commandParameterID = (int)$_REQUEST['commandParameterID'];
	$commandID = (int)$_REQUEST['commandID'];
		
	if ((int)$commandParameterID!=0 && $commandID!=0) {
			$deleteObjFromDevice = 'delete from Command_CommandParameter where FK_Command = ? and FK_CommandParameter = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($commandID,$commandParameterID));
			$out.="
			<script>
				alert('$TEXT_PARAMETER_DELETED_FROM_COMMAND_CONST');
			    opener.document.forms.{$from}.action.value='update';
				opener.document.forms.{$from}.lastAction = 'addNewParameterToCommand';
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
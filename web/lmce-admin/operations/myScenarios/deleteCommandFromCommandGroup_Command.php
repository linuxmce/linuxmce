<?php
function deleteCommandFromCommandGroup_Command($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteCommandFromCommandGroup_Command.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['cgID'];
	
	$commandsDeleted='';
	$parametersDeleted='';
	if ($objID!=0) {
			$deleteObjFromDevice = 'delete from CommandGroup_Command where 
				PK_CommandGroup_Command = ?
			';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			//delete saved values
			if ($dbADO->Affected_Rows()>0) {
				$commandsDeleted = $TEXT_COMMAND_REMOVED_FROM_COMMAND_GROUP_CONST;
				$deleteParamValues = 'delete from CommandGroup_Command_CommandParameter where 
					FK_CommandGroup_Command = ?
				';
				$query = $dbADO->Execute($deleteParamValues,array($objID));
				if ($dbADO->Affected_Rows()>0) {
					$parametersDeleted = $TEXT_PARAMETERS_DELETED_FROM_COMMAND_COMMAND_GROUP_CONST;
				} else {
					$parametersDeleted = $TEXT_NO_PARAMETER_VALUES_DELETED_CONST;
				}
			
			} else {
				$commandsDeleted = $TEXT_COMMAND_NOT_REMOVED_FROM_COMMAND_GROUP_CONST;
			}
			
			
			
			
			
			$out.="
			<script>
				alert('$commandsDeleted $parametersDeleted');
			    opener.document.forms[0].action.value='form';				
				opener.document.forms[0].submit();
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
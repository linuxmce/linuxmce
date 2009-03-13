<?php
function editEventCommands($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editEventCommands.lang.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];

	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting 
		FROM EventHandler
			LEFT JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			LEFT JOIN Criteria ON FK_Criteria=PK_Criteria
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	$rowEH=$resEH->FetchRow();
	$commandGroupID = $rowEH['FK_CommandGroup'];
	$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];

	if(isset($_REQUEST['cgcID']) && (int)$_REQUEST['cgcID']!=0){
		$objID=$_REQUEST['cgcID'];
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editRespondToEvent&error=You are not authorised to modify installation.");
			exit();
		}
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteObjFromDevice,$objID);

		$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteParamValues,$objID);
		
		$_REQUEST['msg']='The command was deleted from the command group.';
	}
	
	
	if ($action=='form') {
		$parmlistToDisplay=(isset($_POST['cannedEvent']) && (int)$_POST['cannedEvent']!=0)?(int)$_POST['cannedEvent']:$rowEH['FK_CannedEvents'];		
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editEventCommands">
		<input type="hidden" name="section" value="editEventCommands">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		
		<table border="0" align="center">
		';
		$out.='
			<tr bgcolor="#E7E7E7">
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td>'.$rowEH['Description'].'</td>
			</tr>
			<tr>
				<td colspan="2">'.editCommandGroupCommands($commandGroupID,$dbADO).'</td>
			</tr>
			<tr>
				<td colspan="2">'.addCommandToCommandGroup($commandGroupID,$dbADO).'</td>
			</tr>
		</table>
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editEventCommands&ehID=".$eventHandlerID."&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}

		list ($isModified,$parametersUpdatedAlert)=processEditScenarioCommands($commandGroupID,$dbADO);		
		
		header('Location: index.php?section=editEventCommands&ehID='.$eventHandlerID.'&msg='.$TEXT_EVENT_HANDLER_UPDATED_CONST);
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_EVENT_COMMANDS_CONST);	
	$output->setNavigationMenu(array($TEXT_ADVANCED_EVENTS_DEVICE_CONST=>'index.php?section=advancedEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_EVENT_COMMANDS_CONST);
	$output->output();
}
?>

<?php
function advancedEvents($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('advancedEvents.lang.php');
	include('eventUtils.inc.php');
		
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	$timedEventArray=array(1=>'Interval', 2=>'Day of week',3=>'Day of month',4=>'Absolute');
	
	$highligh=@$_REQUEST['highligh'];
	
	if((isset($_REQUEST['dID']) && $_REQUEST['dID']!=0) || (isset($_REQUEST['enID']) && $_REQUEST['enID']!=0)){
		if(!$canModifyInstallation){
			header("Location: index.php?section=advancedEvents&error=You are not authorised to modify installation.");
			exit();
		}

		if ($_REQUEST['dID']) {
		$eventHandlerID=$_REQUEST['dID'];
		$queryEventsHandler='
			SELECT EventHandler.*, Criteria.FK_CriteriaParmNesting 
			FROM EventHandler 
				LEFT JOIN Criteria ON FK_Criteria=PK_Criteria
			WHERE PK_EventHandler=?';
		$resEH=$dbADO->Execute($queryEventsHandler,$eventHandlerID);
		if($resEH->RecordCount()!=0){
			$rowEH=$resEH->FetchRow();
			deleteCommandGroup($rowEH['FK_CommandGroup'],$dbADO);
			
			if ($rowEH['FK_CriteriaParmNesting'] != '') {
				$deleteCriteriaParmNesting='DELETE FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?';
				$dbADO->Execute($deleteCriteriaParmNesting,$rowEH['FK_CriteriaParmNesting']);
			}

			if ($rowEH['FK_Criteria'] != '') {
				$deleteCriteria='DELETE FROM Criteria WHERE PK_Criteria=?';
				$dbADO->Execute($deleteCriteria,$rowEH['FK_Criteria']);
			}

			if ($rowEH['FK_CriteriaParmNesting'] != '') {
				$deleteCriteriaParms='DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=?';
				$dbADO->Execute($deleteCriteriaParms,$rowEH['FK_CriteriaParmNesting']);
			}

			$deleteEventHandler='DELETE FROM EventHandler WHERE PK_EventHandler=?';
			$dbADO->Execute($deleteEventHandler,$eventHandlerID);

			reloadEventPlugin($installationID, $dbADO);
			
			$_REQUEST['msg']=translate('TEXT_THE_EVENT_WAS_DELETED_CONST');
		}
		} else if ($_REQUEST['enID']) {
			$queryEventsHandler='UPDATE EventHandler SET Disabled=IF(Disabled=0,1,0)
				WHERE PK_EventHandler=?';
			$dbADO->Execute($queryEventsHandler,$_REQUEST['enID']);
		}
	}
	if ($action=='form') {
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="advancedEvents">
		<input type="hidden" name="section" value="advancedEvents">
		<input type="hidden" name="action" value="add">
		
		<table border="0" align="center" cellpading="3">
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_EVENT_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_FK_EVENT_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_TIMED_EVENT_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_TEMPLATE_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_ENABLED_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_CRITERIA_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_COMMANDS_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_ACTION_CONST').'</B></td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.*, Event.Description AS EventDescription, CannedEvents.Description AS TemplateDescription 
			FROM EventHandler
			LEFT JOIN Event ON EventHandler.FK_Event=PK_Event
			LEFT JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			WHERE EventHandler.FK_Installation=? AND FK_Template IS NULL
		';
		$resEvents=$dbADO->Execute($queryEvents,$installationID);
		if($resEvents->RecordCount()==0){
			$out.='
				<tr>
					<td colspan="8">No records.</td>
				</tr>';
		}
		$lineCount=0;
		$displayedEH=array();
		while($rowEvents=$resEvents->FetchRow()){
			$displayedEH[]=$rowEvents['PK_EventHandler'];
			$lineCount++;
			$color=(($lineCount%2==0)?'#E7E7E7':'#FFFFFF');
			$color=($highligh==$rowEvents['PK_EventHandler'])?'lightgreen':$color;
			$out.='
				<tr bgcolor="'.$color.'">
					<td align="center">'.$rowEvents['PK_EventHandler'].'</td>
					<td><input type="text" name="eventDescription_'.$rowEvents['PK_EventHandler'].'" value="'.$rowEvents['Description'].'"></td>
					<td>'.$rowEvents['EventDescription'].'</td>
					<td>'.@$timedEventArray[$rowEvents['TimedEvent']].'</td>
					<td>'.$rowEvents['TemplateDescription'].'</td>
					<td><center><a href="index.php?section=advancedEvents&enID='.$rowEvents['PK_EventHandler'].'">';
					$out .= $rowEvents['Disabled']?translate('TEXT_FALSE_CONST'):translate('TEXT_TRUE_CONST');
					$out .= '</a></center></td>';
			if ($rowEvents['FK_Criteria']) {
					$out .= '<td align="center"><a href="index.php?section=editCriteria&ehID='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_EDIT_CONST').'</a></td>';
			} else {
					$out .= '<td>&nbsp;</td>';
			}
					$out .= '<td align="center"><a href="index.php?section=editEventCommands&ehID='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_EDIT_CONST').'</a></td>';
					$out .= '<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete the event?\'))self.location=\'index.php?section=advancedEvents&dID='.$rowEvents['PK_EventHandler'].'\'">'.translate('TEXT_DELETE_CONST').'</a></td>';
				$out .= '</tr>';
		}
		$saveBtns=(count($displayedEH)!=0)?'<input type="submit" class="button" name="update" value="'.translate('TEXT_SAVE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'">':'';
		
		$out.='
			<tr bgcolor="#E7E7E7">
				<td colspan="8" align="center"><input type="button" class="button" name="addAdvancedEvent" value="'.translate('TEXT_CREATE_ADVANCED_EVENT_CONST').'" onClick="self.location=\'index.php?section=createAdvancedEvent\'"> '.$saveBtns.'</td>
			</tr>
			<tr>
				<td colspan="8">&nbsp;</td>
			</tr>';
		$out.='
		</table>
			<input type="hidden" name="displayedEH" value="'.join(',',$displayedEH).'">
		</form>
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=advancedEvents&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}
		$displayedEHArray=explode(',',$_POST['displayedEH']);
		foreach ($displayedEHArray AS $ehID){
			$description=cleanString($_POST['eventDescription_'.$ehID]);
			$dbADO->Execute('UPDATE EventHandler SET Description=? WHERE PK_EventHandler=?',array($description,$ehID));
		}
		
		reloadEventPlugin($installationID, $dbADO);

		header("Location: index.php?section=advancedEvents&msg=".translate('TEXT_THE_EVENT_DESCRIPTION_WAS_UPDATED_CONST'));
	}

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_ADVANCED_EVENTS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_ADVANCED_EVENTS_CONST')=>'index.php?section=advancedEvents'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_ADVANCED_EVENTS_CONST'));			
	$output->output();
}
?>

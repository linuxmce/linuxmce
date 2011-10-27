<?php
function respondToEvents($output,$dbADO) {
	// include language file
	includeLangFile('common.lang.php');
	includeLangFile('respondToEvents.lang.php');
	include('eventUtils.inc.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if(isset($_REQUEST['dID']) && $_REQUEST['dID']!=0){
		if(!$canModifyInstallation){
			header("Location: index.php?section=respondToEvents&error=You are not authorised to modify installation.");
			exit();
		}

		$eventHandlerID=$_REQUEST['dID'];
		$queryEventsHandler='
			SELECT EventHandler.*, Criteria.FK_CriteriaParmNesting
			FROM EventHandler
				INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			WHERE PK_EventHandler=? AND TimedEvent IS NULL';
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
			
			$_REQUEST['msg']='The event handler was deleted.';
			reloadEventPlugin($installationID, $dbADO);
		}
	}
	if ($action=='form') {
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="respondToEvents">
		<input type="hidden" name="section" value="respondToEvents">
		<input type="hidden" name="action" value="add">
		
		<table border="0" align="center">
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_EVENT_CONST').'</B></td>
				<td align="center" colspan=3><B>'.translate('TEXT_ACTION_CONST').'</B></td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.*, CannedEvents.Description AS CannedEvent
			FROM EventHandler
				LEFT JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			WHERE EventHandler.FK_Installation=? AND TimedEvent IS NULL AND FK_Template IS NULL
		';
		$resEvents=$dbADO->Execute($queryEvents,$installationID);
		if($resEvents->RecordCount()==0){
			$out.='
				<tr>
					<td colspan="5">'.translate('TEXT_NO_RECORDS_CONST').'</td>
				</tr>';
		}
		$lineCount=0;
		while($rowEvents=$resEvents->FetchRow()){
			$lineCount++;
			$out.='
				<tr bgcolor="'.(($lineCount%2==0)?'#E7E7E7':'#FFFFFF').'">
					<td>'.$rowEvents['Description'].'</td>
					<td>'.$rowEvents['CannedEvent'].'</td>
					<td><a href="index.php?section=advancedEvents&highligh='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_ADVANCED_CONST').'</a></td> ';
			if ($rowEvents['FK_CannedEvents']) {
						$out .= '<td><a href="index.php?section=editRespondToEvent&ehID='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_EDIT_CONST').'</a></td>';
			} else {
						$out .= '<td>&nbsp;</td>';
			}
						$out .= '<td><a href="#" onClick="if(confirm(\''.translate('TEXT_CONFIRM_DELETE_EVENT_CONST').'\'))self.location=\'index.php?section=respondToEvents&dID='.$rowEvents['PK_EventHandler'].'\'">'.translate('TEXT_DELETE_CONST').'</a></td>';
				$out .= '</tr>';
		}
		$queryCannedEvents='SELECT * FROM CannedEvents ORDER By Description ASC';
		$resCannedEvents=$dbADO->Execute($queryCannedEvents);
		$out.='
			<tr>
				<td colspan="5">&nbsp;</td>
			</tr>
			<tr>
				<td><B>'.translate('TEXT_DESCRIPTION_CONST').' *</B></td>
				<td colspan="4"><input type="text" name="Description" value=""></td>
			</tr>		
			<tr>
				<td><B>'.translate('TEXT_NEW_EVENT_CONST').' *</B></td>
				<td colspan="4"><select name="cannedEvent">
					<option value="0">- '.translate('TEXT_PLEASE_SELECT_AN_EVENT_CONST').' -</option>';

		while($rowCE=$resCannedEvents->FetchRow()){
			$out.='<option value="'.$rowCE['PK_CannedEvents'].'">'.$rowCE['Description'].'</option>';
		}
		$out.='
			</select></td>
			</tr>
			<tr>
				<td colspan="5" align="center"><input type="submit" class="button" name="continue" value="'.translate('TEXT_ADD_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
			</tr>';
		$out.='
		</table>
		</form>
		<em>* '.translate('TEXT_REQUIRED_FIELDS_CONST').'</em>
		<script>
		 	var frmvalidator = new formValidator("respondToEvents");			
 			frmvalidator.addValidation("Description","req","'.translate('TEXT_EVENT_DESCRIPTION_REQUIRED_CONST').'");
			frmvalidator.addValidation("cannedEvent","dontselect=0","'.translate('TEXT_EVENTTYPE_REQUIRED_CONST').'");
		</script>		
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventHandler&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}

		$description=cleanString($_POST['Description']);
		$cannedEvent=cleanInteger($_POST['cannedEvent']);
		
		$res=$dbADO->Execute('SELECT * FROM CannedEvents_CriteriaParmList WHERE FK_CannedEvents=?',array($cannedEvent));
		if($res->RecordCount()>0){
			$insertCriteriaParmNesting='INSERT INTO CriteriaParmNesting (IsAnd,IsNot) VALUES (1,0)';
			$dbADO->Execute($insertCriteriaParmNesting);
			$insertID=$dbADO->Insert_ID();
			
			$insertCriteria='INSERT INTO Criteria(FK_CriteriaParmNesting,FK_CriteriaList,Description,FK_Installation) VALUES (?,?,?,?)';
			$dbADO->Execute($insertCriteria,array($insertID,$GLOBALS['EventCriteriaList'],'event',$installationID));
			$criteriaID=$dbADO->Insert_ID();
		}else{
			// null as string since it's used in a composed query
			$criteriaID='NULL';
		}
	
				
		$insertCommandGroup='
			INSERT INTO CommandGroup 
				(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
			VALUES
				(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['EventsHandlerArray'],$installationID,'Event ActionGroup',0,0,0,$GLOBALS['EventsHandlerTemplate']));
		$cgID=$dbADO->Insert_ID();
		
		$insertEventHandler="
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents,FK_Event)
			SELECT '$description',$criteriaID,$installationID,$cgID,$cannedEvent,FK_Event FROM CannedEvents WHERE PK_CannedEvents=?";
		$dbADO->Execute($insertEventHandler,$cannedEvent);
		$ehID=$dbADO->Insert_ID();
		reloadEventPlugin($installationID, $dbADO);
		header("Location: index.php?section=editRespondToEvent&ehID=".$ehID);
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_RESPOND_TO_EVENTS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_RESPOND_TO_EVENTS_CONST')=>'index.php?section=respondToEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_RESPOND_TO_EVENTS_CONST'));			
	$output->output();
}
?>

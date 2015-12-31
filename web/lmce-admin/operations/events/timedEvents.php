<?php
function timedEvents($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('timedEvents.lang.php');
	include('eventUtils.inc.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$timedEventType = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	if (!$lastAction && $action != "add") { //fix bug 618 - Timed Events adding new entries by itself
	  $action="form";
	}
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if(isset($_REQUEST['dID']) && $_REQUEST['dID']!=0){
		if(!$canModifyInstallation){
			header("Location: index.php?section=timedEvents&error=You are not authorised to modify installation.");
			exit();
		}

		$eventHandlerID=$_REQUEST['dID'];
		$queryEventsHandler='
			SELECT EventHandler.*, Criteria.FK_CriteriaParmNesting 
			FROM EventHandler 
				INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			WHERE PK_EventHandler=? AND TimedEvent IS NOT NULL';
		$resEH=$dbADO->Execute($queryEventsHandler,$eventHandlerID);
		if($resEH->RecordCount()!=0){
			$rowEH=$resEH->FetchRow();
			deleteCommandGroup($rowEH['FK_CommandGroup'],$dbADO);
			
			$deleteCriteriaParmNesting='DELETE FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?';
			$dbADO->Execute($deleteCriteriaParmNesting,$rowEH['FK_CriteriaParmNesting']);
			
			$deleteCriteria='DELETE FROM Criteria WHERE PK_Criteria=?';
			$dbADO->Execute($deleteCriteria,$rowEH['FK_Criteria']);
			
			$deleteCriteriaParms='DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=?';
			$dbADO->Execute($deleteCriteriaParms,$rowEH['FK_CriteriaParmNesting']);
			
			$deleteEventHandler='DELETE FROM EventHandler WHERE PK_EventHandler=?';
			$dbADO->Execute($deleteEventHandler,$eventHandlerID);
			
			$_REQUEST['msg']=translate('TEXT_DELETE_EVENT_NOTIFICATION_CONST');
			reloadEventPlugin($installationID, $dbADO);
		}
	}
	if ($action=='form') {
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="timedEvents">
		<input type="hidden" name="section" value="timedEvents">
		<input type="hidden" name="action" value="add">
		
		<table border="0" align="center" cellpading="3">
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
				<td align="center">&nbsp;</td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.* FROM EventHandler
			WHERE EventHandler.FK_Installation=? AND TimedEvent IS NOT NULL
		';
		$resEvents=$dbADO->Execute($queryEvents,$installationID);
		if($resEvents->RecordCount()==0){
			$out.='
				<tr>
					<td colspan="3">'.translate('TEXT_NO_RECORDS_CONST').'</td>
				</tr>';
		}
		$lineCount=0;
		while($rowEvents=$resEvents->FetchRow()){
			$lineCount++;
			$out.='
				<tr bgcolor="'.(($lineCount%2==0)?'#E7E7E7':'#FFFFFF').'">
					<td align="center">'.$rowEvents['Description'].'</td>
					<td align="center">
						<a href="index.php?section=advancedEvents&highligh='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_ADVANCED_CONST').'</a>
						<a href="index.php?section=editTimedEvent&ehID='.$rowEvents['PK_EventHandler'].'">'.translate('TEXT_EDIT_CONST').'</a>
						<a href="#" onClick="if(confirm(\'Are you sure you want to delete the event?\'))self.location=\'index.php?section=timedEvents&dID='.$rowEvents['PK_EventHandler'].'\'">'.translate('TEXT_DELETE_CONST').'</a></td>
				</tr>';
		}
		$out.='
			<tr>
				<td colspan="3">&nbsp;</td>
			</tr>
			<tr>
				<td><B>'.translate('TEXT_NEW_TIMED_EVENT_DESCRIPTION_CONST').'</B>:</td>
				<td colspan="2"><input type="text" name="Description" value=""></td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="1" checked> <B>'.translate('TEXT_INTERVAL_BASED_CONST').':</B></td>
				<td colspan="2">'.translate('TEXT_INTERVAL_BASED_TEXT_CONST').'</td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="2"> <B>'.translate('TEXT_DAY_OF_WEEK_BASED_CONST').':</B></td>
				<td colspan="2">'.translate('TEXT_DAY_OF_WEEK_BASED_TEXT_CONST').'</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="3"> <B>'.translate('TEXT_DAY_OF_MONTH_BASED_CONST').':</B></td>
				<td colspan="2">'.translate('TEXT_DAY_OF_MONTH_BASED_TEXT_CONST').'</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="4"> <B>'.translate('TEXT_ABSOLUTE_CONST').':</B></td>
				<td colspan="2">'.translate('TEXT_ABSOLUTE_TEXT_CONST').'</td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="continue" value="'.translate('TEXT_ADD_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
			</tr>';
		$out.='
		</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("timedEvents");			
 			frmvalidator.addValidation("Description","req","'.translate('TEXT_VALIDATION_DESCRIPTION_CONST').'");
		</script>		
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventHandler&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}

		$description=cleanString($_POST['Description']);
		$timedEventType=cleanInteger($_POST['timedEventType']);
		$insertCriteriaParmNesting='INSERT INTO CriteriaParmNesting (IsAnd,IsNot) VALUES (1,0)';
		$dbADO->Execute($insertCriteriaParmNesting);
		$insertID=$dbADO->Insert_ID();
		
		$insertCriteria='INSERT INTO Criteria(FK_CriteriaParmNesting,FK_CriteriaList,Description,FK_Installation) VALUES (?,?,?,?)';
		$dbADO->Execute($insertCriteria,array($insertID,$GLOBALS['EventCriteriaList'],'event',$installationID));
		$criteriaID=$dbADO->Insert_ID();
		
		$insertCommandGroup='
			INSERT INTO CommandGroup 
				(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
			VALUES
				(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['EventsHandlerArray'],$installationID,'Event ActionGroup',0,0,0,$GLOBALS['EventsHandlerTemplate']));
		$cgID=$dbADO->Insert_ID();
		
		$insertEventHandler="
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents,FK_Event,TimedEvent)
			VALUES
				(?,?,?,?,?,?,?) ";
		$dbADO->Execute($insertEventHandler,array($description,$criteriaID,$installationID,$cgID,NULL,NULL,$timedEventType));
		$ehID=$dbADO->Insert_ID();
		reloadEventPlugin($installationID, $dbADO);
		header("Location: index.php?section=editTimedEvent&ehID=".$ehID);
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_TIMED_EVENTS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_TIMED_EVENTS_CONST')=>'index.php?section=timedEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_TIMED_EVENTS_CONST'));			
	$output->output();
}
?>

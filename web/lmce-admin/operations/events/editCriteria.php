<?php
function editCriteria($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editCriteria.lang.php');
	include('eventUtils.inc.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];
	$GLOBALS['displayedCriteriaParms']=array();

	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting, Criteria.PK_Criteria, Criteria.Description AS CriteriaDescription, CriteriaList.Description AS CriteriaListDescription 
		FROM EventHandler
			INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			INNER JOIN CriteriaList ON FK_CriteriaList=PK_CriteriaList
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	if($resEH->RecordCount()>0){
		$rowEH=$resEH->FetchRow();
		$commandGroupID = $rowEH['FK_CommandGroup'];
		$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];
	}
	
	if ($action=='form') {
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		
		<form action="index.php" method="post" name="editCriteria">
		<input type="hidden" name="section" value="editCriteria">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		<input type="hidden" name="changedCP" value="">';
		
		if($FK_CriteriaParmNesting>0){
			$out.='
		
		<table border="0" align="center">';
		$out.='
			<tr>
				<td><B><a href="index.php?section=editEventCommands&ehID='.$eventHandlerID.'">'.$TEXT_EDIT_EVENT_COMMANDS_CONST.'</a></B></td>
			</tr>
			<tr>
				<td align="right"><B>'.$TEXT_CRITERIA_CONST.'</B></td>
				<td><input type="text" name="criteriaDescription" value="'.$rowEH['CriteriaDescription'].'"></td>
				<td><B>'.$TEXT_TEMPLATE_CONST.'</B></td>
				<td>'.$rowEH['CriteriaListDescription'].'</td>
			</tr>
			<tr>
				<td colspan="4">
			';
		$out.=displayCriteria($FK_CriteriaParmNesting,$eventHandlerID,$installationID,$dbADO);
		$out.='</td>
			</tr>
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
			</tr>
		</table>';
		}else{
			$out.='<div class="err" align="center">'.$TEXT_CRITERIA_NOT_AVAILABLE_CONST.'</div>';
		}
		$out.='
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editCriteria&ehID=".$eventHandlerID."&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}

		if(isset($_GET['dcpID'])){
			$dcpID=$_GET['dcpID'];
			$dbADO->Execute('DELETE FROM CriteriaParm WHERE PK_CriteriaParm=?',$dcpID);
			reloadEventPlugin($installationID, $dbADO);
			header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg='.urlencode($TEXT_CRITERIA_DELETED_CONST));
			exit();
		}
	
		if(isset($_REQUEST['fkCPN'])){
			$fkCPN=$_REQUEST['fkCPN'];
			$CriteriaParmListArray=array_keys(getAssocArray('CriteriaParmList','PK_CriteriaParmList','Description',$dbADO,'','ORDER BY Description ASC LIMIT 0,1'));
			if(count($CriteriaParmListArray)==0){
				error_redirect('No criteria parm list available.','index.php?section=editCriteria&ehID='.$eventHandlerID);
			}
			$dbADO->Execute('INSERT INTO CriteriaParm (FK_CriteriaParmNesting,FK_CriteriaParmList) VALUES(?,?)',array($fkCPN,$CriteriaParmListArray[0]));
		}
		
		$changedCP=(int)@$_POST['changedCP'];
		if($changedCP>0){
			$newCPL=((int)@$_POST['CriteriaParmID_'.$changedCP]>0)?(int)$_POST['CriteriaParmID_'.$changedCP]:NULL;
			$newParm=($newCPL==$GLOBALS['PK_EventParameterParmList']||$newCPL==$GLOBALS['CPL_State'])?(isset($_POST['CriteriaParmParm_'.$changedCP])?(int)$_POST['CriteriaParmParm_'.$changedCP]:NULL):NULL;
			if ($newCPL==$GLOBALS['CPL_State'] && isset($_POST['CriteriaParmParm_'.$changedCP.'_2'])) {
			         $newParm.='.'.$_POST['CriteriaParmParm_'.$changedCP.'_2'];
			}
			$dbADO->Execute('UPDATE CriteriaParm SET FK_CriteriaParmList=?, Parm=?,Value=NULL, Operator=1 WHERE PK_CriteriaParm=?',array($newCPL,$newParm,$changedCP));
		}

		if(isset($_POST['update'])){
			$displayedCriteriaParmArray=explode(',',$_POST['displayedCriteriaParm']);
			foreach ($displayedCriteriaParmArray AS $cpID){
				$operator=@$_POST['CriteriaParmOperator_'.$cpID];
				$value=@$_POST['CriteriaParmValue_'.$cpID];
				$dbADO->Execute('UPDATE CriteriaParm SET Operator=?, Value=? WHERE PK_CriteriaParm=?',array($operator,$value,$cpID));
			}
			reloadEventPlugin($installationID, $dbADO);
		}

		
		if(isset($_REQUEST['chCPN'])){
			$chCPN=(int)$_REQUEST['chCPN'];

			if(isset($_REQUEST['setNot'])){
				$setNot=(int)$_REQUEST['setNot'];
				$dbADO->Execute('UPDATE CriteriaParmNesting SET IsNot=? WHERE PK_CriteriaParmNesting=?',array($setNot,$chCPN));
			}
			if(isset($_REQUEST['setAnd'])){
				$setAnd=(int)$_REQUEST['setAnd'];
				$dbADO->Execute('UPDATE CriteriaParmNesting SET IsAnd=? WHERE PK_CriteriaParmNesting=?',array($setAnd,$chCPN));
			}
		}
		
		if(isset($_REQUEST['delCPN'])){
			$cpnID=$_REQUEST['delCPN'];
			if($cpnID==$FK_CriteriaParmNesting){
				header("Location: index.php?section=editCriteria&ehID=".$eventHandlerID."&error=$TEXT_ERROR_CANNOT_DELETE_ROOT_LEVEL_NESTING_CONST");
				exit();
			}
			deleteCriteriaParmNesting($cpnID,$dbADO);
			header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg='.$TEXT_THE_NESTING_DELETED_CONST);
		}
		
		
		if(isset($_REQUEST['parentCPN'])){
			$parentCPN=((int)$_REQUEST['parentCPN']>0)?(int)$_REQUEST['parentCPN']:NULL;
			$dbADO->Execute('INSERT INTO CriteriaParmNesting (FK_CriteriaParmNesting_Parent,IsAnd,IsNot) VALUES (?,0,0)',$parentCPN);
		}
		
		header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg='.$TEXT_CRITERIA_UPDATED_CONST);
	}
	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_CRITERIA_CONST.' #'.$eventHandlerID.' '.$rowEH['Description']);	
	$output->setNavigationMenu(array($TEXT_ADVANCED_EVENTS_CONST=>'index.php?section=advancedEvents&highligh='.$eventHandlerID));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_CRITERIA_CONST);	
	$output->output();
}
?>

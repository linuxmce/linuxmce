<?php
function editRespondToEvent($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('editRespondToEvent.lang.php');
	include('eventUtils.inc.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];
	$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:2;
	$section=$_REQUEST['section'];
	
	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting 
		FROM EventHandler
			INNER JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
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

		reloadEventPlugin($installationID, $dbADO);

		$_REQUEST['msg']='The command was deleted from the command group.';
	}
	
	if ($action=='form') {
		$parmlistToDisplay=(isset($_POST['cannedEvent']) && (int)$_POST['cannedEvent']!=0)?(int)$_POST['cannedEvent']:$rowEH['FK_CannedEvents'];		
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editRespondToEvent">
		<input type="hidden" name="section" value="editRespondToEvent">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		<input type="hidden" name="oldWizard" value="'.$wizard.'">
		
		<div align="center"><h3>'.translate('TEXT_EDIT_RESPOND_TO_EVENT_CONST').'</h3></div>
		<table border="0" align="center" width="800">
		';
		$out.='
			<tr class="alternate_back">
				<td><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
				<td><input type="text" name="description" value="'.$rowEH['Description'].'"></td>
			</tr>';
		$queryCannedEvents='SELECT * FROM CannedEvents ORDER By Description ASC';
		$resCannedEvents=$dbADO->Execute($queryCannedEvents);
		$out.='
			<tr>
				<td>'.translate('TEXT_EVENT_CONST').':</td>
				<td colspan="2"><select name="cannedEvent" onChange="document.editRespondToEvent.action.value=\'form\'; document.editRespondToEvent.submit();">';

		while($rowCE=$resCannedEvents->FetchRow()){
			$out.='<option value="'.$rowCE['PK_CannedEvents'].'" '.(($rowCE['PK_CannedEvents']==$parmlistToDisplay)?'selected':'').'>'.$rowCE['Description'].'</option>';
		}

		$out.='
			</select></td>
			</tr>';
			$displayedCE_CP=array();
			$queryCriteriaParmlist='
				SELECT CannedEvents_CriteriaParmList.Description, CriteriaParmList.Description AS PK_Table, CriteriaParmList.FK_ParameterType,CannedEvents_CriteriaParmList.Operator,PK_CannedEvents_CriteriaParmList,CannedEvents_CriteriaParmList.FK_CriteriaParmList,DefaultValue,ExtraInfo,Parm
				FROM CannedEvents_CriteriaParmList 
					INNER JOIN CriteriaParmList ON CannedEvents_CriteriaParmList.FK_CriteriaParmList=PK_CriteriaParmList 
				WHERE FK_CannedEvents=?';

			$resCriteriaParmlist=$dbADO->Execute($queryCriteriaParmlist,$parmlistToDisplay);
			while($rowCriteria=$resCriteriaParmlist->FetchRow()){
				$defaultParm=$rowCriteria['Parm'];
				$queryOldValues='SELECT * FROM CriteriaParm WHERE FK_CannedEvents_CriteriaParmList=? AND FK_CriteriaParmNesting=?';
				$resOldValues=$dbADO->Execute($queryOldValues,array($rowCriteria['PK_CannedEvents_CriteriaParmList'],$FK_CriteriaParmNesting));
				if($resOldValues->RecordCount()>0){
					$rowOldValues=$resOldValues->FetchRow();
					$oldOperator=$rowOldValues['Operator'];
					$oldValue=$rowOldValues['Value'];
					$oldParm=$rowOldValues['Parm'];
				}else{
					$oldOperator='';
					$oldValue='';
					$oldParm='';
				}
				$displayedCE_CP[]=$rowCriteria['PK_CannedEvents_CriteriaParmList'];
				$selectedOperator=(!isset($oldOperator))?$rowCriteria['Operator']:@$oldOperator;
				$selectedValue=(!isset($oldValue))?$rowCriteria['DefaultValue']:@$oldValue;
				$out.='<input type="hidden" name="CriteriaParmList_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" value="'.$rowCriteria['FK_CriteriaParmList'].'">';
				$out.='
				<tr>
					<td>'.$rowCriteria['Description'].'</td>
					<td>';
				if($rowCriteria['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$resEventParameter=$dbADO->Execute('SELECT * FROM EventParameter ORDER BY Description ASC');
					$out.='Event parameter: <select name="CriteriaParmParm_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" onChange="document.editRespondToEvent.action.value=\'form\';document.editRespondToEvent.action.value=\'form\';document.editRespondToEvent.submit();">
						<option value="0" ></option>';
					while($rowEP=$resEventParameter->FetchRow()){
						if(isset($_POST['CriteriaParmParm_'.$rowCriteria['PK_CannedEvents_CriteriaParmList']])){
							$isSelectedParm=($rowEP['PK_EventParameter']==$_POST['CriteriaParmParm_'.$rowCriteria['PK_CannedEvents_CriteriaParmList']])?'selected':'';
						}
						else{	
							if($oldParm==''){
								$isSelectedParm=($defaultParm==$rowEP['PK_EventParameter'])?'selected':'';	// check Default value
							}
							else
								$isSelectedParm=($oldParm==$rowEP['PK_EventParameter'])?'selected':'';	// check old value
						}
						$out.='<option value="'.$rowEP['PK_EventParameter'].'" '.$isSelectedParm.'>'.$rowEP['Description'].'</option>';						
						if($isSelectedParm=='selected')				
							$selectedEventParameter=$rowEP['Description'];
					}
					$out.='</select>';
				}
				$out.='
					<select name="CriteriaParmOperator_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
						<option value="1" '.(($selectedOperator==1)?'selected':'').'>=</option>
						<option value="2" '.(($selectedOperator==2)?'selected':'').'>&lt;&gt;</option>
						<option value="3" '.(($selectedOperator==3)?'selected':'').'>&gt;</option>
						<option value="4" '.(($selectedOperator==4)?'selected':'').'>&lt;</option>
						<option value="5" '.(($selectedOperator==5)?'selected':'').'>contain</option>
					</select> ';
				
				if($rowCriteria['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$out.='Value: ';
					switch(@$selectedEventParameter){
						case 'PK_Device':
						case 'PK_Room':
							$tableName=substr($selectedEventParameter,3);
							$out.=generatePullDown('CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'],$tableName,'PK_'.$tableName,'Description',$selectedValue,$dbADO,"WHERE FK_Installation='$installationID'");
						break;
						default:			
							$out.='<input type="textbox" name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" value="'.$oldValue.'">';
						break;
					}
				}else{
					if(substr($rowCriteria['PK_Table'],0,3)=='PK_'){
						$out.='<select name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
							<option value="0" ></option>';
						$tableName=substr($rowCriteria['PK_Table'],3);
						$filterTable=($tableName=='Device' || $tableName=='Room')?" WHERE FK_Installation='".$installationID."' ":'';
						$queryTable="SELECT * FROM $tableName $filterTable ORDER BY Description ASC";
						$resTable=$dbADO->Execute($queryTable);
						while($rowTable=$resTable->FetchRow()){
							$out.='<option value="'.$rowTable[$rowCriteria['PK_Table']].'" '.(($selectedValue==$rowTable[$rowCriteria['PK_Table']])?'selected':'').'>'.$rowTable['Description'].'</option>';
						}
						$out.='</select>';
					}elseif($rowCriteria['PK_Table']=='State'){
						$stateArray=array(5=>'Afternoon',1=>'Daylight',6=>'Evening',3=>'Morning',7=>'Night',2=>'Not Daylight',9=>'Weekday',8=>'Weekend');
						$out.='<select name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
									<option value=""></option>';
							foreach($stateArray AS $key => $value){
								$out.='<option value="'.$key.'" '.(($selectedValue==$key)?'selected':'').'>'.$value.'</option>';
							}
						$out.='			
						</select>';
					}
					else{
						$out.='<input type="text" name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" value="'.$oldValue.'">';
					}
				}
				$out.=($rowCriteria['ExtraInfo']!='')?' '.translate('TEXT_EXTRA_INFO_CONST').': '.$rowCriteria['ExtraInfo']:'';
				$out.='
					</td>
				</tr>				
				';
			}
			$out.='
			<input type="hidden" name="displayedCE_CP" value="'.join(',',$displayedCE_CP).'">
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="continue" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
			</tr>
			<tr>
				<td colspan="3" align="center">'.translate('TEXT_RESPOND_TO_EVENT_NOTES_CONST').'</td>
			</tr>
			';
		$out.='
		<tr>
			<td colspan="2">';
		$out.='
			<table align="center">
			<tr bgcolor="#D1D9EA">
				<td><B>'.translate('TEXT_ADD_COMMANDS_USING_WIZARD_CONST').':</B> <select name="wizard" onChange="document.editRespondToEvent.submit();">
					<option value="0" '.(($wizard==0)?'selected':'').'>'.translate('TEXT_LIGHTING_WIZARD_CONST').'</option>
					<option value="1" '.(($wizard==1)?'selected':'').'>'.translate('TEXT_CLIMATE_WIZARD_CONST').'</option>
					<option value="2" '.(($wizard==2)?'selected':'').'>'.translate('TEXT_ADVANCED_WIZARD_CONST').'</option>
				</select>
				</td>
			</tr>
		</table>
		<center>';
	switch ($wizard){
		case 0:
			$out.=lightingDevicesTable($commandGroupID,$dbADO);
		break;
		case 1:
			$out.=climateDevicesTable($commandGroupID,$dbADO);
		break;
		case 2:
			$out.=advancedCommandGroupCommandsTable($commandGroupID,$section,$dbADO);
		break;
	}
		
		$out.='</center>
			</td>
		</tr>
		</table>
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editRespondToEvent&ehID=".$eventHandlerID."&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}
		$description=cleanString($_POST['description']);
		$cannedEvent=(int)$_POST['cannedEvent'];
		$displayedCE_CPArray=explode(',',$_POST['displayedCE_CP']);
		foreach($displayedCE_CPArray AS $elem){
			$operator=@$_POST['CriteriaParmOperator_'.$elem];
			$value=@$_POST['CriteriaParmValue_'.$elem];
			$parm=isset($_POST['CriteriaParmParm_'.$elem])?$_POST['CriteriaParmParm_'.$elem]:NULL;
			$criteriaParmList=@$_POST['CriteriaParmList_'.$elem];
			
			$queryOldValues='SELECT * FROM CriteriaParm WHERE FK_CannedEvents_CriteriaParmList=? AND FK_CriteriaParmNesting=?';
			$resOldValues=$dbADO->Execute($queryOldValues,array($elem,$FK_CriteriaParmNesting));
			if($resOldValues->RecordCount()==0){
				// insert new values
				if((int)@$FK_CriteriaParmNesting>0){
					$insertCriteriaParm='
						INSERT INTO CriteriaParm 
							(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value,FK_CannedEvents_CriteriaParmList,Parm)
						VALUES
							(?,?,?,?,?,?)';
					$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$criteriaParmList,$operator,$value,$elem,$parm));
				}
			}else{
				$rowOldValues=$resOldValues->FetchRow();
				$oldCriteriaParmList=$rowOldValues['FK_CriteriaParmList'];
				$oldOperator=$rowOldValues['Operator'];
				$oldValue=$rowOldValues['Value'];
				$oldParm=$rowOldValues['Parm'];
				// update if changed

				if($criteriaParmList!=$oldCriteriaParmList || $operator!=$oldOperator || $value!=$oldValue || $parm!=$oldParm){
					$updateCriteriaParm='
						UPDATE CriteriaParm 
							SET FK_CriteriaParmNesting=?, FK_CriteriaParmList=?, Operator=?, Value=?,Parm=? 
						WHERE PK_CriteriaParm=?';
					$dbADO->Execute($updateCriteriaParm,array($FK_CriteriaParmNesting,$criteriaParmList,$operator,$value,$parm,$rowOldValues['PK_CriteriaParm']));
				}
			}			
		}

		// update EventHandler
		$updateEventHandler='UPDATE EventHandler SET Description=?, FK_CannedEvents=? WHERE PK_EventHandler=?';
		$dbADO->Execute($updateEventHandler,array($description,$cannedEvent,$eventHandlerID));

	
		$oldWizard=(int)$_POST['oldWizard'];
		$wizard=(int)$_POST['wizard'];
		
		switch($oldWizard){
			case 0:
				processLightingScenario($commandGroupID,$dbADO);
				$msg=translate('TEXT_THE_EVENT_HANDLER_WAS_UPDATED_CONST');
			break;
			case 1:
				processClimateScenario($commandGroupID,$dbADO);
				$msg=translate('TEXT_THE_EVENT_HANDLER_WAS_UPDATED_CONST');
			break;
			case 2:
				list ($isModified,$parametersUpdatedAlert,$returnHook)=processAdvancedScenarios($commandGroupID,$section,$dbADO);
				$msg=($isModified?translate('TEXT_COMMAND_GROUP_UPDATED_CONST'):translate('TEXT_COMMAND_GROUP_NOT_UPDATED_CONST'))." $parametersUpdatedAlert";
			break;	
		}
	
		reloadEventPlugin($installationID, $dbADO);

		setOrbitersNeedConfigure($installationID,$dbADO);
		if((int)@$_REQUEST['addNewDevice']!=0)
			$sufix='&addNewDevice='.(int)@$_REQUEST['addNewDevice'];
		header("Location: index.php?section=editRespondToEvent&ehID=$eventHandlerID&wizard=$wizard".@$sufix);
		exit();
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_EDIT_RESPOND_TO_EVENT_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_RESPOND_TO_EVENTS_CONST')=>'index.php?section=respondToEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

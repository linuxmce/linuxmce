<?php
function irrigationScenarios($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/irrigationScenarios.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$installationID = (int)@$_SESSION['installationID'];
$arrayID = $GLOBALS['ArrayIDForIrrigation'];	

$out='';
if($action=='form') {
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();

	$out.=setLeftMenu($dbADO).'
	<div align="center" class="err">'.strip_tags(@$_GET['error']).'</div>
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>';

	$out.='
	<form action="index.php" method="POST" name="irrigationScenarios">
		<input type="hidden" name="section" value="irrigationScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="roomID" value="">
		<input type="hidden" name="editedCgID" value="">	
		<input type="hidden" name="roomName" value="">
	
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	$queryRooms='
		SELECT Room.*, Room.Description AS RoomName FROM Room
		WHERE FK_Installation=?
		ORDER BY Room.Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	if($resRooms->RecordCount()==0){
		$out.=$TEXT_IRRIGATION_SCENARIOS_NO_ROOMS_CONST.' <a href="index.php?section=rooms">'.$TEXT_GOTO_ROOMS_CONST.'</a>';
	}
	$displayedCommandGroups=array();
	$firstRoomArray=array();
	while($rowRooms=$resRooms->FetchRow()){
		$out.='
		<tr bgcolor="#D1D9EA">
			<td><B>'.$rowRooms['RoomName'].'</B></td>
			<td>&nbsp;</td>
			<td align="right"><input type="button" class="button" name="add" value="'.$TEXT_ADD_IRRIGATION_SCENARIO_CONST.'" onClick="javascript:document.irrigationScenarios.action.value=\'addToRoom\';document.irrigationScenarios.roomID.value='.$rowRooms['PK_Room'].';document.irrigationScenarios.roomName.value=\''.urlencode($rowRooms['RoomName']).'\';document.irrigationScenarios.submit();"></td>		
		</tr>';
		$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Room=? AND FK_Installation=? AND FK_Template=?
			ORDER BY Sort ASC';
		$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($rowRooms['PK_Room'],$installationID,$GLOBALS['IrrigationScenariosTemplate']));
		if($resCommandGroups->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="3">'.$TEXT_NO_IRRIGATION_SCENARIOS_CONST.'</td>
			</tr>';
		}
		$pos=0;
		$posInRoom=array();
		while($rowCG=$resCommandGroups->FetchRow()){
			$posInRoom[$rowCG['PK_CommandGroup']]=$rowCG['Sort'];
		}
		$resCommandGroups->MoveFirst();
		while($rowCG=$resCommandGroups->FetchRow()){
			if(!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))
				$firstRoomArray[$rowCG['PK_CommandGroup']]=$rowRooms['RoomName'];
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$out.='
			<tr bgcolor="'.(($rowCG['PK_CommandGroup']==@$_REQUEST['lastAdded'])?'lightgreen':$color).'">
				<td><input type="button" class="button" name="posDown" value="U" onClick="self.location=\'index.php?section=irrigationScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=down&posInRoom='.urlencode(serialize($posInRoom)).'\'"> 
					<input type="button" class="button" name="posUp" value="D" onClick="self.location=\'index.php?section=irrigationScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=up&posInRoom='.urlencode(serialize($posInRoom)).'\'">
				 '.$TEXT_DESCRIPTION_CONST.': '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<B>'.$rowCG['Description'].'</B> '.$TEXT_HINT_CONST.': <b>'.$rowCG['Hint'].'</b>':'<b>'.nl2br($rowCG['Description']).': </b>'.$TEXT_HINT_CONST.': <b>'.$rowCG['Hint'].'</b> (See '.$firstRoomArray[$rowCG['PK_CommandGroup']].')').'</td>
				<td>&nbsp;</td>
				<td>
					<a href="#" onclick="document.irrigationScenarios.action.value=\'testScenario\';document.irrigationScenarios.editedCgID.value='.$rowCG['PK_CommandGroup'].';document.irrigationScenarios.submit();">'.$TEXT_TEST_CONST.'</a>
					<a href="#" onclick="document.irrigationScenarios.editedCgID.value='.$rowCG['PK_CommandGroup'].';document.irrigationScenarios.roomID.value='.$rowRooms['PK_Room'].';document.irrigationScenarios.submit();">'.$TEXT_EDIT_CONST.'</a>
					<a href="#" onClick="javascript:if(confirm(\''.$TEXT_IRRIGATION_SCENARIOS_CONFIRM_DELETE_CONST.'\'))self.location=\'index.php?section=irrigationScenarios&action=del&cgDelID='.$rowCG['PK_CommandGroup'].'\';">'.$TEXT_DELETE_CONST.'</a>
				</td>
			</tr>
			';
			$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
		}
	}
	if(count($displayedCommandGroups)>0){
		$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="updateCG" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			<td>&nbsp;</td>
		</tr>';
	}
	$out.='
		</table>
	<input type="hidden" name="displayedCommandGroups" value="'.join(',',$displayedCommandGroups).'">	
	</form>
	';
							
}else{	
	// action='add'
	// insert command group in specified room
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	if(!$canModifyInstallation){
		Header('Location: index.php?section=irrigationScenarios&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
		exit();
	}
	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=irrigationScenarios&msg=$TEXT_IRRIGATION_SCENARIO_DELETED_CONST");
	}
		
	if($action=='addToRoom'){
		$roomID=(int)$_POST['roomID'];
		$roomName=urldecode($_POST['roomName']);
		
		$insertCommandGroup='INSERT INTO CommandGroup (Description,FK_Array,FK_Installation,FK_Template,Hint) VALUES (?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array('New Irrigation Scenario',$arrayID,$installationID,$GLOBALS['IrrigationScenariosTemplate'],$roomName));
		$insertID=$dbADO->Insert_ID();
		
		$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup,FK_Room,Sort) VALUES (?,?,?)';
		$dbADO->Execute($insertCG_Room,array($insertID,$roomID,$insertID));
		
		setOrbitersNeedConfigure($installationID,$dbADO);
		$msg=$TEXT_IRRIGATION_SCENARIO_ADDED_CONST;
	}
	
	if($action=='testScenario'){
		$scenarioToTest=(int)$_REQUEST['editedCgID'];
		testScenario($scenarioToTest);
		header("Location: index.php?section=irrigationScenarios&msg=$TEXT_IRRIGATION_SCENARIO_TEST_CONST");
		exit();
	}
	
	if(isset($_POST['updateCG']) || $action=='externalSubmit' || @(int)$_REQUEST['editedCgID']!=0 || $action=='addToRoom'){
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		/*
		foreach($displayedCommandGroupsArray as $elem){
			$cgDescription=cleanString(@$_POST['commandGroup_'.$elem]);
			$cgHint=cleanString(@$_POST['hintCommandGroup_'.$elem]);
			$updateCG='UPDATE CommandGroup SET Description=?, Hint=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCG,array($cgDescription,$cgHint,$elem));
		}
		*/
		setOrbitersNeedConfigure($installationID,$dbADO);
		if(@(int)$_REQUEST['editedCgID']!=0 && @(int)$_REQUEST['roomID']!=0){
			header('Location: index.php?section=scenarioWizard&from=irrigationScenarios&cgID='.$_REQUEST['editedCgID'].'&roomID='.$_REQUEST['roomID'].'&wizard=2');
			exit();
		}
		$msg=(isset($msg))?$msg:"$TEXT_IRRIGATION_SCENARIO_UPDATED_CONST&lastAdded=".@$insertID;
	}
	
	if(isset($_REQUEST['operation'])){
		$fromCgID=$_REQUEST['cgID'];
		$roomID=$_REQUEST['roomID'];
		$operation=$_REQUEST['operation'];
		$posInRoom=unserialize(stripslashes($_REQUEST['posInRoom']));
		
		if($operation=='up'){
			$offset=array_keys($posInRoom);
			$fromPos=array_search($fromCgID,$offset);
			if(isset($offset[$fromPos+1])){
				$toCgID=$offset[$fromPos+1];
			
				$fromSort=$posInRoom[$fromCgID];
				$toSort=$posInRoom[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_Room SET Sort=? WHERE FK_CommandGroup=? AND FK_Room=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$roomID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$roomID));
				$msg=$TEXT_IRRIGATION_SCENARIO_POSITION_UPDATED_CONST;
			}
		}else{
			$offset=array_keys($posInRoom);
			$fromPos=array_search($fromCgID,$offset);
			if(isset($offset[$fromPos-1])){
				$toCgID=$offset[$fromPos-1];
			
				$fromSort=$posInRoom[$fromCgID];
				$toSort=$posInRoom[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_Room SET Sort=? WHERE FK_CommandGroup=? AND FK_Room=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$roomID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$roomID));
				$msg=$TEXT_IRRIGATION_SCENARIO_POSITION_UPDATED_CONST;
			}
		}
	}


	header("Location: index.php?section=irrigationScenarios&msg=".@$msg);
}
	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_IRRIGATION_SCENARIOS_CONST);
	$output->setNavigationMenu(array($TEXT_MY_SCENARIOS_CONST=>'index.php?section=myScenarios',$TEXT_IRRIGATION_SCENARIOS_CONST=>'index.php?section=irrigationScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_IRRIGATION_SCENARIOS_CONST);
	$output->output();

}
?>
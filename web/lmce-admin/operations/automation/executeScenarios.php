<?php
function executeScenarios($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/executeScenarios.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			
			<form action="index.php" method="POST" name="executeScenarios">
			<input type="hidden" name="section" value="executeScenarios">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="4" cellspacing="1" border="0" align="center">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_SCENARIO_DESCRIPTION_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_SCENARIO_TYPE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>
		';
		$queryEntAreas='
			SELECT 
				EntertainArea.*,
				Room.Description AS RoomName 
			FROM EntertainArea
				INNER JOIN Room on FK_Room=PK_Room
			WHERE FK_Installation=?
			ORDER BY RoomName ASC, Description ASC';
		$resEntAreas=$dbADO->Execute($queryEntAreas,$installationID);
		$initRoom=0;
		$displayedCG=array();
		while($rowEntAreas=$resEntAreas->FetchRow()){
			if($rowEntAreas['FK_Room']!=$initRoom){
				$out.='
				<tr class="alternate_back">
					<td colspan="3" align="left"><B>'.$TEXT_ROOM_CONST.': '.$rowEntAreas['RoomName'].'</B></td>
				</tr>';
				$queryCG_R='
					SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
					FROM CommandGroup
					INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
					INNER JOIN Array ON FK_Array=PK_Array
					WHERE FK_Installation=? AND FK_Room=?
					ORDER BY FK_Array ASC, ScenarioName ASC';
				$resCG_R=$dbADO->Execute($queryCG_R,array($installationID,$rowEntAreas['FK_Room']));
				if($resCG_R->RecordCount()==0){
				$out.='
					<tr class="regular">
						<td colspan="3" align="left">'.$TEXT_NO_RECORDS_CONST.'</td>
					</tr>';
				}			
				$pos=0;	
				while($rowCG_R=$resCG_R->FetchRow()){
					$pos++;
					$displayedCG[]=$rowCG_R['PK_CommandGroup'];
					$out.='
						<tr class="'.(($pos%2==0)?'alternate_back':'').'">
							<td><B>'.$rowCG_R['ScenarioName'].'</B></td>
							<td>'.$rowCG_R['ScenarioType'].'</td>
							<td align="center"><input type="button" class="button_fixed" value="'.$TEXT_TEST_CONST.'" onClick="self.location=\'index.php?section=executeScenarios&action=execute&cgID='.$rowCG_R['PK_CommandGroup'].'\'"><a href="index.php?section=editCommandGroup&cgID='.$rowCG_R['PK_CommandGroup'].'"> <input type="button" class="button_fixed" value="'.$TEXT_EDIT_CONST.'" onclick="self.location=\'index.php?section=editCommandGroup&cgID='.$rowCG_R['PK_CommandGroup'].'\'"></td>
						</tr>';
				}
				$initRoom=$rowEntAreas['FK_Room'];
			}
			$out.='
				<tr class="alternate_back">
					<td colspan="3" align="left"><B>'.$TEXT_ENTERTAIN_AREA_CONST.': '.$rowEntAreas['Description'].'</B></td>
				</tr>';
			$queryCG_E='
				SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
				FROM CommandGroup
				INNER JOIN CommandGroup_EntertainArea ON FK_CommandGroup=PK_CommandGroup
				INNER JOIN Array ON FK_Array=PK_Array
				WHERE FK_Installation=? AND FK_EntertainArea=?';
			$resCG_E=$dbADO->Execute($queryCG_E,array($installationID,$rowEntAreas['PK_EntertainArea']));
			if($resCG_E->RecordCount()==0){
			$out.='
				<tr class="regular">
					<td colspan="3" align="left">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>';
			}
			$pos=0;
			while($rowCG_E=$resCG_E->FetchRow()){
				$pos++;
				$displayedCG[]=$rowCG_E['PK_CommandGroup'];
				$out.='
					<tr class="'.(($pos%2==0)?'alternate_back':'').'">
						<td><B>'.$rowCG_E['ScenarioName'].'</B></td>
						<td>'.$rowCG_E['ScenarioType'].'</td>
						<td align="center"><input type="button" class="button_fixed" value="'.$TEXT_TEST_CONST.'" onClick="self.location=\'index.php?section=executeScenarios&action=execute&cgID='.$rowCG_E['PK_CommandGroup'].'\'"><a href="index.php?section=editCommandGroup&cgID='.$rowCG_E['PK_CommandGroup'].'"> <input type="button" class="button_fixed" value="'.$TEXT_EDIT_CONST.'" onclick="self.location=\'index.php?section=editCommandGroup&cgID='.$rowCG_R['PK_CommandGroup'].'\'"></td>
					</tr>';
			}
		}
		$out.='
			<tr class="alternate_back">
				<td colspan="3" align="left"><B>'.$TEXT_UNASSIGNED_TO_ROOM_OR_EA_CONST.'</B></td>
			</tr>';		
		if(count($displayedCG)==0)
			$displayedCG[]=0;
		$queryCG='
			SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
			FROM CommandGroup
			INNER JOIN Array ON FK_Array=PK_Array
			WHERE FK_Installation=? AND PK_CommandGroup NOT IN ('.join(',',$displayedCG).')';
		$resCG=$dbADO->Execute($queryCG,$installationID);
		if($resCG->RecordCount()==0){
			$out.='
				<tr class="regular">
					<td colspan="3" align="left">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>';
			}		
		$pos=0;
		while($rowCG=$resCG->FetchRow()){
			$pos++;
			$out.='
				<tr class="'.(($pos%2==0)?'alternate_back':'').'">
					<td><B>'.$rowCG['ScenarioName'].'</B></td>
					<td>'.$rowCG['ScenarioType'].'</td>
					<td align="center"><input type="button" class="button_fixed" value="'.$TEXT_TEST_CONST.'" onClick="self.location=\'index.php?section=executeScenarios&action=execute&cgID='.$rowCG['PK_CommandGroup'].'\'"><a href="index.php?section=editCommandGroup&cgID='.$rowCG['PK_CommandGroup'].'"> <input type="button" class="button_fixed" value="'.$TEXT_EDIT_CONST.'" onclick="self.location=\'index.php?section=editCommandGroup&cgID='.$rowCG['PK_CommandGroup'].'\'"></td>
				</tr>';
		}
		$out.='
		</form>';
	}else{
	// process area
		if(isset($_REQUEST['cgID'])){
			$cgID=(int)$_REQUEST['cgID'];
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 0 10 '.$cgID;
			exec($commandToSend);
			
			header('Location: index.php?section=executeScenarios&msg='.$TEXT_SCENARIO_WAS_EXECUTED_CONST.': '.$commandToSend);
			exit();
		}
		
		header('Location: index.php?section=executeScenarios');
	}

	$output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
	$output->setPageTitle($TEXT_EXECUTE_SCENARIOS_CONST);
	$output->setNavigationMenu(array($TEXT_EXECUTE_SCENARIOS_CONST=>'index.php?section=executeScenarios'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EXECUTE_SCENARIOS_CONST);
	$output->output();
}
?>

<?
function pickScenario($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/pickScenario.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$entryID=(int)$_REQUEST['entry'];
	$plID=(int)$_REQUEST['plID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
			function pick(value){
				opener.location=\'index.php?section=editPlaylistAdvanced&action=addCG&plID='.$plID.'&mode='.@$_REQUEST['mode'].'&entry='.$entryID.'&cgID=\'+value;
				self.close();
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">'.$TEXT_PICK_SCENARIO_CONST.'</h3>

			<form action="index.php" method="POST" name="pickScenario">
			<input type="hidden" name="section" value="pickScenario">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="4" cellspacing="0" border="0" align="center">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_SCENARIO_DESCRIPTION_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_SCENARIO_TYPE_CONST.'</B></td>
				<td align="center">&nbsp;</td>
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
				<tr bgcolor="#D1D9EA">
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
				while($rowCG_R=$resCG_R->FetchRow()){
					$displayedCG[]=$rowCG_R['PK_CommandGroup'];
					$out.='
						<tr>
							<td>'.$rowCG_R['ScenarioName'].'</td>
							<td>'.$rowCG_R['ScenarioType'].'</td>
							<td align="center"><a href="javascript:pick('.$rowCG_R['PK_CommandGroup'].');">'.$TEXT_PICK_CONST.'</a></td>
						</tr>';
				}
				$initRoom=$rowEntAreas['FK_Room'];
			}
			$out.='
				<tr bgcolor="#EEEEEE">
					<td colspan="3" align="left"><B>'.$TEXT_ENTERTAIN_AREA_CONST.': '.$rowEntAreas['Description'].'</B></td>
				</tr>';
			$queryCG_E='
				SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
				FROM CommandGroup
				INNER JOIN CommandGroup_EntertainArea ON FK_CommandGroup=PK_CommandGroup
				INNER JOIN Array ON FK_Array=PK_Array
				WHERE FK_Installation=? AND FK_EntertainArea=?';
			$resCG_E=$dbADO->Execute($queryCG_E,array($installationID,$rowEntAreas['PK_EntertainArea']));
			while($rowCG_E=$resCG_E->FetchRow()){
				$displayedCG[]=$rowCG_E['PK_CommandGroup'];
				$out.='
					<tr>
						<td>'.$rowCG_E['ScenarioName'].'</td>
						<td>'.$rowCG_E['ScenarioType'].'</td>
						<td align="center"><a href="javascript:pick('.$rowCG_E['PK_CommandGroup'].');">'.$TEXT_PICK_CONST.'</a></td>
					</tr>';
			}
		}
		$out.='
			<tr bgcolor="#D1D9EA">
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
		while($rowCG=$resCG->FetchRow()){
			$out.='
				<tr>
					<td>'.$rowCG['ScenarioName'].'</td>
					<td>'.$rowCG['ScenarioType'].'</td>
					<td align="center"><a href="javascript:pick('.$rowCG['PK_CommandGroup'].');">'.$TEXT_PICK_CONST.'</a></td>
				</tr>';
		}
		$out.='
			<tr>
				<td colspan="2" align="right"><B>'.$TEXT_REMOVE_SCENARION_CONST.'</B></td>
				<td align="center"><a href="javascript:pick(0);">'.$TEXT_REMOVE_CONST.'</a></td>
			</tr>		
		</table>
			<br><div align="center"><input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="javascript:self.close();"></div>
		</form>';
	}else{
	// process area

		
		header('Location: index.php?section=pickScenario');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PICK_SCENARIO_CONST);
	$output->output();
}
?>
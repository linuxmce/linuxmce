<?php
function sortScenarios($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/sortScenarios.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';

	$installationID = (int)@$_SESSION['installationID'];
	$categoryArray=getAssocArray('Template','PK_Template','Description',$dbADO,'','');
	$categoryArray['']='Not defined';
	$sortBy=(!isset($_REQUEST['sortBy']) || $_REQUEST['sortBy']=='Room')?'Room':'EntertainArea';
	$from=cleanString(@$_REQUEST['from']);

	$areasArray=getAssocArray($sortBy,'PK_'.$sortBy,'Description',$dbADO,'','');
	$areasArray['']='Not assigned';
	
	if(isset($_REQUEST['from'])){
		$backLink='<a href="index.php?section='.$from.'">'.$TEXT_BACK_CONST.'</a>';
	}
	
	if($action=='form') {


		$out.=setLeftMenu($dbADO).'
	<div align="center" class="err">'.strip_tags(@$_GET['error']).'</div>
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>';

		$out.='
	<form action="index.php" method="POST" name="sortScenarios">
		<input type="hidden" name="section" value="sortScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="'.$sortBy.'ID" value="">
		<input type="hidden" name="editedCgID" value="">	
		<input type="hidden" name="sortBy" value="'.$sortBy.'">
		<input type="hidden" name="from" value="'.$from.'">
		
	'.@$backLink.'
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
		if(count($areasArray)==0){
			$out.=$TEXT_NO_ROOMS_IN_INSTALLATION_CONST;
		}
		$displayedCommandGroups=array();
		$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			LEFT JOIN CommandGroup_'.$sortBy.' ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Installation=?
			ORDER BY FK_'.$sortBy.' ASC,Sort ASC';
		$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($installationID));
		$pos=0;


		$resCommandGroups->MoveFirst();
		$startArea=-1;
		while($rowCG=$resCommandGroups->FetchRow()){
			if($rowCG['FK_'.$sortBy]!=$startArea){
				$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="3"><B>'.$areasArray[$rowCG['FK_'.$sortBy]].'</B></td>
			</tr>';
				$startArea=$rowCG['FK_'.$sortBy];
			}
			if(!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))
			$firstAreaArray[$rowCG['PK_CommandGroup']]=$areasArray[$rowCG['FK_'.$sortBy]];
			$pos++;
			$out.='
			<tr bgcolor="'.(($rowCG['PK_CommandGroup']==@$_REQUEST['lastAdded'])?'lightgreen':'').'">
				<td width="80" align="center">'.(($rowCG['FK_'.$sortBy.'']!='')?'
					<input type="button" class="button" name="posDown" value="U" onClick="self.location=\'index.php?section=sortScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&'.$sortBy.'ID='.$rowCG['FK_'.$sortBy.''].'&operation=up&sort='.$rowCG['Sort'].'&sortBy='.$sortBy.'&from='.$from.'\'"> 
					<input type="button" class="button" name="posUp" value="D" onClick="self.location=\'index.php?section=sortScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&'.$sortBy.'ID='.$rowCG['FK_'.$sortBy.''].'&operation=down&sort='.$rowCG['Sort'].'&sortBy='.$sortBy.'&from='.$from.'\'">':'').'</td>
				<td> Description: '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"> Hint: <input type="text" name="hintCommandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Hint'].'">':'<b>'.$rowCG['Description'].': </b>Hint: <b>'.$rowCG['Hint'].'</b> (See '.$firstAreaArray[$rowCG['PK_CommandGroup']].')').'
				Category: <B>'.$categoryArray[$rowCG['FK_Template']].'</B>
				</td>
				<td>'.(($pos==1)?'Default ON':'').(($pos==2)?'Default OFF':'').'</td>
			</tr>
			';
			$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
		}

		if(count($displayedCommandGroups)>0){
			$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="updateCG" value="Update"  ></td>
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
			Header('Location: index.php?section=sortScenarios&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}

//$dbADO->debug=true;
		if(isset($_REQUEST['operation'])){
			$fromCgID=$_REQUEST['cgID'];
			$areaID=@$_REQUEST[''.$sortBy.'ID'];
			$operation=$_REQUEST['operation'];
			$sort=$_REQUEST['sort'];
			if($operation=='up'){
				if($sort>0){
					$res=$dbADO->Execute('SELECT FK_CommandGroup,Sort FROM CommandGroup_'.$sortBy.' WHERE FK_'.$sortBy.'=? AND Sort<? ORDER BY Sort DESC LIMIT 0,1',array($areaID,$sort));
					if($res->RecordCount()>0){
						$row=$res->FetchRow();
						
						$updateCG_R='UPDATE CommandGroup_'.$sortBy.' SET Sort=? WHERE FK_CommandGroup=? AND FK_'.$sortBy.'=?';
						$dbADO->Execute($updateCG_R,array($sort,$row['FK_CommandGroup'],$areaID));
						$dbADO->Execute($updateCG_R,array($row['Sort'],$fromCgID,$areaID));
						$msg=$TEXT_POSITIONS_UPDATED_CONST;
					}
				}
			}else{
				if($sort>0){
					$res=$dbADO->Execute('SELECT FK_CommandGroup,Sort FROM CommandGroup_'.$sortBy.' WHERE FK_'.$sortBy.'=? AND Sort>? ORDER BY Sort Asc LIMIT 0,1',array($areaID,$sort));
					if($res->RecordCount()>0){
						$row=$res->FetchRow();
						
						$updateCG_R='UPDATE CommandGroup_'.$sortBy.' SET Sort=? WHERE FK_CommandGroup=? AND FK_'.$sortBy.'=?';
						$dbADO->Execute($updateCG_R,array($sort,$row['FK_CommandGroup'],$areaID));
						$dbADO->Execute($updateCG_R,array($row['Sort'],$fromCgID,$areaID));
						$msg=$TEXT_POSITIONS_UPDATED_CONST;
					}
				}
			}
		}else{
			$msg=$TEXT_POSITIONS_NOT_UPDATED_CONST;
		}


		header("Location: index.php?section=sortScenarios&msg=".@$msg.'&sortBy='.$sortBy.'&from='.$from);
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_SORT_SCENARIOS_CONST);
	$output->setNavigationMenu(array($TEXT_MY_SCENARIOS_CONST=>'index.php?section=myScenarios',$TEXT_SORT_SCENARIOS_CONST=>'index.php?section=sortScenarios&sortBy='.$sortBy));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SORT_SCENARIOS_CONST);
	$output->output();

}
?>
<?
function rubyCodes($output,$dbADO,$mediaADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/irGsdCodes.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

	$time_start = getmicrotime();
	

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	global $inputCommandsArray, $dspmodeCommandsArray,$powerCommands;
	
	$out='';
//	$dbADO->debug=true;;
	$installationID = (int)$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$label=(isset($_REQUEST['label']))?$_REQUEST['label']:'infrared';
	$GLOBALS['label']=$label;
	
	$dtID = (int)$_REQUEST['dtID'];
	$deviceID = (isset($_REQUEST['deviceID']) && (int)@$_REQUEST['deviceID']!=0)?(int)$_REQUEST['deviceID']:NULL;
	$userID = (int)@$_SESSION['userID'];
	if ($dtID==0) {
		header("Location: index.php?section=login");
		exit();
	}
	
	// get InfraredGroup_Commmand from remote location only when deviceID is not defined
	if((int)$deviceID==0){
		$isImported=GetIRCodesForDevice(NULL,$dbADO,$dtID);
	}
	
	if(!isset($_REQUEST['irGroup'])){
		$resDefaultIG=$dbADO->Execute('SELECT FK_InfraredGroup FROM DeviceTemplate WHERE PK_DeviceTemplate=?',$dtID);
		if($resDefaultIG->RecordCount()>0){
			$rowDefaultIG=$resDefaultIG->FetchRow();
			$irGroup=$rowDefaultIG['FK_InfraredGroup'];
		}else{
			$irGroup=0;
		}
	}else
		$irGroup=(int)$_REQUEST['irGroup'];
	$GLOBALS['displayedIRGC']=array();
	$GLOBALS['displayedCommands']=array();

	$selectDTData='
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				FK_InfraredGroup,
				DeviceTemplate_AV.*
			FROM DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?';
	$resDTData=$dbADO->Execute($selectDTData,$dtID);
	if($resDTData->RecordCount()==0){
		header("Location: index.php?section=userHome");
		exit();
	}
	$rowDTData=$resDTData->FetchRow();

	$togglePower=(@$rowDTData['TogglePower']==1)?1:0;
	$toggleInput=(@$rowDTData['ToggleInput']==1)?1:0;
	$toggleDSP=(@$rowDTData['ToggleDSP']==1)?1:0;
	$manufacturerID=$rowDTData['FK_Manufacturer'];
	$deviceCategoryID=$rowDTData['FK_DeviceCategory'];
	
	
	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
		</script>	
		
	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="rubyCodes">
			<input type="hidden" name="section" value="rubyCodes">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="infraredGroupID" value="'.$irGroup.'">
			<input type="hidden" name="irgroup_command" value="">
			<input type="hidden" name="label" value="'.$label.'">';


		$selectedIRGrops=array();
		$selectedIRGrops[]=$rowDTData['FK_InfraredGroup'];

		$inputCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$dbADO,'INNER JOIN DeviceTemplate_Input ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$dspmodeCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$dbADO,'INNER JOIN DeviceTemplate_DSPMode ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$powerCommands=array($GLOBALS['genericONCommand']=>'ON',$GLOBALS['genericOFFCommand']=>'OFF');
		
		$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$irGroup.'">
		
		<h3>'.$TEXT_EDIT_GSD_CODES_CONST.'</h3>
		<table border="0" width="100%" class="normaltext">
			<tr>
				<td class="err" colspan="2">'.$TEXT_WARNING_CHANGES_AFFECT_ALL_CONST.' <B>'.$rowDTData['Template'].'</B>.</td>
			</tr>
			<tr>
				<td valign="top" colspan="2">'.$TEXT_DEVICE_TEMPLATE_CONST.' <B>'.$rowDTData['Template'].'</B>, '.strtolower($TEXT_DEVICE_CATEGORY_CONST).' <B>'.$rowDTData['Category'].'</B>, '.strtolower($TEXT_MANUFACTURER_CONST).' <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>
		';
			$irGroups=getAssocArray('InfraredGroup','PK_InfraredGroup','Description',$dbADO,'WHERE FK_Manufacturer='.$manufacturerID.' AND FK_DeviceCategory='.$deviceCategoryID,'ORDER BY Description ASC');
			$error_not_saved=(count($irGroups)>0 && ($irGroup==0 || is_null($irGroup)))?'<span class="err">'.$TEXT_ERROR_IRGROUP_NOT_SAVED_CONST.'</span>':'';
			$error_no_group=(count($irGroups)==0)?'<span class="err">'.$TEXT_ERROR_NO_GROUP_CONST.'</span>':'';
			
			$out.='
			<tr>
				<td colspan="2">'.$TEXT_USES_GROUP_CODESET_CONST.' '.pulldownFromArray($irGroups,'irGroup',$irGroup,'onxChange="document.rubyCodes.submit();"','key','').' '.$error_not_saved.'
			</td>
			</tr>
			<tr>
				<td colspan="2"><input type="text" name="new_group_description" value=""> <input type="submit" name="add_group" value="'.$TEXT_CREATE_GROUP_CONST.'" class="button"> '.$error_no_group.'</td>
		</tr>			
			';
			
		$out.='
			<tr>
				<td valign="top" width="250">'.$TEXT_IMPLEMENT_COMMAND_GROUPS_CONST.' </td>
				<td>'.DeviceCommandGroupTable($dtID,$deviceCategoryID,$dbADO).'</td>
		</tr>
		';
		
		$out.='		
			<tr>
				<td colspan="3" align="center"><input type="button" class="button" name="button" value="'.$TEXT_ADD_REMOVE_COMMANDS_CONST.'" onClick="windowOpen(\'index.php?section=infraredCommands&infraredGroup='.$irGroup.'&deviceID='.$deviceID.'&dtID='.$dtID.'&rootNode=1&norestrict=1\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"> <input type="submit" class="button" name="update" value="Update" '.((!isset($_SESSION['userID']))?'disabled':'').'> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>';
		
		// extract data from InfraredGroup_Command an put it in multi-dimmensional array
		$codesArray=extractCodesTree($irGroup,$dtID,$dbADO);
		
		// display the html rows 
		$out.=getCodesTableRows('rubyCodes',$irGroup,$dtID,$deviceID,$codesArray,$togglePower,$toggleInput,$toggleDSP);

		$out.='
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'" '.((!isset($_SESSION['userID']))?'disabled':'').'> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
	';
		$out.='
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
		</form>
		<iframe name="codeTester" src="" style="display:none;"></iframe><br>		
	';	
		
	} else {
		$time_start = getmicrotime();
		//$dbADO->debug=true;
		$existingCommands=($irGroup==0)?array():array_keys(getAssocArray('InfraredGroup_Command','FK_Command','PK_InfraredGroup_Command',$dbADO,'WHERE FK_InfraredGroup='.$irGroup));
		
		$newIRGroup=((int)@$_POST['irGroup']>0)?(int)$_POST['irGroup']:NULL;
		$oldIRGroup=(int)$_POST['oldIRGroup'];

		
		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];
			if($action!='delete'){
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,IRData) SELECT FK_InfraredGroup,FK_Command,IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=rubyCodes&from=$from&dtID=$dtID&deviceID=$deviceID&irGroup=$irGroup&msg=$TEXT_RUBY_CODE_ADDED_CONST&label=".$GLOBALS['label']);
				exit();
			}else{
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array('',$irg_c));
				
				header("Location: index.php?section=rubyCodes&from=$from&dtID=$dtID&deviceID=$deviceID&irGroup=$irGroup&msg=$TEXT_CUSTOM_CODE_DELETED_CONST&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		if(isset($_POST['update']) && isset($_SESSION['userID'])){
			if($newIRGroup!=$oldIRGroup){
	
				$dbADO->Execute('UPDATE DeviceTemplate SET FK_InfraredGroup=? WHERE PK_DeviceTemplate=?',array($newIRGroup,$dtID));
	
				header("Location: index.php?section=rubyCodes&from=$from&dtID=$dtID&deviceID=$deviceID&irGroup=$newIRGroup&msg=$TEXT_IR_GROUP_CHANGED_FOR_SELECTED_DEVICE_TEMPLATE_CONST&label=".$GLOBALS['label']);
				exit();
			}			
			
			$displayedCommands=explode(',',$_POST['displayedCommands']);
			$irGroup=($irGroup==0)?NULL:$irGroup;

			if(isset($_POST['deviceCG'])){
				$deviceCGArray=explode(',',$_POST['deviceCG']);
				$oldCheckedDCG=explode(',',$_POST['oldCheckedDCG']);
			
				foreach ($deviceCGArray AS $deviceCG){
					if(isset($_POST['dcg_'.$deviceCG])){
	
						$commands=explode(',',$_POST['commands_'.$deviceCG]);
						
						foreach ($commands AS $commandID){
							if(!in_array($commandID,$displayedCommands)){			
								$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,IRData) VALUES (?,?,?)',array($irGroup,$commandID,''));
							}
						}
						if(!in_array($deviceCG,$oldCheckedDCG))
							$dbADO->Execute('INSERT INTO DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate, FK_DeviceCommandGroup) VALUES (?,?)',array($dtID,$deviceCG));
	
					}elseif(!isset($_POST['dcg_'.$deviceCG]) && in_array($deviceCG,$oldCheckedDCG)){
						// delete from IRG_C
						$commands=explode(',',$_POST['commands_'.$deviceCG]);
						if(count($commands)>0){
							$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE FK_Command in ('.join(',',$commands).') AND FK_InfraredGroup=?',array('',$irGroup));
	
						}
						$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate=? AND FK_DeviceCommandGroup=?',array($dtID,$deviceCG));
					}
				}
			}			
			
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				if(isset($_POST['irData_'.$ig_c])){
					$irData=$_POST['irData_'.$ig_c];
					$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
				}
			}

			$commandsDisplayed=array_unique(explode(',',$_POST['displayedCommands']));

			$time_end= getmicrotime();
			//print '<p class="normaltext">Page generated in '.round(($time_end-$time_start),3).' s.';
			
			header("Location: index.php?section=rubyCodes&from=$from&deviceID=$deviceID&dtID=$dtID&irGroup=$irGroup&msg=$TEXT_RUBY_CODES_UPDATED_CONST&label=".$GLOBALS['label']);
			exit();
		}

		if(isset($_POST['add_group']) && $_POST['new_group_description']!=''){
			$description=cleanString($_POST['new_group_description']);
			$res=$dbADO->Execute('INSERT INTO InfraredGroup (FK_DeviceCategory,FK_Manufacturer,Description) VALUES (?,?,?)',array($deviceCategoryID,$manufacturerID,$description));
			$irGroup=$dbADO->Insert_ID();
			$dbADO->Execute('UPDATE DeviceTemplate SET FK_InfraredGroup=? WHERE PK_DeviceTemplate=?',array($irGroup,$dtID));
			
			header("Location: index.php?section=rubyCodes&from=$from&deviceID=$deviceID&dtID=$dtID&irGroup=$irGroup&msg=$TEXT_CODES_GROUP_ADDED_CONST");
			exit();
		}
		
		header("Location: index.php?section=rubyCodes&from=$from&dtID=$dtID&deviceID=$deviceID&irGroup=$irGroup&lastAction=".@$lastAction);
	}
	$time_end = getmicrotime();
	//$out.='<br><p class="normaltext">Page generated in '.round(($time_end-$time_start),3).' s.';
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_GSD_CODES_CONST);
	$output->output();
}
?>

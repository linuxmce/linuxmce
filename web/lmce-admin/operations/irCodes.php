<?php
function irCodes($output,$dbADO,$mediaADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/irGsdCodes.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	global $inputCommandsArray, $dspmodeCommandsArray,$powerCommands;
	$GLOBALS['DT_&_Room']=1;
	
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
		
		if($isImported!=0){
			$_SESSION['error_message']=$TEXT_ERROR_CANNOT_RETRIEVE_IR_CODES_CONST;
			$_SESSION['retry_url']='index.php?section='.$from;
	
			$out='
			<script>
				opener.location="index.php?section=error_message";
				self.close();
			</script>
			';
			die($out);
		}
	}
	
	
	
	if(!isset($_REQUEST['infraredGroupID'])){
		$resDefaultIG=$dbADO->Execute('SELECT FK_InfraredGroup FROM DeviceTemplate WHERE PK_DeviceTemplate=?',$dtID);
		if($resDefaultIG->RecordCount()>0){
			$rowDefaultIG=$resDefaultIG->FetchRow();
			$infraredGroupID=$rowDefaultIG['FK_InfraredGroup'];
		}else{
			$infraredGroupID=0;
		}
	}else
		$infraredGroupID=(int)$_REQUEST['infraredGroupID'];
	$GLOBALS['displayedIRGC']=array();
	$GLOBALS['displayedCommands']=array();

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
		</script>	
		
	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="irCodes">
			<input type="hidden" name="section" value="irCodes">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
			<input type="hidden" name="irgroup_command" value="">
			<input type="hidden" name="label" value="'.$label.'">';
		$selectDTData='
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				FK_InfraredGroup,
				DeviceTemplate_AV.*,
				DeviceTemplate_AV.FK_DeviceTemplate AS AVTemplate,
				Device.FK_Device_ControlledVia AS Parent,
				DeviceTemplate.FK_CommMethod AS FK_CommMethod
			FROM Device
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_Device=?';

		$resDTData=$dbADO->Execute($selectDTData,array($deviceID));
		if($resDTData->RecordCount()==0){
			$selectDT='
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				FK_InfraredGroup,
				DeviceTemplate_AV.*,
				DeviceTemplate_AV.FK_DeviceTemplate AS AVTemplate,
				DeviceTemplate.FK_CommMethod AS FK_CommMethod
			FROM DeviceTemplate 
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?';
			$resDTData=$dbADO->Execute($selectDT,array($dtID));
		}
		
		$rowDTData=$resDTData->FetchRow();
		$togglePower=(@$rowDTData['TogglePower']==1)?1:0;
		$toggleInput=(@$rowDTData['ToggleInput']>=1)?1:0;
		$toggleDSP=(@$rowDTData['ToggleDSP']==1)?1:0;
		
		// create the record in DeviceTemplate_AV for templates who doesn't have it
		if(is_null($rowDTData['AVTemplate'])){
			$dbADO->Execute('INSERT IGNORE INTO DeviceTemplate_AV (FK_DeviceTemplate) VALUES (?)',$dtID);
		}
		
		$manufacturerID=$rowDTData['FK_Manufacturer'];
		$deviceCategoryID=$rowDTData['FK_DeviceCategory'];
		$deviceParent=@$rowDTData['Parent'];

		$selectedIRGrops=array();
		$selectedIRGrops[]=$rowDTData['FK_InfraredGroup'];

		$inputCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$dbADO,'INNER JOIN DeviceTemplate_Input ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$dspmodeCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$dbADO,'INNER JOIN DeviceTemplate_DSPMode ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$powerCommands=array($GLOBALS['genericONCommand']=>'ON',$GLOBALS['genericOFFCommand']=>'OFF');
		
		$editLabel=($label=='infrared')?$TEXT_EDIT_IR_CODES_CONST:$TEXT_EDIT_GSD_CODES_CONST;
		$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$infraredGroupID.'">
		<h3>'.$editLabel.'</h3>
		<table border="0" width="100%" class="normaltext">
			<tr>
				<td class="err" colspan="2">'.$TEXT_WARNING_CHANGES_AFFECT_ALL_CONST.' <B>'.$rowDTData['Template'].'</B>.</td>
			</tr>
			<tr>
				<td valign="top" colspan="2">'.$TEXT_DEVICE_TEMPLATE_CONST.' <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> and manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>
			<tr>
				<td valign="top" colspan="2">'.$TEXT_DELAYS_CONST.': '.$TEXT_POWER_CONST.': <B>'.$rowDTData['IR_PowerDelay'].'</B> '.$TEXT_MILISECONDS_CONST.', '.$TEXT_MODE_CONST.': <B>'.$rowDTData['IR_ModeDelay'].'</B> '.$TEXT_MILISECONDS_CONST.', '.$TEXT_OTHER_CONST.': <B>'.$rowDTData['DigitDelay'].'</B> '.$TEXT_MILISECONDS_CONST.'  <a href="index.php?section=addModel&step=2&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><td>
			</tr>
			<tr>
				<td valign="top" colspan="2">'.$TEXT_TUNING_CONST.': <B>'.((str_replace('E','',$rowDTData['NumericEntry'])=='')?$TEXT_NO_FIXED_DIGITS_CONST:$TEXT_FIXED_DIGITS_CONST.': '.str_replace('E','',$rowDTData['NumericEntry'])).'</B>  ['.((strpos($rowDTData['NumericEntry'],'E')!==false)?'x':'').'] '.$TEXT_TERMINATE_WITH_ENTER_CONST.' <a href="index.php?section=addModel&step=3&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">'.$TEXT_POWER_CONST.': <B>'.(($rowDTData['TogglePower']==0)?$TEXT_DISCRETE_CONST:$TEXT_TOGGLE_CONST).'</B> <a href="index.php?section=addModel&step=4&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">'.$TEXT_INPUTS_CONST.' <B>'.(($rowDTData['ToggleInput']==0)?$TEXT_DISCRETE_CONST:$TEXT_TOGGLE_CONST).'</B>: <B>'.join(', ',$inputCommandsArray).'</B> <a href="index.php?section=addModel&step=5&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">'.$TEXT_DSP_MODES_CONST.' <B>'.(($rowDTData['ToggleDSP']==0)?(count($dspmodeCommandsArray)>0?$TEXT_DISCRETE_CONST:'NONE'):$TEXT_TOGGLE_CONST).'</B>: <B>'.join(', ',$dspmodeCommandsArray).'</B> <a href="index.php?section=addModel&step=6&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><td>
			</tr>
			<tr>
				<td valign="top" colspan="2">'.$TEXT_RepeatIR_CONST.' '.$TEXT_RepeatGeneral_CONST.': <B>'.$rowDTData['RepeatIR'].'</B>, '.$TEXT_RepeatVolume_CONST. '<B>: '.$rowDTData['RepeatVolume'].'</B> <a href="index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1">['.$TEXT_CHANGE_EXPLAIN_CONST.']</a><BR><BR><td>
			</tr>
			';
		if(is_null($rowDTData['FK_CommMethod'])){
			// device template need to have a comm method
			$out.='
				<tr>
					<td valign="top" width="250" class="err" colspan="2">'.$TEXT_MISSING_COMM_METHOD_CONST.' 
					<a href="javascript:windowOpen(\'index.php?section=commMethod&dtID='.$dtID.'&from=irCodes\',\'width=400,height=300,toolbar=1,scrollbars=1,resizable=1\');">'.$TEXT_ADD_COMM_METHOD_CONST.'</a></td>
			</tr>
			';
				
		}else{
			if(isset($deviceID)){
				$out.='
				<tr>
					<td valign="top">'.$TEXT_THIS_DEVICE_IS_CONTROLLED_VIA_CONST.': </td>
					<td valign="top">'.controlledViaPullDown('controlledVia',$deviceID,$dtID,$deviceCategoryID,$deviceParent,$dbADO).'<td>
				</tr>';
			}
				$irGroups=getAssocArray('InfraredGroup','PK_InfraredGroup','Description',$dbADO,'WHERE FK_Manufacturer='.$manufacturerID.' AND FK_DeviceCategory='.$deviceCategoryID,'ORDER BY Description ASC');
				$error_not_saved=(count($irGroups)>0 && ($infraredGroupID==0 || is_null($infraredGroupID)))?'<span class="err">'.$TEXT_ERROR_IRGROUP_NOT_SAVED_CONST.'</span>':'';
				
				$out.='
				<tr>
					<td>'.$TEXT_USES_GROUP_CODESET_CONST.' </td>
					<td>'.pulldownFromArray($irGroups,'irGroup',$infraredGroupID,'onChange="document.irCodes.submit();"','key','').' <input type="button" class="button" name="step7" value="Help me choose" onclick="self.location=\'index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID.'\'"> '.$error_not_saved.'</td>
			</tr>';
				
			$out.='
				<tr>
					<td valign="top" width="250">'.$TEXT_IMPLEMENT_COMMAND_GROUPS_CONST.' </td>
					<td>'.DeviceCommandGroupTable($dtID,$deviceCategoryID,$dbADO).'</td>
			</tr>
			';
		}		
		$out.='		
			<tr>
				<td colspan="3" align="center">
					<input type="button" class="button" name="button" value="'.$TEXT_ADD_REMOVE_COMMANDS_CONST.'" onClick="windowOpen(\'index.php?section=infraredCommands&infraredGroup='.$infraredGroupID.'&deviceID='.$deviceID.'&dtID='.$dtID.(($GLOBALS['label']!='infrared')?'&rootNode=1':'').'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');">
					<input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'" '.((!isset($_SESSION['userID']))?'disabled':'').'>
					<input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">
				</td>
			</tr>';
		
		// extract data from InfraredGroup_Command an put it in multi-dimmensional array
		$codesArray=extractCodesTree($infraredGroupID,$dtID,$dbADO);
		
		// display the html rows 
		$out.=getCodesTableRows('irCodes',$infraredGroupID,$dtID,$deviceID,$codesArray,$togglePower,$toggleInput,$toggleDSP);
		

		$out.='
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'" '.((!isset($_SESSION['userID']))?'disabled':'').'></td>
			</tr>
	';

		$out.='
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
		</form>
		<iframe name="codeTester" src="" style="display:none;"></iframe>
	<!-- <span class="normaltext"><a href="mailto:support@plutohome.com?subject=IR Group '.$infraredGroupID.' x Device Template '.$dtID.' x UserID '.$userID.'">'.$TEXT_IR_CONTACT_US_CONST.'</a>.</span> -->
	<br><br>		
	';	

	} else {
		$time_start = getmicrotime();
		//$dbADO->debug=true;

		$newIRGroup=((int)@$_REQUEST['irGroup']>0)?(int)$_REQUEST['irGroup']:NULL;
		$oldIRGroup=(int)@$_REQUEST['oldIRGroup'];
		if($newIRGroup!=$oldIRGroup){

			$dbADO->Execute('UPDATE DeviceTemplate SET FK_InfraredGroup=? WHERE PK_DeviceTemplate=?',array($newIRGroup,$dtID));
			// sync all codes
			$isSync=GetIRCodesForDevice(NULL,$dbADO,$dtID);
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$newIRGroup&msg=$TEXT_IR_GROUP_CHANGED_FOR_SELECTED_DEVICE_TEMPLATE_CONST&label=".$GLOBALS['label']);
			exit();
		}
		
		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];

			if($action=='delete') {
				$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',array($irg_c));

				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=$TEXT_CUSTOM_CODE_DELETED_CONST&label=".$GLOBALS['label']);
				exit();
			} else if($action=='clear') {
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array('',$irg_c));
				
				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=$TEXT_CUSTOM_CODE_DELETED_CONST&label=".$GLOBALS['label']);
				exit();
			} else {
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,IRData) SELECT FK_InfraredGroup,FK_Command,IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code added.&label=".$GLOBALS['label']);
				exit();
			}
			
		}
		
		if(isset($_POST['update']) && isset($_SESSION['userID'])){
			
			$deviceCGArray=explode(',',$_POST['deviceCG']);
			$oldCheckedDCG=explode(',',$_POST['oldCheckedDCG']);
			$displayedCommands=explode(',',$_POST['displayedCommands']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
			
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				if(isset($_POST['irData_'.$ig_c])){
					$irData=$_POST['irData_'.$ig_c];
					$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
				}
			}
						
			$commands_added=array();
			foreach ($deviceCGArray AS $deviceCG){
				if(isset($_POST['dcg_'.$deviceCG])){

					$commands=explode(',',$_POST['commands_'.$deviceCG]);
					
					foreach ($commands AS $commandID){
						if(!in_array($commandID,$displayedCommands) && !in_array($commandID,$commands_added)){	
							$commands_added[]=$commandID;		
							$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,IRData) VALUES (?,?,?)',array($infraredGroupID,$commandID,''));
						}
					}
					if(!in_array($deviceCG,$oldCheckedDCG))
						$dbADO->Execute('INSERT INTO DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate, FK_DeviceCommandGroup) VALUES (?,?)',array($dtID,$deviceCG));

				}elseif(!isset($_POST['dcg_'.$deviceCG]) && in_array($deviceCG,$oldCheckedDCG)){
					// delete from IRG_C
					$commands=explode(',',$_POST['commands_'.$deviceCG]);
					if(count($commands)>0){
						$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE FK_Command in ('.join(',',$commands).') AND FK_InfraredGroup=?',array('',$infraredGroupID));

					}
					$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate=? AND FK_DeviceCommandGroup=?',array($dtID,$deviceCG));
				}
			}
			
			


			$commandsDisplayed=array_unique(explode(',',$_POST['displayedCommands']));

			
			// update device controlled via
			if(isset($_POST['controlledVia'])){
				$controlledVia=((int)$_POST['controlledVia']>0)?(int)$_POST['controlledVia']:NULL;
				$dbADO->Execute('UPDATE Device SET FK_Device_ControlledVia=? WHERE PK_Device=?',array($controlledVia,$deviceID));
			}
			
			$time_end= getmicrotime();
			//print '<p class="normaltext">Page generated in '.round(($time_end-$time_start),3).' s.';
			
			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=$TEXT_IR_CODES_UPDATED_CONST&label=".$GLOBALS['label']);
			exit();
		}

		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
	}
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$editLabel);
	$output->output();
}

?>

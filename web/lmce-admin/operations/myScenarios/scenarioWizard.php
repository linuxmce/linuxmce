<?php
function scenarioWizard($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/scenarioWizard.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='';
$installationID = (int)@$_SESSION['installationID'];
$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:0;
$from=$_REQUEST['from'];
$roomID=(int)@$_REQUEST['roomID'];
$cgID=$_REQUEST['cgID'];
$section=$_REQUEST['section'];

if($action=='form') {
	if($roomID!=0){
		$queryRoom='SELECT Description FROM Room WHERE PK_Room=?';
		$resRoom=$dbADO->Execute($queryRoom,$roomID);
		$rowRoom=$resRoom->FetchRow();
	}
	
	$queryCG='SELECT Description,Hint FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();

	$filePath=$GLOBALS['scenariosPicsPath'].$cgID.'.png';
	if(file_exists($filePath)){
		$randNumber=rand(0,99999);
		$scenarioImg='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'"><br>';
	}else
		$scenarioImg='&nbsp;';		

	$out.='
		<div align="center" class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>	
		<div align="center" class="err"><B>'.stripslashes(@$_GET['err']).'</B></div>	
	
		<form action="index.php" method="POST" name="scenarioWizard" '.(($wizard==0)?'onSubmit="return validateForm();"':'').' enctype="multipart/form-data">
			<input type="hidden" name="section" value="scenarioWizard">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$roomID.'">
			<input type="hidden" name="cgID" value="'.$cgID.'">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="oldWizard" value="'.$wizard.'">
			<input type="hidden" name="testCmd" value="">
	
		<a href="index.php?section='.$from.'">'.$TEXT_BACK_CONST.'</a>
			<table width="300" align="center">
			<tr class="alternate_back">
				<td><B>'.$TEXT_SCENARIO_NAME_CONST.'</B></td>
				<td><textarea name="description" style="width:100%">'.$rowCG['Description'].'</textarea></td>
			</tr>	
			<tr>
				<td><B>'.$TEXT_HINT_CONST.'</B> </td>
				<td><input type="text" name="hint" value="'.$rowCG['Hint'].'" style="width:100%"></td>
			</tr>
			<tr class="alternate_back">
				<td>&nbsp;</td>
				<td>'.$scenarioImg.'</td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_PICTURE_CONST.'</B> </td>
				<td><input type="file" name="pic" value=""></td>
			</tr>	
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button_fixed" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
			</tr>	
			<tr class="alternate_back">
				<td colspan="2"><B>'.$TEXT_EDIT_SCENARIO_USING_WIZARD_CONST.'</B> <select name="wizard" onChange="document.scenarioWizard.submit();">
					<option value="0" '.(($wizard==0)?'selected':'').'>'.$TEXT_LIGHTING_WIZARD_CONST.'</option>
					<option value="1" '.(($wizard==1)?'selected':'').'>'.$TEXT_CLIMATE_WIZARD_CONST.'</option>
					<option value="2" '.(($wizard==2)?'selected':'').'>'.$TEXT_IRRIGATION_WIZARD_CONST.'</option>
					<option value="3" '.(($wizard==3)?'selected':'').'>'.$TEXT_ADVANCED_WIZARD_CONST.'</option>
				</select>
				</td>
			</tr>
		</table>
		<table align="center">
			<tr>
				<td>';
	switch ($wizard){
		case 0:
			$out.=lightingDevicesTable($cgID,$dbADO);
		break;
		case 1:
			$out.=climateDevicesTable($cgID,$dbADO);
		break;
		case 2:
			$out.=irrigationCommandGroupCommandsTable($cgID,$section,$dbADO);
		break;
		case 3:
			$out.=advancedCommandGroupCommandsTable($cgID,$section,$dbADO);
		break;
	}
	$out.='</td>
		</tr>
	</table>
	</form>
	';
}else{	
	// action='add'
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	if(!$canModifyInstallation){
		Header('Location: index.php?section=scenarioWizard&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
		exit();
	}
	$oldWizard=(int)$_POST['oldWizard'];
	$wizard=(int)$_POST['wizard'];

	$cgDescription=cleanString(@$_POST['description']);
	$cgHint=cleanString(@$_POST['hint']);
	$updateCG='UPDATE CommandGroup SET Description=?, Hint=? WHERE PK_CommandGroup=?';
	$dbADO->Execute($updateCG,array($cgDescription,$cgHint,$cgID));

	switch($oldWizard){
		case 0:
			processLightingScenario($cgID,$dbADO);
			$msg=$TEXT_SCENARIO_UPDATED_CONST;
		break;
		case 1:
			processClimateScenario($cgID,$dbADO);
			$msg=$TEXT_SCENARIO_UPDATED_CONST;
		break;
		case 2:
			processIrrigationScenario($cgID,$section,$dbADO);
			$msg=$TEXT_SCENARIO_UPDATED_CONST;
		break;			
		case -1:
		case 3:
			list ($isModified,$parametersUpdatedAlert,$returnHook)=processAdvancedScenarios($cgID,$section,$dbADO);
			$msg="Command Group ".($isModified?"":"not")." updated! $parametersUpdatedAlert";
		break;	
	}

	// upload scenario picture
	$errNotice='';
	$filePath=$GLOBALS['scenariosPicsPath'].$cgID.'.png';
	if($_FILES['pic']['name']!=''){
		switch($_FILES['pic']['type']){
			case 'image/x-png':
			case 'image/png':
			case 'image/jpg':
			case 'image/pjpeg':
			case 'image/jpeg':
			$invalidType=false;
			break;
			default:
			$invalidType=true;
			break;
		}

		if($invalidType===false){
			if(!move_uploaded_file($_FILES['pic']['tmp_name'],$filePath)){
				$errNotice.=$_FILES['pic']['name'].' wasn\'t uploaded, check the rights for '.$filePath;
			}else{
				// convert to png and/or resize if necessary
				exec('chmod 777 -R '.$filePath);
				$imageinfo = @getimagesize( $filePath );
				if($imageinfo[0]!=160 && $imageinfo[1]!=160){
					$flag=resizeImage($filePath,$filePath,160,160,1);
				}
			}
		}else
		$errNotice.=$_FILES['pic']['name'].' is not a valid PNG file.<br>';
	}
	$errToLink=($errNotice!='')?'&error='.urlencode($errNotice):'';
	
	
	setOrbitersNeedConfigure($installationID,$dbADO);
	if($wizard!=$oldWizard){
		if((int)@$_REQUEST['addNewDevice']!=0)
			$sufix='&addNewDevice='.(int)@$_REQUEST['addNewDevice'];
		header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard".@$sufix.$errToLink.'#hook_0');
		exit();
	}else{
		header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=$TEXT_SCENARIO_UPDATED_CONST"."$errToLink#hook_".@$returnHook);
		exit();
	}
	
	header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard");
}
	switch($from){
		case 'lightingScenarios':
			$backLabel=$TEXT_WIZARD_LIGHTING_SCENARIOS_CONST;
		break;
		case 'climateScenarios':
			$backLabel=$TEXT_WIZARD_CLIMATE_SCENARIOS_CONST;
		break;
		case 'mobileScenarios':
			$backLabel=$TEXT_WIZARD_MOBILE_ORBITER_SCENARIOS_CONST;
		break;
		case 'sleepingScenarios':
			$backLabel=$TEXT_WIZARD_Sleeping_Scenarios_SCENARIOS_CONST;
		break;
		default:
			$backLabel=$TEXT_BACK_CONST;
		break;
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_SCENARIO_CONST.' #'.$cgID);
	$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setNavigationMenu(array($TEXT_MY_SCENARIOS_CONST=>'index.php?section=myScenarios',$backLabel=>'index.php?section='.$from));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();

}
?>

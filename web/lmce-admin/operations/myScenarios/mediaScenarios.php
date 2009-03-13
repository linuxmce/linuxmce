<?php
function mediaScenarios($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaScenarios.lang.php');
	
	global $dbPlutoMainDatabase;	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

//	$dbADO->debug=true;			
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$arrayID=$GLOBALS['ArrayIDForMedia'];
	$templateWizard=$GLOBALS['MediaScenariosTemplate'];			// from table Template, PK_Template for Media Wizard
		
	$queryEntAreas='
		SELECT 
			EntertainArea.*,
			Room.Description AS RoomName 
		FROM EntertainArea
			INNER JOIN Room on FK_Room=PK_Room
		WHERE FK_Installation=?
		ORDER BY RoomName ASC, Description ASC';
	$resEntAreas=$dbADO->Execute($queryEntAreas,$installationID);
	$eaDescriptions=array();
	while($rowEA=$resEntAreas->FetchRow()){
		$eaDescriptions[$rowEA['PK_EntertainArea']]=$rowEA['Description'];
	}
	
	$resEntAreas->MoveFirst();
	$displayedMediaScenarios=array();

	$out=$TEXT_MEDIA_SCENARIOS_NOTE_CONST;
	if($action=='form'){
		$devicesMediaType=getDevicesArrayFromCategory($GLOBALS['rootAVEquipment'],$dbADO);	// start with all A/V/ devices
		$out.=setLeftMenu($dbADO).'
			<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
			</script>
			<form action="index.php" method="POST" name="mediaScenarios">
				<input type="hidden" name="section" value="mediaScenarios">
				<input type="hidden" name="action" value="add">	
				<input type="hidden" name="delCG" value="">	
				<input type="hidden" name="optionEntArea" value="">	
				<input type="hidden" name="optionName" value="">	
				<input type="hidden" name="actionType" value="">
				<input type="hidden" name="EntAreaDescription" value="'.@$eaDescriptions[$_POST['newEntArea']].'">
				<input type="hidden" name="GoTo" value="">
			
			<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
			
			<table align="center">
				<tr>
					<td colspan="2"><B>'.$TEXT_ADD_MEDIA_SCENARIO_CONST.'</B><a name="addMS"></a></td>
				</tr>
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.'</td>
					<td><textarea name="newDescription" rows="3">'.@$_POST['newDescription'].'</textarea></td>
				</tr>
				<tr>
					<td>'.$TEXT_ENTERTAIN_AREA_CONST.'</td>
					<td><select name="newEntArea" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()">
						<option value="0">'.$TEXT_PLEASE_SELECT_CONST.'</option>';
			
			while($rowEntAreas=$resEntAreas->FetchRow()){
				$out.='<option value="'.$rowEntAreas['PK_EntertainArea'].'" '.((@$_POST['newEntArea']==$rowEntAreas['PK_EntertainArea'])?'selected':'').'>'.$rowEntAreas['Description'].'</option>';
			}
			$out.='
					</select></td>
				</tr>';
			if($resEntAreas->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="2" class="err">'.$TEXT_NO_ENTERTAIN_AREA_CONST.' <a href="index.php?section=rooms">'.$TEXT_GOTO_ROOMS_CONST.'</a></td>
				</tr>';
			}
			
			if((int)@$_POST['newEntArea']!=0){
				$queryDevices='
					SELECT DISTINCT Device.*
						FROM Device_EntertainArea
					INNER JOIN Device ON FK_Device=PK_Device
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=Device.FK_DeviceTemplate
					WHERE FK_EntertainArea=?';
				$resDevices=$dbADO->Execute($queryDevices,array($_POST['newEntArea']));
				$deviceTemplatesArray=array();
				while($rowDevices=$resDevices->FetchRow()){
					if(!in_array($rowDevices['PK_Device'],$devicesMediaType)){
						$devicesMediaType[$rowDevices['PK_Device']]=$rowDevices['Description'];
						$deviceTemplatesArray[$rowDevices['PK_Device']]=$rowDevices['FK_DeviceTemplate'];
					}
				}
				$out.='
					<tr>
						<td>'.$TEXT_DEVICE_CONST.'</td>
						<td><select name="MSDevice" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()" '.((substr(@$_POST['MSDevice'],-1)=='-')?'style="background:red;"':'').'>
							<option value="0" style="background:white;">'.$TEXT_PLEASE_SELECT_CONST.'</option>';

				foreach ($devicesMediaType AS $deviceID=>$description){
					$out.='<option value="'.$deviceID.'-'.@$deviceTemplatesArray[$deviceID].'" '.((@$_POST['MSDevice']==$deviceID.'-'.@$deviceTemplatesArray[$deviceID])?'selected':'').' '.((!isset($deviceTemplatesArray[$deviceID]))?'style="background:red;"':'style="background:white;"').'>'.$description.'</option>';
				}
				$out.='
						</select>'.((count($devicesMediaType)!=count($deviceTemplatesArray))?$TEXT_NO_AV_PROPERTIES_SET_CONST:'').'</td>
					</tr>';
			if($resDevices->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="2" class="err">'.$TEXT_NO_MEDIA_DEVICES_CONST.'</td>
				</tr>';
			}

				
				if(isset($_POST['MSDevice']) && $_POST['MSDevice']!='0' && substr($_POST['MSDevice'],-1)!='-'){
					$deviceArray=explode('-',$_POST['MSDevice']);
					$out.='
					<tr>
						<td>Type</td>
						<td><select name="newType" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()">
							<option value="0">'.$TEXT_PLEASE_SELECT_CONST.'</option>';
					$getMediaTypes='
						SELECT * FROM MediaType 
							INNER JOIN DeviceTemplate_MediaType ON FK_MediaType=PK_MediaType
						WHERE FK_DeviceTemplate=? ORDER BY Description ASC';
					
					$resMediaTypes=$dbADO->Execute($getMediaTypes,$deviceArray[1]);
					while($rowMediaTypes=$resMediaTypes->FetchRow()){
						$out.='<option value="'.$rowMediaTypes['PK_MediaType'].'" '.((@$_POST['newType']==$rowMediaTypes['PK_MediaType'])?'selected':'').'>'.$rowMediaTypes['Description'].'</option>';
					}
					$out.='
						</select>
						</td>
					</tr>';
					if($resMediaTypes->RecordCount()==0){
						$out.='
							<tr>
								<td colspan="2" class="err">'.$TEXT_NO_MEDIA_TYPE_CONST.'</td>
							</tr>';
					}						
					
					if(isset($_POST['newType']) && $_POST['newType']!='0'){
						$out.='
						<tr>
							<td colspan="2" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"  ></td>
						</tr>
						';
					}
				}
			}
		$out.='
			</table>
		
			<table width="100%" border="0">';
		$resEntAreas->MoveFirst();
		while($rowEntAreas=$resEntAreas->FetchRow()){

			$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="5" align="left"><B>'.stripslashes($rowEntAreas['Description']).'</B></td>
			</tr>
			<tr>
				<td colspan="5">'.autogeneratedScenarios($arrayID,$rowEntAreas['PK_EntertainArea'],$dbADO,3,'mediaScenarios').'</td>
			</tr>			
			';
			$selectMediaScenarios='
				SELECT CommandGroup.Description, PK_CommandGroup
				FROM CommandGroup
					JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
				WHERE CommandGroup.FK_Array=? AND CommandGroup_EntertainArea.FK_EntertainArea=? AND CommandGroup.FK_Installation=? AND FK_Template=?';
			$resMediaScenarios=$dbADO->Execute($selectMediaScenarios,array($arrayID,$rowEntAreas['PK_EntertainArea'],$installationID,$templateWizard));
			if($resMediaScenarios->RecordCount()==0){
				$out.='
				<tr>
					 <td align="center" colspan="7"><hr>'.$TEXT_NO_MEDIA_SCENARIOS_CONST.'</td>
				</tr>';
			}
			
			while($rowMediaScenarios=$resMediaScenarios->FetchRow()){
				$displayedMediaScenarios[]=$rowMediaScenarios['PK_CommandGroup'];
				$query = "
					SELECT * FROM CommandGroup_Command_CommandParameter 
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command = PK_CommandGroup_Command
					WHERE FK_Command = ? AND FK_CommandGroup=?";
				$resParams=$dbADO->Execute($query,array($GLOBALS['MediaScenariosCommand'],$rowMediaScenarios['PK_CommandGroup']));
				$paramsArray=array();
				if($resParams->RecordCount()!=0){
					
				$out.='
				<tr class="alternate_back">
					<td align="left"><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_TYPE_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_DEVICE_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_DEVICE_TEMPLATE_CONST.'</B></td>
					<td align="centern"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>';
					while($rowParams=$resParams->FetchRow()){
						$paramsArray[$rowParams['FK_CommandParameter']]=$rowParams['IK_CommandParameter'];
					}
				
					If ($paramsArray[$GLOBALS['commandParamPK_MediaType']] != "") {
						$getMediaType='SELECT * FROM MediaType WHERE PK_MediaType=?';
						$resMediaType=$dbADO->Execute($getMediaType,$paramsArray[$GLOBALS['commandParamPK_MediaType']]);
						$rowMediaType=$resMediaType->FetchRow();
						$mediaType=$rowMediaType['Description'];
					} else {
						$mediaType="unspecified";
					}
					
					if ($paramsArray[$GLOBALS['commandParamPK_Device']] != "") {
						$getDevice='SELECT * FROM Device WHERE PK_Device=?';
						$resDevice=$dbADO->Execute($getDevice,$paramsArray[$GLOBALS['commandParamPK_Device']]);
						$rowDevice=$resDevice->FetchRow();
						$deviceName=$rowDevice['Description'];
					} else {
						$deviceName="unspecified";
					}	
					
					if ($paramsArray[$GLOBALS['commandParamPK_DeviceTemplate']] != "") {
						$getDeviceTemplate='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
						$resDeviceTemplate=$dbADO->Execute($getDeviceTemplate,$paramsArray[$GLOBALS['commandParamPK_DeviceTemplate']]);
						$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
						$deviceTemplateName=$rowDevice['Description'];
					} else {
						$deviceTemplateName="unspecified";
					}
				}
								
				$out.='
				<tr>
					<td>'.$rowMediaScenarios['Description'].'</td>
					<td align="center">'.@$mediaType.' #'.@$paramsArray[$GLOBALS['commandParamPK_MediaType']].'</td>
					<td align="center">'.@$deviceName.' #'.@$paramsArray[$GLOBALS['commandParamPK_Device']].'</td>
					<td align="center">'.@$deviceTemplateName.' #'.@$paramsArray[$GLOBALS['commandParamPK_DeviceTemplate']].'</td>
					<td align="center">
						<a href="index.php?section=mediaScenarios&cgID='.$rowMediaScenarios['PK_CommandGroup'].'&action=testScenario">'.$TEXT_TEST_CONST.'</a> 
						<a href="index.php?section=editCommandGroup&cgID='.$rowMediaScenarios['PK_CommandGroup'].'">'.$TEXT_EDIT_CONST.'</a> 
						<a href="#" onClick="javascript:if(confirm(\''.$TEXT_CONFIRM_DELETE_MEDIA_SCENARIO_CONST.'\')){document.mediaScenarios.action.value=\'delete\';document.mediaScenarios.delCG.value=\''.$rowMediaScenarios['PK_CommandGroup'].'\';document.mediaScenarios.submit()}">'.$TEXT_DELETE_CONST.'</a>
					</td>
				</tr>
				';
			}
		}
		
		if(count($displayedMediaScenarios)>0){
			$remotesBtn='<input type="submit" class="button" name="chooseRemotes" value="'.$TEXT_CHOOSE_PREFERRED_REMOTES_CONST.'" onclick="windowOpen(\'index.php?section=mediaRemotes\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\')";>';
		}
			$out.='
			<tr>
				<td colspan="7" align="center">'.@$remotesBtn.' <input type="button" class="button" name="sortScenarios" value="'.$TEXT_SORT_SCENARIOS_CONST.'"  onclick="self.location=\'index.php?section=sortScenarios&sortBy=EntertainArea&from=mediaScenarios\'"></td>
			</tr>
			<input type="hidden" name="displayedMediaScenarios" value="'.(join(',',$displayedMediaScenarios)).'">
				<tr>
					<td colspan="7">&nbsp;</td>
				</tr>
			</table>	
			</form>
			<script>
		 		var frmvalidator = new formValidator("mediaScenarios");
 				//frmvalidator.addValidation("newDescription","req","Please enter a description");			
			</script>
		';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);

		if($action=='testScenario'){
			$scenarioToTest=(int)$_REQUEST['cgID'];
			testScenario($scenarioToTest);
			header("Location: index.php?section=mediaScenarios&msg=$TEXT_MEDIA_SCENARIO_TEST_CONST");
			exit();
		}
		if ($canModifyInstallation) {
			if($action=='delete'){
				$delCG=$_POST['delCG'];
				deleteCommandGroup($delCG,$dbADO);
				setOrbitersNeedConfigure($installationID,$dbADO);
			}else{
				if(isset($_POST['add'])){
					$newDescription=cleanString($_POST['newDescription']);
					$deviceArray=explode('-',$_POST['MSDevice']);
					$newPKDevice=$deviceArray[0];
					$newFKDeviceTemplate=$deviceArray[1];
					$newEntArea=$_POST['newEntArea'];
					$newType=$_POST['newType'];
					$EntAreaDescription=stripslashes($_POST['EntAreaDescription']);

					$insertMediaScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) VALUES (?,?,?,?,?)';
					$dbADO->Execute($insertMediaScenario,array($arrayID,$installationID,$newDescription,$templateWizard,$EntAreaDescription));
					$insertID=$dbADO->Insert_ID();

					$insertDeviceCommandGroup='INSERT INTO CommandGroup_EntertainArea (FK_EntertainArea, FK_CommandGroup,Sort) VALUES (?,?,?)';
					$dbADO->Execute($insertDeviceCommandGroup,array(@$_POST['newEntArea'],$insertID,$insertID));

					$queryMediaPlugin='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?';
					$res=$dbADO->Execute($queryMediaPlugin,array($GLOBALS['rootMediaPlugin'],$installationID));
					if($res->RecordCount()!=0){
						$row=$res->FetchRow();
						$mediaPluginID=$row['PK_Device'];

						$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) values(?,?,?)";
						$insertRs = $dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['MediaScenariosCommand'],$mediaPluginID));
						$CG_C_insertID=$dbADO->Insert_ID();

						$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';

						$values=array();
						$values[$GLOBALS['commandParamPK_Device']]=$newPKDevice;
						$values[$GLOBALS['commandParamPK_MediaType']]=$newType;
						$values[$GLOBALS['commandParamPK_DeviceTemplate']]=$newFKDeviceTemplate;		
						$values[$GLOBALS['commandParamPK_EntertainArea']]=$newEntArea;		
						
						addScenarioCommandParameters($CG_C_insertID,$GLOBALS['MediaScenariosCommand'],$dbADO,$values);						
					
					}
					setOrbitersNeedConfigure($installationID,$dbADO);
					$msg=$TEXT_MEDIA_SCENARIO_ADDED_CONST;
				}

				if(isset($_POST['optionEntArea']) && $_POST['optionEntArea']!=''){

					$FK_EntertainArea=$_POST['optionEntArea'];
					$optionName=$_POST['optionName'];
					$optionValue=$_POST['actionType'];
					if($optionValue=='addOption'){
						$getTemplateID='SELECT * FROM Template WHERE Description=?';
						$resTemplateID=$dbADO->Execute($getTemplateID,'Media Wiz - '.$optionName);
						$rowTemplateID=$resTemplateID->FetchRow();
						$FK_Template=$rowTemplateID['PK_Template'];
						$EntAreaDescription=$_POST['EntAreaDescription'];


						// Aaron program
						$command='sudo -u root /usr/pluto/bin/UpdateEntArea -h localhost -i '.$installationID.' -D '.$dbPlutoMainDatabase.' -e '.$FK_EntertainArea.' -t '.$FK_Template;
						exec($command);

						setOrbitersNeedConfigure($installationID,$dbADO);
						$msg=$TEXT_MEDIA_SCENARIO_ADDED;
					}else{
						$selectOptions='
								SELECT PK_CommandGroup FROM CommandGroup
									INNER JOIN CommandGroup_EntertainArea on CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
									INNER JOIN Template ON FK_Template=PK_Template
								WHERE Template.Description=? AND FK_EntertainArea=?';
						$resOptions=$dbADO->Execute($selectOptions,array('Media Wiz - '.$optionName,$FK_EntertainArea));
						$rowOption=$resOptions->FetchRow();
						deleteCommandGroup($rowOption['PK_CommandGroup'],$dbADO);
						setOrbitersNeedConfigure($installationID,$dbADO);
						$msg=$TEXT_MEDIA_SCENARIO_DELETED_CONST;
					}
				}
			}
		}
	 
		header("Location: index.php?section=mediaScenarios&msg=".@$msg);
	}
	if(@$_REQUEST['GoTo']=='addMS')
		$jumpTo=';self.location=\'#addMS\'';

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_MEDIA_SCENARIOS_CONST);
	$output->setNavigationMenu(array($TEXT_MY_SCENARIOS_CONST=>'index.php?section=myScenarios',$TEXT_MEDIA_SCENARIOS_CONST=>'index.php?section=mediaScenarios'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MEDIA_SCENARIOS_CONST);	
	$output->output();
}
?>

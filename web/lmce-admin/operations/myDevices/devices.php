<?php
function devices($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('devices.lang.php');

	global $dbPlutoMainDatabase,$wikiHost;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'lights';
	$installationID = (int)@$_SESSION['installationID'];

	$extraCategoryArray=array();
	$title=translate('TEXT_'.strtoupper($type).'_TYPE_CONST');
	switch($type){
		case 'interfaces':
			$deviceCategory=$GLOBALS['rootInterfaces'];
			$output->setHelpSrc($wikiHost.'/index.php/Interfaces');
		break;
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$title=translate('TEXT_TITLE_AV_EQUIPMENT_CONST');
		break;
		case 'lights':
			$deviceCategory=$GLOBALS['rootLights'];
			$specificFloorplanType=$GLOBALS['LightingFoorplanType'];
			$output->setHelpSrc($wikiHost.'/index.php/Lights');
		break;
		case 'climate':
			$deviceCategory=$GLOBALS['rootClimate'];
			$specificFloorplanType=$GLOBALS['ClimateFoorplanType'];
			$output->setHelpSrc($wikiHost.'/index.php/Climate');
		break;
		case 'security':
			$deviceCategory=$GLOBALS['rootSecurity'];
			$extraCategoryArray[]=$GLOBALS['rootGenericIO'];
			$specificFloorplanType=$GLOBALS['SecurityFoorplanType'];
			$output->setHelpSrc($wikiHost.'/index.php/Security');
		break;
		case 'surveillance_cameras':
			$deviceCategory=$GLOBALS['rootCameras'];
			$specificFloorplanType=$GLOBALS['SecurityFoorplanType'];
			$output->setHelpSrc($wikiHost.'/index.php/Surveillance_Cameras');
			$GLOBALS['Room_Name']=2;
			$lightsArray=getDevicesArrayFromCategory($GLOBALS['rootLights'],$dbADO);
			$sensorsArray=getDevicesArrayFromCategory($GLOBALS['rootSecurity'],$dbADO);
			
			$lightsRelated=getDevicesRelated($lightsArray,$dbADO);
			$sensorsRelated=getDevicesRelated($sensorsArray,$dbADO);
		break;
		case 'phones':
			$deviceCategory=$GLOBALS['rootPhones'];
			$specificFloorplanType=$GLOBALS['PhonesFoorplanType'];
		break;		
		case 'irrigation':
			$specificFloorplanType=$GLOBALS['ClimateFoorplanType'];
			$deviceCategory=$GLOBALS['IrrigationDevices'];
		break;			
		case 'orbiters':
			$deviceCategory=$GLOBALS['rootOrbiterID'];
		break;			
		default:
			$deviceCategory=$GLOBALS['rootLightsInterfaces'];
	}

	// get the device categories for wizard
	$restrictedCategories=getDescendantsForCategory($deviceCategory,$dbADO);
	if(count($restrictedCategories)==0){
		error_redirect('No devices categories for selected wizard.','');
	}
	
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'ORDER BY Description ASC');
	
	if(isset($_REQUEST['lastAdded']) && (int)$_REQUEST['lastAdded']!=0){
		$rs=$dbADO->Execute('SELECT Comments FROM DeviceTemplate WHERE PK_DeviceTemplate=?',(int)$_REQUEST['lastAdded']);
		$row=$rs->FetchRow();
		if($row['Comments']!=''){
			$out.='<script>
				alert(\''.addslashes($row['Comments']).'\')
			</script>';
		}
	}

	
	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
	<form action="index.php" method="POST" name="devices">
	<script>
	 	var frmvalidator = new formValidator("devices");
	</script>
				
	<input type="hidden" name="section" value="devices">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">	
		
		<div id="preloader" style="display:;">
			<table width="100%">
				<tr>
					<td align="center">'.translate('TEXT_LOADING_NOTICE_CONST').'</td>
				</tr>
			</table>
		</div>
		
		<div id="content" style="display:none;">
		<table align="center" cellpadding="0" cellspacing="0" border="0">
				';			
		
				$displayedDevices=array();
				$GLOBALS['DeviceDataToDisplay']=array();
				$queryDevice='
					SELECT 
						Device.PK_Device,
						Device.Description,
						Device.IPaddress,
						Device.MACaddress,
						Device.FK_Device_ControlledVia,
						Device.FK_Device_RouteTo,
						Device.FK_Room,
						Device.FK_DeviceTemplate,
						DeviceTemplate.Description AS TemplateName, 
						DeviceCategory.Description AS CategoryName, 
						Manufacturer.Description AS ManufacturerName, 
						IsIPBased, 
						Device.FK_Device_ControlledVia
						FK_DeviceCategory,
						DeviceData.Description AS dd_Description, 
						Device_DeviceData.FK_DeviceData,
						ParameterType.Description AS typeParam, 
						Device_DeviceData.IK_DeviceData,
						ShowInWizard,ShortDescription,
						AllowedToModify,
						DeviceTemplate_DeviceData.Description AS Tooltip,
						Parent.Description AS PDescription 
					FROM Device 
					LEFT JOIN Device_DeviceData ON Device_DeviceData.FK_Device=Device.PK_Device
					LEFT JOIN DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData
					LEFT JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
					LEFT JOIN Device Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
					LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
					LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 			
					WHERE DeviceTemplate.FK_DeviceCategory IN ('.join(',',$restrictedCategories).') AND Device.FK_Installation=? ';
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$firstDevice=0;
				$deviceDataArray=array();
				while($rowD=$resDevice->FetchRow()){
					if(!in_array($rowD['PK_Device'],$displayedDevices)){
						$displayedDevices[]=$rowD['PK_Device'];
					}
					
					// fill in the device data array
					if($rowD['PK_Device']!=$firstDevice){
						$firstDevice=$rowD['PK_Device'];
						$deviceDataArray[$firstDevice]=array();
						$deviceDataArray[$firstDevice][]=$rowD;
					}else{
						$deviceDataArray[$firstDevice][]=$rowD;
					}					
				}


			if($resDevice->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="3" align="center">'.translate('TEXT_NO_RECORDS_CONST').'</td>
				</tr>';
			}		
			$resDevice->MoveFirst();
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
					$out.='
					<input type="hidden" name="parent_'.$rowD['PK_Device'].'" value="'.$rowD['FK_Device_ControlledVia'].'">
					<tr class="tablehead">
					<td align="center" width="200"><B>'.translate('TEXT_DESCRIPTION_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
					<td align="center" width="400"><B>'.translate('TEXT_DEVICE_DATA_CONST').'</B></td>
					<td align="center" width="135"><B>'.translate('TEXT_ACTION_CONST').'</B></td>
					</tr>
					<tr class="regular">
						<td class="alternate_back" align="left" valign="top" title="'.translate('TEXT_DEVICE_TEMPLATE_CONST').': '.$rowD['TemplateName'].', '.translate('TEXT_DEVICE_CATEGORY_CONST').': '.$rowD['CategoryName'].', '.strtolower(translate('TEXT_MANUFACTURER_CONST')).': '.$rowD['ManufacturerName'].'">
							'.translate('TEXT_DEVICE_NUM_CONST').': '.$rowD['PK_Device'].'<br>
							'.translate('TEXT_DEVICE_TEMPLATE_NUM_CONST').': '.$rowD['FK_DeviceTemplate'].'<br>
							'.translate('TEXT_CONTROLLED_BY_CONST').': <a href="javascript:windowOpen(\'index.php?section=editDeviceControlledVia&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');" title="'.translate('TEXT_CLICK_TO_CHANGE_CONST').'">'.((is_null($rowD['FK_Device_ControlledVia']))?translate('TEXT_EDIT_CONST'):$rowD['PDescription']).'</a><br><br>
							<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'" style="width:200px;"><br><br>'.pulldownFromArray($roomsArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:200px;"').'
							<br><br>							
							'.device_test_buttons($type,$rowD['PK_Device'],$dbADO).'						
						</td>
						<td align="right" valign="top">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased'],@$specificFloorplanType,1,'textarea').'</td>
						<td align="center" valign="top" class="alternate_back">
							<input value="'.translate('TEXT_HELP_CONST').'" type="button" class="button_fixed" name="help" onClick="self.location=\''.$wikiHost.'/index.php/'.wikiLink($rowD['TemplateName']).'\'"><br>
							<input type="button" class="button_fixed" name="edit_'.$rowD['PK_Device'].'" value="'.translate('TEXT_ADVANCED_CONST').'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
							<input type="submit" class="button_fixed" name="delete_'.$rowD['PK_Device'].'" value="'.translate('TEXT_DELETE_CONST').'"  onClick="if(!confirm(\'Are you sure you want to delete this device?\'))return false;">
						</td>
					</tr>';
					if($type=='surveillance_cameras'){
						$associatedLightText=(count(@$lightsRelated[$rowD['PK_Device']])>0)?join('<br>',array_values($lightsRelated[$rowD['PK_Device']])):'- '.translate('TEXT_NO_DEVICES_ASSOCIATED_CONST').' -';
						$associatedSensorsText=(count(@$sensorsRelated[$rowD['PK_Device']])>0)?join('<br>',array_values($sensorsRelated[$rowD['PK_Device']])):'- '.translate('TEXT_NO_DEVICES_ASSOCIATED_CONST').' -';
						
						$out.='
						<tr>
							<td colspan="3" height="1" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
						</tr>
						<tr>
							<td style="padding-left:10px;"class="alternate_back"><B>'.translate('TEXT_LIGHTS_CONST').':</B> </td>
							<td align="left">'.$associatedLightText.'</td>
							<td align="center" class="alternate_back"><input type="button" class="button_fixed" name="edit1" value='.translate('TEXT_EDIT_CONST').' onClick="windowOpen(\'index.php?section=editCameraRelated&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'&type=lights\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');"></td>
						</tr>
						<tr>
							<td colspan="3" height="1" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
						</tr>
						<tr>
							<td style="padding-left:10px;" class="alternate_back"><B>'.translate('TEXT_SENSORS_CONST').':</B> </td>
							<td align="left">'.$associatedSensorsText.'</td>
							<td align="center" class="alternate_back"><input type="button" class="button_fixed" name="edit2" value='.translate('TEXT_EDIT_CONST').' onClick="windowOpen(\'index.php?section=editCameraRelated&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'&type=sensors\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');"></td>
						</tr>';
					}
					$out.='
					<tr>
						<td colspan="3" height="3" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
					</tr>
					<tr><td colspan="3">&nbsp;</td></tr>
					<tr><td colspan="3">&nbsp;</td></tr>';
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$zoneBtn=($type=='security')?' <input type="button" class="button" name="gotoZones" value="Security Zones" onClick="self.location=\'index.php?section=zones&type=security\'">':'';
				$updateCancelBtns='<input type="submit" class="button" name="update" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'">';
			}
			$addGC100Btn=($type=='interfaces')?'<input type="submit" class="button" name="addGC100" value="'.translate('TEXT_ADD_GC100_CONST').'"> ':'';
			
			$out.='
				<tr>
					<td colspan="7">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="7" align="center">'.@$updateCancelBtns.' '.@$zoneBtn.' '.$addGC100Btn.'<input type="button" class="button" name="button" value="'.translate('TEXT_ADD_DEVICE_CONST').'" onClick="document.devices.action.value=\'externalSubmit\';document.devices.submit();windowOpen(\'index.php?section=deviceTemplatePicker&from='.urlencode('devices&type='.$type).'&categoryID='.$deviceCategory.'&allowAdd=1&parentID=0\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
			</div>
	</form>
	';
		$output->setScriptInBody('onLoad="document.getElementById(\'preloader\').style.display=\'none\';document.getElementById(\'content\').style.display=\'\';";');			
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&type=$type&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
				
				if($type=='phones'){
					$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh';
					exec_batch_command($cmd);
				}
			}
		}
				
		$msg='';
		if(isset($_POST['update']) || $action=='externalSubmit' || isset($_POST['addGC100']) || isset($_POST['device_test'])){
			if(isset($_POST['addGC100'])){
				$infraredPlugIn=getInfraredPlugin($installationID,$dbADO);
				
				$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$infraredPlugIn.' 1 276';
				exec($commandToSend);
				$msg='Command to detect gc100 was sent: '.$commandToSend;
			}
			
			setDCERouterNeedConfigure($installationID,$dbADO);
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
	
				$description=stripslashes(@$_POST['description_'.$value]);
				if(isset($_POST['ip_'.$value])){
					$oldIpAddress=$_POST['oldIP_'.$value];
					$oldMacAddress=$_POST['oldMAC_'.$value];
						
					$ip=$_POST['ip_'.$value];
					$mac=$_POST['mac_'.$value];
					$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					
					// set DHCP settings
					$cmd='sudo -u root /usr/pluto/bin/Diskless_RenameFS.sh --devid '.$key.' --oldip "'.$oldIpAddress.'" --newip "'.$ip.'" --oldmac "'.$oldMacAddress.'" --newmac "'.$mac.'"';
					exec($cmd);
				}
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				
				$updateDevice='UPDATE Device SET Description=?, FK_Room=? '.@$updateMacIp.' WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($description,$room,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					if(isset($_POST['oldDeviceData_'.$value.'_'.$ddValue])){
						$deviceData=cleanString(@$_POST['deviceData_'.$value.'_'.$ddValue]);
						$oldDeviceData=cleanString(@$_POST['oldDeviceData_'.$value.'_'.$ddValue]);
						$isDisabled=(int)@$_POST['isDisabled_'.$value.'_'.$ddValue];
						if($oldDeviceData!=$deviceData && $isDisabled!=1){
							if($oldDeviceData=='NULL'){
								$insertDDD='
									INSERT INTO Device_DeviceData 
										(FK_Device, FK_DeviceData, IK_DeviceData)
									VALUES 
										(?,?,?)';
								$dbADO->Execute($insertDDD,array($value,$ddValue,$deviceData));
							}
							else{
								$updateDDD='
									UPDATE Device_DeviceData 
										SET IK_DeviceData=? 
									WHERE FK_Device=? AND FK_DeviceData=?';
								$dbADO->Execute($updateDDD,array($deviceData,$value,$ddValue));
							}
						}
					}
				}
			}
			if($type=='phones'){
				$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh';
				exec_batch_command($cmd);
			}			
			
			$msg.='<br>Devices updated.';
			
			foreach($displayedDevicesArray as $value){
				process_test_device($value,@$_POST['parent_'.$value],@$_POST['deviceData_'.$value.'_12'],$dbADO);			
			}
		}
		
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$insertID=exec_batch_command('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);
				$insertID=createDevice($deviceTemplate,$installationID,NULL,NULL,$dbADO);
				setDCERouterNeedConfigure($installationID,$dbADO);
			}
			
			if($type=='phones'){
				$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh';
				exec_batch_command($cmd);
			}
			header("Location: index.php?section=devices&type=$type&lastAdded=$deviceTemplate");
			exit();
		}
		header("Location: index.php?section=devices&type=$type&msg=".urlencode($msg));
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle($title);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(((isset($title))?$title:translate('TEXT_'.strtoupper($type).'_TYPE_CONST'))=>'index.php?section=devices&type='.$type));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getDevicesRelated($devicesRelated,$dbADO){
	$devices=array();
	if(count($devicesRelated)==0){
		return $devices;
	}
	
	$res=$dbADO->Execute('SELECT * FROM Device_Device_Related WHERE (FK_Device IN ('.join(',',array_keys($devicesRelated)).') OR FK_Device_Related IN ('.join(',',array_keys($devicesRelated)).'))');
	while($row=$res->FetchRow()){
		if(isset($devicesRelated[$row['FK_Device']])){
			$devices[ $row['FK_Device_Related'] ][ $row['FK_Device'] ]=$devicesRelated[$row['FK_Device']];
		}else{
			$devices[$row['FK_Device']][$row['FK_Device_Related']]=$devicesRelated[$row['FK_Device_Related']];
		}
	}
	return $devices;
}

function device_test_buttons($type,$deviceID,$dbADO){
	//TODO: it would be nice to make a new file with DT constants...
	define('WALL_OUTLET', 1897);
	define('STANDARD_THERMOSTAT', 4);
	define('LIGHT_SWITCH', 37);
	define('LIGHT_SWITCH_DIMMABLE', 38);
	define('LIGHT_DRAPES_SWITCH', 68);
	define('LIGHT_SWITCH_RGB', 1993);
	define('STANDARD_SPRINKLER', 1637);
	define('STANDARD_IRRIGATION_SPRINKLER', 1780);
	define('LIGHT_HUE', 2257);

	$query='SELECT FK_DeviceTemplate FROM Device WHERE PK_Device='.$deviceID.'';
	$res=$dbADO->Execute($query);
	$row=$res->FetchRow();
	$deviceTemplate=$row['FK_DeviceTemplate'];

	$out='<center><B>'.translate('TEXT_TEST_COMMANDS_CONST').':</B></center>';
	if($type=='irrigation'){
		if($deviceTemplate==STANDARD_IRRIGATION_SPRINKLER){
		$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
		       <input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">
			<input type="submit" class="button" name="delay_'.$deviceID.'" value="Delay 5 min.">
		';
		}
		return $out;
	}
	else if($type=='lights'){
		$out.='
		<input type="hidden" name="device_test" value="1">';
		
		if($deviceTemplate==LIGHT_SWITCH) {
			$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
			<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">';
		}

		else if ($deviceTemplate==LIGHT_SWITCH_DIMMABLE or $deviceTemplate==LIGHT_SWITCH_RGB or $deviceTemplate==LIGHT_HUE) {
			$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
			<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">
			<input type="submit" class="button" name="50_'.$deviceID.'" value="50%"><br>';

		}
		else if($deviceTemplate==LIGHT_DRAPES_SWITCH) {
			$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_CLOSE_CONST').'"> 
			<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OPEN_CONST').'">
			<input type="submit" class="button" name="50_'.$deviceID.'" value="50%"><br>';
		} else {
			$out.='No test commands supported for this lighting device';
		}

		return $out;
	} 
	else if($type=='climate') {
		$out.='
		<input type="hidden" name="device_test" value="1">';
		
		if($deviceTemplate==WALL_OUTLET) { 
			$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
			<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">';
		}
		else if($deviceTemplate==STANDARD_SPRINKLER) { 
			$out.='<input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
			<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">';
		}
		else if($deviceTemplate==STANDARD_THERMOSTAT) {
			$out.='<table border="0">
			<tr>
				<td>'.translate('TEXT_POWER_CONST').' </td>
				<td><input type="submit" class="button" name="on_'.$deviceID.'" value="'.translate('TEXT_ON_CONST').'"> 
				<input type="submit" class="button" name="off_'.$deviceID.'" value="'.translate('TEXT_OFF_CONST').'">
				<input type="submit" class="button" name="50_'.$deviceID.'" value="50%"
				</td>
			</tr>
			<tr>
				<td>'.translate('TEXT_FAN_CONST').': </td>
				<td><input type="submit" class="button" name="fan_on_'.$deviceID.'" value="Fan On">
					<input type="submit" class="button" name="fan_auto_'.$deviceID.'" value="Fan Auto"><br> 
				</td>
			</tr>
			<tr>
				<td>'.translate('TEXT_MODE_CONST').': </td>
				<td><input type="submit" class="button" name="mode_heat_'.$deviceID.'" value="'.translate('TEXT_HEAT_CONST').'">
					<input type="submit" class="button" name="mode_cool_'.$deviceID.'" value="'.translate('TEXT_COOL_CONST').'">
					<input type="submit" class="button" name="mode_auto_'.$deviceID.'" value="'.translate('TEXT_AUTO_CONST').'">
				</td>
			</tr>
			<tr>
				<td>'.translate('TEXT_TEMP_CONST').': </td>
				<td><input type="submit" class="button" name="set_temp72_'.$deviceID.'" value="72 deg.">
				<input type="submit" class="button" name="set_temp68_'.$deviceID.'" value="68 deg.">
				</td>
			</tr>	
			</table>	
			';
		} else {
			$out.=translate('TEXT_NO_TEST_CLIMATE_CONST');
		}
		return $out;
	}
	
	return '';
}

function process_test_device($deviceID,$parentID,$port,$dbADO){
	$cmd='sudo -u root /usr/pluto/bin/MessageSend localhost -targetType device 0 '.$deviceID.' 1 ';
	
	if(isset($_POST['on_'.$deviceID])){
		$cmd.='192';
		$send=1;
	}
	if(isset($_POST['off_'.$deviceID])){
		$cmd.='193';
		$send=1;
	}
	if(isset($_POST['50_'.$deviceID])){
		$cmd.='184 76 50';
		$send=1;
	}
	if(isset($_POST['fan_on_'.$deviceID])){
		$cmd.='279 8 1';
		$send=1;
	}
	if(isset($_POST['fan_auto_'.$deviceID])){
		$cmd.='279 8 0';
		$send=1;
	}
	if(isset($_POST['mode_heat_'.$deviceID])){
		$cmd.='280 8 H';
		$send=1;
	}
	if(isset($_POST['mode_cool_'.$deviceID])){
		$cmd.='280 8 C';
		$send=1;
	}
	if(isset($_POST['mode_auto_'.$deviceID])){
		$cmd.='280 8 A';
		$send=1;
	}
	if(isset($_POST['set_temp72_'.$deviceID])){
		$cmd.='278 5 72';
		$send=1;
	}
	if(isset($_POST['set_temp68_'.$deviceID])){
		$cmd.='278 5 68';
		$send=1;
	}
	if(isset($_POST['delay_'.$deviceID])){
		$cmd.='257 102 5';
		$send=1;
	}
	if(@$send==1){
		exec_batch_command($cmd);
	}
	return;
}
?>

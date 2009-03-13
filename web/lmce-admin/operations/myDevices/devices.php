<?
function devices($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/devices.lang.php');

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
	$title=strtoupper(str_replace('_',' ',$type));
	switch($type){
		case 'interfaces':
			$deviceCategory=$GLOBALS['rootInterfaces'];
			$output->setHelpSrc($wikiHost.'/index.php/Interfaces');
		break;
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$title=$TEXT_TITLE_AV_EQUIPMENT_CONST;
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
					<td align="center">'.$TEXT_LOADING_NOTICE_CONST.'</td>
				</tr>
			</table>
		</div>
		
		<div id="content" style="display:none;">
		<table align="center" cellpadding="0" cellspacing="0">
				<tr class="tablehead">
					<td align="center"><B>#</B></td>
					<td align="center"><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ROOM_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_CONTROLLED_BY_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_DEVICE_DATA_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>';			
		
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
					<td colspan="5" align="center">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>';
			}		
			$resDevice->MoveFirst();
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
					$out.='
					<input type="hidden" name="parent_'.$rowD['PK_Device'].'" value="'.$rowD['FK_Device_ControlledVia'].'">
					<tr class="regular">
						<td align="center" class="alternate_back">'.$rowD['PK_Device'].'</td>
						<td class="alternate_back" align="center" title="'.$TEXT_DEVICE_TEMPLATE_CONST.': '.$rowD['TemplateName'].', '.$TEXT_DEVICE_CATEGORY_CONST.': '.$rowD['CategoryName'].', '.strtolower($TEXT_MANUFACTURER_CONST).': '.$rowD['ManufacturerName'].'"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"></td>
						<td>'.pulldownFromArray($roomsArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room']).'</td>
						<td class="alternate_back"><a href="javascript:windowOpen(\'index.php?section=editDeviceControlledVia&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');" title="'.$TEXT_CLICK_TO_CHANGE_CONST.'">'.((is_null($rowD['FK_Device_ControlledVia']))?$TEXT_EDIT_CONST:$rowD['PDescription']).'</a></td>
						<td align="right" valign="top">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased'],@$specificFloorplanType,1,'textarea').'</td>
						<td align="center" valign="center" class="alternate_back">
							'.lights_test_buttons($type,$rowD['PK_Device'],$dbADO).'
							<input value="'.$TEXT_HELP_CONST.'" type="button" class="button_fixed" name="help" onClick="self.location=\''.$wikiHost.'/index.php/'.wikiLink($rowD['TemplateName']).'\'"><br>
							<input type="button" class="button_fixed" name="edit_'.$rowD['PK_Device'].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
							<input type="submit" class="button_fixed" name="delete_'.$rowD['PK_Device'].'" value="'.$TEXT_DELETE_CONST.'"  onClick="if(!confirm(\'Are you sure you want to delete this device?\'))return false;">
						</td>
					</tr>';
					if($type=='surveillance_cameras'){
						$associatedLightText=(count(@$lightsRelated[$rowD['PK_Device']])>0)?join(', ',array_values($lightsRelated[$rowD['PK_Device']])):'- '.$TEXT_NO_DEVICES_ASSOCIATED_CONST.' -';
						$associatedSensorsText=(count(@$sensorsRelated[$rowD['PK_Device']])>0)?join(', ',array_values($sensorsRelated[$rowD['PK_Device']])):'- '.$TEXT_NO_DEVICES_ASSOCIATED_CONST.' -';
						
						$out.='
						<tr class="alternate_back">
							<td colspan="5" style="padding-left:10px;"><B>'.$TEXT_LIGHTS_CONST.':</B> '.$associatedLightText.'</td>
							<td><input type="button" class="button_fixed" name="edit1" value='.$TEXT_EDIT_CONST.' onClick="windowOpen(\'index.php?section=editCameraRelated&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'&type=lights\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');"></td>
						</tr>
						<tr class="alternate_back">
							<td colspan="5" style="padding-left:10px;"><B>'.$TEXT_SENSORS_CONST.':</B> '.$associatedSensorsText.'</td>
							<td><input type="button" class="button_fixed" name="edit2" value='.$TEXT_EDIT_CONST.' onClick="windowOpen(\'index.php?section=editCameraRelated&deviceID='.$rowD['PK_Device'].'&from='.urlencode('devices&type='.$type).'&type=sensors\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');"></td>
						</tr>';
					}
					$out.='
					<tr>
						<td colspan="8" height="3" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
					</tr>';
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$zoneBtn=($type=='security')?' <input type="button" class="button" name="gotoZones" value="Security Zones" onClick="self.location=\'index.php?section=zones&type=security\'">':'';
				$updateCancelBtns='<input type="submit" class="button" name="update" value="Update"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">';
			}
			$addGC100Btn=($type=='interfaces')?'<input type="submit" class="button" name="addGC100" value="Add gc100"> ':'';
			
			$out.='
				<tr>
					<td colspan="7">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="7" align="center">'.@$updateCancelBtns.' '.@$zoneBtn.' '.$addGC100Btn.'<input type="button" class="button" name="button" value="Add device" onClick="document.devices.action.value=\'externalSubmit\';document.devices.submit();windowOpen(\'index.php?section=deviceTemplatePicker&from='.urlencode('devices&type='.$type).'&categoryID='.$deviceCategory.'&allowAdd=1&parentID=0\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
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
			header("Location: index.php?section=devices&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
				
				if($type=='phones'){
					$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.$value;
					exec_batch_command($cmd);
				}
			}
		}
				
		$msg='';
		if(isset($_POST['update']) || $action=='externalSubmit' || isset($_POST['addGC100']) || isset($_POST['lights_test'])){
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
				$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.join(' ',$displayedDevicesArray);
				exec_batch_command($cmd);
			}			
			
			$msg.='<br>Devices updated.';
			
			foreach($displayedDevicesArray as $value){
				process_test_lights($value,@$_POST['parent_'.$value],@$_POST['deviceData_'.$value.'_12'],$dbADO);			
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
				$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.$insertID;
				exec_batch_command($cmd);
			}
			header("Location: index.php?section=devices&type=$type&lastAdded=$deviceTemplate");
			exit();
		}
		header("Location: index.php?section=devices&type=$type&msg=".urlencode($msg));
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($title);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(((isset($title))?$title:strtoupper(str_replace('_',' ',$type)))=>'index.php?section=devices&type='.$type));
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
			$devices[$row['FK_Device_Related']][$row['FK_Device']]=$devicesRelated[$row['FK_Device']];
		}else{
			$devices[$row['FK_Device']][$row['FK_Device_Related']]=$devicesRelated[$row['FK_Device_Related']];
		}
	}
	return $devices;
}

function lights_test_buttons($type,$deviceID,$dbADO){
	if($type!='lights'){
		return '';
	}
	
	$out='
	<input type="hidden" name="lights_test" value="1"> 
	<input type="submit" class="button" name="on_'.$deviceID.'" value="ON"> 
	<input type="submit" class="button" name="off_'.$deviceID.'" value="OFF"> 
	<input type="submit" class="button" name="50_'.$deviceID.'" value="50%"><br>';
	
	return $out;
}

function process_test_lights($deviceID,$parentID,$port,$dbADO){
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
	
	if(@$send==1){
		exec_batch_command($cmd);
	}
}
?>

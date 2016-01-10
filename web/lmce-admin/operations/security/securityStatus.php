<?php
function securityStatus($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/securityStatus.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$alarmModesArray=array("UNARMED","ARMED", "AT HOME SECURE", "SLEEPING", "ENTERTAINING", "EXTENDED AWAY");
	$bypassArray=array("NORMAL","WAIT", "BYPASS", "PERMBYPASS");
	$deviceCategory=$GLOBALS['rootSecurity'];

	$displayedDevices=array();
	if($action=='form'){
		getDeviceCategoryChildsArray($deviceCategory,$dbADO);
		$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;
		
		$queryDeviceTemplate='
			SELECT * FROM DeviceTemplate 
				WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
			ORDER BY Description ASC';
		$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
		$DTArray=array();
		while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
			$DTArray[$rowDeviceCategory['Description']]=$rowDeviceCategory['PK_DeviceTemplate'];
		}
		if(count($DTArray)==0)
			$DTArray[]=0;
		
		$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
		$resRooms=$dbADO->Execute($queryRooms,$installationID);
		$roomArray=array();
		while($rowRoom=$resRooms->FetchRow()){
			$roomArray[$rowRoom['PK_Room']]=$rowRoom['Description'];
		}
	
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="securityStatus">
	<input type="hidden" name="section" value="securityStatus">
	<input type="hidden" name="action" value="update">		
	
	<table align="center" cellpading="3" cellspacing="3">
			<tr>
				<td align="right"> <B>'.$TEXT_ALARM_CONST.':</B></td>
				<td align="right"><select name="globalAlarmMode">
					<option value="0">- '.$TEXT_SELECT_MODE_CONST.' -</option>';
	foreach($alarmModesArray AS $alarmMode){
		$out.='<option value="'.$alarmMode.'">'.$alarmMode.'</option>';
	}
	$out.='
				</select></td>
				<td colspan="4"><input type="submit" class="button" name="setAlarm" value="'.$TEXT_SET_MODE_CONST.'"></td>
			</tr>
			<tr>
				<td align="center" colspan="7">&nbsp;</td>
			</tr>
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_DEVICE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DEVICE_TEMPLATE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_ALARM_MODE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_BYPASS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DELAY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_TRIPPED_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_FLOORPLANOBJECTTYPE_CONST.'</B></td>
			</tr>';
		foreach($roomArray as $roomID=>$roomName){
			$out.='
				<tr>
					<td colspan="7" align="center" class="alternate_back"><B>'.$roomName.'</B></td>
				</tr>';
			$queryDevice='
				SELECT 
					Device.Description, State,Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType,PK_Device
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					LEFT JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room=?';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID,$roomID));
			if($resDevice->RecordCount()==0){
				$out.='
						<tr>
							<td colspan="7" align="center">'.$TEXT_NO_DEVICES_CONST.'</td>
						</tr>
					';
			}
		
			while($rowD=$resDevice->FetchRow()){
				$displayedDevices[]=$rowD['PK_Device'];
				$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center"><select name="alarmMode_'.$rowD['PK_Device'].'">';
					$stateParts=explode(',',$rowD['State']);
					foreach($alarmModesArray AS $alarmMode){
						$out.='<option value="'.$alarmMode.'" '.(($alarmMode==$stateParts[0])?'selected':'').'>'.$alarmMode.'</option>';
					}
				$out.='
				</select></td>
					<td align="center"><select name="bypass_'.$rowD['PK_Device'].'">';
					foreach ($bypassArray AS $bypass){
						$out.='<option value="'.$bypass.'" '.(($bypass==@$stateParts[1])?'selected':'').'>'.$bypass.'</option>';
					}
					$out.='</select></td>
					<td align="center"><input type="checkbox" name="delay" '.((@$stateParts[2]!='')?'checked':'').' disabled></td>
					<td align="center"><input type="checkbox" name="tripped" '.(($rowD['Status']=='TRIPPED')?'checked':'').' disabled></td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>
				<input type="hidden" name="oldDelay_'.$rowD['PK_Device'].'" value="'.@$stateParts[2].'">
					';
			}
		}
		$out.='
				<tr>
					<td colspan="7" align="center" bgcolor="#EEEEEE"><B>'.$TEXT_NOT_ASIGNED_CONST.'</B></td>
				</tr>
			';
			$queryDevice='
				SELECT 
					Device.Description, State, Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType, PK_Device
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					LEFT JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room IS NULL';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID));
		if($resDevice->RecordCount()==0){
			$out.='
					<tr>
						<td colspan="7" align="center">'.$TEXT_NO_DEVICES_CONST.'</td>
					</tr>
				';
		}
	
		while($rowD=$resDevice->FetchRow()){
			$displayedDevices[]=$rowD['PK_Device'];
			$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center"><select name="alarmMode_'.$rowD['PK_Device'].'">';
			$stateParts=explode(',',$rowD['State']);
			foreach($alarmModesArray AS $alarmMode){
				$out.='<option value="'.$alarmMode.'" '.(($alarmMode==$stateParts[0])?'selected':'').'>'.$alarmMode.'</option>';
			}
			$out.='
				</select></td>
					<td align="center"><select name="bypass_'.$rowD['PK_Device'].'">';
			foreach ($bypassArray AS $bypass){
				$out.='<option value="'.$bypass.'" '.(($bypass==@$stateParts[1])?'selected':'').'>'.$bypass.'</option>';
			}
			$out.='</select></td>
					<td align="center"><input type="checkbox" name="delay" '.((@$stateParts[2]!='')?'checked':'').' disabled></td>
					<td align="center"><input type="checkbox" name="tripped" '.(($rowD['Status']!='')?'TRIPPED':'').' disabled></td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>
				<input type="hidden" name="oldDelay_'.$rowD['PK_Device'].'" value="'.@$stateParts[2].'">
					';
		}

	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td align="center" colspan="7"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>';
	}

	$out.='
	</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
	</form>';
	}else{
		// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=securityStatus&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		$globalAlarmMode=$_POST['globalAlarmMode'];

		foreach ($displayedDevicesArray AS $deviceID){
			$alarmMode=($globalAlarmMode!='0')?$globalAlarmMode:$_POST['alarmMode_'.$deviceID];
			$bypass=(@$_POST['bypass_'.$deviceID]=='NORMAL')?'':@$_POST['bypass_'.$deviceID];
			$delay=@$_POST['oldDelay_'.$deviceID];
			
			$newState=$alarmMode.(($bypass=='' && $delay=='')?'':','.$bypass.','.$delay);
						
			$dbADO->Execute('UPDATE Device SET State=? WHERE PK_Device=?',array($newState,$deviceID));
			
			header("Location: index.php?section=securityStatus&msg=$TEXT_SECURITY_STATUS_UPDATED_CONST");
		}
	}

	$output->setMenuTitle($TEXT_SECURITY_CONST.' |');
	$output->setPageTitle($TEXT_SECURITY_STATUS_CONST);
	$output->setNavigationMenu(array($TEXT_SECURITY_STATUS_CONST=>'index.php?section=securityStatus'));				
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SECURITY_STATUS_CONST);
	$output->output();
}
?>

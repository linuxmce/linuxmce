<?php
function securitySettings($output,$dbADO,$securitydbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/securitySettings.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	$deviceCategory=$GLOBALS['rootSecurity'];
	
	$queryAlertTypes='SELECT * FROM AlertType';
	$res=$securitydbADO->Execute($queryAlertTypes);
	$alertTypesLabels=array();
	while($row=$res->FetchRow()){
		$alertTypesLabels[$row['PK_AlertType']]=$row['Description'];
	}
	
	$pullDownArray = array(0=>$TEXT_DO_NOTHING_CONST,2=>$TEXT_ANNOUNCEMENT_CONST,3=>$TEXT_SNAP_PICTURE_CONST);
	
	$properties = array('disarmed', 'armed - away', 'armed - at home', 'sleeping', 'entertaining', 'extended away');
	
	$SecurityDevicesDT=getDeviceTemplatesFromCategory($GLOBALS['rootSecurity'],$dbADO,1);
	$CamerasDevicesDT=getDeviceTemplatesFromCategory($GLOBALS['rootCameras'],$dbADO,1);

	$DTIDArray=array_keys($SecurityDevicesDT+$CamerasDevicesDT);
	$DTArray=array_values($SecurityDevicesDT+$CamerasDevicesDT);

	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		
	<form action="index.php" method="POST" name="securitySettings">
	<input type="hidden" name="section" value="securitySettings">
	<input type="hidden" name="action" value="add">	
		
	<p align="center">'.$TEXT_REACTION_TO_SENSORS_NOTE_CONST.'</p>
		<table align="center">
			<tr class="tablehead">
				<td></td>
				<td align="center"><B>'.$TEXT_MONITOR_MODE_CONST.'</B></td>';
			foreach($properties AS $label){
				$out.='
					<td align="center"><B>N</B></td>
					<td align="center"><B>'.$label.'</B></td>';
			}
		$out.='				
			</tr>';
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$joinArray[]=0;
				$queryDevice='
					SELECT 
						Device.*,DeviceTemplate.Description AS DTemplate
					FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE
						FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[]=$rowD['PK_Device'];
					$queryAlertType='
						SELECT Device_DeviceData.IK_DeviceData
						FROM Device_DeviceData 
						WHERE FK_Device=? AND FK_DeviceData=?';
					$resAlertType=$dbADO->Execute($queryAlertType,array($rowD['PK_Device'],$GLOBALS['securityAlertType']));
					if($resAlertType->RecordCount()>0){
						$rowAlertType=$resAlertType->FetchRow();
						$pullDownArray[1]=@$alertTypesLabels[$rowAlertType['IK_DeviceData']];
						$selectedAlertType=$rowAlertType['IK_DeviceData'];
					}else
						$selectedAlertType=0;
					ksort($pullDownArray);
					reset($pullDownArray);
					$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
					$resDDD=$dbADO->Execute($queryDDD,array($rowD['PK_Device'],$GLOBALS['securityAlert']));
					if($resDDD->RecordCount()>0){
						$rowDDD=$resDDD->FetchRow();
						$oldProperties=$rowDDD['IK_DeviceData'];
						$oldPropertiesArray=explode(',',$oldProperties);
					}else {
						switch($selectedAlertType){
							case 1:		// Security Breach
								$oldPropertiesArray=array(0,0,1,0,0,0,1);
							break;
							case 2:		// Fire Alarm
								$oldPropertiesArray=array(0,1,1,1,1,1,1);
							break;
							case 3:		// Air Quality
								$oldPropertiesArray=array(0,1,1,1,1,1,1);
							break;
							case 4:		// Movement
								$oldPropertiesArray=array(0,0,0,2,0,2,0);
							break;
							default:		// no Alert Type
								$oldPropertiesArray=array(0,0,0,0,0,0,0);
							break;
						}
						unset($oldProperties);
					}
					$out.='<input type="hidden" name="oldProperties_'.$rowD['PK_Device'].'" value="'.@$oldProperties.'">';
					$bgcolor=(count($displayedDevices)%2==0)?'#FFFFFF':'#F0F3F8';
					$out.='
						<tr bgcolor="'.$bgcolor.'">
							<td align="center"><b>'.$rowD['Description'].'</b><br>('.$rowD['DTemplate'].')</td>';
					$out.='<td align="center"><input type="checkbox" name="monitor_mode_'.$rowD['PK_Device'].'" '.((@$oldPropertiesArray[0]==1)?'checked':'').'></td>';
					foreach($properties AS $itemNo=> $itemValue){
						$isNotifyChecked=(substr(@$oldPropertiesArray[$itemNo+1],0,1)=='N')?'checked':'';
						$out.='
							<td><input type="checkbox" name="notify_'.str_replace(' ','',$itemValue).'_'.$rowD['PK_Device'].'" '.$isNotifyChecked.'></td>
							<td><select name="'.str_replace(' ','',$itemValue).'_'.$rowD['PK_Device'].'">';
						foreach($pullDownArray AS $key=>$value){
							$out.='<option value="'.$key.'" '.((str_replace('N','',@$oldPropertiesArray[$itemNo+1])==$key)?'selected':'').'>'.$value.'</option>';
						}
						$out.='</select></td>';
					}
					$out.='</tr>';
				}
				$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
		$out.='
			<tr>
				<td colspan="13" align="left">* '.$TEXT_REACTION_TO_SENSORS_NOTIFICATION_CONST.'</td>
			</tr>		
			<tr>
				<td colspan="13" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>
		</table>
		</form>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=securitySettings&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		// process and redirect
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray AS $device){
			$oldProperties=$_POST['oldProperties_'.$device];
			$newProperties=(isset($_POST['monitor_mode_'.$device]))?1:0;
			foreach($properties AS $itemNo=> $itemValue){
				$notify=(isset($_POST['notify_'.str_replace(' ','',$itemValue).'_'.$device]))?'N':'';
				$newProperties.=','.$notify.$_POST[str_replace(' ','',$itemValue).'_'.$device];
			}
			$updateDDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_DeviceData=? AND FK_Device=?';
			$dbADO->Execute($updateDDD,array($newProperties,$GLOBALS['securityAlert'],$device));
		}		
		
		header("Location: index.php?section=securitySettings&msg=$TEXT_SECURITY_SETTINGS_UPDATED_CONST");		
	}
	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_REACTION_TO_SENSORS_CONST);
	$output->setNavigationMenu(array($TEXT_REACTION_TO_SENSORS_CONST=>'index.php?section=securitySettings'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_REACTION_TO_SENSORS_CONST);
	$output->output();
}
?>
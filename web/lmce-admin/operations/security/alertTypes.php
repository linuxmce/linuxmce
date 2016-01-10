<?php
function alertTypes($output,$securitydbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/alertTypes.lang.php');
	
	/* @var $securitydbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);
	$deviceNames=getDeviceNames($dbADO);
	$securityPlugIn=getDeviceFromDT($installationID,$GLOBALS['SecurityPlugin'],$dbADO);
	
	if ($action=='form') {
		$out.=setLeftMenu($dbADO).'<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
			<form action="index.php" method="post" name="alertTypes">
			<input type="hidden" name="section" value="alertTypes">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="deviceToRemove" value="">
			<table border="0" align="center">
				<tr class="tablehead">
					<td align="center"><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_DELAY_BEFORE_ALARM_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_EXIT_DELAY_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ALARM_DURATION_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_POOL_ALERTS_CONST.'</B></td>
				</tr>
			';
		$displayedAlertTypes=array();
		$query='SELECT * FROM AlertType ORDER BY Description ASC';
		$res=$securitydbADO->Execute($query);
		while($row=$res->FetchRow()){
			$displayedAlertTypes[]=$row['PK_AlertType'];
			$out.='
				<tr>
					<td align="center"><B>'.$row['Description'].'</B></td>
					<td align="center">'.(($row['PK_AlertType']!=5)?'<input type="text" name="delay_'.$row['PK_AlertType'].'" value="'.$row['DelayBeforeAlarm'].'">':'N/A').'</td>
					<td align="center">'.((in_array($row['PK_AlertType'],array(1,4)))?'<input type="text" name="exitDelay_'.$row['PK_AlertType'].'" value="'.$row['ExitDelay'].'">':'N/A').'</td>
					<td align="center">'.(($row['PK_AlertType']!=5)?'<input type="text" name="duration_'.$row['PK_AlertType'].'" value="'.$row['AlarmDuration'].'">':'N/A').'</td>
					<td align="center"><input type="text" name="PoolAlerts_'.$row['PK_AlertType'].'" value="'.$row['PoolAlerts'].'"></td>
				</tr>
			';
		}
		$devicesList=explode(',',getDeviceData($securityPlugIn,$GLOBALS['PK_Device'],$dbADO));
		if(count($devicesList)==0 || (count($devicesList)==1 && $devicesList[0]=='')){
			$deviceNamesList=$TEXT_NO_DEVICE_SELECTED_CONST;
			$devicesList=array(0);
		}
		else{
			$tmpArray=array();
			foreach ($devicesList AS $dev){
				if((int)$dev!=0){
					$tmpArray[]='<tr class="alternate_back"><td><a href="index.php?section=editDeviceParams&deviceID='.$dev.'">'.$deviceNames[$dev].'</a></td><td> <a href="javascript:if(confirm(\''.$TEXT_CONFIRM_DELETE_ANNOUCEMENT_DEVICE_CONST.'\')){document.alertTypes.deviceToRemove.value='.$dev.';document.alertTypes.submit();}">'.$TEXT_DELETE_CONST.'</a></td></tr>';
				}
			}
			$deviceNamesList='<table cellpadding="2" cellspacing="1">'.join('',$tmpArray).'</table>';
		}
		
		$out.='
				<tr>
					<td align="right"><B>'.$TEXT_ANNOUNCEMENT_DEVICES_CONST.': *</B></td>
					<td colspan="4" valign="top">'.$deviceNamesList.devicesTree('device','',$dbADO,' AND PK_Device NOT IN ('.join(',',$devicesList).')').'</td>
				</tr>
				<tr>
					<td align="left" colspan="5">* '.$TEXT_PICK_DEVICE_INFO_CONST.'</td>
				</tr>		
				<tr>
					<td align="center" colspan="5"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
				</tr>		
			</table>
			<input type="hidden" name="oldDevices" value="'.join(',',$devicesList).'">
			<input type="hidden" name="displayedAlertTypes" value="'.join(',',$displayedAlertTypes).'">
		</form>
		';
	} else {
		$displayedAlertTypesArray=explode(',',$_POST['displayedAlertTypes']);
		foreach($displayedAlertTypesArray as $PK_AlertType){
			$delay=(isset($_POST['delay_'.$PK_AlertType]) && $_POST['delay_'.$PK_AlertType]!='')?@$_POST['delay_'.$PK_AlertType]:NULL;
			$exitDelay=(isset($_POST['exitDelay_'.$PK_AlertType]) && $_POST['exitDelay_'.$PK_AlertType]!='')?$_POST['exitDelay_'.$PK_AlertType]:NULL;
			$duration=(isset($_POST['duration_'.$PK_AlertType]) && $_POST['duration_'.$PK_AlertType]!='')?$_POST['duration_'.$PK_AlertType]:NULL;
			$PoolAlerts=(isset($_POST['PoolAlerts_'.$PK_AlertType]) && $_POST['PoolAlerts_'.$PK_AlertType]!='')?$_POST['PoolAlerts_'.$PK_AlertType]:NULL;
			
			$updateAlertType='UPDATE AlertType SET DelayBeforeAlarm=?, ExitDelay=?, AlarmDuration=?,PoolAlerts=? WHERE PK_AlertType=?';
			$securitydbADO->Execute($updateAlertType,array($delay,$exitDelay,$duration,$PoolAlerts,$PK_AlertType));
		}

		if((int)$_POST['device']!=0){
			$oldDD=$_POST['oldDevices'];
			$newDevice=(int)$_POST['device'];
			$newDD=($oldDD!='')?$oldDD.','.$newDevice:$newDevice;
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newDD,$securityPlugIn,$GLOBALS['PK_Device']));
		}
		if((int)$_POST['deviceToRemove']!=0){
			$oldDevicesArray=explode(',',$_POST['oldDevices']);
			$newDD=implode(',',array_diff($oldDevicesArray,array((int)$_POST['deviceToRemove'])));
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newDD,$securityPlugIn,$GLOBALS['PK_Device']));
		}
		
		header("Location: index.php?section=alertTypes&msg=$TEXT_ALERTS_TYPES_UPDATED_CONST");
	}
	
	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_ALERT_TYPES_CONST);
	$output->setNavigationMenu(array($TEXT_ALERT_TYPES_CONST=>'index.php?section=alertTypes'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ALERT_TYPES_CONST);
	$output->output();  		
}
?>

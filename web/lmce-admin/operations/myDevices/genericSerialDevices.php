<?php
function genericSerialDevices($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/genericSerialDevices.lang.php');
		
	global $dbPlutoMainDatabase,$wikiHost;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['GenericSerialDevices'];

	
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'ORDER BY Description ASC');

	if(isset($_REQUEST['lastAdded'])){
		$rs=$dbADO->Execute('SELECT Comments FROM DeviceTemplate WHERE PK_DeviceTemplate=?',(int)$_REQUEST['lastAdded']);
		$row=$rs->FetchRow();
		if($row['Comments']!=''){
			$out.='<script>
				alert(\''.addslashes($row['Comments']).'\')
			</script>';
		}
	}

	
	if ($action == 'form') {
		$queryDevice='
			SELECT Device.* 
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_DeviceCategory=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['CategoryCore'],$installationID));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
		}
		if(isset($coreID)){
			$resDevice_StartupScript=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			$rowShare=$resDevice_StartupScript->FetchRow();
			$sharedWithOthers=($rowShare['Enabled']==1)?1:0;

			$portsArray=array();
			$resPorts=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($coreID,$GLOBALS['Port']));
			if($resPorts->RecordCount()>0){
				$rowPorts=$resPorts->FetchRow();
				$partsArray=explode(';',$rowPorts['IK_DeviceData']);
				foreach($partsArray AS $part){
					$secondParts=explode('|',$part);
					if(count($secondParts)==2)
					$portsArray[$secondParts[0]]=$secondParts[1];
					else
					$portsArray[$part]=$part;
				}
			}
		}
		
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="genericSerialDevices">
	<input type="hidden" name="section" value="genericSerialDevices">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">	
	';
			
		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				
					';
				$displayedGSDDevices=array();
				$displayedGSDDevicesDescription=array();
				$queryDevice='
					SELECT Device.*,ImplementsDCE
					FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE CommandLine=? AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['GenericSerialDeviceCommandLine'],$installationID));
				while($rowD=$resDevice->FetchRow()){
					$displayedGSDDevices[]=$rowD['PK_Device'];
					$displayedGSDDevicesDescription[]=$rowD['Description'];
					
					$qdev=$dbADO->Execute('
						SELECT Device.* 
						FROM DeviceTemplate_DeviceTemplate_ControlledVia 
						INNER JOIN DeviceTemplate ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=PK_DeviceTemplate
						INNER JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate
						WHERE FK_DeviceTemplate_ControlledVia = ? AND ImplementsDCE=1',$rowD['FK_DeviceTemplate']);
					if($qdev->RecordCount()>0){
						while($rowDev=$qdev->FetchRow()){
							$displayedGSDDevices[]=$rowDev['PK_Device'];
							$displayedGSDDevicesDescription[]=$rowDev['Description'];
							$gsdChilds=array();
							$gsdChilds=getChildDevices($gsdChilds,$rowDev['PK_Device'],$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=? AND ImplementsDCE=1');
							foreach ($gsdChilds AS $devID=>$devName){
								$displayedGSDDevices[]=$devID;
								$displayedGSDDevicesDescription[]=$devName;
							}
						}
					}
				}
				$resDevice->Close();

				if(count($displayedGSDDevices)==0)
					$displayedGSDDevices[]=0;
				$displayedDevices=array();
				$GLOBALS['DeviceDataToDisplay']=array();
				$DeviceDataToDisplay=array();
				$DDTypesToDisplay=array();	
				$queryDevice='
					SELECT 
						PK_Device,
						Device.Description,
						IPaddress,
						MACaddress,
						FK_Device_ControlledVia,
						FK_Device_RouteTo,
						FK_Room,
						Device.FK_DeviceTemplate,
						Device.FK_Device_ControlledVia,
						DeviceTemplate.Description AS TemplateName, 
						DeviceCategory.Description AS CategoryName, 
						Manufacturer.Description AS ManufacturerName, 
						IsIPBased, 
						FK_DeviceCategory,
						DeviceData.Description AS dd_Description, 
						Device_DeviceData.FK_DeviceData,
						ParameterType.Description AS typeParam, 
						Device_DeviceData.IK_DeviceData,
						ShowInWizard,ShortDescription,
						AllowedToModify,
						DeviceTemplate_DeviceData.Description AS Tooltip
					FROM Device 
					LEFT JOIN Device_DeviceData ON Device_DeviceData.FK_Device=PK_Device
					LEFT JOIN DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData
					LEFT JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
					LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
					LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 			
					WHERE PK_Device IN ('.join(',',$displayedGSDDevices).')';
				$resDevice=$dbADO->Execute($queryDevice);
				$childOf=array();
				$firstDevice=0;
				$deviceDataArray=array();
	
				while($rowD=$resDevice->FetchRow()){
					if($rowD['FK_Device_ControlledVia']==$rowD['FK_Device_RouteTo'])
						$childOf[$rowD['PK_Device']]=$rowD['FK_Device_ControlledVia'];
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
				$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category
				if(count($joinArray)==0)
					$joinArray[]=0;
			$resDevice->MoveFirst();
			
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
				
					$deviceName=(@$childOf[$rowD['PK_Device']]=='')?'<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'" style="width:200px;">':'<input type="hidden" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"><B>'.$rowD['Description'].'</B>';
					$roomPulldown=pulldownFromArray($roomsArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:200px;"');
										
					$wikiLink=wikiLink($rowD['TemplateName']);
						
					$buttons='	
						<input value="'.$TEXT_HELP_CONST.'" type="button" class="button_fixed" name="help" onClick="window.open(\''.$wikiHost.'index.php/Documentation_by_Device_Templates#'.$wikiLink.'\');"><br>
						<input type="button" class="button_fixed" name="edit_'.$rowD['PK_Device'].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
						<input type="button" class="button_fixed" name="btn" value="'.$TEXT_RUBY_SOURCE_CODE_CONST.'" onClick="windowOpen(\'index.php?section=rubyCodes&from=genericSerialDevices&deviceID='.$rowD['PK_Device'].'&dtID='.$rowD['FK_DeviceTemplate'].'&from=genericSerialDevices&label=ruby\',\'width=1024,height=768,toolbar=1,scrollbars=1,resizable=1\');"><br>';
					$buttons.='
					<input type="button" class="button_fixed" name="resync_'.$rowD['PK_Device'].'" value="'.$TEXT_RESYNC_CONST.'"  onclick="self.location=\'index.php?section=resyncCodes&from=genericSerialDevices&dtID='.$rowD['FK_DeviceTemplate'].'\';"><br>
					<input type="submit" class="button_fixed" name="delete_'.$rowD['PK_Device'].'" value="'.$TEXT_DELETE_CONST.'"  onclick="return confirm(\''.$TEXT_DELETE_GCD_CONFIRMATION_CONST.'\');">';
					if($rowD['FK_DeviceTemplate']==$GLOBALS['CanBus']){
						$buttons.='<br><input type="submit" class="button_fixed" name="reinit_'.$rowD['PK_Device'].'" value="'.$TEXT_REINIT_CANBUS_CONST.'" >';
						$buttons.='<br><input type="submit" class="button_fixed" name="reinit_noemon_'.$rowD['PK_Device'].'" value="'.$TEXT_REINIT_NOEMON_CONST.'" >';
					}
					
					$out.='</td>';
					
					$controlledByPulldown='<select name="controlledBy_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
					if(count(@$GLOBALS['DeviceIDControlledVia'])>0){
						foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
							$controlledByPulldown.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
						}
					}
					$controlledByPulldown.='</select>';
					unset($GLOBALS['DeviceIDControlledVia']);
					unset($GLOBALS['DeviceControlledVia']);

				$controlledViaLink='
				<input type="hidden" name="controlledBy_'.$rowD['PK_Device'].'" value="'.$rowD['FK_Device_ControlledVia'].'">		
				<a href="javascript:windowOpen(\'index.php?section=editDeviceControlledVia&deviceID='.$rowD['PK_Device'].'&from=genericSerialDevices\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');" title="'.$TEXT_CLICK_TO_CHANGE_CONST.'">'.((is_null($rowD['FK_Device_ControlledVia']))?$TEXT_EDIT_CONST:GetDeviceName($dbADO,$rowD['FK_Device_ControlledVia'])).'</a>';
					
				$out.='
				<tr class="tablehead">

					<td align="center"><B>'.$TEXT_DEVICE_INFO_CONST.'</B></td>
					
					
					<td align="center"><B>'.$TEXT_DATA_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>
					<tr>
						<td class="alternate_back" align="left" valign="top" title="'.$TEXT_DEVICE_TEMPLATE_CONST.': '.$rowD['TemplateName'].', '.$TEXT_DEVICE_CATEGORY_CONST.': '.$rowD['CategoryName'].', '.strtolower($TEXT_MANUFACTURER_CONST).': '.$rowD['ManufacturerName'].'"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>
						'.$TEXT_DEVICE_CONST.'#: '.$rowD['PK_Device'].'<br>
						'.$TEXT_DEVICE_TEMPLATE_CONST.'#: '.$rowD['FK_DeviceTemplate'].'<br>
						'.$TEXT_CONTROLLED_BY_CONST.': '.$controlledViaLink.'<br><br>
						'.$deviceName.'<br><br>
						'.$roomPulldown.'<br>
						</td>
						<td valign="top" align="right">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased']).'</td>
						<td class="alternate_back" align="center" valign="top">'.$buttons.'</td>
					</tr>
					<tr>
						<td colspan="3" height="3" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
					</tr>';					
				}
			}
			
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',cleanArray($GLOBALS['DeviceDataToDisplay'])).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',array_values($displayedDevices)).'">';

			if($resDevice->RecordCount()!=0){
				$updateBtns='<input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">';
			}
			$out.='
				<tr>
					<td colspan="8">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="8" align="center">'.@$updateBtns.' <input type="button" class="button" name="button" value="'.$TEXT_ADD_GSD_DEVICE_CONST.'" onClick="document.genericSerialDevices.action.value=\'externalSubmit\';document.genericSerialDevices.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=genericSerialDevices\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("genericSerialDevices");
 //			frmvalidator.addValidation("Description","req","'.$TEXT_GSD_DEVICE_DESCRIPTION_REQUIRED_CONST.'");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","'.$TEXT_GSD_DEVICE_TEMPLATE_REQUIRED_CONST.'");			
		</script>
	
	</form>
	';
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=genericSerialDevices&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		
		$oldShareIRCodes=@(int)$_POST['oldShareIRCodes'];
		$coreID=@(int)$_POST['coreID'];
		$resShare=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
		if(isset($_POST['shareIRCodes'])){
			if($oldShareIRCodes==0 && $coreID!=0){
				if($resShare->RecordCount()==0)
					$dbADO->Execute('INSERT INTO Device_StartupScript (FK_Device, FK_StartupScript, Enabled) VALUES (?,?,1)',array($coreID,$GLOBALS['ShareIRCodes']));
				else
					$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=1 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}else{
			if($oldShareIRCodes!=0){
				$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=0 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}

		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
			}
			
			if(isset($_POST['reinit_'.$value])){
				ReinitCanBus($value,"CANBUS");
			}
			
			if(isset($_POST['reinit_noemon_'.$value])){
				ReinitCanBus($value, "NOEMON");
			}
		}
		
		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
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
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;

					if(isset($_POST['controlledBy_'.$value])){
						$updateDevice='UPDATE Device SET Description=?, FK_Room=?, FK_Device_ControlledVia=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));
					}
					foreach($DeviceDataToDisplayArray as $ddValue){
						$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:'';
						$oldDeviceData=@$_POST['oldDeviceData_'.$value.'_'.$ddValue];

						if($oldDeviceData!=$deviceData){
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
			
			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
		}
				
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);	
				$insertID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);				
				
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);
			}
			header("Location: index.php?section=genericSerialDevices&lastAdded=$deviceTemplate#deviceLink_".$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=genericSerialDevices&msg=$TEXT_GSD_DEVICES_UPDATED_CONST".@$anchor);		
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_GENERIC_SERIAL_DEVICES_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_GENERIC_SERIAL_DEVICES_CONST=>'index.php?section=genericSerialDevices'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_GENERIC_SERIAL_DEVICES_CONST);
	$output->output();
}

function getChildDevices($childs,$deviceID,$dbADO,$filter){
	if($deviceID==''){
		return $childs;
	}
	$res=$dbADO->Execute('
		SELECT * 
		FROM Device '.$filter,$deviceID);
	while($row=$res->FetchRow()){
		$childs[$row['PK_Device']]=$row['Description'];
		$childs=getChildDevices($childs,$row['PK_Device'],$dbADO,$filter);
	}
	
	return $childs;
}

function ReinitCanBus($myObj, $myParam='CANBUS')
{
	exec("/usr/pluto/bin/MessageSend localhost 0 $myObj 1 776 51 $myParam");
}
function GetDeviceName($dbADO,$PK_Device) {
	$query='SELECT Description FROM Device WHERE PK_Device="'.$PK_Device.'"';
	$res=$dbADO->Execute($query);
	$row=$res->FetchRow();
	return $row['Description'];
}
?>

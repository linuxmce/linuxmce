<?php
function editDeviceParams($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('editDeviceParams.lang.php');
	
	global $wikiHost;
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

	$out='';
	$deviceID = (int)$_REQUEST['deviceID'];
	$installationID = (int)@$_SESSION['installationID'];
	$registeredArray=array(
		1=>translate('TEXT_YES_CONST'),
		0=>translate('TEXT_NO_CONST'),
		-1=>translate('TEXT_REGISTERED_RELOAD_NEEDED_CONST')
	);

	$manualyConfigureArray=array(
		0=>translate('TEXT_AUTO_CONFIGURE_CONST'),
		1=>translate('TEXT_MANUAL_CONFIGURE_CONST'),
		-1=>translate('TEXT_FORCE_INTO_EVERY_ROOM_CONST')
	);

	if (!isset($_SESSION['userLoggedIn']) || $_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login&last=editDeviceParams&deviceID=$deviceID");
		exit();
	}
	$query = "
		SELECT 
			Device.FK_DeviceTemplate,
			Device.FK_Device_ControlledVia,
			Device.Description,
			Device.IPaddress,
			Device.MACaddress,
			Device.IgnoreOnOff,
			Device.NeedConfigure,
			DeviceTemplate.Description as MDL_description,
			Device.FK_Room, 
			DeviceTemplate.Comments,
			DeviceTemplate.ManufacturerURL,
			DeviceTemplate.InternalURLSuffix,
			DeviceTemplate.FK_DeviceCategory,
			DeviceTemplate.IsIPBased,
			DeviceTemplate.FK_DeviceCategory,
			Device.PingTest,
			Device.State,
			Device.Status,
			DeviceTemplate.ImplementsDCE,
			Parent.FK_DeviceTemplate AS ParentDT,
			Device.Disabled,
			Device.ManuallyConfigureEA,
			Device.Registered
		FROM Device
		LEFT JOIN Device Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device
		INNER JOIN DeviceTemplate on Device.FK_DeviceTemplate = PK_DeviceTemplate
		WHERE Device.PK_Device = ?";
	$res = $dbADO->Execute($query,array($deviceID));
	
	
	if ($res) {
		$row = $res->FetchRow();
		$DeviceTemplate = $row['FK_DeviceTemplate'];	
		$controlledVia = $row['FK_Device_ControlledVia'];
		$description = stripslashes($row['Description']);
		$mdlDescription = stripslashes($row['MDL_description']);
		$ipAddress = stripslashes($row['IPaddress']);
		$macAddress = stripslashes($row['MACaddress']);
		$ignoreOnOff= $row['IgnoreOnOff'];
		$deviceNeedConfigure= $row['NeedConfigure'];
		$deviceRoom=$row['FK_Room'];
		$dtComments=$row['Comments'];
		$ManufacturerURL=$row['ManufacturerURL'];
		$InternalURLSuffix=$row['InternalURLSuffix'];
		$DeviceCategory= $row['FK_DeviceCategory'];	
		$PingTest=$row['PingTest'];
		$State=$row['State'];
		$Status=$row['Status'];
		$ImplementsDCE=$row['ImplementsDCE'];
		$deviceDisabled=$row['Disabled'];
		$IsIPBased=$row['IsIPBased'];
		$dcID=$row['FK_DeviceCategory'];
		$ManuallyConfigureEA=$row['ManuallyConfigureEA'];
		$registered=$row['Registered'];

		$coreSystemLog=($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'])?'&nbsp;&nbsp;&nbsp;<a href="javascript:windowOpen(\'index.php?section=followLog&deviceID='.$deviceID.'&system_log=1\',\'width=1024,height=768,scrollbars=1,resizable=1,fullscreen=1\');">System log</a>':'';

		// if the device is child of DCE router, append DCE router device ID to logs url to be able to use DCE router log instead of plugin log
		$dceRouterSuffix=($row['ParentDT']==$GLOBALS['rootDCERouter'])?'&parentID='.$row['FK_Device_ControlledVia']:'';
	}
	
	if ($DeviceTemplate==0) {
		$_SESSION['error_message']='Invalid device ID';
		$_SESSION['retry_url']='index.php?section=editDeviceParams&deviceID='.$deviceID;
		header("Location: index.php?section=error_message");
		exit();
	}
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if ($action == 'form') {
	$deviceDataFromMasterDevice = "
		SELECT 						
			DeviceTemplate_DeviceData.FK_DeviceData
		 FROM 
			DeviceTemplate_DeviceData
		WHERE (FK_DeviceTemplate='$DeviceTemplate')
	";	
	
	$resDatafromMasterDevice = $dbADO->Execute($deviceDataFromMasterDevice);
	$deviceDataToShow=array();
	$deviceDataToShow[]=0;
	
	if ($resDatafromMasterDevice) {
		while($row=$resDatafromMasterDevice->FetchRow())
			$deviceDataToShow[]=$row['FK_DeviceData'];
	}

	$childsNo = getChildsNo($deviceID,$dbADO);
	
	if(isCritical($DeviceTemplate)){
		$deleteLink='<a href="javascript: if (confirm(\''.translate('TEXT_DELETE_CRITICAL_DEVICE_CONFIRMATION_CONST').'\')) {document.location.href=\'index.php?section=deleteCriticalDevice&deviceID='.$deviceID.'&from=editDeviceParams\';}">'.translate('TEXT_DELETE_DEVICE_CONST').'</a> '.translate('TEXT_NOT_RECOMENDED_CONST');
	}else{
		$deleteLink='<a href="javascript: if (confirm(\''.translate('TEXT_DELETE_DEVICE_CONFIRMATION_CONST').'\')) {document.location.href=\'index.php?section=deleteMyDevice&deviceID='.$deviceID.'&from=editDeviceParams\';}">'.translate('TEXT_DELETE_THIS_DEVICE_CONST').'</a>';
	}
	
	$manufHomeLink=($ManufacturerURL!='')?'<a href="'.$ManufacturerURL.'" target="_blank" title="'.translate('TEXT_MANUFACTURER_URL_CONST').'"><img src="include/images/home.png" border="0" align="middle"></a>':'';
	$internalLink=($ManufacturerURL!='' && $InternalURLSuffix!='')?'<a href="'.$ManufacturerURL.$InternalURLSuffix.'" title="'.translate('TEXT_DEVICE_URL_MANUFACTURER_SITE_CONST').'" target="_blank"><img src="include/images/file.png" border="0" align="middle"></a>':'';
	$resetDeviceLink=($ImplementsDCE==1)?'<a href="index.php?section=editDeviceParams&deviceID='.$deviceID.'&action=reset">'.translate('TEXT_RESET_DEVICE_CONST').'</a>':'';
	
	if(isset($_REQUEST['showNote'])){
		$Alert='alert("'.translate('TEXT_ADVANCED_PREPARATION_NOTE_CONST').'");
		';
	}

	$wikiLink=wikiLink($mdlDescription);
	$validOrbiters=getValidOrbitersArray($installationID,$dbADO);
	if(in_array($deviceID,$validOrbiters)){
		$orbiterSuffix='&orbiter=1';
	}

	$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		'.@$Alert.'
	</script>
	
	
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div class="confirm" align="center"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	
	<table width="100%">
		<tr>
			<td><a href="index.php?section=addMyDevice&parentID='.$deviceID.'">'.($deviceID==0?translate('TEXT_CREATE_TOP_LEVEL_DEVICE_CONST'):translate('TEXT_CREATE_CHILD_DEVICE_CONST')).'</a> &nbsp; &nbsp; &nbsp;
			'.$deleteLink.' &nbsp;&nbsp; 
			<a href="javascript:windowOpen(\'index.php?section=sendCommand&deviceID='.$deviceID.'\',\'width=800,height=600,scrollbars=1,resizable=1\');">'.translate('TEXT_SEND_COMMAND_TO_DEVICE_CONST').'</a> &nbsp; &nbsp; &nbsp; '.$resetDeviceLink.'
			&nbsp;&nbsp;
			<a href="backupChildDevices.php?deviceID='.$deviceID.'" target="_blank">'.translate('TEXT_BACKUP_CHILD_DEVICES_CONST').'</a>
			&nbsp;&nbsp;
			<a href="index.php?section=restoreChildDevices&deviceID='.$deviceID.'">'.translate('TEXT_RESTORE_CHILD_DEVICES_CONST').'</a>
	
			</td>
			<td align="right"><a href="javascript:windowOpen(\'index.php?section=errorLog&deviceID='.$deviceID.$dceRouterSuffix.@$orbiterSuffix.'\',\'width=1024,height=768,scrollbars=1,resizable=1,fullscreen=1\');">'.translate('TEXT_VIEW_ERRORS_IN_LOG_CONST').'</a>&nbsp;&nbsp;&nbsp;
				<a href="javascript:windowOpen(\'index.php?section=fullLog&deviceID='.$deviceID.$dceRouterSuffix.@$orbiterSuffix.'\',\'width=1024,height=768,scrollbars=1,resizable=1,fullscreen=1\');">'.translate('TEXT_VIEW_WHOLE_LOG_CONST').'</a>&nbsp;&nbsp;&nbsp;
				<a href="javascript:windowOpen(\'index.php?section=followLog&deviceID='.$deviceID.$dceRouterSuffix.@$orbiterSuffix.'\',\'width=1024,height=768,scrollbars=1,resizable=1,fullscreen=1\');">'.translate('TEXT_FOLLOW_LOG_CONST').'</a>
				'.$coreSystemLog.'
			</td>
		</tr>
	</table>
	<form method="post" action="index.php" name="editDeviceParams">
	<script>
	 	var frmvalidator = new formValidator("editDeviceParams");
	</script>
		
	<fieldset>
	<legend>'.translate('TEXT_DEVICE_INFO_CONST').' #'.$deviceID.'</legend>
	<table border="0">
		<tr>
			<td>'.translate('TEXT_DESCRIPTION_CONST').' *</td>
			<td colspan="2"><input type="text" name="DeviceDescription" value="'.$description.'" class="input_big"></td>
		</tr>
	<tr>
		<td>'.translate('TEXT_DEVICE_TEMPLATE_CONST').'</td>
		<td width="230" bgcolor="#B9B9B9"><B>'.$mdlDescription.' #'.$DeviceTemplate.'</B></td>
		<td><input value="'.translate('TEXT_VIEW_CONST').'" type="button" class="button" name="controlGoToMDL" onClick="windowOpen(\'index.php?section=editMasterDevice&model='.$DeviceTemplate.'&from=editDeviceParams&fromDeviceID='.$deviceID.'\',\'width=1024,height=768,toolbar=1,scrollbars=1,resizable=1\');"> <input value="'.translate('TEXT_HELP_CONST').'" type="button" class="button" name="help" onClick="window.open(\''.$wikiHost.'/index.php/'.$wikiLink.'\');">'.$manufHomeLink.' '.$internalLink.'</td>
	</tr>
	<tr>
		<td>'.translate('TEXT_DEVICE_TEMPLATE_COMMENTS_CONST').'</td>
		<td colspan="2">'.$dtComments.'</td>
	</tr>
	<tr>
		<td valign="top">'.translate('TEXT_CONTROLLED_BY_CONST').'</td>
		<td colspan="2">
		';
	
		$GLOBALS['DT_&_Room']=1;
		$out.=controlledViaPullDown('controlledVia',$deviceID,$DeviceTemplate,$DeviceCategory,$controlledVia,$dbADO,'0,- '.translate('TEXT_PLEASE_SELECT_CONST').' -','class="input_big" onchange="document.editDeviceParams.submit();"');
					
		$out.='		
		</td>
	</tr>
	<tr>
		<td>'.translate('TEXT_ROOM_CONST').'</td>
		<td colspan="2"><select name="Room" class="input_big">
			<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>';
					if(isMediaDirector($deviceID,$dbADO)){
						$mdArray=getDeviceTemplatesFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
					
						$selectRooms='
							SELECT Room.Description, PK_Room,Device.Description AS md,PK_Device 
							FROM Room 
							LEFT JOIN Device ON FK_Room=PK_Room AND FK_DeviceTemplate IN ('.join(',',$mdArray).')
							WHERE Room.FK_Installation=? 
							ORDER BY Description ASC';
					}else{
						$selectRooms='
							SELECT Room.Description, PK_Room
							FROM Room 
							WHERE Room.FK_Installation=? 
							ORDER BY Description ASC';
					}
					$resRooms=$dbADO->Execute($selectRooms,$installationID);
					while($rowRooms=$resRooms->FetchRow()){
						$out.='<option value="'.$rowRooms['PK_Room'].'" '.(($rowRooms['PK_Room']==$deviceRoom)?'selected':'').'>'.$rowRooms['Description'].((!is_null(@$rowRooms['PK_Device']))?' [ '.@$rowRooms['md'].' ]':'').'</option>';
					}
					$out.='
				</select></td>
		</tr>
		<tr>
			<td>'.translate('TEXT_ENTERTAIN_AREAS_CONST').'</td>
			<td bgcolor="#B9B9B9">';
					$selectCheckedEA='SELECT * FROM Device_EntertainArea WHERE FK_Device=?';
					$resCheckedEA=$dbADO->Execute($selectCheckedEA,$deviceID);
					$checkedArray=array();
					while($rowCheckedEA=$resCheckedEA->FetchRow()){
						$checkedArray[]=$rowCheckedEA['FK_EntertainArea'];
					}

					$selectEntertainArea='
						SELECT 
							EntertainArea.*
						FROM EntertainArea
							INNER JOIN Room ON FK_Room=PK_Room
						WHERE FK_installation=?';
					$displayedEntAreas=array();
					$oldEntAreas=array();
					$resEntertainArea=$dbADO->Execute($selectEntertainArea,$installationID);
					while($rowEntertainArea=$resEntertainArea->FetchRow()){
						$oldEntAreas[]=(in_array($rowEntertainArea['PK_EntertainArea'],$checkedArray))?1:0;
						$displayedEntAreas[]=$rowEntertainArea['PK_EntertainArea'];
						$out.='<input type="checkbox" value="1" name="entArea_'.$rowEntertainArea['PK_EntertainArea'].'" '.((in_array($rowEntertainArea['PK_EntertainArea'],$checkedArray))?'checked':'').'> '.$rowEntertainArea['Description'].'<br>';
					}
					$out.='
					<input type="hidden" name="displayedEntAreas" value="'.join(',',$displayedEntAreas).'">
					<input type="hidden" name="oldEntAreas" value="'.join(',',$oldEntAreas).'">
				</td>
			</tr>					
			<tr>
				<td>'.translate('TEXT_MANUALLY_CONFIGURE_EA_CONST').'</td>
				<td>'.pulldownFromArray($manualyConfigureArray,'ManuallyConfigureEA',$ManuallyConfigureEA,'','key','- '.translate('TEXT_PLEASE_SELECT_CONST').' -',-100).'</td>
			</tr>			
			<tr>
				<td>'.translate('TEXT_IP_ADDRESS_CONST').'</td>
				<td><input name="ipAddress" value="'.$ipAddress.'" class="input_big"></td>
			</tr>
			<tr>
				<td>'.translate('TEXT_MAC_ADDRESS_CONST').'</td>
				<td><input name="macAddress" value="'.$macAddress.'" class="input_big"></td>
			</tr>
			<tr>
				<td>'.translate('TEXT_IGNORE_ON_OFF_CONST').'</td>
				<td>'.translate('TEXT_ENABLED_CONST').'<input type="radio" value="1" name="IgnoreOnOff" '.($ignoreOnOff==1?' checked="checked" ':'').'> &nbsp; 
					'.translate('TEXT_DISABLED_CONST').':<input type="radio" value="0" name="IgnoreOnOff" '.($ignoreOnOff==0?' checked="checked" ':'').'></td>
			</tr>
			<tr>
				<td colspan="3"><input type="checkbox" name="needConfigure" value="1" '.(($deviceNeedConfigure==1)?'checked':'').' onClick="javascript:document.editDeviceParams.submit();"> '.translate('TEXT_RECONFIGURE_DEVICE_CONST').'</td>
			</tr>
			<tr>
				<td colspan="3"><input type="checkbox" name="PingTest" value="1" '.(($PingTest==1)?'checked':'').' onClick="javascript:document.editDeviceParams.submit();"> '.translate('TEXT_CONNECTION_KEEP_ALIVE_CONST').'</td>
			</tr>
			<tr>
				<td>'.translate('TEXT_DEVICE_STATE_CONST').'</td>
				<td>'.getStateFormElement($deviceID,'State',$State,$dbADO).'</td>
			</tr>
			<tr>
				<td>'.translate('TEXT_STATUS_CONST').'</td>
				<td><input type="text" name="Status" value="'.$Status.'" class="input_big"></td>
			</tr>
			<tr>
				<td colspan="3"><input type="checkbox" name="deviceDisabled" value="1" '.(($deviceDisabled==1)?'checked':'').' onClick="javascript:document.editDeviceParams.submit();"> '.translate('TEXT_DISABLED_CONST').'</td>
			</tr>					
			<tr>
				<td colspan="3">'.translate('TEXT_REGISTERED_CONST').': <B>'.$registeredArray[$registered].'</B></td>
			</tr>					
			<tr>
				<td colspan="3" align="left"><input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"> <input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"></td>
			</tr>
	</table>
	</fieldset>
	<br />
	<script>
		frmvalidator.addValidation("DeviceDescription","req","Please enter a description");
		frmvalidator.addValidation("macAddress","mac","'.translate('TEXT_INVALID_MAC_ADDRESS_CONST').'");
		frmvalidator.addValidation("ipAddress","ip","'.translate('TEXT_INVALID_IP_ADDRESS_CONST').'");
	</script>
	<table width="100%">
		<tr>
			<td valign="top" width="50%">
	<fieldset style="height:80%">				
	<legend>'.translate('TEXT_DEVICE_PIPES_USED_CONST').'</legend>
	<table>
		<tr>
			<td>
				<table>
					';
	
	$selectPipes = 'SELECT * FROM Pipe Order By Description ASC';
	$resSelectPipes = $dbADO->Execute($selectPipes);
	
	$selectPipesUsed = 'SELECT Device_Device_Pipe.*,D1.Description as Desc_From,D2.Description as Desc_To,D2.FK_DeviceTemplate AS DT_To
							 FROM Device_Device_Pipe 
								INNER JOIN Device D1 ON D1.PK_Device = FK_Device_From
								INNER JOIN Device D2 ON D2.PK_Device = FK_Device_To
						WHERE FK_Device_From = ?';
	$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($deviceID));
	
	if ($resSelectPipesUsed) {
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {
			$selectInputs = '
				SELECT * 
				FROM Command 
				INNER JOIN DeviceTemplate_Input ON FK_Command=PK_Command
				WHERE FK_CommandCategory=22 AND FK_DeviceTemplate=?
				ORDER BY Description ASC';
			$resSelectInputs = $dbADO->Execute($selectInputs,$rowSelectedPipesUsed['DT_To']);
			
			$selectOutputs = '
				SELECT * FROM Command 
				INNER JOIN DeviceTemplate_Output ON FK_Command=PK_Command
				WHERE FK_CommandCategory=27 AND FK_DeviceTemplate=?
				Order By Description ASC';
			$resSelectOutputs = $dbADO->Execute($selectOutputs,$rowSelectedPipesUsed['DT_To']);
			
			$out.='<tr><td>'.$rowSelectedPipesUsed['Desc_To']." <input type='hidden' name='deviceTo_{$rowSelectedPipesUsed['FK_Device_To']}'> &nbsp;&nbsp;&nbsp;&nbsp;</td>";
			
			$selectInputsTxt='';
			while ($rowSelInputs = $resSelectInputs->FetchRow()) {
				$selectInputsTxt.= '<option '.($rowSelInputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Input']?" selected='selected' ":"").' value="'.$rowSelInputs['PK_Command'].'">'.$rowSelInputs['Description'].'</option>';
			}
			
			$out.='<td> Input on '.$rowSelectedPipesUsed['Desc_To'].' <select name="input_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>'.$selectInputsTxt.'</select></td>';
			
			$selectOutputsTxt='';
			while ($rowSelOutputs = $resSelectOutputs->FetchRow()) {
				$selectOutputsTxt.= '<option '.($rowSelOutputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Output']?" selected='selected' ":"").' value="'.$rowSelOutputs['PK_Command'].'">'.$rowSelOutputs['Description'].'</option>';
			}
			
			$out.='<td> Output on '.$rowSelectedPipesUsed['Desc_From'].' <select name="output_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>'.$selectOutputsTxt.'</select></td>';
			

			$resSelectPipes->MoveFirst();			
			$selectPipesTxt='';
			while ($rowSelPipes = $resSelectPipes->FetchRow()) {
				$selectPipesTxt.= ($rowSelPipes['PK_Pipe']==$rowSelectedPipesUsed['FK_Pipe'])?$rowSelPipes['Description']:"";
			}
			
			$out.='<td> Pipe '.$selectPipesTxt.'</td>';
			$out.='<td><input value="'.translate('TEXT_DELETE_CONST').'" type="button" class="button" onClick="if (confirm(\''.translate('TEXT_CONFIRM_DELETE_PIPE_CONST').'\')) {windowOpen(\'index.php?section=deleteDevicePipeFromDevice&deviceFromID='.$rowSelectedPipesUsed['FK_Device_From'].'&deviceToID='.$rowSelectedPipesUsed['FK_Device_To'].'&pipe='.$rowSelectedPipesUsed['FK_Pipe'].'&from=editDeviceParams\',\'width=100,height=100,toolbar=1,scrollbars=1,resizable=1\');}"></td>';
			
			$out.='</tr>';
		}
	}
	
	$out.='
				</table>
			</td>
		</tr>
		<tr>
			<td>
				<select name="addDeviceForPiping">
					<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>
					';
			
				$querySelectDevice = "SELECT Description,PK_Device FROM Device Where FK_Installation = ? Order By Description ASC";
				$resSelectDevice = $dbADO->Execute($querySelectDevice,array($installationID));
				 if ($resSelectDevice) {
				 	while ($rowSelectDevice = $resSelectDevice->FetchRow()) {
				 		$out.='<option value="'.$rowSelectDevice['PK_Device'].'">'.$rowSelectDevice['Description'].'</option>'."\n";
				 	}
				 }
			$resSelectPipes->MoveFirst();			
			$selectPipesTxt='';
			while ($rowSelPipes = $resSelectPipes->FetchRow()) {
				$selectPipesTxt.= '<option value="'.$rowSelPipes['PK_Pipe'].'">'.$rowSelPipes['Description'].'</option>';
			}
				 
			$out.='
				</select>
				<select name="addPipe">
					<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>'.$selectPipesTxt.'</select>
			<input type="submit" class="button" name="submitX" value="'.translate('TEXT_ADD_CONST').'"  >
			</td>
		</tr>
	    <tr><td><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  ></td></tr>
	</table>
	</fieldset>
		</td>
		<td>
	<fieldset style="height:80%">				
	<legend>'.translate('TEXT_MEMBER_OF_GROUP_CONST').'</legend>
	
	<table>				';

	$selectDeviceGroups = 'SELECT DeviceGroup.Description,Device.PK_Device,DeviceGroup.PK_DeviceGroup FROM 
		Device_DeviceGroup			
			INNER JOIN Device ON FK_Device = PK_Device
			INNER JOIN DeviceGroup ON FK_DeviceGroup = PK_DeviceGroup
		WHERE Device_DeviceGroup.FK_Device = ?
	';
	$resDevicesGroups = $dbADO->Execute($selectDeviceGroups,array($deviceID));
	
	$deviceGroups = array();
	
	$deviceGroups[]=0;
	if ($resDevicesGroups) {	
		if ($resDevicesGroups->RecordCount()==0){
			$out.='<tr><td colspan="2">'.translate('TEXT_NO_GROUPS_FOR_DEVICE_CONST').'</a></td></tr>';
		} 
			
		while ($rowDevicesGroup = $resDevicesGroups->FetchRow()) {
			$out.="<tr>
					<td>".$rowDevicesGroup['Description']."</td>					
					<td>
						<input value='Delete' type='button' onClick=\"if (confirm('".translate('TEXT_CONFIRM_DELETE_DEVICE_FROM_GROUP_CONST')."')) {windowOpen('index.php?section=deleteDeviceFromDeviceGroup&deviceID=$deviceID&deviceGroupID=".$rowDevicesGroup['PK_DeviceGroup']."&from=editDeviceParams','width=200,height=200,toolbar=1,scrollbars=1,resizable=1');}\">
					</td>
				  </tr>";
			$deviceGroups[]=$rowDevicesGroup['PK_DeviceGroup'];
		}
	}
					
	$selectRemainingGroups = "SELECT Description,PK_DeviceGroup from DeviceGroup where FK_Installation = ? and PK_DeviceGroup not in (".join(",",$deviceGroups).") order by Description ASC";
	$resRemainingGroups = $dbADO->Execute($selectRemainingGroups,array($installationID));
	
	$remainingGroups = '<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>';
	if ($resRemainingGroups) {
		while ($rowRemainingGroups = $resRemainingGroups->FetchRow()) {
			$remainingGroups.='<option value="'.$rowRemainingGroups['PK_DeviceGroup'].'">'.$rowRemainingGroups['Description'].'</option>';
		}
	}
	$out.='
	
	<tr><td><select name="addNewGroup">'.$remainingGroups.'</select></td><td><input type="submit" class="button"   name="submitX" value="'.translate('TEXT_ADD_CONST').'"></td></tr>		
	<tr><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createDeviceGroup&deviceID='.$deviceID.'&from=editDeviceParams\',\'width=400,height=400,toolbar=1,scrollbars=1,resizable=1\');">'.translate('TEXT_CREATE_NEW_DEVICE_GROUP_CONST').'</a></td></tr>
	
		<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  ></td></tr>
	</table>
	</fieldset>
			</td>
		</tr>
	</table>
	
	<fieldset>
	<legend>'.translate('TEXT_RELATED_DEVICES_CONST').'</legend>
					
	<table>
	';
	$selectRelatedDevices = '
		SELECT Device_Device_Related.Value,FK_Device_Related,Device.Description,DeviceTemplate.Description as  DT_Desc 
		FROM Device_Device_Related 			
		INNER JOIN Device ON FK_Device_Related = PK_Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE FK_Device = ?
	';
	$resRelatedDevices = $dbADO->Execute($selectRelatedDevices,array($deviceID));
	
	$deviceRelatedData = array();
	$deviceRelatedData[]=0;
	if ($resRelatedDevices) {	
		$relatedDevicesCount=$resRelatedDevices->RecordCount();
		if ($relatedDevicesCount==0){
			$out.='<tr><td colspan="2">'.translate('TEXT_NO_RELATED_DEVICES_CONST').'</a></td></tr>';
		} 
			
		while ($rowRelatedDevice = $resRelatedDevices->FetchRow()) {
			$out.="<tr>
					<td>".$rowRelatedDevice['Description']." ({$rowRelatedDevice['DT_Desc']}) #".$rowRelatedDevice['FK_Device_Related']."</td><td>
						<input type=\"text\" name=\"relatedDeviceValue_".$rowRelatedDevice['FK_Device_Related']."\" value=\"".stripslashes($rowRelatedDevice['Value'])."\">
					</td>
					<td>
						<input value='Delete' class='button' type='button' onClick=\"if (confirm('".translate('TEXT_CONFIRM_DELETE_DEVICE_CONST')."')) {windowOpen('index.php?section=deleteDeviceRelatedFromDeviceParams&deviceID=$deviceID&relatedID=".$rowRelatedDevice['FK_Device_Related']."&from=editDeviceParams','width=200,height=200,toolbar=1,scrollbars=1,resizable=1');}\">
					</td>
				  </tr>";
			$deviceRelatedData[]=$rowRelatedDevice['FK_Device_Related'];
		}
	}
	
	$selectAddNewRelatedDevices = "
		SELECT Device.Description,PK_Device,DeviceTemplate.Description as DT_Desc 
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE FK_Installation = ? and PK_Device != ? AND PK_Device NOT IN (".join(",",$deviceRelatedData).") AND FK_DeviceTemplate=?
		ORDER BY Description ASC";
	
	$resAddNewRelatedDevices = $dbADO->Execute($selectAddNewRelatedDevices,array($installationID,$deviceID,$DeviceTemplate));
	$newDeviceRelated='<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>';
	if ($resAddNewRelatedDevices) {
		while ($rowAddNewRelatedDevices = $resAddNewRelatedDevices->FetchRow()) {
			$newDeviceRelated.='<option value="'.$rowAddNewRelatedDevices['PK_Device'].'">'.$rowAddNewRelatedDevices['Description'].'('.$rowAddNewRelatedDevices['DT_Desc'].')</option>';
		}
	}
	$out.='
		
		<tr>
			<td>'.(($relatedDevicesCount!=0)?'&nbsp;':'<select name="addNewDeviceRelated">'.$newDeviceRelated.'</select>').' <input type="submit" class="button" name="submitX" value="'.translate('TEXT_ADD_CONST').'"  ></td>
			<td>&nbsp;</td>
		</tr>
	
		<input type="hidden" name="selectedRelatedDevice" value="'.(join(",",$deviceRelatedData)).'">
		<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  ></td></tr>
	</table>	
	</fieldset>
	
	<br />
	<fieldset>
		<legend>'.translate('TEXT_DEVICE_DATA_CONST').'</legend>
	
		
			<input type="hidden" value="editDeviceParams" name="section">
			<input type="hidden" value="add" name="action">
			<input type="hidden" value="'.$deviceID.'" name="deviceID">			

	';
	$GLOBALS['DeviceDataToDisplay']=array();
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
		FROM DeviceData 
		INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
		INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
		INNER JOIN Device ON FK_Device=PK_Device
		LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
		INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 			
		WHERE PK_Device=?';

	$resDevice=$dbADO->Execute($queryDevice,$deviceID);
	$firstDevice=0;
	$deviceDataArray=array();
	while($rowD=$resDevice->FetchRow()){
		$displayedDevices[]=$rowD['PK_Device'];
					
		// fill in the device data array
		if($rowD['PK_Device']!=$firstDevice){
			$firstDevice=$rowD['PK_Device'];
			$deviceDataArray[$firstDevice]=array();
			$deviceDataArray[$firstDevice][]=$rowD;
		}else{
			$deviceDataArray[$firstDevice][]=$rowD;
		}					
	}

/*	
	$deviceData = array();
	$deviceData[]=0;
	if ($resDeviceData) {	
		if ($resDeviceData->RecordCount()==0){
			$out.='<tr><td colspan="2">No device data</a></td></tr>';
		} 
			
		while ($rowDevicedata = $resDeviceData->FetchRow()) {
			//print_array($rowDevicedata);
			if($rowDevicedata['PK_DD']==$GLOBALS['Port']){
				$formElement=serialPortsPulldown('deviceData_'.$rowDevicedata['PK_DD'],stripslashes($rowDevicedata['IK_DeviceData']),$rowDevicedata['AllowedToModify'],getTopLevelParent($deviceID,$dbADO),$dbADO,$deviceID).' '.$rowDevicedata['ExtraInfo'];
			}else{
				$formElement="<textarea name=\"deviceData_".$rowDevicedata['PK_DD']."\" ".(($rowDevicedata['AllowedToModify']==1 || $rowDevicedata['DT_DD_Exists']=='')?'':'disabled')." rows=\"1\">".stripslashes($rowDevicedata['IK_DeviceData'])."</textarea> ".$rowDevicedata['ExtraInfo'];
			}
			if($rowDevicedata['PK_DD']==$GLOBALS['PortChannel']){
				$choicesArray=parentHasChoices($deviceID,$dbADO);
				if(count($choicesArray)>0){
					$formElement=pulldownFromArray($choicesArray,'deviceData_'.$rowDevicedata['PK_DD'],$rowDevicedata['IK_DeviceData']);
				}
			}
			
				
			$out.="
				<tr>
					<td>#{$rowDevicedata['PK_DD']} {$rowDevicedata['DD_desc']}({$rowDevicedata['PT_Desc']})</td>
					<td>".$formElement."</td>
				</tr>";
			$deviceData[]=$rowDevicedata['PK_DD'];
		}
	}
	*/

	$out.=((count($deviceDataArray)>0)?formatDeviceData($deviceID,$deviceDataArray[$deviceID],$dbADO,$IsIPBased,getSpecificFloorplanType($dcID,$dbADO),1,'textarea','input_big').'
		<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
		<input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  >
	':'').'	
	</fieldset>
	</form>
	<em>* '.translate('TEXT_REQUIRED_FIELDS_CONST').'</em>

	
	<br />';
	
	
	} else {
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		if($action=='reset'){
			$command="/usr/pluto/bin/MessageSend localhost 0 $deviceID 7 1";
			exec($command);
			Header('Location: index.php?section=editDeviceParams&deviceID='.$deviceID.'&msg='.translate('TEXT_DEVICE_RESETED_CONST'));
			exit();			
		}
		$oldIpAddress = $ipAddress;
		$oldMacAddress = $macAddress;

		$description = cleanString($_POST['DeviceDescription']);
		$ipAddress = cleanString($_POST['ipAddress']);
		$macAddress = cleanString($_POST['macAddress']);
		$ignoreOnOff = cleanInteger($_POST['IgnoreOnOff']);
		$controlledVia = (@$_POST['controlledVia']!='0')?cleanInteger(@$_POST['controlledVia']):NULL;
		$needConfigure = (isset($_POST['needConfigure']))?cleanInteger($_POST['needConfigure']):0;
		$PingTest=(isset($_POST['PingTest']))?1:0;
		$State= (isset($_POST['State']))?cleanString($_POST['State']):getStateValue('State');
		$Status= cleanString($_POST['Status']);
		$room=(@$_POST['Room']!='0')?@$_POST['Room']:NULL;	
		$deviceDisabled= (isset($_POST['deviceDisabled']))?cleanInteger($_POST['deviceDisabled']):0;
		$isOrbiter=isOrbiter($deviceID,$dbADO);
		$ManuallyConfigureEA=(int)@$_REQUEST['ManuallyConfigureEA'];
		
		// for devices from category Storage Devices, checking "reconfigure device" also set state = **RUN_CONFIG**
		if($needConfigure==1 && isStorageDevice($DeviceCategory,$dbADO)===true){
			$Status='**RUN_CONFIG**';
		}
	
		
		if(isMediaDirector($deviceID,$dbADO,1) && $usedBy=roomIsUsed($room,$deviceID,$dbADO)){
			Header('Location: index.php?section=editDeviceParams&deviceID='.$deviceID.'&error='.translate('TEXT_ROOM_USED_BY_ANOTHER_MD_CONST').urlencode(': '.$usedBy));
			exit();			
		}

		$addNewDeviceRelated = (int)$_POST['addNewDeviceRelated'];
		if ($addNewDeviceRelated!=0) {
			$queryInsertDeviceRelated = 'INSERT INTO Device_Device_Related 
			(FK_Device,FK_Device_Related)
		values(?,?)
			';
			$res=$dbADO->Execute($queryInsertDeviceRelated,array($deviceID,$addNewDeviceRelated));			
		}
		
		$addNewGroup = (int)$_POST['addNewGroup'];
		if ($addNewGroup!=0) {
			$queryInsertDeviceGroup = 'INSERT INTO Device_DeviceGroup
			(FK_Device,FK_DeviceGroup)
		values(?,?)
			';
			$res=$dbADO->Execute($queryInsertDeviceGroup,array($deviceID,$addNewGroup));			
		}
		
		
		$selectedRelatedDevices = $_POST['selectedRelatedDevice'];
		$selectedRelatedDevicesArray = explode(",",$selectedRelatedDevices);
		foreach ($selectedRelatedDevicesArray as $selectedRelatedDev) {
			$selectOldValueQuery = "SELECT Value From Device_Device_Related WHERE FK_Device = ? AND  FK_Device_Related = ?";
			$selectOldValueRes = $dbADO->Execute($selectOldValueQuery,array($deviceID,$selectedRelatedDev));
			$selectOldValueRow = array();
			 if ($selectOldValueRes) {
			 	$selectOldValueRow = $selectOldValueRes->FetchRow();			 	
			 }
			$valueRelatedDevice = cleanString(@$_POST["relatedDeviceValue_".$selectedRelatedDev]);
			if ($valueRelatedDevice!=$selectOldValueRow) {
				$updateQuery = 'UPDATE Device_Device_Related SET Value = ? WHERE FK_Device = ? AND FK_Device_Related = ?';
				$resUpdate = $dbADO->Execute($updateQuery,array($valueRelatedDevice,$deviceID,$selectedRelatedDev));
			}
		}

		// save the Device Data values
		$selectedDate = cleanString(@$_POST['DeviceDataToDisplay']);
		$selectedDateArray = explode(",",$selectedDate);
		if (!is_array($selectedDateArray)) {$selectedDateArray=array();$selectedDateArray[]=0;}

		foreach ($selectedDateArray as $elem) {
			if(isset($_POST['oldDeviceData_'.$deviceID.'_'.$elem])){
				$value=cleanString(@$_POST['deviceData_'.$deviceID.'_'.$elem]);
				$oldDeviceData=cleanString(@$_POST['oldDeviceData_'.$deviceID.'_'.$elem]);
				$isDisabled=(int)@$_POST['isDisabled_'.$deviceID.'_'.$elem];
				
				if($oldDeviceData!=$value && $isDisabled!=1){
					$query = "update Device_DeviceData set IK_DeviceData = ? where  FK_Device = ? and FK_DeviceData = ?";
					$rs=$dbADO->Execute($query,array($value,$deviceID,$elem));	

					if(@$isOrbiter==1 && ($elem==$GLOBALS['UsealphablendedUI'] || $elem==$GLOBALS['UseOpenGLeffects']) && $dbADO->Affected_Rows()>0){
						restartX($value,getTopLevelParentIP($value,$dbADO));
					}

					if($elem==$GLOBALS['MobileOrbiterPhone']){
						$rowOldDD=$res->FetchRow();
						$securityPlugin=getDeviceFromDT($installationID,$GLOBALS['SecurityPlugin'],$dbADO);
						$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=REPLACE(IK_DeviceData,\','.$rowOldDD['IK_DeviceData'].',\',?) WHERE FK_Device=? AND FK_DeviceData=?',array(','.$value.',',$securityPlugin,$GLOBALS['MobileOrbiterNotification']));
					}
				}
				
				// set simlink for security plugin path	
				if($elem==$GLOBALS['Path'] && $DeviceTemplate==$GLOBALS['SecurityPlugin']){
					exec('sudo -u root /usr/pluto/bin/SetWebLinks.sh \''.$value.'\' \'/var/www/lmce-admin/security_images\'',$retArray);
					$error=@join('',$retArray);
				}
			}
		}	
		if (trim($description)!='') {		
			$selectOldValues = 'SELECT * FROM Device where PK_Device = ?';	
			$resOldValues = $dbADO->Execute($selectOldValues,array($deviceID));
				if ($resOldValues) {
					$rowOldValues = $resOldValues->FetchRow();
					$old_description = cleanString($rowOldValues['Description']);
					$old_DeviceTemplate = cleanInteger($rowOldValues['FK_DeviceTemplate']);
						//check if master device list ischanged. if yes, delete the data values
				}
			
			
			$query = "
				UPDATE Device 
				SET 
					Description=?,
					IPaddress=?,
					MACaddress=?,
					IgnoreOnOff=?,
					NeedConfigure=?,
					FK_Room=?,
					PingTest=?,
					State=?,
					`Status`=?,
					Disabled=?,
					ManuallyConfigureEA=?
				WHERE PK_Device = ?";
			$dbADO->Execute($query,array($description,$ipAddress,$macAddress,$ignoreOnOff,$needConfigure,$room,$PingTest,$State,$Status,$deviceDisabled,$ManuallyConfigureEA,$deviceID));
			
			updateRoomForEmbeddedDevices($deviceID,$room,$dbADO);
			
			setDCERouterNeedConfigure($installationID,$dbADO);
			updateDeviceControlledBy($deviceID,$controlledVia,$dbADO);
			
			$EntAreasArray=explode(',',$_POST['displayedEntAreas']);
			$OldEntAreasArray=explode(',',$_POST['oldEntAreas']);
			
			foreach($EntAreasArray AS $key => $value){
				$entArea=(isset($_POST['entArea_'.$value]))?(int)@$_POST['entArea_'.$value]:0;
				if($entArea!=$OldEntAreasArray[$key]){
					if($entArea==1){
						addDeviceToEntertainArea($deviceID,$value,$dbADO);
						updateEAForEmbeddedDevices($deviceID,$value,$dbADO);
					}else{
						$deleteDeviceEntertainArea='DELETE FROM Device_EntertainArea WHERE FK_Device=? AND FK_EntertainArea=?';
						$dbADO->Execute($deleteDeviceEntertainArea,array($deviceID,$value));
					}
				}
			}
		}
		
			
		//pipes
		$addDeviceForPiping = cleanInteger(@$_POST['addDeviceForPiping']);
		$addPipe = cleanInteger(@$_POST['addPipe']);
		
		if ($addDeviceForPiping!=0 && $addPipe!=0) {
			$checkDDP=$dbADO->Execute('SELECT * FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?',array($deviceID,$addDeviceForPiping,$addPipe));
			if($checkDDP->RecordCount()==0){
				$insertDevicePipe  = "INSERT INTO Device_Device_Pipe (FK_Device_From,FK_Device_To,FK_Pipe) VALUES(?,?,?)";
				$res=$dbADO->Execute($insertDevicePipe,array($deviceID,$addDeviceForPiping,$addPipe));
				$msg='Device Pipe added.';
			}
			else
				$error=translate('TEXT_ERROR_DEVICE_PIPE_EXISTS_CONST');
		}
		
		$selectPipesUsed = 'SELECT Device_Device_Pipe.*,D1.Description as Desc_From,D2.Description as Desc_To
							 FROM Device_Device_Pipe 
								INNER JOIN Device D1 ON D1.PK_Device = FK_Device_From
								INNER JOIN Device D2 ON D2.PK_Device = FK_Device_To
						WHERE FK_Device_From = ?';
		$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($deviceID));
		
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {	
			$inputName='input_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'];
			$outputName='output_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'];
			
			$input=((int)@$_POST[$inputName]!=0)?(int)@$_POST[$inputName]:NULL;
			$pipeOutput=((int)@$_POST[$outputName]!=0)?$_POST[$outputName]:NULL;
						
			$deviceTo=$rowSelectedPipesUsed['FK_Device_To'];
			$pipe=$rowSelectedPipesUsed['FK_Pipe'];
			$updateDevicePipe = 'UPDATE Device_Device_Pipe SET FK_Command_Input=?,FK_Command_Output=? WHERE FK_Device_From = ? AND FK_Device_To = ? AND FK_Pipe=?';
			$res=$dbADO->Execute($updateDevicePipe,array($input,$pipeOutput,$deviceID,$deviceTo,$pipe));
		}

		if(isPhone($DeviceTemplate,$dbADO)){
			$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.$deviceID;
			exec($cmd);
		}

		// set dhcp conf
		$cmd='sudo -u root /usr/pluto/bin/Diskless_RenameFS.sh --devid '.$deviceID.' --oldip "'.$oldIpAddress.'" --newip "'.$ipAddress.'" --oldmac "'.$oldMacAddress.'" --newmac "'.$macAddress.'"';
		exec($cmd);
		
		$out.='
		<script>
			self.location=\'index.php?section=editDeviceParams&deviceID='.$deviceID.((isset($error))?'&error='.$error:'&msg=Device updated').'\';
			//top.frames[\'treeframe\'].location=\'index.php?section=leftMenu\';
		</script>';
	} else {
		Header('Location: index.php?section=editDeviceParams&deviceID='.$deviceID.'&error='.translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
		exit();
	}
}


$parentsForMenu = getMyDeviceParents($deviceID,$dbADO);
$parentsForMenuArray = explode("**&&**",$parentsForMenu);
$parentsForMenuArray=array_reverse($parentsForMenuArray);
$navMenuString = array();

$navMenuString[]=('My Devices:!:index.php?section=myDevices');


foreach ($parentsForMenuArray as $parent) {
	$parentArray = explode("||&&||",$parent);
	if (count($parentArray)==2)
		$navMenuString[]=($parentArray[1].':!:index.php?section=editDeviceParams&deviceID='.$parentArray[0]);	
}


eval("\$c=array(\$navMenuString);");

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_DEVICE_CONST').' #'.$deviceID.': '.$description);

	$output->setNavigationMenu($c);
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function getMyDeviceParents($childID,$dbADO) {
	//$dbADO->debug=true;
	$stringResult = '';
	
	if ($childID!=0) {
		$queryGP = "select FK_Device_ControlledVia,Description,PK_Device from Device where PK_Device = $childID order by Description Asc";
		$resGP = $dbADO->Execute($queryGP);
		
		if ($resGP) {
			while ($row=$resGP->FetchRow()) {				
					$stringResult.= $row['PK_Device'].'||&&||'.$row['Description'].'**&&**';
					$stringResult.= getMyDeviceParents($row['FK_Device_ControlledVia'],$dbADO);
			}
		}	
	}
	
	return $stringResult;
}

// check if the room has another media director assigned
// since we need to restrict a 1/1 relationship between mds and rooms
function roomIsUsed($room,$deviceID,$dbADO){
	$mdArray=getDeviceTemplatesFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);

	$query='
		SELECT Room.Description, PK_Room,Device.Description AS md,PK_Device 
		FROM Room 
		LEFT JOIN Device ON FK_Room=PK_Room AND FK_DeviceTemplate IN ('.join(',',$mdArray).')
		WHERE Room.FK_Installation=? AND PK_Device!=? AND PK_Room=?
		ORDER BY Description ASC';
	$res=$dbADO->Execute($query,array((int)$_SESSION['installationID'],$deviceID,$room));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}
	
	return false;
}

function isStorageDevice($DeviceCategory,$dbADO){
	$categories=getAncestorsForCategory($DeviceCategory,$dbADO);
	if(in_array($GLOBALS['StorageDevices'],$categories)){
		return true;
	}
	
	return false;
}
?>

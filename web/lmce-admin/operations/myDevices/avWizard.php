<?php
function avWizard($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/avWizard.lang.php');
	
	global $dbPlutoMainDatabase,$wikiHost;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$_SESSION['selectedEntArea']=(isset($_REQUEST['entArea']))?(int)$_REQUEST['entArea']:-2; //make the default All Entertain Areas		

	$deviceCategory=$GLOBALS['rootAVEquipment'];
	$specificFloorplanType=$GLOBALS['AVEquipmentFlorplanType'];
	$output->setHelpSrc('/wiki/index.php/AV_Devices');

	// get AV categories Device Templates
	$categoriesArray=getDescendantsForCategory($deviceCategory,$dbADO);
	
	// get associative array with rooms
	$roomArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'ORDER BY Description ASC');

	// get associative array with entertain areas and add "all entertain areas" to it
	$entAreas=getAssocArray('EntertainArea','PK_EntertainArea','EntertainArea.Description',$dbADO,'INNER JOIN Room ON FK_Room=PK_Room WHERE FK_Installation='.$installationID,'ORDER BY EntertainArea.Description ASC');
	$entAreas[-2]=$TEXT_ALL_ENTERTAIN_AREAS_CONST;
	
	// get AV device templates, infrared Device templates and specialised device templates and put yhem in global variables
	// this are used for controlled_via function, to avoid having them extracted from DB for each device
	global $avArray,$controlledByIfIR,$controlledByIfNotIR;
	$avArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
	$controlledByIfIR=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
	$controlledByIfNotIR=getDevicesFromCategories(array($GLOBALS['rootComputerID']),$dbADO);
	
	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$categoriesArray).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$dtArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$dtArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}


	

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
		// get share with others
		$queryDevice='
			SELECT Device.*,Device_StartupScript.Enabled 
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Device_StartupScript ON FK_Device=PK_Device AND FK_StartupScript=?
			WHERE FK_DeviceCategory=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['ShareIRCodes'],$GLOBALS['CategoryCore'],$installationID));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
			$sharedWithOthers=($rowDevice['Enabled']==1)?1:0;
		}
		
		switch ($_SESSION['selectedEntArea']){
			case -2:
				$filter='';
			break;
			case 0:
				$filter=' AND FK_EntertainArea IS NULL';
			break;
			default:
				$filter=' AND FK_EntertainArea ='.$_SESSION['selectedEntArea'];
			break;
		}
		
		$queryConnectedToDevices='
			SELECT DISTINCT Device.*
			FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceTemplate_Input ON DeviceTemplate_Input.FK_DeviceTemplate=Device.FK_DeviceTemplate 
			WHERE Device.FK_DeviceTemplate IN ('.join(',',$dtArray).') AND FK_Installation=?';	
		$resConnectedToDevices=$dbADO->Execute($queryConnectedToDevices,$installationID);
		$conD=array();
		while($rowConD=$resConnectedToDevices->FetchRow()){
			$conD[$rowConD['PK_Device']]=$rowConD['Description'];
		}
		$resConnectedToDevices->Close();

		// get devices and their details
		$displayedDevices=array();
		$joinArray=$dtArray;
		$joinArray[]=0; 	// used only for query when there are no DT in selected category
		$queryDevice='
			SELECT 
				FK_MediaType,
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
				DeviceTemplate.IsIPBased, 
				DeviceTemplate.FK_DeviceCategory,
				DeviceData.Description AS dd_Description, 
				Device_DeviceData.FK_DeviceData,
				ParameterType.Description AS typeParam, 
				Device_DeviceData.IK_DeviceData,
				ShowInWizard,ShortDescription,
				AllowedToModify,
				DeviceTemplate_DeviceData.Description AS Tooltip,
				Parent.Description AS PDescription,
				Parent.FK_DeviceTemplate AS ParentDT,
				DeviceTemplate.IsEmbedded,
				Parent.FK_DeviceTemplate AS ParentDT,
				ParentDT.FK_DeviceCategory AS ParentCategory
			FROM DeviceData 
			INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
			INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
			INNER JOIN Device ON Device_DeviceData.FK_Device=Device.PK_Device
			LEFT JOIN Device Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
			LEFT JOIN DeviceTemplate ParentDT ON Parent.FK_DeviceTemplate=ParentDT.PK_DeviceTemplate
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate 
			LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate AND FK_MediaType in (1,11)
			LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
			INNER JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory 
			INNER JOIN Manufacturer ON DeviceTemplate.FK_Manufacturer=PK_Manufacturer 		
			LEFT JOIN Device_EntertainArea ON Device_EntertainArea.FK_Device=Device.PK_Device	
			WHERE Device.FK_DeviceTemplate IN ('.join(',',$joinArray).') AND Device.FK_Installation=? '.$filter.'
			GROUP BY PK_Device,PK_DeviceData 
			ORDER BY FK_Device_RouteTo DESC, IsEmbedded DESC,Device.Description ASC';
		$resDevice=$dbADO->Execute($queryDevice,$installationID);
		
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="avWizard">
	<input type="hidden" name="section" value="avWizard">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	
	<div id="preloader" style="display:;">
		<table width="100%">
			<tr>
				<td align="center">'.$TEXT_LOADING_CONST.'</td>
			</tr>
		</table>
	</div>
	<div id="content" style="display:none;">
		<a href="javascript:windowOpen(\'index.php?section=connectionWizard\',\'width=1024,height=760,toolbar=1,scrollbars=1,resizable=1\');">'.$TEXT_CONNECTION_WIZARD_CONST.'</a><div align="center"><input type="checkbox" name="shareIRCodes" value="1" '.((@$sharedWithOthers>0)?'checked':'').' onClick="document.avWizard.submit();"> '.$TEXT_SHARE_CODES_CONST.'</div>
		
		<input type="hidden" name="coreID" value="'.$coreID.'">
		<input type="hidden" name="oldShareIRCodes" value="'.((@$sharedWithOthers>0)?'1':'0').'">
		
		<div align="center"><B>'.$TEXT_CHOOSE_ENTERTAIN_AREA_CONST.'</B>:<BR> '.pulldownFromArray($entAreas,'entArea',$_SESSION['selectedEntArea'],'onchange="document.avWizard.action.value=\'form\';document.avWizard.submit();" style="width:200px;"','key',$TEXT_UNASSIGNED_CONST).'</div>

		<table align="center" border="0" cellpadding="0" cellspacing="0">';
		$childOf=array();
		$firstDevice=0;
		$deviceDataArray=array();
		$liveTVArray=array();
		while($rowD=$resDevice->FetchRow()){
			if($rowD['FK_Device_ControlledVia']==$rowD['FK_Device_RouteTo'] || ($rowD['IsEmbedded']==1 && in_array($rowD['ParentDT'],$dtArray)))
				$childOf[$rowD['PK_Device']]=$rowD['FK_Device_ControlledVia'];
			$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
			
			if($rowD['PK_Device']!=$firstDevice){
				$firstDevice=$rowD['PK_Device'];
				$deviceDataArray[$firstDevice]=array();
				$deviceDataArray[$firstDevice][]=$rowD;
			}else{
				$deviceDataArray[$firstDevice][]=$rowD;
			}
			
			// if the records have a FK_MediaType 1 or 11, add to array
			if(!is_null($rowD['FK_MediaType']) && !in_array($rowD['PK_Device'],$liveTVArray)){
				$liveTVArray[]=$rowD['PK_Device'];
			}
		}
		

		if(count($displayedDevices)==0){
			$out.='
				<tr>
					<td align="center" colspan="7" class="alternate_back"><B>'.$TEXT_NO_RECORDS_CONST.'</B></td>
				</tr>';
		}

		$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category
		if(count($joinArray)==0)
			$joinArray[]=0;
		$resDevice->MoveFirst();
		$pos=0;
		$embededRows=array();
		$GLOBALS['DeviceDataToDisplay']=array();
		$deviceDisplayed=0;
		while($rowD=$resDevice->FetchRow()){
			if($rowD['PK_Device']!=$deviceDisplayed){
				
				$deviceDisplayed=$rowD['PK_Device'];
				$pos++;
	
				$deviceName=(@$childOf[$rowD['PK_Device']]=='')?'<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'" style="width:200px;">':'<input type="hidden" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"><B>'.$rowD['Description'].'</B>';

				$roomPulldown=pulldownFromArray($roomArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:200px;"','key','&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- '.$TEXT_SELECT_ROOM_CONST.' -&nbsp;&nbsp;&nbsp;&nbsp;');
				
			
				$buttons='
						<input value="'.$TEXT_HELP_CONST.'" type="button" class="button_fixed" name="help" onClick="window.open(\''.$wikiHost.'index.php/Documentation_by_Device_Templates#'.wikiLink($rowD['TemplateName']).'\');"><br>
						<input type="button" class="button_fixed" name="edit_'.$rowD['PK_Device'].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
						<input type="button" class="button_fixed" name="btn" value="'.$TEXT_AV_PROPERTIES_CONST.'" onClick="windowOpen(\'index.php?section=irCodes&dtID='.$rowD['FK_DeviceTemplate'].'&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=1024,height=768,toolbar=1,scrollbars=1,resizable=1\');"><br>
						<input type="button" class="button_fixed" name="resync_'.$rowD['PK_Device'].'" value="'.$TEXT_RESYNC_CONST.'"  onclick="self.location=\'index.php?section=resyncCodes&from=avWizard&dtID='.$rowD['FK_DeviceTemplate'].'\';"><br>
						<input type="submit" class="button_fixed" name="delete_'.$rowD['PK_Device'].'" value="'.$TEXT_DELETE_CONST.'"  onclick="if(confirm(\''.$TEXT_DELETE_DEVICE_CONFIRMATION_CONST.'\'))return true;else return false;">
				</td>';
//				$controlledByPulldown=controlledViaPullDown('controlledBy_'.$rowD['PK_Device'],$rowD['PK_Device'],$rowD['FK_DeviceTemplate'],$rowD['FK_DeviceCategory'],$rowD['FK_Device_ControlledVia'],$dbADO);


				$controlledViaLink='
				<input type="hidden" name="controlledBy_'.$rowD['PK_Device'].'" value="'.$rowD['FK_Device_ControlledVia'].'">		
				<a href="javascript:windowOpen(\'index.php?section=editDeviceControlledVia&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');" title="'.$TEXT_CLICK_TO_CHANGE_CONST.'">'.((is_null($rowD['FK_Device_ControlledVia']))?$TEXT_EDIT_CONST:$rowD['PDescription']).'</a>';
				$devicePipes=getPipes($rowD['PK_Device'],$dbADO);
	
				unset($GLOBALS['DeviceIDControlledVia']);
				unset($GLOBALS['DeviceControlledVia']);
	

				if(@$childOf[$rowD['PK_Device']]==''){
					$out.='<tr class="tablehead">
						<td align="center" width="200"><B>'.$TEXT_DEVICE_ROOM_CONTROLLED_BY_CONST.'</B></td>
						<td align="center" width="200"><B>'.$TEXT_PIPES_CONST.'</B></td>
						<td align="center" width="200"><B>'.$TEXT_DEVICE_DATA_CONST.'</B></td>
						<td align="center" width="125"><B>'.$TEXT_ACTION_CONST.'</B></td>
					</tr>		

					<tr>
						<td rowspan="5" align="left" valign="top" class="alternate_back">
							'.$TEXT_DEVICE_NUM_CONST.': '.$rowD['PK_Device'].'<br>
							'.$TEXT_DEVICE_TEMPLATE_NUM_CONST.': '.$rowD['FK_DeviceTemplate'].'<br>
							'.$TEXT_CONTROLLED_BY_CONST.': '.$controlledViaLink.'<br><br>
							<a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'<br>'.$roomPulldown.'

						</td>
						<td align="center" height="10"><B>'.$TEXT_AUDIO_PIPE_CONST.'</B></td>
						<td class="alternate_back" rowspan="5" valign="top" align="right">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased'],0,1,'textarea').'</td>
						<td align="center" rowspan="5" valign="top">'.$buttons.'</td>
					</tr>
					<tr>
						<td height="20">
						<u>'.$TEXT_OUTPUT_CONST.'</u>: '.@$devicePipes['1']['output'].'<br>
						<u>'.$TEXT_TO_CONST.'</u>: '. @$devicePipes['1']['to'].'<br>
						<u>'.$TEXT_INPUT_CONST.'</u>: '.@$devicePipes['1']['input'].'<br><br>
						</td>
					</tr>
					<td align="center" height="10"><B>'.$TEXT_VIDEO_PIPE_CONST.'</B></td>
					<tr>			

						<td height="20">
						<u>'.$TEXT_OUTPUT_CONST.'</u>: '.@$devicePipes['2']['output'].'<br>
						<u>'.$TEXT_TO_CONST.'</u>: '. @$devicePipes['2']['to'].'<br>
						<u>'.$TEXT_INPUT_CONST.'</u>: '.@$devicePipes['2']['input'].'<br><br>
						</td>
						
					</tr>
					<tr>
						<td align="center" valign="top"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');">'.$TEXT_EDIT_PIPES_CONST.'</a></td>
					</tr>';
					if(isset($embededRows[$rowD['PK_Device']])){
						foreach($embededRows[$rowD['PK_Device']] as $tuner){
							$out.=$tuner;
						}
					}
					$out.='
					<tr>
						<td colspan="7" height="3" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
					</tr>
					<tr><td colspan="7">&nbsp;</td></tr>
					<tr><td colspan="7">&nbsp;</td></tr>

					';						
				}else{
					// hardcoded: if parent is in device category "Output zone" (168), allow room pulldown
					$roomPulldown=($rowD['ParentCategory']=168 && $rowD['IsEmbedded']==1)?pulldownFromArray($roomArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:200px;"','key','&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- '.$TEXT_SELECT_ROOM_CONST.' -&nbsp;&nbsp;&nbsp;&nbsp;'):'';
					
					$embededRows[$rowD['FK_Device_ControlledVia']][]='
					<tr>
						<td colspan="7" height="1" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
					</tr>
					<tr>
						<td align="center" class="alternate_back" valign="top" align="left"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
						<td align="center"><B>'.$TEXT_AUDIO_PIPE_CONST.'</B></td>
						<td class="alternate_back" align="center" rowspan="5">- '.$TEXT_EMBEDED_DEVICE_CONST.' -</td>
						<td rowspan="5">&nbsp;</td>
					</tr>
					<tr>
						<td class="alternate_back" rowspan="4" align="center" valign="top">- '.$TEXT_EMBEDED_DEVICE_CONST.' -'.$roomPulldown.'</td>
						<td height="20">
						<u>'.$TEXT_OUTPUT_CONST.'</u>: '.@$devicePipes['1']['output'].'<br>
						<u>'.$TEXT_TO_CONST.'</u>: '. @$devicePipes['1']['to'].'<br>
						<u>'.$TEXT_INPUT_CONST.'</u>: '.@$devicePipes['1']['input'].'<br><br>
						</td>
					</tr>
					</tr>
					<tr>
						<td align="center"><B>'.$TEXT_VIDEO_PIPE_CONST.'</B></td>
					</tr>
					<tr>			
						<td height="20">
						<u>'.$TEXT_OUTPUT_CONST.'</u>: '.@$devicePipes['2']['output'].'<br>
						<u>'.$TEXT_TO_CONST.'</u>: '. @$devicePipes['2']['to'].'<br>
						<u>'.$TEXT_INPUT_CONST.'</u>: '.@$devicePipes['2']['input'].'<br><br>
						</td>
					</tr>

					</tr>
					<tr>
						<td align="center" valign="top"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');">'.$TEXT_EDIT_PIPES_CONST.'</a></td><
					</tr>';

				}

			}
		}
		$out.='
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';

		if($resDevice->RecordCount()!=0){
			$updateBtns='<input type="submit" class="button_fixed" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button_fixed" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">';
		}
		$out.='
				<tr>
					<td colspan="7">* '.$TEXT_FLOORPLAN_NOTE_CONST.'</td>
				</tr>
				<tr>
					<td colspan="7" align="center"><input type="button" class="button_fixed" name="button" value="'.$TEXT_ADD_DEVICE_CONST.'" onClick="document.avWizard.action.value=\'externalSubmit\';document.avWizard.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=avWizard&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"> '.@$updateBtns.'</td>
				</tr>
			</table>
			<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
		</form>
		<script>
		 	var frmvalidator = new formValidator("avWizard");
 		</script>
	
	</form>
	</div>
	';


		$output->setScriptInBody('onLoad="document.getElementById(\'preloader\').style.display=\'none\';document.getElementById(\'content\').style.display=\'\';";');
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=avWizard&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
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
		}

		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
				foreach($displayedDevicesArray as $key => $value){
					$description=stripslashes(@$_POST['description_'.$value]);
					$updateMacIp='';
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
						updateRoomForEmbeddedDevices($value,$room,$dbADO);
					}else{
						$updateDevice='UPDATE Device SET Description=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$value));
					}
					if(isset($_POST['room_'.$value])){
						$updateDevice='UPDATE Device SET FK_Room=? WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($room,$value));
					}
					
					foreach($DeviceDataToDisplayArray as $ddValue){
						$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?cleanString($_POST['deviceData_'.$value.'_'.$ddValue]):0;
						$oldDeviceData=cleanString(@$_POST['oldDeviceData_'.$value.'_'.$ddValue]);
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


					processReceiver($value,$dbADO);
				}
			}

			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
		}

		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID,$ret);
				$insertID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);
				if((int)@$_SESSION['selectedEntArea']>0){
					$dbADO->Execute('UPDATE Device,EntertainArea SET Device.FK_Room = EntertainArea.FK_Room WHERE PK_Device=? AND PK_EntertainArea=?',array($insertID,$_SESSION['selectedEntArea']));
					$dbADO->Execute('INSERT INTO Device_EntertainArea (FK_Device,FK_EntertainArea) VALUES (?,?)',array($insertID,$_SESSION['selectedEntArea']));
				}
				
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);

				if(isInfrared($deviceTemplate,$dbADO)){
					// add video port if does not exist
					$dbADO->Execute('INSERT IGNORE INTO Device_DeviceData (FK_Device, FK_DeviceData) VALUES (?,?)',array($insertID,$GLOBALS['InfraredPort']));
				}
			}
			header("Location: index.php?section=avWizard&lastAdded=$deviceTemplate#deviceLink_".@$insertID);
			exit();
		}


		header("Location: index.php?section=avWizard&msg=$TEXT_DEVICES_WAS_UPDATED_CONST".@$anchor);
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_AV_EQUIPMENT_CONST);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_AV_EQUIPMENT_CONST=>'index.php?section=avWizard'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_AV_EQUIPMENT_CONST);
	$output->output();
}

function getSubDT($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT FK_DeviceTemplate
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['FK_DeviceTemplate'];
	}else{
		return 0;
	}
}

function getSubDevice($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT PK_Device
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}else{
		return 0;
	}
}

function recreateDevice($mdID,$categoryID,$dtID,$installationID,$roomID,$dbADO)
{
	$deviceID=getSubDevice($mdID,$categoryID,$dbADO);
	if($deviceID!=0){
		deleteDevice($deviceID,$dbADO);
	}
	if($dtID!=0){
		createDevice($dtID,$installationID,$mdID,$roomID,$dbADO,1);
	}
}
?>

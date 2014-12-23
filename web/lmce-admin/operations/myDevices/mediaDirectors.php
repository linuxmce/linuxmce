<?php
function mediaDirectors($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('mediaDirectors.lang.php');

	global $dbPlutoMainDatabase,$wikiHost;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$pvrArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory IN ('.$GLOBALS['PVRCaptureCards'].','.$GLOBALS['DigitalTVCards'].')','ORDER BY Description ASC');
	$soundArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['SoundCards'],'ORDER BY Description ASC');
	$videoArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['VideoCards'],'ORDER BY Description ASC');
	$audioSettingsArray=array(
		'M'=>'Manual settings', 
		'C'=>'SPDIF Coax', 
		'O'=>'SPDIF TosLink', 
		'S'=>'Stereo', 
		'L'=>'Multi-channel analog',
		'H'=>'HDMI',
		);
	$deinterlaceQualityArray=array_flip(array(
		"None" => "",
		"Lowest" => "tvtime:method=LineDoubler,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1",
		"Low" => "tvtime:method=LinearBlend,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1",
		"Medium" => "tvtime:method=Greedy,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1",
		"High" => "tvtime:method=Greedy,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=0",
		"Higher" => "tvtime:method=Greedy2Frame,enabled=1,pulldown=vektor,framerate_mode=full,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=0",
		"Highest" => "tvtime:method=Greedy2Frame,enabled=1,pulldown=vektor,framerate_mode=full,judder_correction=1,use_progressive_frame_flag=1,chroma_filter=1,cheap_mode=0",
		"vdpau" => "tvtime:method=use_vo_driver",
		"Custom"=>"*"));
	$uiArray=array(
		'001'=>'UI1',
		'114'=>'UI2 with alpha blending',
		'104'=>'UI2 without alpha blending'
	);	
	
	$infraredReceiversArray=getDescendantsForCategory($GLOBALS['InfraredReceivers'],$dbADO);
	$osOnHardArray=array(
		''=>'None',
		'Windows'=>'Windows',
		'Mac'=>'Mac',
		'Linux'=>'Linux',
		'Other'=>'Other'
	);
	
	$deviceCategory=$GLOBALS['rootMediaDirectors'];
	$specificFloorplanType=$GLOBALS['EntertainmentZone'];
	$output->setHelpSrc('/wiki/index.php/Media_Directors');

	// get selected category Device Templates
	$mdArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO,1);
	$DTArray=array_values($mdArray);
	$DTIDArray=array_keys($mdArray);

	// get AV Device Templates
	$avDTIDArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);

	$queryRooms='
		SELECT Room.Description, PK_Room,Device.Description AS md,PK_Device 
		FROM Room 
		LEFT JOIN Device ON FK_Room=PK_Room AND FK_DeviceTemplate IN ('.join(',',$DTIDArray).')
		WHERE Room.FK_Installation=? 
		ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	$roomPulldownArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		if((!is_null($rowRoom['PK_Device']))){
			$suffix=' [ '.$rowRoom['md'].' ]';
		}else{
			$suffix='';	
		}
		$roomArray[]=$rowRoom['Description'].$suffix;
		$roomIDArray[]=$rowRoom['PK_Room'];
		$roomPulldownArray[$rowRoom['PK_Room']]=$rowRoom['Description'].$suffix;
	}

	if(isset($_REQUEST['newMD']) && (int)$_REQUEST['newMD']==1){
		$out.='
		<script>
			alert("'.translate('TEXT_WAIT_FOR_PREPARATION_NOTIFICATION_CONST').'")
		</script>';

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
			
			function set_ac3(deviceID){
				// 1) if any non-SPDIF is selected in "Audio settings", the "AC3 pass-through" is de-selected AND disabled (so user cannot check it)
				// 2) if any SPDFIF is selected, the "AC3 pass-through" is enabled (so user can check it if he wants)
				
				eval("var audioSettings=document.mediaDirectors.audioSettings_"+deviceID+".value");
				if(audioSettings!="C" && audioSettings!="O" && audioSettings!="H"){
					eval("document.mediaDirectors.ac3_"+deviceID+".checked=false;");
					eval("document.mediaDirectors.ac3_"+deviceID+".disabled=true;");
				}else{
					eval("document.mediaDirectors.ac3_"+deviceID+".disabled=false;");
				}
			}
	</script>
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="mediaDirectors">
	<input type="hidden" name="section" value="mediaDirectors">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	';
		
		$infraredAndSpecialisedDevices=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
		$specialisedAndComputerDevices=getDevicesFromCategories(array($GLOBALS['rootComputerID'],$GLOBALS['specialized']),$dbADO);

		$out.='<table align="center" border="0" cellpadding="0" cellspacing="0">';
				if(count($avDTIDArray)==0)
					$avDTIDArray[]=0;
				$displayedAVDevices=array();
				$displayedAVDevicesDescription=array();
				$queryDevice='
					SELECT Device.*
					FROM Device 
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$avDTIDArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedAVDevices[]=$rowD['PK_Device'];
					$displayedAVDevicesDescription[]=$rowD['Description'];
				}
				$resDevice->Close();
				
				$queryConnectedToDevices='
					SELECT DISTINCT Device.*
					FROM Device 
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceTemplate_Input ON DeviceTemplate_Input.FK_DeviceTemplate=Device.FK_DeviceTemplate 
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$avDTIDArray).') AND FK_Installation=?';	
				$resConnectedToDevices=$dbADO->Execute($queryConnectedToDevices,$installationID);
				$conD=array();
				while($rowConD=$resConnectedToDevices->FetchRow()){
					$conD[$rowConD['PK_Device']]=$rowConD['Description'];
				}
				$resConnectedToDevices->Close();
							
				$displayedDevices=array();
				$GLOBALS['DeviceDataToDisplay']=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	
				$joinArray[]=0; 	// used only for query when there are no DT in selected category
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
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=? ';
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$childOf=array();
				$firstDevice=0;
				$deviceDataArray=array();
				$oldAudioDD=null;
				$oldVideoDD=null;
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
					
					if($rowD['PK_Device']!=$firstDevice){
						$firstDevice=$rowD['PK_Device'];
						$deviceDataArray[$firstDevice]=array();
					}else{
						$deviceDataArray[$firstDevice][]=$rowD;
					}
					
					$oldAudioDD=($rowD['FK_DeviceData']==$GLOBALS['AudioSettings'])?$rowD['IK_DeviceData']:$oldAudioDD;
					if(!is_null($oldAudioDD)){
						$oldAudioSettings[$rowD['PK_Device']]=str_replace(array('3','K'), '', $oldAudioDD);
						$oldAC3[$rowD['PK_Device']]=(strstr($oldAudioDD,'3') && in_array($oldAudioSettings[$rowD['PK_Device']],array('C','O','H')))?'checked':(!in_array($oldAudioSettings[$rowD['PK_Device']],array('C','O','H'))?' disabled':'');
					}
					$oldVideoDD=($rowD['FK_DeviceData']==$GLOBALS['VideoSettings'])?$rowD['IK_DeviceData']:$oldVideoDD;
					if(!is_null($oldVideoDD)){
						$oldVideoDDArray=explode('/',$oldVideoDD);
						$oldResolution=@$oldVideoDDArray[0];
						$oldRefresh=@$oldVideoDDArray[1];
					}
					
				}

				$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category

			$resDevice->MoveFirst();
			$pos=0;
			$embededRows=array();
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
					$pos++;
					
					$deviceName='Device #: '.$rowD['PK_Device'].'<br>';
					$deviceName.=translate('TEXT_DEVICE_TEMPLATE_CONST').' #: '.$rowD['FK_DeviceTemplate'].'('.$rowD['TemplateName'].')<br><br>';
					$deviceName.='<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'" style="width:225px;">';
					
					$roomPulldown='<select name="room_'.$rowD['PK_Device'].'">
							<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
					foreach($roomIDArray as $key => $value){
						$roomPulldown.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
					}
					$roomPulldown.='</select>';
					$devicePipes=getPipes($rowD['PK_Device'],$dbADO);
	
					$buttons='
					<input value="'.translate('TEXT_HELP_CONST').'" type="button" class="button_fixed" name="help" onClick="window.open(\''.$wikiHost.'/index.php/Documentation_by_Device_Templates#'.wikiLink($rowD['TemplateName']).'\');"><br>
					<input type="button" class="button_fixed" name="edit_'.$rowD['PK_Device'].'" value="'.translate('TEXT_ADVANCED_CONST').'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
					<input type="submit" class="button_fixed" name="delete_'.$rowD['PK_Device'].'" value="'.translate('TEXT_DELETE_CONST').'"  onclick="if(confirm(\''.translate('TEXT_CONFIRM_DELETE_MEDIA_DIRECTOR_CONST').'\'))return true;else return false;">';
					$buttons.=(isDiskless($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']]))?'<br><input type="submit" class="button_fixed" name="rebuild_diskless_'.$rowD['PK_Device'].'" value="'.translate('TEXT_REBUILD_DISKLESS_IMAGE_CONST').'">':'';
						
					
					$controlledByPulldown='&nbsp;';
						
					$out.='
				<tr class="tablehead">
					<td align="center" width="200"><B>'.translate('TEXT_DEVICE_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
					<td align="center" width="200"><B>'.translate('TEXT_PIPES_CONST').'</B></td>
					<td align="center" width="400"><B>'.translate('TEXT_DEVICE_DATA_CONST').'</B></td>
					<td align="center" width="125"><B>'.translate('TEXT_ACTION_CONST').'</B></td>
				</tr>

					<tr>
						<td rowspan="5" align="left" valign="top" class="alternate_back"><a name="deviceLink_'.$rowD['PK_Device'].'"></a><br>'.$deviceName.'<br><br>'.pulldownFromArray($roomPulldownArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:225px;"').'</td>
						<td align="center" height="10"><B>'.$TXT_AUDIO_PIPE_CONST.'</B></td>
						<td rowspan="5" valign="top" align="right" class="alternate_back">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased']).'</td>
						<td align="center" rowspan="5" valign="top"><br>'.$buttons.'</td>
					</tr>
					<tr>
						<td valign="top"  height="20">
						<u>'.translate('TEXT_OUTPUT_CONST').'</u>: '.@$devicePipes['1']['output'].'<br>
						<u>'.translate('TEXT_TO_CONST').'</u>: '.@$devicePipes['1']['to'].'<br>
						<u>'.translate('TEXT_INPUT_CONST').'</u>: '.@$devicePipes['1']['input'].'<br><br>
						</td>
					</tr>
					<tr>			
						<td height="10" align="center"><B>'.$TXT_VIDEO_PIPE_CONST.'</B></td>
					</tr>
					<tr>
						<td valign="top"  height="20">
						<u>'.translate('TEXT_OUTPUT_CONST').'</u>: '.@$devicePipes['2']['output'].'<br>
						<u>'.translate('TEXT_TO_CONST').'</u>: '.@$devicePipes['2']['to'].'<br>
						<u>'.translate('TEXT_INPUT_CONST').'</u>: '.@$devicePipes['2']['input'].'<br><br>
						</td>

					</tr>
					<tr>
						<td valign="top" align="center"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=mediaDirectors\',\'width=600,height=300,toolbar=1,scrollbars=1,resizable=1\');">'.translate('TEXT_EDIT_PIPES_CONST').'</a></td>					
					</tr>
					';

					$orbiterMDChild=getMediaDirectorOrbiterChild($rowD['PK_Device'],$dbADO);
					if($orbiterMDChild){
						$soundDevice=getSubDT($rowD['PK_Device'],$GLOBALS['SoundCards'],$dbADO);
						$videoDevice=getSubDT($rowD['PK_Device'],$GLOBALS['VideoCards'],$dbADO);
						
						$openGL=getDeviceData($orbiterMDChild,$GLOBALS['UseOpenGLeffects'],$dbADO);
						$alphaBlended=getDeviceData($orbiterMDChild,$GLOBALS['UsealphablendedUI'],$dbADO);
						$ui=getDeviceData($orbiterMDChild,$GLOBALS['DD_PK_UI'],$dbADO);
						$uiSelected=(int)$openGL.(int)$alphaBlended.(int)$ui;
						
						// if deinterlaced mode is custom, replace with * for pulldown
						$raw_diq=get_child_device_data($orbiterMDChild,$GLOBALS['DeinterlacingMode'],$dbADO);
						$diq=(!in_array($raw_diq,array_keys($deinterlaceQualityArray)))?'*':$raw_diq;

						$osOnHardDrive=getDeviceData($rowD['PK_Device'],$GLOBALS['DD_OS_ON_HARD_DRIVE'],$dbADO);
						
						$out.='
							<input type="hidden" name="oldSound_'.$rowD['PK_Device'].'" value="'.$soundDevice.'">
							<input type="hidden" name="oldVideo_'.$rowD['PK_Device'].'" value="'.$videoDevice.'">
							<tr>
								<td colspan="4" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1"></td>
							</tr>	
							<tr>					
								<td colspan="4">
									<table border="0" width="100%">
										<tr>
											<td valign="top" rowspan="2" class="alternate_back">'.translate('TEXT_PVR_CAPTURE_CARD_CONST').' '.pulldownFromArray($pvrArray,'PVRCaptureCard_'.$rowD['PK_Device'],0,'style="width:200px;"','key','None').'<br>
											'.getPVRCards(array_keys($pvrArray),$_SESSION['installationID'],$rowD['PK_Device'],(int)$rowD['FK_Device_ControlledVia'],$dbADO).'
											</td>
											<td valign="top" rowspan="2"></td>
											<td align="right" class="alternate_back">'.translate('TEXT_SOUND_CARD_CONST').' '.pulldownFromArray($soundArray,'SoundCard_'.$rowD['PK_Device'],$soundDevice,'style="width:200px;"','key','Standard Sound Card').'<br>
											'.translate('TEXT_AUDIO_SETTINGS_CONST').' '.pulldownFromArray($audioSettingsArray,'audioSettings_'.$rowD['PK_Device'],@$oldAudioSettings[$rowD['PK_Device']],'style="width:200px;" onChange="set_ac3('.$rowD['PK_Device'].')"').'<br>
											'.translate('TEXT_AC3_PASSTHROUGH_CONST').' <input type="checkbox" name="ac3_'.$rowD['PK_Device'].'" value="3" '.@$oldAC3[$rowD['PK_Device']].'><br>
											</td>
											<td valign="top" rowspan="2"></td>
											<td align="right" valign="top" class="alternate_back">'.translate('TEXT_VIDEO_CARD_CONST').' '.pulldownFromArray(@$videoArray,'VideoCard_'.$rowD['PK_Device'],$videoDevice,'style="width:200px;" onChange="if(confirm(\''.translate('TEXT_CHANGE_VIDEO_CARD_CONFIRMATION_CONST').'\')){document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();}"','key','Standard Video Card').'<br>
											</td>
										</tr>
									</table>
								</td>
							</tr>';
	
						$out.='<tr>
							<td colspan="4" bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1"></td>
							</tr>
							<tr>
								<td colspan="4">
									<table cellpadding="0" cellspacing="0" border="0">
										<tr>
											<td><B>'.translate('TEXT_SOFTWARE_MODULES_CONST').'</B></td>
											<td><input type="button" class="button_fixed" name="edit_modules" value="'.translate('TEXT_EDIT_CONST').'" onClick="windowOpen(\'index.php?section=editModules&deviceID='.$rowD['PK_Device'].'&from=mediaDirectors\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
											<td width="20">&nbsp;</td>
											<td><B>'.translate('TEXT_HARDWARE_ACCELERATION_CONST').'</B></td>
											<td>'.pulldownFromArray($GLOBALS['hardware_acceleration'],'acceleration_'.$rowD['PK_Device'],getAcceleration($orbiterMDChild,$dbADO),'style="width:300px;"','key','').'</td>
											<td width="20">&nbsp;</td>
											<td><B>'.((is_null($rowD['FK_Device_ControlledVia']))?translate('TEXT_OS_ON_HARD_DRIVE_CONST'):'&nbsp;').'</B></td>
											<td>'.((is_null($rowD['FK_Device_ControlledVia']))?pulldownFromArray($osOnHardArray,'osOnHardDrive_'.$rowD['PK_Device'],$osOnHardDrive,'style="width:300px;"','key',''):'&nbsp;').'</td>
										</tr>
										<tr>
											<td><B>'.translate('TEXT_INFRARED_REMOTES_YOU_WILL_USE_CONST').'</B></td>
											<td>'.displayRemotes($rowD['PK_Device'],$dbADO,'mediaDirectors').'</td>
											<td>&nbsp;</td>
											<td><B>'.translate('TEXT_DEINTERLACE_QUALITY_CONST').'</B></td>
											<td>'.pulldownFromArray($deinterlaceQualityArray,'diq_'.$rowD['PK_Device'],$diq,'style="width:300px;"','key','').'</td>
											<td>&nbsp;</td>
											<td><!--<B>'.translate('TEXT_UI_CONST').'</B>--></td>
											<td><!--'.pulldownFromArray($uiArray,'ui_'.$rowD['PK_Device'],$uiSelected,'style="width:200px;"').'--></td>
											
										</tr>
										<tr>
											<td><B>'.translate('TEXT_COMPUTING_APPLICATIONS_CONST').'</B></td>
											<td><input type="button" class="button_fixed" name="editCA_" value="'.translate('TEXT_EDIT_CONST').'" onclick="windowOpen(\'index.php?section=editComputingApplications&mdID='.$rowD['PK_Device'].'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\')"></td>
											<td>&nbsp;</td>
											<td><B>'.translate('TEXT_INFRARED_RECEIVERS_CONST').'</B></td>
											<td colspan="4">'.displayReceivers($rowD['PK_Device'],$infraredReceiversArray,$dbADO).'</td>
										</tr>
										';
							/*
							// removed since the screensaver is set as optional device for md
							// will put it back when there will be more than one device							
									$out.='
										<tr>
											<td><B>'.translate('TEXT_SCREEN_SAVER_CONST').'</B></td>
											<td>'.displayScreenSavers($orbiterMDChild,$dbADO).'</td>
										</tr>';
							*/
							$out.='
									</table>
									
								</td>
							</tr>
							<tr><td>&nbsp;</td></tr>
							<tr><td>&nbsp;</td></tr>
							<tr>
								<td colspan="4" bgcolor="black" height="5"><img src="include/images/spacer.gif" border="0"></td>
							</tr>
							<tr><td>&nbsp;</td></tr>
							<tr><td>&nbsp;</td></tr>';	
							
					}
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';

			if($resDevice->RecordCount()!=0){
				$update_buttons='<input type="submit" class="button_fixed" name="update" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button_fixed" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'">';
			}
			$out.='
				<tr>
					<td colspan="8">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="8" align="center">'.@$update_buttons.' <input type="button" class="button_fixed" name="button" value="'.translate('TEXT_ADD_MD_CONST').'" onClick="document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=mediaDirectors&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
				</tr>
				<tr>
					<td colspan="8" align="left">&nbsp;</td>
				</tr>			
			</table>
	</form>
	';
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=mediaDirectors&type=$type&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		if($action=='delCard'){
			deleteDevice((int)$_REQUEST['delCard'],$dbADO);
			header("Location: index.php?section=mediaDirectors&msg=".translate('TEXT_PVR_CARD_DELETED_CONST'));		
			exit();
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
				$mdData=getFieldsAsArray('Device','IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$value);
				$cmd='sudo -u root /usr/pluto/bin/Diskless_DeleteFS.sh "'.$mdData['IPaddress'][0].'" "'.$mdData['MACaddress'][0].'"';
				exec_batch_command($cmd);
				deleteDevice($value,$dbADO);
				$cmd='sudo -u root /usr/pluto/bin/DHCP_config.sh';
				exec_batch_command($cmd);
			}
			
			if(isset($_POST['rebuild_diskless_'.$value])){
				$dbADO->Execute('DELETE FROM Package_Device WHERE FK_Device=?',array($value));
				// also clean list of Add Software packages installed on this M/D
				$dbADO->Execute('DELETE FROM Software_Device WHERE FK_Device=?',array($value));
				
				$mdData=getFieldsAsArray('Device','IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$value);
				$cmd='sudo -u root /usr/pluto/bin/Diskless_DeleteFS.sh "'.$mdData['IPaddress'][0].'" "'.$mdData['MACaddress'][0].'"';
				exec_batch_command($cmd);
				$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',array($value));

				header('Location: index.php?section=setupDisklessMD');
				exit();
			}
			
		}
		
		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
				$usedRooms=array();
				foreach($displayedDevicesArray as $key => $value){
					if($_POST['room_'.$value]!=0){
						$usedRooms[$_POST['room_'.$value]][]=$value;
					}
				}
				foreach ($usedRooms AS $usedID=>$usedArray){
					if(count($usedArray)>1){
						header('Location: index.php?section=mediaDirectors&error='.translate('TEXT_ERROR_DUPLICATE_ROOMS_CONST'));
						exit();
					}
				}
				
				foreach($displayedDevicesArray as $key => $value){
					$description=stripslashes(@$_POST['description_'.$value]);
					if(isset($_POST['ip_'.$value])){
						$oldIpAddress=$_POST['oldIP_'.$value];
						$oldMacAddress=$_POST['oldMAC_'.$value];
						
						$ip=$_POST['ip_'.$value];
						$mac=$_POST['mac_'.$value];
						$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					}
												
					$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
					$dbADO->Execute('UPDATE Device Dparent JOIN Device Dchild ON Dchild.FK_Device_ControlledVia=Dparent.PK_Device SET Dparent.FK_Room=? WHERE Dchild.PK_Device=?',array($room,$value));
					
					if(isset($_POST['controlledBy_'.$value])){
						$updateDevice='UPDATE Device SET Description=?, FK_Room=?, FK_Device_ControlledVia=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));
						
					}else{
						$updateDevice='UPDATE Device SET Description=?, FK_Room=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$value));
					}
					

					if(@$updateMacIp!=''){
						// set DHCP settings
						$cmd='sudo -u root /usr/pluto/bin/Diskless_RenameFS.sh --devid '.$value.' --oldip "'.$oldIpAddress.'" --newip "'.$ip.'" --oldmac "'.$oldMacAddress.'" --newmac "'.$mac.'"';
						exec_batch_command($cmd);
					}
										
					foreach($DeviceDataToDisplayArray as $ddValue){
						$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:0;
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
					
					$orbiterMDChild=getMediaDirectorOrbiterChild($value,$dbADO);
					
					//
					setAcceleration($orbiterMDChild,cleanString($_REQUEST['acceleration_'.$value]),$dbADO);
					if($_REQUEST['diq_'.$value]!='*'){
						set_child_device_data($orbiterMDChild,$GLOBALS['DeinterlacingMode'],cleanString($_REQUEST['diq_'.$value]),$dbADO);
					}
					// setUI($orbiterMDChild,@$_REQUEST['ui_'.$value],$dbADO);
					
					// add/delete PVR Capture Card, sound card and video card
					
					$pvrDT=$_POST['PVRCaptureCard_'.$value];
					if($pvrDT!=0){
						createDevice($pvrDT,$_SESSION['installationID'],$value,$room,$dbADO,1);
						//recreateDevice($value,$GLOBALS['PVRCaptureCards'],$pvrDT,$_SESSION['installationID'],$room,$dbADO,$GLOBALS['DigitalTVCards']);
					}
					$soundDT=$_POST['SoundCard_'.$value];
					if($soundDT!=$_POST['oldSound_'.$value]){
						recreateDevice($value,$GLOBALS['SoundCards'],$soundDT,$_SESSION['installationID'],$room,$dbADO);
					}
					$videoDT=$_POST['VideoCard_'.$value];
					if($videoDT!=$_POST['oldVideo_'.$value]){
						recreateDevice($value,$GLOBALS['VideoCards'],$videoDT,$_SESSION['installationID'],$room,$dbADO,0,1);
						$cmd='/usr/pluto/bin/Config_Device_Changes.sh';
						exec_batch_command($cmd);
					}

					processReceiver($value,$dbADO);
					
					// removed screensaver since it's optional device for MD
					// processScreenSavers($orbiterMDChild,$dbADO);
					$err=processAudioSettings($value,$dbADO);
					if(isset($_POST['osOnHardDrive_'.$value])){
						set_device_data($value,$GLOBALS['DD_OS_ON_HARD_DRIVE'],$_POST['osOnHardDrive_'.$value],$dbADO);
					}
				}
			}
			
			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec_batch_command($commandToSend);
		}
		processRemotes($dbADO);
		if(isset($_REQUEST['add']) && !isset($_REQUEST['mdID'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$insertID=exec_batch_command('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID,$ret);	
				$insertID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);				
				
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec_batch_command($commandToSend);
			}
			header("Location: index.php?section=mediaDirectors&newMD=1&lastAdded=$deviceTemplate#deviceLink_".@$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=mediaDirectors&msg=".translate('TEXT_MD_UPDATED_CONST')."&error=".@$err.@$anchor);		
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_MEDIA_DIRECTORS_CONST'));

	$output->setNavigationMenu(array(translate('TEXT_MEDIA_DIRECTORS_CONST')=>'index.php?section=mediaDirectors'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_MEDIA_DIRECTORS_CONST'));
	$output->output();
}

function getSubDT($mdID,$categoryID,$dbADO,$alternateCategory=0)
{
	$res=$dbADO->Execute('
		SELECT FK_DeviceTemplate
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['FK_DeviceTemplate'];
	}elseif($alternateCategory!=0){
		return getSubDT($mdID,$alternateCategory,$dbADO);
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

function recreateDevice($mdID,$categoryID,$dtID,$installationID,$roomID,$dbADO,$alternateCategory=0,$needConfigure=0)
{
	$deviceID=getSubDevice($mdID,$categoryID,$dbADO);
	if($deviceID!=0){
		deleteDevice($deviceID,$dbADO);
	}elseif($alternateCategory!=0){
		$altDevice=getSubDevice($mdID,$alternateCategory,$dbADO);
		if($altDevice!=0){
			deleteDevice($altDevice,$dbADO);
		}
	}
	if($dtID!=0){
		$newDevice=createDevice($dtID,$installationID,$mdID,$roomID,$dbADO,1);
		if($needConfigure==1 && $newDevice>0){
			$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$newDevice);
		}
	}
}

function getPVRCards($pvrArray,$instalationID,$mdID,$parentID,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaDirectors.lang.php');
	
	$filterParent=($parentID!=0)?'OR FK_Device_ControlledVia='.$parentID:'';

	$res=$dbADO->Execute('SELECT PK_Device,Description FROM Device WHERE FK_Installation=? AND (FK_Device_ControlledVia=? '.$filterParent.') AND FK_DeviceTemplate IN ('.join(',',$pvrArray).')',array($instalationID,$mdID));
	$pvrCards=array();
	if($res->RecordCount()==0){
		return '<div align="left">'.translate('TEXT_NO_PVR_CARDS_CONST').'</div>';
	}
	while($row=$res->fetchRow()){
		$pvrCards[$row['PK_Device']]=$row['Description'];
	}

	$out='<table cellpadding="2" cellspacing="0" align="right" width="100%">';
	$pos=0;
	foreach ($pvrCards AS $pid=>$description){
		$pos++;
		$color=($pos%2!=0)?'#F0F3F8':'#FFFFFF';
		$out.='
			<tr bgcolor="'.$color.'">
				<td>'.$description.'</td>
				<td align="right"><input type="button" class="button" name="del_pvr" value="'.translate('TEXT_DELETE_CONST').'" onClick="if(confirm(\''.translate('TEXT_CONFIRM_DELETE_PVR_CARD_CONST').'\'))self.location=\'index.php?section=mediaDirectors&action=delCard&delCard='.$pid.'\';"></td>
			</tr>';
	}
	$out.='</table>';
	
	return $out;
}

function isDiskless($deviceID,$deviceData){
	foreach ($deviceData AS $pos=>$ddArray){
		if($ddArray['PK_Device']==$deviceID && $ddArray['FK_DeviceData']==$GLOBALS['DisklessBoot'] && $ddArray['IK_DeviceData']==1){
			return true;
		}
	}
	
	return false;
}

function setUI($orbiterID,$ui,$dbADO){
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$openGL=substr($ui,0,1);
	$alphaBlended=substr($ui,1,1);
	$PK_UI=substr($ui,2,1);
	
	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?",array($openGL,$orbiterID,$GLOBALS['UseOpenGLeffects']));
	$changedGL=$dbADO->Affected_Rows();
	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?",array($alphaBlended,$orbiterID,$GLOBALS['UsealphablendedUI']));
	$changedAB=$dbADO->Affected_Rows();
	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?",array($PK_UI,$orbiterID,$GLOBALS['DD_PK_UI']));
	$changedUI=$dbADO->Affected_Rows();
	
	if($changedGL>0 || $changedAB>0 || $changedUI>0){
		// full regen the orbiter
		$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$orbiterID.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$orbiterID.' 21 "-r"';
		exec_batch_command($commandToSend);
		
		// call the script who will restart X
		restartX($orbiterID,getTopLevelParentIP($orbiterID,$dbADO));
	}
}
?>

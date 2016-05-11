<?php
function wizardOrbiters($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('wizardOrbiters.lang.php');
	includeLangFile('editDeviceParams.lang.php');

	// multipage display variables
	$orbitersPerPage=5;
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	global $dbPlutoMainDatabase,$excludedData;
	/* @var $dbADO ADOConnection */
	/* @var $resDevice ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootOrbiterID'];

	$orbitersDTArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO,1);
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID, 'ORDER BY Description ASC');

	if(isset($_REQUEST['lastAdded']) && !isset($_REQUEST['page'])){
		$newOrbiterAlert='alert("'.translate('TEXT_ADVANCED_PREPARATION_NOTE_CONST').'");';
	}
	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		'.@$newOrbiterAlert.'

	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.@stripslashes($_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="devices">

	<script>
	 	var frmvalidator = new formValidator("devices");
	</script>

	<input type="hidden" name="section" value="wizardOrbiters">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="page" value="'.$page.'">

	'.translate('TEXT_DOWNLOAD_CONST').' <a href="index.php?section=orbitersWin">'.translate('TEXT_ORBITER_WIN_INSTALLER_CONST').'</a><br>
	'.translate('TEXT_PRIVACY_SETTINGS_CONST').': <a href="index.php?section=usersOrbiters">'.translate('TEXT_RESTRICT_ACCES_TO_USERS_CONST').'</a>
		<div id="preloader" style="display:;">
			<table width="100%" border="0">
				<tr>
					<td align="center">'.translate('TEXT_ORBITERS_PAGE_LOADING_CONST').'</td>
				</tr>
			</table>
		</div>

		<div id="content" style="display:none;">
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">
					<input type="submit" class="button" name="QuickRegenAll" value="'.translate('TEXT_QUICK_REGEN_ALL_CONST').'">
					<input type="submit" class="button" name="FullRegenAll" value="'.translate('TEXT_FULL_REGEN_ALL_CONST').'">
					<input type="checkbox" name="reset_all" value="1"> '.translate('TEXT_RESET_ROUTER_WHEN_DONE_REGENERATING_CONST').'</td>
			</tr>';



		// Media Sort Option - Golgoj4
		//  From pluto_media - MediaType_AtrributeType
		//  string sSQL = "EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType) + " and MediaSortOption is not null order by MediaSortOption";
		$vArray=array("Recently Used" => -1, "Filename" => "0", "Title" => 13, "Performer"=>2, "Genre"=>8, "Director"=>1, "Program"=>12);
		$sortHandle=$dbADO->Execute('select EK_AttributeType_DefaultSort from MediaType WHERE PK_MediaType=5');
		if ($sortHandle) {
			$sortRow=$sortHandle->FetchRow();
			$curOption=$sortRow['EK_AttributeType_DefaultSort'];
		}
		$out.='
		<tr colspan="2">
		<td  align="left"><legend><b>'.translate('TEXT_VIDEO_CONST').'</b></legend>
		<select name ="VideosortOption">';
		foreach ($vArray as $Option => $val )
		{
			$out.='
                        <option value="'.$val.'"';
                        if ($val == $curOption) {
                                $out .= " selected";
                        }
                        $out .= '>'.$Option.'</option>';
		};

		$out.='
		<input type="submit" class="button" name="setDefaultSortVideo" value="'.translate('TEXT_DEFAULT_SORT_CONST').'">
		</td>';

		$asortArray = array("Recently Used" => -1, "Filename" => "0", "Title" => 13, "Performer"=>2, "Genre"=>8, "Album"=> 3, "Album Artist"=>57);
		$sortHandle=$dbADO->Execute('select EK_AttributeType_DefaultSort from MediaType WHERE PK_MediaType=4');
                if ($sortHandle) {
                        $sortRow=$sortHandle->FetchRow();
                        $curOption=$sortRow['EK_AttributeType_DefaultSort'];
                }
		$out.='
		<td  align="right"><legend><b>'.translate('TEXT_AUDIO_CONST').'</b></legend>
		<select name ="AudiosortOption">';

		foreach ($asortArray as $aOption => $aval )
		{
			$out.='
                        <option value="'.$aval.'"';
                        if ($aval == $curOption) {
                                $out .= " selected";
                        }
                        $out .= '>'.$aOption.'</option>';
		};

		$out.='
		<input type="submit" class="button" name="setDefaultSortAudio" value="'.translate('TEXT_DEFAULT_SORT_CONST').'">
		</td>
		</tr>';



			$displayedDevices=array();
			$DeviceDataToDisplay=array();
			$DeviceDataDescriptionToDisplay=array();

			$orbiterDD=array();
			// WARNING: hard-coded values
			$orbiterDD[]=3;			// default user
			$orbiterDD[]=20;		// No effects
			$orbiterDD[]=21;		// Main
			$orbiterDD[]=22;		// Sleeping menu
			$orbiterDD[]=23;		// Screen saver menu
			$orbiterDD[]=24;		// Skin
			$orbiterDD[]=25;		// Size
			$orbiterDD[]=26;		// Language
			$orbiterDD[]=56;		// Timeout
			$orbiterDD[]=84;		// Leave Monitor on for OSD
			$orbiterDD[]=91;		// Main Menu
			$orbiterDD[]=104;		// UI
			$orbiterDD[]=111;		// Using Infrared
			$orbiterDD[]=150;		// Spacing - Reduce image by
			$orbiterDD[]=169;		// Use alpha blended UI
			$orbiterDD[]=172;		// Use Open GL effects
			$orbiterDD[]=282;		// Queue instead of Instant Play

			$excludedData['standard_roaming_orbiters']=array('state',84,150,169,172);
			$excludedData['mobile_orbiters']=array(84,20,'room','wifi',56,150,169,172);
			$excludedData['on_screen_orbiters']=array('dt','ip_mac','wifi','state',25,169,172);

			$queryData='
					SELECT
						IF(FK_DeviceCategory=2,\'mobile_orbiters\',IF(Device.FK_DeviceTemplate=62,\'on_screen_orbiters\',\'standard_roaming_orbiters\')) AS OrbiterGroup,
						Device.*,
						DeviceTemplate.Description AS TemplateName,
						DeviceCategory.Description AS CategoryName,
						FK_DeviceCategory,
						Manufacturer.Description AS ManufacturerName,
						IsIPBased,
						RegenInProgress,RegenStatus,RegenPercent,
						DeviceData.Description AS ddDescription,
						ParameterType.Description AS typeParam,
						Device_DeviceData.IK_DeviceData,
						Device_DeviceData.FK_DeviceData,
						ShowInWizard,
						ShortDescription,
						AllowedToModify,
						DeviceTemplate_DeviceData.Description AS Tooltip
					FROM Device
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
					LEFT JOIN Orbiter ON PK_Orbiter=PK_Device
					INNER JOIN Device_DeviceData ON Device_DeviceData.FK_Device=PK_Device
					INNER JOIN DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData
					INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType
					LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
					WHERE Device.FK_DeviceTemplate IN ('.join(',',array_keys($orbitersDTArray)).') AND Device_DeviceData.FK_DeviceData IN ('.join(',',$orbiterDD).') AND FK_Installation=?
					ORDER BY OrbiterGroup ASC, Device.Description ASC';

			$resDevice=$dbADO->Execute($queryData,$installationID);

			$orbiterGroupDisplayed='';
			$orbiterDisplayed='';
			$PingTest=0;
			$isOSD=0;
			$regenArray=array();
			$content=array();
			$properties=array();
			$pos=0;

			while($rowD=$resDevice->FetchRow()){
				if(!in_array($rowD['PK_Device'],$displayedDevices)){
					$displayedDevices[]=$rowD['PK_Device'];
				}
			}
			// multipage settings
			$noRecords=count($displayedDevices);
			$noPages=ceil($noRecords/$orbitersPerPage);
			$linkPages=array();
			$cleanUrl='index.php?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']);
			for($i=1;$i<$noPages+1;$i++){
				$linkPages[]=($i==$page)?'<span class="normal_row">['.$i.']</span>':'<a class="red_link" href="'.$cleanUrl.'&page='.$i.'">'.$i.'</a>';
			}
			$linksBar=(($page>1)?'<a href="'.$cleanUrl.'&page='.($page-1).'">&lt;&lt;</a> ':'');
			$linksBar.=join(' ',$linkPages);
			$linksBar.=($page!=$noPages && $noRecords!=0)?' <a href="'.$cleanUrl.'&page='.($page+1).'">&gt;&gt;</a>':'';

			$resDevice->MoveFirst();
			$lastDevice=0;
			$devicesOnPage=array();
			while($rowD=$resDevice->FetchRow()){
				if($lastDevice!=$rowD['PK_Device']){
					$lastDevice=$rowD['PK_Device'];

					$pos++;
				}
				if($pos>($page-1)*$orbitersPerPage && $pos<=$page*$orbitersPerPage){
					$orbiterGroupDisplayed=$rowD['OrbiterGroup'];

					$itemDisabled=((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'');
					$out.=($itemDisabled=='disabled')?'<input type="hidden" name="isDisabled_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="1">':'';

					if(!in_array($rowD['PK_Device'],$devicesOnPage)){
						$devicesOnPage[]=$rowD['PK_Device'];
					}

					if(!in_array($rowD['FK_DeviceData'],$DeviceDataToDisplay)){
						$DeviceDataToDisplay[]=$rowD['FK_DeviceData'];
					}

					$PingTest=$rowD['PingTest'];
					$regenArray[$rowD['PK_Device']]['regen']=(int)$rowD['RegenInProgress'];
					$regenArray[$rowD['PK_Device']]['status']=$rowD['RegenStatus'];
					$regenArray[$rowD['PK_Device']]['percent']=$rowD['RegenPercent'];

					if($rowD['FK_DeviceData']==84 && @$ddValue==1){
						$isOSD=1;
					}


					if(!in_array('dt',$excludedData[$orbiterGroupDisplayed])){
						$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['dt']='
								<tr>
									<td align="right" width="200"><B>'.translate('TEXT_DEVICE_TEMPLATE_CONST').'</B></td>
									<td align="left" width="400" title="'.translate('TEXT_DEVICE_CATEGORY_CONST').': '.$rowD['CategoryName'].', '.strtolower(translate('TEXT_MANUFACTURER_CONST')).': '.$rowD['ManufacturerName'].'">'.$rowD['TemplateName'].'</td>
								</tr>
								';
					}

					if(!in_array('state',$excludedData[$orbiterGroupDisplayed])){
						$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['state']='
								<tr>
									<td align="right"><B>'.translate('TEXT_STATE_CONST').'</B></td>
									<td>'.getStateFormElement($rowD['PK_Device'],'State_'.$rowD['PK_Device'],$rowD['State'],$dbADO).'</td>
								</tr>
								';
					}

					if(!in_array('description',$excludedData[$orbiterGroupDisplayed])){
						$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['description']='
								<tr>
									<td align="right"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
									<td align="left"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.stripslashes($rowD['Description']).'" style="width:200px;"> # '.$rowD['PK_Device'].'</td>
								</tr>';
					}

					if($rowD['IsIPBased']==1 && !in_array('ip_mac',$excludedData[$orbiterGroupDisplayed])){
						$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['ip_mac']='
									<tr>
										<td align="right"><B>'.translate('TEXT_IP_ADDRESS_CONST').'</B></td>
										<td><input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'" style="width:200px;"></td>
									</tr>
									<tr>
										<td align="right"><B>'.translate('TEXT_MAC_ADDRESS_CONST').'</B></td>
										<td><input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'" style="width:200px;"></td>
									</tr>';
					}

					if(!in_array('room',$excludedData[$orbiterGroupDisplayed])){
						$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['room']='
								<tr>
									<td align="right"><B>'.translate('TEXT_ROOM_CONST').'</B></td>
									<td>'.pulldownFromArray($roomsArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room'],'style="width:200px;"').'</td>
								</tr>
								';
					}

					if(!in_array($rowD['FK_DeviceData'],$excludedData[$orbiterGroupDisplayed])){
						@$content[$orbiterGroupDisplayed][$rowD['PK_Device']]['dd'].=formatDDRows($rowD,$dbADO);
					}


					$properties[$rowD['PK_Device']]['regenArray']=$regenArray[$rowD['PK_Device']];
					$properties[$rowD['PK_Device']]['PingTest']=$PingTest;
					$properties[$rowD['PK_Device']]['isOSD']=$isOSD;
				}

			}

			$content['mobile_orbiters']=(@$content['mobile_orbiters']=='')?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['mobile_orbiters'];
			$content['standard_roaming_orbiters']=(@$content['standard_roaming_orbiters']=='')?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['standard_roaming_orbiters'];
			$content['on_screen_orbiters']=(@$content['on_screen_orbiters']=='')?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['on_screen_orbiters'];
			$out.='
				<tr>
					<td colspan="2" align="right">'.$linksBar.'</td>
				</tr>
				<tr>
					<td class="tablehead" colspan="2" align="center"><B>'.translate('TEXT_MOBILE_PHONE_ORBITERS_CONST').'</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center">'.orbiterTable($content['mobile_orbiters'],'mobile_orbiters',$properties).'</td>
				</tr>
				<tr>
					<td class="tablehead" colspan="2" align="center"><B>'.translate('TEXT_ON_SCREEN_DISPLAYS_FOR_MEDIA_DIRECTORS_CONST').'</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center">'.orbiterTable($content['on_screen_orbiters'],'on_screen_orbiters',$properties).'</td>
				</tr>
				<tr>
					<td class="tablehead" colspan="2" align="center"><B>'.translate('TEXT_STANDARD_ROAMING_ORBITERS_CONST').'</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center">'.orbiterTable($content['standard_roaming_orbiters'],'standard_roaming_orbiters',$properties).'</td>
				</tr>
				<tr>
					<td colspan="2" align="right">'.$linksBar.'</td>
				</tr>
			';


			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
				<input type="hidden" name="devicesOnPage" value="'.join(',',$devicesOnPage).'">';
			$out.='
			</table>
			<table align="center" border="0">
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2" align="center">'.pulldownFromArray($orbitersDTArray,'deviceTemplate',0).'
					<input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_ORBITER_CONST').'"  ></td>
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
			header("Location: index.php?section=wizardOrbiters&error=translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST')");
			exit(0);
		}

		if(isset($_POST['QuickRegenAll'])){
			$ResetRouter=((int)@$_POST['reset_all']==1)?' 24 1':'';
			setOrbitersNeedConfigure($installationID,$dbADO);

			$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template 0 '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 0 21 "-a"'.$ResetRouter;
			exec($commandToSend);
			$regen='Q_ALL';
		}
		if(isset($_POST['FullRegenAll'])){
			$ResetRouter=((int)@$_POST['reset_all']==1)?' 24 1':'';
			setOrbitersNeedConfigure($installationID,$dbADO);

			$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template 0 '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 0 21 "-r"'.$ResetRouter;
			exec($commandToSend);
			$regen='F_ALL';
		}



		// Added in query to change default sort - Golgoj4
		if(isset($_POST['setDefaultSortVideo']))
		{
			if ($_POST['VideosortOption'] == 0)
			{
				$sortVal = 'null';;
			}
			else
			{
				$sortVal =$_POST['VideosortOption'];
			}
			$dbADO->Execute('UPDATE MediaType SET EK_AttributeType_DefaultSort=? WHERE PK_MediaType=5 OR PK_MediaType=3',$sortVal);
			header("Location: index.php?section=wizardOrbiters&msg='Please Regen and Reload for changes to take effect'");
			die;
		}

		if(isset($_POST['setDefaultSortAudio']))
		{
			if ($_POST['AudiosortOption'] == 0)
			{
				$AudiosortVal = 'null';;
			}
			else
			{
				$AudiosortVal =$_POST['AudiosortOption'];
			}

			$dbADO->Execute('UPDATE MediaType SET EK_AttributeType_DefaultSort=? WHERE PK_MediaType=4 or PK_MediaType=2',$AudiosortVal);

			header("Location: index.php?section=wizardOrbiters&msg='Please Regen and Reload for changes to take effect'");
			die;
		}

		if(isset($_POST['update_qOrbiter_skins']))
		{
			//added script to update qorbiter skins. Alblasco1702
			exec_batch_command('sudo -u root /var/www/lmce-admin/skins/updateQorbiterSkins.sh');
		}

		// only devices on page are used
		$displayedDevicesArray=explode(',',$_POST['devicesOnPage']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
			}
			if(isset($_POST['quickRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value);
				$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$value);
				$updateOrbiters=true;
				$ResetRouter=((int)@$_POST['reset_'.$value]==1)?' 24 1':'';

				$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$value.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$value.' 21 "-a" '.$ResetRouter;
				exec($commandToSend,$tmp);
				$regen='Q_'.$value;
			}
			if(isset($_POST['fullRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value);
				$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$value);
				$updateOrbiters=true;
				$ResetRouter=((int)@$_POST['reset_'.$value]==1)?' 24 1':'';

				$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$value.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$value.' 21 "-r"'.$ResetRouter;
				exec($commandToSend);
				$regen='F_'.$value;
			}
		}

		if(isset($_POST['update']) || isset($updateOrbiters) || $action=='externalSubmit'){

			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$deviceTemplate=(int)@$_POST['deviceTemplate_'.$value];
				$description=@$_POST['description_'.$value];
				if(isset($_POST['ip_'.$value])){
					$ip=$_POST['ip_'.$value];
					$mac=$_POST['mac_'.$value];
					$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
				}else{
					$updateMacIp='';
				}
				$updateQuery='';

				if(isset($_POST['room_'.$value])){
					$room=((int)@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:'NULL';
					$updateQuery.=',FK_Room='.$room.'';
				}

				if(isset($_POST['PingTest_'.$value])){
					$pingTest=(int)@$_POST['PingTest_'.$value];
					$updateQuery.=',PingTest='.$pingTest;
				}

				$State= (isset($_POST['State_'.$value]))?cleanString($_POST['State_'.$value]):getStateValue('State_'.$value);
				$updateQuery.=',State=\''.$State.'\'';

				$updateDevice='UPDATE Device SET Description=? '.$updateQuery.' '.@$updateMacIp.' WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($description,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					if($ddValue!=91){
						if($ddValue!=56){
							$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:NULL;
						}else{
							$deviceData=(isset($_POST['timeoutSS_'.$value]))?$_POST['timeoutSS_'.$value].','.$_POST['timeoutPO_'.$value]:NULL;
						}
						$oldDeviceData=@$_POST['oldDeviceData_'.$value.'_'.$ddValue];
						$isDisabled=(int)@$_POST['isDisabled_'.$value.'_'.$ddValue];

						if($oldDeviceData!=$deviceData && $isDisabled!=1){
							$updateDDD='
								UPDATE Device_DeviceData
									SET IK_DeviceData=?
								WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($updateDDD,array($deviceData,$value,$ddValue));

							// if it's device data for skin and the value is changed, set orbiter needConfigure
							if($ddValue==$GLOBALS['Skin'] && $dbADO->Affected_Rows()>0){
								$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$value);
							}


							if(($ddValue==$GLOBALS['UsealphablendedUI'] || $ddValue==$GLOBALS['UseOpenGLeffects']) && $dbADO->Affected_Rows()>0){
								restartX($value,getTopLevelParentIP($value,$dbADO));
							}


							if($ddValue==$GLOBALS['Size'] && (int)$deviceData!=0){
								$sizeArray=getFieldsAsArray('Size','Width,Height',$dbADO,'WHERE PK_Size='.$deviceData);
								if(count($sizeArray)>0){
									$resX=$sizeArray['Width'][0];
									$resY=$sizeArray['Height'][0];
									$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resX,$value,$GLOBALS['ScreenWidth']));
									$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resY,$value,$GLOBALS['ScreenHeight']));
								}
							}
						}
					}
				}

				// main menu
				$mainMenu=((int)@$_POST['mainMenu_'.$value]>0)?(int)$_POST['mainMenu_'.$value]:'';
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($mainMenu,$value,91));
			}
		}

		if(isset($_POST['add'])){
			$deviceTemplate=(int)$_POST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID.' -C 0';
				//$insertID=exec($cmd);
				$insertID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);
				$suffix='&lastAdded='.$insertID;

				// full regen
				$regenCmd='/usr/pluto/bin/MessageSend localhost -targetType template '.$insertID.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$insertID.' 21 "-r"';
				exec($regenCmd);

				$regen='F_ALL';
			}
		}

		$commandMessage=(isset($commandToSend))?'<br>'.translate('TEXT_ORBITER_COMMAND_SENT_CONST').': '.$commandToSend:'';
		header("Location: index.php?section=wizardOrbiters&page=".$_REQUEST['page']."&msg=".urlencode(translate('TEXT_ORBITERS_UPDATED_CONST').$commandMessage)."&regen=".@$regen.@$suffix);
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_ORBITES_CONST'));

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_ORBITES_CONST')=>'index.php?section=wizardOrbiters'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_ORBITES_CONST'));
	$output->output();
}


// format and return device data rows
function formatDDRows($rowD,$dbADO)
{
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('wizardOrbiters.lang.php');

	$ddHTML='';
	$selectedMenu=0;
	$jsValidation='';
	$ddValue=$rowD['IK_DeviceData'];
	if((@$rowD['ShowInWizard']==1 || @$rowD['ShowInWizard']=='') && $rowD['FK_DeviceData']!=91){
		$formElementName='deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'];
		if($rowD['FK_DeviceData']!=56){
			$ddHTML.='
				<tr>
					<td align="right" title="'.@$rowD['Tooltip'].'"><b>'.((@$rowD['ShortDescription']!='')?$rowD['ShortDescription']:str_replace('PK_','',$rowD['ddDescription'])).'</b></td>
					<td align="left">';
			switch($rowD['typeParam']){
				case 'int':
				if(in_array($rowD['ddDescription'],$GLOBALS['DeviceDataLinkedToTables']))
				{
					$tableName=str_replace('PK_','',$rowD['ddDescription']);
					if(!isset($GLOBALS['ddTableArray_'.$tableName])){
						if($tableName!='Users')
							$GLOBALS['ddTableArray_'.$tableName]=getAssocArray($tableName,$rowD['ddDescription'],'Description',$dbADO,'','ORDER BY Description ASC');
						else
							$GLOBALS['ddTableArray_'.$tableName]=getAssocArray('Users','PK_Users','UserName',$dbADO,'','ORDER BY UserName ASC');
					}

					$ddHTML.=pulldownFromArray($GLOBALS['ddTableArray_'.$tableName],'deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'],$ddValue,((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').' style="width:200px;"'.(($rowD['FK_DeviceData']==24)?'onChange="document.wizardOrbiters.action.value=\'form\';document.wizardOrbiters.submit();"':''),'key','- Please select -');
					//Update button qorbiter skins. Alblasco1702
					if ( $rowD['TemplateName'] == 'qOrbiter' && $rowD['FK_DeviceData']==24) {
							$ddHTML.= '&nbsp;<input class="button" name="update_'.$rowD['TemplateName'].'_skins" value="update&nbsp;'.$rowD['TemplateName'].'" type="submit" />';
							}
				}
				else
					$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').' style="width:200px;">';
				$jsValidation.="frmvalidator.addValidation(\"$formElementName\",\"numeric\",\"".translate('TEXT_WARNING_NUMERICAL_ONLY_CONST')."\");\n";
				break;
				case 'bool':
					$ddHTML.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').' style="width:200px;">';
				break;
				default:
					$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').' style="width:200px;">';
			}
			if($rowD['FK_DeviceData']==172){
				$ddHTML.=' If checked you need to reload the router';
			}
			$ddHTML.='</td>
				</tr>';
		}
		else{
			$parts=explode(',',$ddValue);
			$ddHTML.='
					<tr>
						<td align="right"><B>'.translate('TEXT_SECONDS_BEFORE_SCREEN_SAVER_CONST').'</B></td>
						<td><input type="text" name="timeoutSS_'.$rowD['PK_Device'].'" value="'.@$parts[0].'" style="width:200px;"></td>
					</tr>
						<tr>
						<td align="right"><B>'.translate('TEXT_SECONDS_BEFORE_POWER_OFF_CONST').'</B></td>
						<td><input type="text" name="timeoutPO_'.$rowD['PK_Device'].'" value="'.@$parts[1].'" style="width:200px;"></td>
					</tr>							';
		}
		$ddHTML.='
					<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.((!is_null($rowD['IK_DeviceData']))?$ddValue:'NULL').'">';

		unset($ddValue);
	}

	return $ddHTML.'<script>'.$jsValidation.'</script>';
}

function displayButtons($orbiter,$RegenInProgress){
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('wizardOrbiters.lang.php');

	$out='';

	$out.='
		<tr>
			<td align="right"><input type="checkbox" name="reset_'.$orbiter.'" value="1"></td>
			<td>'.translate('TEXT_RESET_ROUTER_WHEN_DONE_REGENERATING_CONST').'</td>
		</tr>';
	if(@$RegenInProgress==1){
		$out.='
			<tr>
				<td colspan="2" align="center"><iframe src="index.php?section=orbiterRegenInProgress&orbiterID='.$orbiter.'" style="width:100%;height:80px;border:0;"></iframe></td>
			</tr>
		';
	}

	$out.='
			<tr>
				<td align="center" colspan="2">
					<input type="submit" class="button" name="quickRegen_'.$orbiter.'" value="'.translate('TEXT_QUICK_REGEN_CONST').'"  >&nbsp;&nbsp;
					<input type="submit" class="button" name="fullRegen_'.$orbiter.'" value="'.translate('TEXT_FULL_REGEN_CONST').'"  >&nbsp;&nbsp;
					<input type="button" class="button" name="customBg" value="'.translate('TEXT_CUSTOM_BACKGROUND_CONST').'"  onclick="windowOpen(\'index.php?section=customBackground&oID='.$orbiter.'\',\'width=600,height=400,scrollbars=1,resizable=1\');">&nbsp;&nbsp;
					<input type="submit" class="button" name="update" value="'.translate('TEXT_UPDATE_CONST').'"  >&nbsp;&nbsp;
					<input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'">&nbsp;&nbsp;
					<input type="button" class="button" name="edit_'.$orbiter.'" value="'.translate('TEXT_ADVANCED_CONST').'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$orbiter.'\';">&nbsp;&nbsp;
					<input type="submit" class="button" name="delete_'.$orbiter.'" value="'.translate('TEXT_DELETE_CONST').'"  onclick="if(!confirm(\''.translate('TEXT_CONFIRM_DELETE_ORBITER_CONST').'\'))return false;">
				</td>
			</tr>
	';
	return $out;
}

function displayWiFiRow($orbiter,$isOSD,$PingTest){
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('wizardOrbiters.lang.php');

	$out='';

	if($isOSD==0){
		$out.='
		<tr>
			<td align="right"><B>'.translate('TEXT_WIFI_CONNECTION_CONST').'</B></td>
			<td><input type="checkbox" name="PingTest_'.$orbiter.'" value="1" '.(($PingTest==1)?'checked':'').'></td>
		</tr>';
	}

	return $out;
}

function orbiterTable($content,$orbiterGroupDisplayed,$properties){

	global $excludedData;
	$out='';
	$pos=0;
	if(!is_array($content)){
		return $out;
	}
	foreach ($content AS $orbiter=>$valueArray){
		$pos++;
		$out.='<table width="100%" bgcolor="#DEDEDE" border="0">
			<tr>';
			foreach ($valueArray AS $row){
				$out.=$row;
			}
			$isOSD=$properties[$orbiter]['isOSD'];
			$PingTest=$properties[$orbiter]['PingTest'];
			$regenArray=$properties[$orbiter]['regenArray'];
			if(!in_array('wifi',$excludedData[$orbiterGroupDisplayed])){
				$out.=displayWiFiRow($orbiter,$PingTest,$isOSD);
			}

			$regenQueued=substr(@$_REQUEST['regen'],2);
			$regenBox=($regenQueued==$orbiter || $regenQueued=='ALL' || $regenArray['regen']==1)?1:0;
			$out.=displayButtons($orbiter,$regenBox);
		$out.='</tr>
		</table>
		<br>';
	}


	return $out;
}
?>

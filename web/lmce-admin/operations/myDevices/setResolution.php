<?php
function setResolution($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/setResolution.lang.php');
	
	return('Obsolete');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$mdID=(int)$_REQUEST['mdID'];
	$installationID = (int)@$_SESSION['installationID'];
	$videoSettingsArray=array(
		'M'=>'Manual',
		'640 480'=>'640x480',
		'800 600'=>'800x600',
		'1024 768'=>'1024x768',
		'1280 1024'=>'1280x1024',
		'1600 1200'=>'1600x1200',
		'848 480 p'=>'480p (848x480 progressive)',
		'1280 720 p'=>'720p (1280x720 progressive)',
		'1280 800'=>'1280x800',
		'1920 1080 i'=>'1080i (1920x1080 interlaced)',
		'1920 1080 p'=>'1080p (1920x1080 progressive)',
	);
	$refreshArray=array(
		'50'=>'50 Hz',
		'60'=>'60 Hz',
		'65'=>'65 Hz',
		'72'=>'72 Hz',
		'75'=>'75 Hz',
		'80'=>'80 Hz',
		'85'=>'85 Hz',
		'100'=>'100 Hz',
		'120'=>'120 Hz',
	);
	
	$connectorsArray=array(
		'VGA'=>'VGA',
		'DVI'=>'DVI/HDMI',
		'Component'=>'Component',
		'S-Video'=>'S-Video',
		'Composite'=>'Composite'
	);
	$tvStandardArray=array(
		'NTSC'=>'NTSC',
		'Pal'=>'Pal',
		'Secam'=>'Secam'
	);
	
	
	$oldValues=getFieldsAsArray('Device_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$mdID.' AND FK_DeviceData='.$GLOBALS['VideoSettings']);
	if(!is_null(@$oldValues['IK_DeviceData'])){
		$oldDD=explode('/',$oldValues['IK_DeviceData'][0]);
		$oldResolution=@$oldDD[0];
		$oldRefresh=@$oldDD[1];
	}
	$mdDetails=getFieldsAsArray('Device','IPaddress,FK_Device_ControlledVia',$dbADO,'WHERE PK_Device='.$mdID);
	$ipAddress=(is_null($mdDetails['FK_Device_ControlledVia'][0]))?$mdDetails['IPaddress'][0]:'127.0.0.1';

	$connector=getDeviceData($mdID,$GLOBALS['DD_CONNECTOR'],$dbADO);
	$tvStandard=getDeviceData($mdID,$GLOBALS['DD_TV_STANDARD'],$dbADO);

	
	if ($action=='form') {

		$out.='

		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="setResolution">
		<input type="hidden" name="section" value="setResolution">
		<input type="hidden" name="action" value="set">
		<input type="hidden" name="mdID" value="'.$mdID.'">
		
<br><br>';



$out.='
<table>
	<tr>
		<td><B>'.$TEXT_RESOLUTION_CONST.' *</B></td>
		<td>'.pulldownFromArray($videoSettingsArray,'resolution',@$oldResolution).'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_REFRESH_CONST.' *</B></td>
		<td>'.pulldownFromArray($refreshArray,'refresh',@$oldRefresh).'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_CONNECTOR_CONST.' *</B></td>
		<td>'.pulldownFromArray(@$connectorsArray,'connector',@$connector).'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_TV_STANDARD_CONST.' *</B></td>
		<td>'.pulldownFromArray(@$tvStandardArray,'tvstandard',@$tvStandard).'</td>
	</tr>
	

		<tr>
			<td><b>'.$TEXT_SET_RESOLUTION_QUICKRELOAD_ROUTER_CONST.'</b></td>
			<td align="right"><input type="checkbox" name="updateOrbiters" value="1" checked></td>
		</tr>
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="test" value="'.$TEXT_APPLY_CONST.'"></td>
	</tr>
</table>		
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
			var frmvalidator = new formValidator("setResolution");
 			frmvalidator.addValidation("resolution","dontselect=0","'.$TEXT_RESOLUTION_REQUIRED_CONST.'");
			frmvalidator.addValidation("refresh","dontselect=0","'.$TEXT_REFRESH_REQUIRED_CONST.'");
		</script>
		';
		
	}else {
	
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setResolution&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$resolution=$_POST['resolution'];
		$refresh=$_POST['refresh'];
		$newVS=$resolution.'/'.$refresh;
					
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newVS,$mdID,$GLOBALS['VideoSettings']));
		
		set_device_data($mdID,$GLOBALS['DD_CONNECTOR'],$_POST['connector'],$dbADO);
		set_device_data($mdID,$GLOBALS['DD_TV_STANDARD'],$_POST['tvstandard'],$dbADO);

		if(@$_POST['updateOrbiters']==1){
			// update device data for onscreen orbiter or orbiter
			$orbiterArray=getOSDFromMD($mdID,$dbADO);
			@list($resX, $resY, $resType) = explode(' ', $resolution);

			if(!is_null($orbiterArray['FK_DeviceTemplate'])){

				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resX,$orbiterArray['PK_Device'],$GLOBALS['ScreenWidth']));
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resY,$orbiterArray['PK_Device'],$GLOBALS['ScreenHeight']));

				$sizeFields=getFieldsAsArray('Size','PK_Size',$dbADO,'WHERE Width='.$resX,' AND Height='.$resY);

				if(isset($sizeFields['PK_Size'][0])){
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($sizeFields['PK_Size'][0],$orbiterArray['PK_Device'],$GLOBALS['Size']));
				}
			}

			// append orbiter ID to orbiter plugin status, to be regenerated
			$orbiterPlugIn=getInfraredPlugin($installationID,$dbADO);
			if(isset($orbiterArray['PK_Device'])){
				$dbADO->Execute('UPDATE Device SET Status=IF(Status IS NULL,?,concat(Status, ?)) WHERE PK_Device=?',array($orbiterArray['PK_Device'],','.$orbiterArray['PK_Device'],$orbiterPlugIn));
			}

			// do quick reload router
			$command='/usr/pluto/bin/MessageSend localhost 0 -1000 7 1';
			exec($command);

		}

		$msg=$TEXT_SET_RESOLUTION_SUCCESS_CONST;

		md_apply_resolution($mdID,$ipAddress);

		//full regen
		$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$orbiterArray['PK_Device'].' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$orbiterArray['PK_Device'].' 21 "-r" 24 1';				
		exec_batch_command($commandToSend);

		
		header("Location: index.php?section=setResolution&mdID=$mdID&msg=$msg");
	}
	
	$output->setMenuTitle('Media Directors |');
	$output->setPageTitle($TEXT_SET_RESOLUTION_TITLE_CONST);
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SET_RESOLUTION_TITLE_CONST);			
	$output->output();
}

function md_apply_resolution($mediaDirectorID,$ipAddress){
	$commandToSend="sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '$ipAddress' '/usr/pluto/bin/SetupAudioVideo.sh'";
	exec_batch_command($commandToSend);
}
?>

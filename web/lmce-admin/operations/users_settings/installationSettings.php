<?php
function installationSettings($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('installationSettings.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$currentTimeZone=str_replace('/usr/share/zoneinfo/','',exec('readlink /etc/localtime'));
	$ripFormats=array('mp3'=>'mp3','ogg'=>'ogg', 'flac'=>'flac', 'wav'=>'wav');
	$telecom_language=array('1'=>'English', '2'=>utf8_encode('French'), '3'=>'German', '11'=>'Portuguese (Brazilian)');
	$telecom_selectedLanguage='1'; // default 
	
	$installationID = cleanInteger($_SESSION['installationID']);
	$dceRouterID=getDeviceFromDT($installationID,$GLOBALS['rootDCERouter'],$dbADO);
	$ttsDeviceID=getDeviceFromDT($installationID,$GLOBALS['TTS'],$dbADO);
	
	if($dceRouterID!==null){
		$dceRouterDD=getDD($dceRouterID,$GLOBALS['Language'].','.$GLOBALS['PK_City'],$dbADO);
		$defLanguage=$dceRouterDD[$GLOBALS['Language']];
		$selectedCity=(int)$dceRouterDD[$GLOBALS['PK_City']];
		if($selectedCity>0){
			$cityData=getFieldsAsArray('City','FK_Region,FK_Country',$dbADO,'WHERE PK_City='.$selectedCity);
			$selectedRegion=$cityData['FK_Region'][0];
		}
	}

	if($ttsDeviceID!==null){
		$ttsDD=getDD($ttsDeviceID,$GLOBALS['Language'],$dbADO);
		$telecom_selectedLanguage=(int)$ttsDD[$GLOBALS['Language']];
		if($telecom_selectedLanguage < 1) $telecom_selectedLanguage = 1;
	}
	
	$eventPluginID=getDeviceFromDT($installationID,$GLOBALS['EventPlugIn'],$dbADO);
	if($eventPluginID!==null){
		$eventPluginDD=getDD($eventPluginID,$GLOBALS['Longitude'].','.$GLOBALS['Latitude'],$dbADO);
		$Longitude=$eventPluginDD[$GLOBALS['Longitude']];
		$Latitude=$eventPluginDD[$GLOBALS['Latitude']];
	}

	$mediaPluginID=getDeviceFromDT($installationID,$GLOBALS['rootMediaPlugin'],$dbADO);
	if($mediaPluginID!==null){
		$mediaPluginDD=getDD($mediaPluginID,$GLOBALS['RipFormat'],$dbADO);
		$selectedRipFormat=$mediaPluginDD[$GLOBALS['RipFormat']];
	}else{
		$selectedRipFormat='mp3;128;cbr';
	}
	
	$query = "
		SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version 
		WHERE 
		PK_Installation = ?
	";
	$selectInstallation = $dbADO->Execute($query,array($installationID));
	
	if ($selectInstallation) {
		$rowInstallation = $selectInstallation->FetchRow();			
	}
	$selectedCountry=(isset($_POST['countryID']))?(int)$_POST['countryID']:$rowInstallation['FK_Country'];	
	
	if ($action=='form') {		
		$out.=setLeftMenu($dbADO).'
		
		<script>
		function setOptions(){
			show_mp3=(document.installationSettings.rip.value=="mp3")?"":"none";
			show_ogg=(document.installationSettings.rip.value=="ogg")?"":"none";
			show_flac=(document.installationSettings.rip.value=="flac")?"":"none";
			show_wav=(document.installationSettings.rip.value=="wav")?"":"none";
		
			document.getElementById("mp3_opt").style.display=show_mp3;
			document.getElementById("ogg_opt").style.display=show_ogg;
			document.getElementById("flac_opt").style.display=show_flac;
			document.getElementById("wav_opt").style.display=show_wav;
		}
		
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}		
		</script>
		
		
		<form action="index.php" method="post" name="installationSettings">
		<input type="hidden" name="section" value="installationSettings">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="timeZoneText" value="update">
		<input type="hidden" name="old_telecom_language" value="'.$telecom_selectedLanguage.'">

		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<table width="600px">			
				<!-- LOCATION -->
				<tr>
					<td colspan="4" align="center" class="tablehead"><B>'.translate('TEXT_LOCATION_CONST').'</B>:</td>
				</tr>		
				<tr>
					<td width="100"><B>'.translate('TEXT_DESCRIPTION_CONST').'&nbsp;*</B></td>
					<td><input type="text" size="30" name="Description" value="'.((isset($_REQUEST['Description']))?$_REQUEST['Description']:$rowInstallation['Description']).'"></td>
				</tr>
				
				<tr>
					<td><B>'.translate('TEXT_NAME_CONST').'</B></td>
					<td><input type="text" size="30" name="Name" value="'.((isset($_REQUEST['Name']))?$_REQUEST['Name']:$rowInstallation['Name']).'"></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_ADDRESS_CONST').'</B></td>
					<td><input type="text" size="30" name="Address" value="'.((isset($_REQUEST['Address']))?$_REQUEST['Address']:$rowInstallation['Address']).'"></td>
				</tr>
				<tr>
					<td colspan="4">'.translate('TEXT_ADDRESS_INFO_CONST').'</td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_CITY_CONST').'</B></td>
					<td><input type="text" size="30" name="City" value="'.((isset($_REQUEST['City']))?$_REQUEST['City']:$rowInstallation['City']).'"></td>
					<td><B>'.translate('TEXT_STATE_CONST').'</B></td>
					<td><input type="text" size="30" name="State" value="'.((isset($_REQUEST['State']))?$_REQUEST['State']:$rowInstallation['State']).'"></td>
				</tr>
				<tr>
					<td colspan="4" class="tablehead" align="center"><B>'.translate('TEXT_LONGITUDE_CONST').'/'.translate('TEXT_LATITUDE_CONST').'</B></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_COUNTRY_CONST').'</B></td>
					<td>'.generatePullDown('countryID','Country','PK_Country','Description',$selectedCountry,$dbADO,'','onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();" style="width:230px;"').'</td>
				</tr>';
				if($selectedCountry>0){
					$selectedRegion=(isset($_POST['region']))?(int)$_POST['region']:(int)@$selectedRegion;	
					$regionsArray=getAssocArray('Region','PK_Region','Region',$dbADO,'WHERE FK_Country='.$selectedCountry);

					$out.='
					<tr>
						<td><B>'.translate('TEXT_REGION_CONST').'</B></td>
						<td>'.pulldownFromArray($regionsArray,'region',@$selectedRegion,'onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();" style="width:230px;"').'</td>
					</tr>';

					if($selectedRegion>0 && in_array($selectedRegion,array_keys($regionsArray))){
						$citiesArray=getCities($selectedRegion,$dbADO);
						$output->setScriptInHead(getCitiesCoordsArray($dbADO,' WHERE FK_Region='.$selectedRegion));
					$out.='
					<tr>
						<td><B>'.translate('TEXT_CITY_CONST').'</B></td>
						<td>'.pulldownFromArray($citiesArray,'city_coords',@$selectedCity,'onChange="setCoordinates()" style="width:230px;"').'</td>
					</tr>';
					
					}
				}
				$out.='		
				<tr>
					<td><B>'.translate('TEXT_LONGITUDE_CONST').'</B></td>
					<td><input type="text" size="30" name="Longitude" value="'.@$Longitude.'"></td>
					<td><B>'.translate('TEXT_LATITUDE_CONST').'</B></td>
					<td><input type="text" size="30" name="Latitude" value="'.@$Latitude.'"></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_ZIP_CODE_CONST').'</B></td>
					<td><input type="text" size="30" name="Zip" value="'.$rowInstallation['Zip'].'"></td>
				</tr>
				<tr>
					<td colspan="4" class="tablehead" align="center"><B>'.translate('TEXT_LANGUAGE_SETTINGS').'</B></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_LANGUAGE_ASTERISK').'</B></td>
					<td>'.pulldownFromArray($telecom_language,'telecom_language',$telecom_selectedLanguage,'').'</td>
				</tr>
				<tr>
					<td colspan="4" align="center" class="tablehead"><B>'.translate('TEXT_CURRENT_TIMEZONE_CONST').'</B></td>
				</tr>
				<tr>
					<td colspan="4" align="center" class="normal_row"><B>'.$currentTimeZone.'</B> <input type="button" class="button" name="setTimezone" value="'.translate('TEXT_SET_TIMEZONE_CONST').'" onclick="windowOpen(\'index.php?section=setTimezone\',\'width=640,height=480,toolbar=1,scrollbars=1,resizable=1\');"></td>
				</tr>
				<tr>
					<td colspan="4" align="center" class="tablehead"><B>'.translate('TEXT_LOGLEVELS_CONST').'</B></td>
				</tr>
				<tr>
					<td colspan="4" align="center" class="normal_Row"><B><input type="button" class="button" name="setTimezone" value="'.translate('TEXT_SET_LOGLEVELS_CONST').'" onclick="windowOpen(\'index.php?section=setLoglevels\',\'width=640,height=,toolbar=1,scrollbars=1,resizable=1\');"></td>
				';
			
		$pulldownValue=(strpos($selectedRipFormat,';')!==false)?substr($selectedRipFormat,0,strpos($selectedRipFormat,';')):$selectedRipFormat;
		$out.='
				<tr>
					<td colspan="4" align="center" class="tablehead"><B>'.translate('TEXT_MISCELANEOUS_CONST').'</B>:</td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_RIPPING_FORMAT_CONST').': </B></td>
					<td>'.pulldownFromArray($ripFormats,'rip',$pulldownValue,'onChange="setOptions();" style="width:180px;"').'</td>
				</tr>				
				'.rippingSettings($selectedRipFormat).'
				<tr>
					<td colspan="4" align="center"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  > <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
				</tr>
			</table>
			<em>* '.translate('TEXT_REQUIRED_FIELDS_CONST').'</em>
		</form>
		
		<script>
		 	var frmvalidator = new formValidator("installationSettings");
 			frmvalidator.addValidation("Description","req","'.translate('TEXT_INSTALLATION_DESCRIPTION_REQUIRED_CONST').'");
		</script>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=installationSettings&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		

		$description = cleanString(@$_POST['Description'],50);
		$name = cleanString(@$_POST['Name'],50);
		$address = cleanString(@$_POST['Address'],50);
		$city = cleanString(@$_POST['City'],50);
		$state = cleanString(@$_POST['State'],50);
		$zip = cleanString(@$_POST['Zip'],50);
		$country=((int)$_POST['countryID']!=0)?(int)$_POST['countryID']:NULL;
		$newLanguage=(int)@$_POST['newLanguage'];
		
		$city_coords=(int)@$_POST['city_coords'];
		$Longitude=$_POST['Longitude'];
		$Latitude=$_POST['Latitude'];
		
		$old_tts_Language=(int)@$_POST['old_telecom_language'];
		$tts_Language=((int)$_POST['telecom_language']!=0)?(int)$_POST['telecom_language']:1;
		$updateDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
		
		$err='';
		if($dceRouterID!==null){
			$dbADO->Execute($updateDD,array($city_coords,$dceRouterID,$GLOBALS['PK_City']));
		}else{
			$err.=translate('TEXT_ERROR_UNABLE_TO_FIND_DCE_ROUTER_CONST').'<br>';
		}	
		
		if($ttsDeviceID!==null){
			$dbADO->Execute($updateDD,array($tts_Language,$ttsDeviceID,$GLOBALS['Language']));
		}else{
			$err.=translate('TEXT_ERROR_UNABLE_TO_FIND_TTS_CONST').'<br>';
		}

		if($eventPluginID!==null){
			$dbADO->Execute($updateDD,array($Longitude,$eventPluginID,$GLOBALS['Longitude']));
			$dbADO->Execute($updateDD,array($Latitude,$eventPluginID,$GLOBALS['Latitude']));
		}else{
			$err.=translate('TEXT_ERROR_UNABLE_TO_FIND_EVENT_PLUGIN_CONST').'<br>';
		}

		$rip=$_POST['rip'];
		switch ($rip){
			case 'mp3':
				$bit_rate=($_POST['bit_rate_user']!=0 && $_POST['bit_rate_user']>=96 && $_POST['bit_rate_user']<=320)?$_POST['bit_rate_user']:$_POST['bit_rate_predefined'];
				$cbr_vbr=$_POST['cbr_vbr'];
				$rip="$rip;$bit_rate;$cbr_vbr";
			break;
			case 'ogg':
				$ql=$_POST['ogg_ql'];
				$rip="$rip;$ql";
			break;			
		}		
		if($mediaPluginID!==null){
			$dbADO->Execute($updateDD,array($rip,$mediaPluginID,$GLOBALS['RipFormat']));
		}else{
			$err.=translate('TEXT_ERROR_UNABLE_TO_FIND_MEDIA_PLUGIN_CONST').'<br>';
		}

		
		if ($installationID!=0 && $description!='') {
			$queryUpdate = 'UPDATE Installation Set Description=?,Name=?,Address=?,City=?,State=?,Zip=?,FK_Country=? 
							WHERE PK_Installation = ?';
			$dbADO->Execute($queryUpdate,array($description,$name,$address,$city,$state,$zip,$country,$installationID));
			
			$newTimeZone=@$_POST['newTimeZone'];
			$oldTimeZone=@$_POST['oldTimeZone'];
			if($newTimeZone!=$oldTimeZone){
				$cmdToSend='sudo -u root /usr/pluto/bin/SetTimeZone.sh '.trim($newTimeZone);
				exec($cmdToSend);	
			}
			if($newLanguage!=$defLanguage){
				if(is_null($defLanguage)){
					$dbADO->Execute('INSERT INTO Device_DeviceData (IK_DeviceData,FK_Device,FK_DeviceData) VALUES (?,?,?)',array($newLanguage,$dceRouterID,$GLOBALS['Language']));
				}else{
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newLanguage,$dceRouterID,$GLOBALS['Language']));
				}
			}
			
			if($tts_Language != $old_tts_Language) {
				$cmdToSend='sudo -u root /usr/pluto/bin/generate_users_voicemenu.pl';
				exec($cmdToSend);	
			}
			$suffix=(isset($err))?'&error='.$err:'';
			header("Location: index.php?section=installationSettings&from=$from&msg=".translate('TEXT_INSTALLATION_UPDATED_CONST').$suffix);
			exit();
		} else {
			header("Location: index.php?section=installationSettings&error=".translate('TEXT_ERROR_INVALID_ID_OR_EMPTY_NAME_CONST'));
		}
	}
	
	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_INSTALLATION_SETTINGS_CONST'));
	
	$output->setNavigationMenu(array(translate('TEXT_INSTALLATION_SETTINGS_CONST')=>"index.php?section=installationSettings"));
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_INSTALLATION_SETTINGS_CONST'));			
	$output->output();
}

// return a javascript array with coordinates
function getCitiesCoordsArray($dbADO,$filter='')
{
	
	$out='
	<script>
	function setCoordinates()
	{
		currentCity=document.installationSettings.city_coords.value;
		try{
			document.installationSettings.Longitude.value=coords_long[currentCity];
			document.installationSettings.Latitude.value=coords_lat[currentCity];
		}catch(e){
			// do nothing
		}
	}
	';
	$res=$dbADO->Execute('SELECT * FROM City '.$filter);
	$out.='
		var coords_long=new Array('.$res->RecordCount().');
		var coords_lat=new Array('.$res->RecordCount().');
	';
	while($row=$res->FetchRow()){
		$out.='
		coords_long['.$row['PK_City'].']='.$row['Longitude'].';
		coords_lat['.$row['PK_City'].']='.$row['Latitude'].';';
	}
	$out.='
	</script>
	';

	return $out;
}

// display options for each rip format
function rippingSettings($selectedRipFormat){
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('installationSettings.lang.php');
	
	$bitRateArray=array(96=>96,128=>128,192=>192,256=>256,320=>320);
	$optarray=array();
	for($i=1;$i<11;$i++){
		$optarray[$i]=$i;
	}	
	
	$parts=explode(';',$selectedRipFormat);
	$ripFormat=$parts[0];
	switch ($ripFormat){
		case 'mp3':
			$bit_rate=@$parts[1];
			$cbr_vbr=@$parts[2];
		break;
		case 'ogg':
			$ql=(isset($parts[1]) && $parts[1]>0 && $parts[1]<11)?$parts[1]:3;
		break;
		
	}
	$ql=(!isset($ql))?3:$ql;
	
	$out='
			<tr id="mp3_opt" style="display:'.(($ripFormat=='mp3')?'':'none').';">
				<td><B>'.translate('TEXT_BIT_RATE_CONST').'</B></td>
				<td>'.pulldownFromArray($bitRateArray,'bit_rate_predefined',@$bit_rate,'onchange="document.installationSettings.bit_rate_user.value=document.installationSettings.bit_rate_predefined.value;" style="width:180px;"','key','').'</td>
				<td>or type</td>
				<td><input type="text" name="bit_rate_user" value="'.@$bit_rate.'"></td>
				<tr>
					<td>&nbsp;</td>
					<td><input type="radio" name="cbr_vbr" value="cbr" '.((!isset($cbr_vbr) || @$cbr_vbr=='cbr')?'checked':'').'>CBR <input type="radio" name="cbr_vbr" value="vbr" '.((@$cbr_vbr=='vbr')?'checked':'').'> VBR</td>
				</tr>
			</tr>
			<tr id="ogg_opt" style="display:'.(($ripFormat=='ogg')?'':'none').';">
				<td><B>'.translate('TEXT_QUALITY_CONST').'</B></td>
				<td>'.pulldownFromArray($optarray,'ogg_ql',@$ql,'style="width:180px;"','key','').'</td>
			</tr>
			<tr id="flac_opt" style="display:'.(($ripFormat=='flac')?'':'none').';">
				<td colspan="2">'.translate('TEXT_NO_OPTIONS_AVAILABLE_CONST').'</td>
			</tr>
			<tr id="wav_opt" style="display:'.(($ripFormat=='wav')?'':'none').';">
				<td colspan="2">'.translate('TEXT_NO_OPTIONS_AVAILABLE_CONST').'</td>
			</tr>
';
	
	return $out;
}

// get an assoc array from database for cities or retrieve them from Internet if no one is found
function getCities($selectedRegion,$dbADO){
	includeLangFile('installationSettings.lang.php');
	
	// TODO: remove comment to use global variable instead of hard-coded devel one
	global $LinuxMCEHost;
		
	/* @var $dbADO ADOConnection */
	$res=$dbADO->Execute('SHOW TABLES LIKE "City"');
	if($res->RecordCount()!=0){
		$existing=getAssocArray('City','PK_City','City',$dbADO,'WHERE FK_Region='.$selectedRegion,'ORDER BY City ASC');
		if(count($existing)==0){
			$isImported=import_remote_sql($LinuxMCEHost.'/GetCity.php?PK_Region='.$selectedRegion,$dbADO);
			return getAssocArray('City','PK_City','City',$dbADO,'WHERE FK_Region='.$selectedRegion,'ORDER BY City ASC');
		}
		return $existing;
	}else{
		$isCreated=import_remote_sql($LinuxMCEHost.'/GetCity.php?Create=1',$dbADO);
		if($isCreated==0){
			$isImported=import_remote_sql($LinuxMCEHost.'/GetCity.php?PK_Region='.$selectedRegion,$dbADO);
			return getAssocArray('City','PK_City','City',$dbADO,'WHERE FK_Region='.$selectedRegion);
		}else{
			error_redirect(translate('TEXT_ERROR_IMPORT_FAILED_CONST'),'index.php?section=installationSettings');
		}
		
	}
}
?>

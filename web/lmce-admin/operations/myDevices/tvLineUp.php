<?php
function tvLineUp($output,$dbADO,$mediaADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/tvLineUp.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$installationID = (int)$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = (int)@$_REQUEST['deviceID'];
	
	$fields=getFieldsAsArray('Installation','Zip',$dbADO,'WHERE PK_Installation='.$installationID);

	$mediaProviderDD=explode(',',getDeviceData($deviceID,$GLOBALS['PK_MediaAttribute'],$dbADO));
	if(count($mediaProviderDD)==1){	
		$InstallationZipCode=$fields['Zip'][0];
		$checkedChannels='all';
	}else{
		$providerFields=getFieldsAsArray('MediaProvider','ID',$mediaADO,'WHERE PK_MediaProvider='.$mediaProviderDD[0]);
		$data=explode(',',$providerFields['ID'][0]);
		
		$InstallationZipCode=$data[0];
		$provider=$data[1];
		$checkedChannels=$mediaProviderDD;
		// remove first 2 records: zip code and provider id
		unset($checkedChannels[0]);
	}	
	
	// if zip codeand provider are sent throu GET, they are used instead of df values
	$zip=(isset($_REQUEST['zipCode']))?$_REQUEST['zipCode']:$InstallationZipCode;
	$provider= (isset($_REQUEST['provider']))?@$_REQUEST['provider']:@$provider;
	
	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
		</script>	
		<h3>'.$TEXT_TV_LINEUP_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
				
	<div class="err"><br>'.@$_GET['error'].'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="tvLineUp">
			<input type="hidden" name="section" value="tvLineUp">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">

			<table>
				<tr class="alternate_back">
					<td><B>'.$TEXT_PROVIDERS_FOR_ZIP_CODE_CONST.'</B> <input type="text" name="zipCode" value="'.$zip.'"></td>
					<td><input type="button" class="button" name="show" value="'.$TEXT_SHOW_PROVIDERS_CONST.'" onClick="document.tvLineUp.action.value=\'form\';document.tvLineUp.submit();"></td>
				</tr>
				<tr>
					<td colspan="2"> '.providersList($zip,$provider,$mediaADO,$checkedChannels).'</td>
				</tr>				
			</table>
		</form>
	';	

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=tvLineUp&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		$providerID=@$_POST['providerID'];
		$channelsArray=explode(',',@$_POST['channels']);
		$checkedChannels=array();
		foreach ($channelsArray AS $ch){
			if(isset($_POST['channel_'.$ch])){
				$checkedChannels[]=$ch;
			}
		}

		$sourceID=$zip.','.$provider.','.$providerID;
		$exitingMediaProviders=getAssocArray('MediaProvider','PK_MediaProvider','Description',$mediaADO,'WHERE `Type`=\'myth\' AND ID=\''.$sourceID.'\'');
		if(!in_array($provider,array_values($exitingMediaProviders))){
			$mediaADO->Execute('INSERT INTO MediaProvider (Description,`Type`,ID) VALUES (?,?,?)',array($provider,'myth',$sourceID));
			$mediaProviderID=$mediaADO->Insert_Id();
		}else{
			list($mediaProviderID)=array_keys($exitingMediaProviders);
		}
				
		set_device_data($deviceID,$GLOBALS['PK_MediaAttribute'],$mediaProviderID.','.join(',',$checkedChannels),$dbADO);

		
		header("Location: index.php?section=tvLineUp&from=$from&deviceID=$deviceID&from=$from&msg=$TEXT_SAVED_CONST");
	}
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_TV_LINEUP_CONST);
	$output->output();
}

function providersList($zip,$provider,$mediaADO,$channelsSelected='all'){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/tvLineUp.lang.php');
	
		
	$headendsArray=getFieldsAsArray('Headends','DISTINCT he_headend_id,he_community_name',$mediaADO,'WHERE he_zip_code LIKE \''.$zip.'\'','ORDER BY he_community_name ASC');
	if(count($headendsArray)==0){
		return $TEXT_NO_PROVIDERS_FOR_SELECTED_ZIP_CODE_CONST;
	}

	$out='<table cellpadding="2" cellspacing="0" width="100%">';
	$providerID='';
	for($i=0;$i<count($headendsArray['he_headend_id']);$i++){
		$checked='';
		if(($headendsArray['he_community_name'][$i]==$provider)?'checked':''){
			$checked='checked';
			$providerID=$headendsArray['he_headend_id'][$i];
		}
		$out.='
		<tr>
			<td width="20"><input type="radio" name="provider" value="'.$headendsArray['he_community_name'][$i].'" '.$checked.' onClick="document.tvLineUp.action.value=\'form\';document.tvLineUp.submit();"></td>
			<td>'.$headendsArray['he_community_name'][$i].'</td>
		</tr>';
	}
	if($providerID!=''){
		$out.='
		<tr>
			<td colspan="2" class="alternate_back"><B>'.$TEXT_CHANNELS_CONST.'</B></td>
		</tr>';
		$res=$mediaADO->Execute('
			SELECT DISTINCT tf_station_name,tf_station_num 
			FROM Stations st
 			LEFT JOIN Lineups li ON li.cl_station_num=st.tf_station_num
 			WHERE li.cl_headend_id=\''.$providerID.'\'
 			ORDER BY tf_station_name ASC');
		$channels=array();
		while($row=$res->FetchRow()){
			$channels[]=$row['tf_station_num'];
		$out.='
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="channel_'.$row['tf_station_num'].'" '.(((is_array($channelsSelected) && in_array($row['tf_station_num'],$channelsSelected)) || $channelsSelected=='all')?'checked':'').' value="1">'.$row['tf_station_name'].'</td>
		</tr>';
		}
		$out.='
		<tr>
			<td colspan="2" class="alternate_back" align="center"><input type="submit" class="button" value="'.$TEXT_SAVE_CONST.'"></td>
		</tr>
		<input type="hidden" name="providerID" value="'.$providerID.'">
		<input type="hidden" name="channels" value="'.join(',',@$channels).'">		
		';
		
	}
	$out.='</table>
';
	
	return $out;
}
?>
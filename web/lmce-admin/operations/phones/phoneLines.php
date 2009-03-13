<?
function phoneLines($output,$astADO,$dbADO) {
	global $wikiHost;
	
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	// check if database pluto_vdr exist
	$databases=$dbADO->MetaDatabases();
	if(!in_array('asterisk',$databases)){
		error_redirect($TEXT_ASTERISK_DB_NOT_FOUND_CONST,'index.php?section=phoneLines');
	}
		
	
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$providerData=get_available_providers();
	
	$keywords=array();
	foreach ($providerData AS $prID=>$providerArray){
		$keywords[$providerArray['keyword']]=$prID;
	}
	
	if(isset($_REQUEST['provider'])){
		$provider=$_REQUEST['provider'];
	}
	$editedID=@$_REQUEST['eid'];
	if($editedID!=0 && $editedID!=''){
		$phoneData=getPLDetails($editedID,$_REQUEST['type'],$astADO);
		$provider=$keywords[$phoneData['Data']];
	}


	$pulldownOptions='';
	foreach (array_keys($providerData) AS $option){
		$pulldownOptions.='<option value="'.$option.'" '.((@$provider==$option)?'selected':'').'>'.$option.'</option>';
	}
		
	if(isset($provider)){
		$providerUrl=@$providerData[@$_REQUEST['provider']]['url'];
		$providerScript='create_amp_phoneline.sh '.@$providerData[@$_REQUEST['provider']]['keyword'];
		$userBox='
		<input type="hidden" name="editedID" value="'.@$editedID.'">
		<input type="hidden" name="edited_type" value="'.@$_REQUEST['type'].'">
		
		<table align="center" cellpadding="3" cellspacing="0">
			<tr>
				<td>'.$TEXT_URL_CONST.': </td>
				<td><a href="'.$providerUrl.'" target="_blank">'.$providerUrl.'</a></td>
			</tr>
			<tr>
				<td>&nbsp; </td>
				<td>&nbsp;</td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td><B>'.$TEXT_USERNAME_CONST.' *</B> </td>
				<td><input type="text" name="username" value="'.@$phoneData['Username'].'"></td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td colspan="2">'.$TEXT_PHONE_LINE_USERNAME_NOTE_CONST.'</td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_PASSWORD_CONST.' *</B> </td>
				<td><input type="text" name="password" value="'.@$phoneData['Password'].'"></td>
			</tr>
			<tr>
				<td colspan="2">'.$TEXT_PHONE_LINE_PASSWORD_NOTE_CONST.'</td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td><B>'.$TEXT_PHONE_NUMBER_CONST.'</B> </td>
				<td><input type="text" name="phone" value="'.@$phoneData['PhoneNumber'].'"></td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td colspan="2">'.$TEXT_PHONE_LINE_NUMBER_NOTE_CONST.'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_HOST_CONST.'</B> </td>
				<td><input type="text" name="host" value="'.@$phoneData['Host'].'"> </td>
			</tr>		
			<tr>
				<td colspan="2">'.$TEXT_PHONE_LINE_HOST_NOTE_CONST.'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="'.$TEXT_ADD_CONST.'" value="Submit"> <input type="reset" class="button" name="cancelBtn1" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>
		</table>
		
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("phoneLines");
 			frmvalidator.addValidation("username","req","'.$TEXT_USERNAME_REQUIRED_CONST.'");			
			frmvalidator.addValidation("password","req","'.$TEXT_PASSWORD_REQUIRED_CONST.'");			
		</script>		
		';
	}
	
	if($action=='form'){
		$manualConfiguration=isManualConfig($dbADO);
		$manualConfigNote='<div align="center">'.$TEXT_MANUAL_CONFIG_NOTE_CONST.' <a href="index.php?section=amp">'.$TEXT_CLICK_HERE_CONST.'</a></div>';
		
		$out.='
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		
		'.phoneLinesLocalSettings($dbADO).'
		
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">
			
			<h3 align="center">'.$TEXT_PHONE_LINES_CONST.'</h3>
			'.(($manualConfiguration===true)?$manualConfigNote:phoneLinesTable($astADO));
		if(isset($GLOBALS['count']) && $GLOBALS['count']==0 || $editedID!=0){
			$out.='
			<table align="center">
				<tr>
					<td>'.$TEXT_CHOOSE_PROVIDER_CONST.'</td>
					<td><select name="provider" onChange="document.phoneLines.action.value=\'form\';document.phoneLines.submit();">
						<option value=""> - '.$TEXT_PLEASE_SELECT_CONST.' -</option>
						'.$pulldownOptions.'
					</select></td>
				</tr>
				<tr>
					<td colspan="2">'.@$userBox.'</td>
				</tr>		
			</table>
		';
		}
	
		$activateAutomaticConfiguration='<br><input type="button" class="button" name="activate_auto" value="Activate Automatic Configuration" onClick="self.location=\'index.php?section=phoneLines&action=activateAuto\'">';
		$activateManualConfiguration='To learn about how to add more than 1 phone line, or to add a voice over ip provider that\'s not on the list, click <a href="'.$wikiHost.'index.php/Manual_Phones_Configuration" target="_blank">here</a>.<br><br> <input type="button" class="button" name="activate_manual" value="Deactivate Automatic Configuration" onClick="self.location=\'index.php?section=phoneLines&action=activateManual\'">';
			
		$out.='
			<div align="center">
			'.(($manualConfiguration===true)?$activateAutomaticConfiguration:$activateManualConfiguration).'	
			</div>
			
		</form>
		';
	}elseif($action=='activateAuto'){
		$out=activate_autoconfig_form();
	}elseif($action=='activateManual'){
		$out=activate_manualconfig_form();
	}else{
	// process area
		if(isset($_POST['Add'])){
			$editedID=$_REQUEST['editedID'];
			if($editedID!=''){
				$phoneData=getPLDetails($editedID,$_REQUEST['edited_type'],$astADO);
				deletePhoneLine(str_replace('9999','',$editedID),$phoneData['Data'],$phoneData['PhoneNumber']);
			}
			
			$username=cleanString($_POST['username']);
			$password=cleanString($_POST['password']);
			$phone=($_POST['phone']=='')?$username:cleanString($_POST['phone']);
			$host=cleanString($_POST['host']);

			$cmd='sudo -u root /usr/pluto/bin/'.$providerScript.' '.$username.' '.$password.' '.$phone.' '.$host;

			$ret=exec_batch_command($cmd);
			
			if($ret===false){
				$suffix='&err='.$TEXT_ADD_PHONE_LINE_ERROR_CONST;
			}else{
				$suffix='&msg='.$TEXT_ADD_PHONE_LINE_CMD_SENT_CONST;
			}

			header('Location: index.php?section=phoneLines'.@$suffix);
			exit();
		}
		
		if(isset($_REQUEST['id'])){
			$id=str_replace('9999','',$_REQUEST['id']);
			$line_name=$_REQUEST['line_name'];
			$phone_number=$_REQUEST['phone_number'];
			$ret=deletePhoneLine($id,$line_name,$phone_number);
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_PHONE_LINE_DELETED_CONST);
			exit();
		}
		
		if(isset($_POST['update_settings'])){
			$val_141=(isset($_POST['ckb_141']))?$_POST['value_141']:0;
			$val_142=(isset($_POST['ckb_141']) && isset($_POST['ckb_142']))?$_POST['value_142']:-1;
			$val_143=$_POST['value_143'];
			$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
			if(is_null($telecomPlugin)){
				header('Location: index.php?section=phoneLines&error='.$TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST);
				exit();
			}
			
			$query='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			
			$dbADO->Execute($query,array($val_141,$telecomPlugin,$GLOBALS['TelecomLocalPrefix']));
			$dbADO->Execute($query,array($val_142,$telecomPlugin,$GLOBALS['TelecomPrependDigit']));
			$dbADO->Execute($query,array($val_143,$telecomPlugin,$GLOBALS['TelecomLocalNumberLength']));
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_PHONE_SETTINGS_UPDATED_CONST);
			exit();
		}
		
		if(isset($_REQUEST['activate_manual'])){
			manualConfigSwitch($dbADO);
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_MANUAL_CONFIGURATION_ACTIVATED_CONST);
			exit();
		}

		if(isset($_REQUEST['activate_auto'])){
			manualConfigSwitch($dbADO);
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_AUTOMATIC_CONFIGURATION_ACTIVATED_CONST);
			exit();
		}

		if(isset($_REQUEST['activate_auto_override'])){
			// todo: asterisk cleanup
			manualConfigSwitch($dbADO);
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_AUTOMATIC_OVERRIDE_CONFIGURATION_ACTIVATED_CONST);
			exit();
		}

		
		header('Location: index.php?section=phoneLines');
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_PHONE_LINES_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_LINES_CONST=>'index.php?section=phoneLines'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PHONE_LINES_CONST);
	$output->output();
}

function phoneLinesTable($astADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	$phoneState=getPhonesState();
	$providersKeywords=get_providers_by_type('SIP');

	$where='';
	if(count($providersKeywords)>0){
		$whereArray=array();
		foreach ($providersKeywords as $key){
			$whereArray[]="sip.data='$key'";
		}
		$where=' AND ('.join(' OR ',$whereArray).')';
	}
	
	
	$GLOBALS['count']=0;
	$res=$astADO->Execute("
		SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata,siph.data AS hdata 
		FROM sip 
		INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
		INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
		INNER JOIN sip siph ON (siph.id=sip.id) AND (siph.keyword='host')
		WHERE (sip.keyword='account') $where");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td align="center"><B>'.$TEXT_TYPE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DATA_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_PASSWORD_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_USERNAME_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_HOST_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_PHONE_NUMBER_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_STATUS_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\'custom-linuxmce,10'.substr($row['id'],-1).',1\''));
		$phoneNumber=@$incomingData[0];
	
		$GLOBALS['count']++;
		$color=($GLOBALS['count']%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>SIP</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td>'.$row['hdata'].'</td>
			<td>'.$phoneNumber.'</td>
			<td>'.@$phoneState['SIP'][$row['pdata']].'</td>
			<td align="center">
				<a href="index.php?section=phoneLines&type=SIP&eid='.$row['id'].'">'.$TEXT_EDIT_CONST.'</a> 
				<a href="index.php?section=incomingCallsSettings&type=SIP&id='.$row['id'].'">'.$TEXT_SETTINGS_CONST.'</a> 
				<a href="javascript:if(confirm(\''.$TEXT_DELETE_PHONE_LINE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'&line_name='.$row['data'].'&phone_number='.$phoneNumber.'\'">'.$TEXT_DELETE_CONST.'</a>
			</td>
		</tr>';
	}

	$providersKeywords=get_providers_by_type('IAX');
	$where='';
	if(count($providersKeywords)>0){
		$whereArray=array();
		foreach ($providersKeywords as $key){
			$whereArray[]="iax.data='$key'";
		}
		$where=' AND ('.join(' OR ',$whereArray).')';
	}
	
	$res=$astADO->Execute("
		SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata,iaxh.data AS hdata 
		FROM iax 
		INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
		INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
		INNER JOIN iax iaxh ON (iaxh.id=iax.id) AND (iaxh.keyword='host')
		WHERE (iax.keyword='account') $where");
	while($row=$res->FetchRow()){
		$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\'custom-linuxmce,10'.substr($row['id'],-1).',1\''));
		$phoneNumber=@$incomingData[0];

		$GLOBALS['count']++;
		$color=($GLOBALS['count']%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>
			IAX</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td>'.$row['hdata'].'</td>
			<td>'.$phoneNumber.'</td>
			<td>'.@$phoneState['IAX'][$row['pdata']].'</td>
			<td align="center">
				<a href="index.php?section=phoneLines&type=IAX&eid='.$row['id'].'">'.$TEXT_EDIT_CONST.'</a> 
				<a href="index.php?section=incomingCallsSettings&type=IAX&id='.$row['id'].'">'.$TEXT_SETTINGS_CONST.'</a> 
				<a href="javascript:if(confirm(\''.$TEXT_DELETE_PHONE_LINE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'&line_name='.$row['data'].'&phone_number='.$phoneNumber.'\'">'.$TEXT_DELETE_CONST.'</a>
			</td>
		</tr>';
	}
	
	$out.='</table><br>';
	if($GLOBALS['count']==0){
		$out='';
	}
	
	return $out;
}

function phoneLinesLocalSettings($dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	$installationID = (int)@$_SESSION['installationID'];	
	$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
	if(is_null($telecomPlugin)){
		return '<span class="err">'.$TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST.'</span>';
	}
	
	$ddArray=getAssocArray('Device_DeviceData','FK_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$telecomPlugin);

	$out='
	<form action="index.php" method="POST" name="phoneSettings">
	<input type="hidden" name="section" value="phoneLines">
	<input type="hidden" name="action" value="update">
	
	<table align="center" cellpadding="3" cellspacing="0" bgcolor="#F0F3F8">
		<tr>
			<td colspan="2"><input type="checkbox" name="ckb_141" value="1" '.((@$ddArray['141']!=0 && @$ddArray['141']!='')?'checked':'').'> '.$TEXT_PREPEND_AREA_CODE_CONST.' <input type="text" name="value_141" value="'.@$ddArray['141'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="ckb_142" value="1" '.((@$ddArray['142']!=-1)?'checked':'').'> '.$TEXT_PREPEND_DIGIT_CONST.' <input type="text" name="value_142" value="'.((@$ddArray['142']==-1)?'':@$ddArray['142']).'" style="width:25px;"></td>
		</tr>
		<tr>
			<td>'.$TEXT_LOCAL_NUMBER_LENGTH_CONST.' <input type="text" name="value_143" value="'.@$ddArray['143'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td align="center"><input type="submit" class="button" name="update_settings" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>	
	</table>
	</form>';
	
	return $out;
}

function getPLDetails($id,$type,$astADO){
	if($type=='SIP'){
		$providersKeywords=get_providers_by_type('SIP');
	
		$where='';
		if(count($providersKeywords)>0){
			$whereArray=array();
			foreach ($providersKeywords as $key){
				$whereArray[]="sip.data='$key'";
			}
			$where=' AND ('.join(' OR ',$whereArray).')';
		}		
		$res=$astADO->Execute("
			SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata,siph.data AS hdata 
			FROM sip 
			INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
			INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
			INNER JOIN sip siph ON (siph.id=sip.id) AND (siph.keyword='host')
			WHERE (sip.keyword='account') $where AND sip.id='$id'");
	}else{
		$providersKeywords=get_providers_by_type('IAX');
		$where='';
		if(count($providersKeywords)>0){
			$whereArray=array();
			foreach ($providersKeywords as $key){
				$whereArray[]="iax.data='$key'";
			}
			$where=' AND ('.join(' OR ',$whereArray).')';
		}
			
		$res=$astADO->Execute("
			SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata,iaxh.data AS hdata 
			FROM iax 
			INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
			INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
			INNER JOIN iax iaxh ON (iaxh.id=iax.id) AND (iaxh.keyword='host')
			WHERE (iax.keyword='account') $where AND iax.id='$id'");
	}
	$data=array();
	$row=$res->FetchRow();
	$data['Password']=$row['sdata'];
	$data['Username']=$row['pdata'];
	$data['Data']=$row['data'];
	$data['Host']=$row['hdata'];
	
	$incomingID='custom-linuxmce,10'.substr($id,-1).',1';
	$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\''.$incomingID.'\''));
	$data['PhoneNumber']=@$incomingData[0];
	
	return $data;
}

function deletePhoneLine($id,$line_name,$phone_nr){
	/*
	$url='http://'.$_SERVER['SERVER_ADDR'].'/admin/config.php';
	$params='display=6&extdisplay=OUT_'.$id.'&action=deltrunk';
			
	$answer=queryExternalServer($url.'?'.$params);
	*/
	// command to del phone line usr/pluto/bin/delete_phoneline.pl <line_nr> <line_name> <phone_nr>
	$cmd="sudo -u root /usr/pluto/bin/delete_phoneline.pl $id $line_name $phone_nr";

	$answer=exec_batch_command($cmd,1);
	
	return $answer;
}

function getPhonesState(){
	$state['SIP']=getSIPState();
	$state['IAX']=getIAXState();

	return $state;
}


function getSIPState(){
	// SIP get state command	
	$cmd='sudo -u root /usr/sbin/asterisk -rx "sip show registry"';
	$response=exec_batch_command($cmd,1);

	// response look like this
	/*
	   -- Remote UNIX connection
	Host                            Username       Refresh State
	sip.inphonex.com:5060           7588648             50 Registered
	sip.inphonex.com:5060           123456             100 Unregistered
	Verbosity is at least 3
	*/
	
	$state=array();
	
	$lines=explode("\n",$response);
	$last=count($lines);
	for($i=0;$i<$last;$i++){
		if(strpos($lines[$i],'Host')!==false || strpos($lines[$i],'Verbosity')!==false || strpos($lines[$i],'Core debug')!==false || strpos($lines[$i],'UNIX')!==false)
		unset($lines[$i]);	
	}


	foreach ($lines AS $line){
		$parsed=array();
		$parts=explode(" ",$line);

		foreach ($parts AS $pos=>$value){
			if(trim($value)!=''){
				$parsed[]=$value;
			}
		}

		$dataArray=array_slice($parsed,4);
		$ampersand=strpos(@$parsed[1],'@');
		$nr=($ampersand===false)?@$parsed[1]:substr(@$parsed[1],0,$ampersand);
		$state[$nr]=(!isset($parsed[4]))?@$parsed[3]:$parsed[3].' '.join(' ',$dataArray);
	}


	return $state;
}

function getIAXState(){
	// IAX get state command	
	/*
Host                  dnsmgr  Username    Perceived             Refresh  State
207.174.202.3:4569    N       plutosf     82.77.255.193:4569         60  Registered
*/
	$cmd='sudo -u root /usr/sbin/asterisk -rx "iax2 show registry"';
	$response=exec_batch_command($cmd,1);
	
	$state=array();
	
	$lines=explode("\n",$response);
	$last=count($lines);
	for($i=0;$i<$last;$i++){
		if(strpos($lines[$i],'Host')!==false || strpos($lines[$i],'Verbosity')!==false || strpos($lines[$i],'Core debug')!==false)
		unset($lines[$i]);	
	}

	foreach ($lines AS $line){
		$parsed=array();
		$parts=explode(" ",$line);

		foreach ($parts AS $pos=>$value){
			if(trim($value)!=''){
				$parsed[]=$value;
			}
		}

		$dataArray=array_slice($parsed,6);
		$ampersand=strpos(@$parsed[2],'@');
		$nr=($ampersand===false)?@$parsed[2]:substr(@$parsed[2],0,$ampersand);
		$state[$nr]=(!isset($parsed[6]))?@$parsed[5]:$parsed[5].' '.join(' ',$dataArray);
	}

	return $state;
}

function isManualConfig($dbADO){
	$rs=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData INNER JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceData=? AND FK_DeviceTemplate=?',array($GLOBALS['DD_ManualConfiguration'],$GLOBALS['Asterisk']));
	if($rs->RecordCount()==0){
		return false;
	}
	$row=$rs->FetchRow();
	
	return ($row['IK_DeviceData']==1)?true:false;
}

function manualConfigSwitch($dbADO){
	$rs=$dbADO->Execute('UPDATE Device_DeviceData INNER JOIN Device ON FK_Device=PK_Device SET IK_DeviceData=IF(IK_DeviceData=1,0,1) WHERE FK_DeviceData=? AND FK_DeviceTemplate=?',array($GLOBALS['DD_ManualConfiguration'],$GLOBALS['Asterisk']));
	if($rs->RecordCount()==0){
		return false;
	}
	$row=$rs->FetchRow();
	
	return ($row['IK_DeviceData']==1)?true:false;
}

function activate_autoconfig_form(){
	$out='
		<form action="index.php" method="POST" name="phoneLines">
		<input type="hidden" name="section" value="phoneLines">
		<input type="hidden" name="action" value="update">

		<div align="center">
			Automatic phone lines configuration may not work with the phone lines configured in advanced mode.<br>
			<h3><font color="red">Are you sure you want to activate automatic configuration?</font></h3><br><br>
			<input type="submit" class="button" name="activate_auto" value="Yes and keep my advanced settings"> <input type="submit" class="button" name="activate_auto_override" value="Yes and remove my advanced settings"> <input type="submit" class="button" name="cancel" value="No">
		</div>
			
		</form>
		';
	return $out;
}

function activate_manualconfig_form(){
	global $wikiHost;
	
	$out='
		<form action="index.php" method="POST" name="phoneLines">
		<input type="hidden" name="section" value="phoneLines">
		<input type="hidden" name="action" value="update">

		<div align="center">
			<h3><font color="red">Are you sure you want to activate manual phone lines configuration? </font></h3>
			<a href="'.$wikiHost.'index.php/Manual_Phones_Configuration" target="_blank">(read more)</a><br><br>
			
			<input type="submit" class="button" name="activate_manual" value="Yes"> <input type="submit" class="button" name="cancel" value="No">
		</div>
			
		</form>
		';
	return $out;
}

function get_available_providers(){
	
	// file format: [provider name]\t[url]\t[keyword]\t[SIP|IAX]
	$providerData=array();
	$confFile=@file('/etc/asterisk/provider_list.txt');
	
	
	if($confFile=='' || count($confFile)==0){
		return $providerData;
	}
	
	foreach ($confFile AS $line){
		if(trim($line)!=''){
			$parts=explode("\t",$line);
			
			$providerData[$parts[0]]['url']=trim($parts[1]);
			$providerData[$parts[0]]['keyword']=trim($parts[2]);
		}
	}
	asort($providerData);
	
	return $providerData;
}

function get_providers_by_type($type){
	
	// file format: [provider name]\t[url]\t[keyword]\t[SIP|IAX]
	$providerData=array();
	$confFile=@file('/etc/asterisk/provider_list.txt');
	
	
	if($confFile=='' || count($confFile)==0){
		return $providerData;
	}
	
	foreach ($confFile AS $line){
		if(trim($line)!=''){
			$parts=explode("\t",$line);
			
			$providerData[trim($parts[3])][]=trim($parts[2]);
		}
	}

	return $providerData[$type];
}

?>

<?php
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('phoneLines.lang.php');

function phoneLines($output,$astADO,$dbADO,$telecomADO) {
	global $wikiHost, $editedID, $editdata;
	
	// check if database asterisk exists
	$databases=$dbADO->MetaDatabases();
	if(!in_array('asterisk',$databases)){
		error_redirect(translate('TEXT_ASTERISK_DB_NOT_FOUND_CONST'),'index.php?section=phoneLines');
	}	
	
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out=setLeftMenu($dbADO);
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$editedID=@$_REQUEST['eid'];
	if(isset($_REQUEST['id'])) $id=$_REQUEST['id'];

	if($action=='form'){
		$ProtocolList=array('SIP','IAX','SPA','GTALK');
		$DTMFList=array('rfc2833','inband','info','auto');
		$out.='
		<script type="text/javascript">
			function toggleFax() 	{
				if(document.phoneLines.isfax.checked) {
					document.getElementById("idFaxMail").style.display = "";
					document.getElementById("idFaxHeader").style.display = "";
				} else {
					document.getElementById("idFaxMail").style.display = "none";
					document.getElementById("idFaxHeader").style.display = "none";					
				}
			}
		</script>
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		
		'
		.emergencySettings($dbADO,$astADO)
		.'
		
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">
			
			<h3 align="center">'.translate('TEXT_PHONE_LINES_CONST').'</h3>
			'.phoneLinesTable($astADO);
		
		// edit phoneline block
		if(isset($GLOBALS['count']) && $GLOBALS['count']==0 || isset($_REQUEST['eid'])){
			if(isset($GLOBALS['count']) && $GLOBALS['count']==0) $out.='<input type="hidden" name="isFirst" value="true">';
			$res=$dbADO->Execute("SELECT PK_Users, CONCAT(FirstName,' ',LastName) AS Name FROM Users WHERE ForwardEmail <> '' ORDER BY LastName,FirstName");
			while($row=$res->FetchRow()) $UsersList[$row['PK_Users']]=$row['Name'];	
			$out.='
			<br /><hr /><br />
			<input type="hidden" name="editedID" value="'.@$editedID.'">
			<table align="center" cellpadding="3" cellspacing="0">
				<tr class="tablehead">
					<td><B>'.translate('TEXT_NAME_CONST').'</B></td>
					<td><input type="text" name="name" value="'.$editdata['name'].'"></td>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_ENABLED_CONST').'</B></td>
					<td><input type="checkbox" name="enabled" value="1" '.((@$editdata['enabled']=='yes')?'checked':'').'></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_FAX_CONST').'</B></td>
					<td><input type="checkbox" name="isfax" value="1" '.((@$editdata['isfax']=='yes')?'checked':'').' onchange="toggleFax();"></td>
					
				</tr>
				<tr id="idFaxMail" '.((@$editdata['isfax']!='yes')?'style="display: none;"':'').'>
					<td>&nbsp;&nbsp;'.translate('TEXT_FAX_EMAIL_CONST').'
					<td>
						<select name="faxmail"  style="width: auto">';
						foreach ($UsersList as $id => $name) $out.='<option value="'.$id.'" '.(($editdata['faxmail'] == $id)?'selected="selected"':'').'>'.$name.'</option>';
						$out .='</select>
					</td>
				</tr>
				<tr id="idFaxHeader" '.((@$editdata['isfax']!='yes')?'style="display: none;"':'').'>
					<td>&nbsp;&nbsp;'.translate('TEXT_FAX_HEADER_CONST').'
					<td colspan="2"><input type="text" name="faxheader" maxlength="30" size="40" value="'.$editdata['faxheader'].'"></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_PREFIX_CONST').'</B></td>
					<td><input type="text" name="prefix" maxlength="2" size="3" value="'.$editdata['prefix'].'"></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_USERNAME_CONST').' *</B></td>
					<td><input type="text" name="username" value="'.$editdata['username'].'"></td>
					<td>'.translate('TEXT_PHONE_LINE_USERNAME_NOTE_CONST').'</td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_PASSWORD_CONST').' *</B></td>
					<td><input type="password" name="pass" value="'.$editdata['password'].'"></td>
					<td>'.translate('TEXT_PHONE_LINE_PASSWORD_NOTE_CONST').'</td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_PHONE_NUMBER_CONST').' *</B></td>
					<td><input type="text" name="phone" value="'.$editdata['phonenumber'].'"></td>
					<td>'.translate('TEXT_PHONE_LINE_NUMBER_NOTE_CONST').'</td></tr>
				<tr>
					<td><B>'.translate('TEXT_HOST_CONST').' *</B></td>
					<td><input type="text" name="host" value="'.$editdata['host'].'"></td>
					<td>'.translate('TEXT_PHONE_LINE_HOST_NOTE_CONST').'</td>
				</tr>		
				<tr>
					<td><B>'.translate('TEXT_PROTOCOL_CONST').' *</B></td>
					<td><select name="proto">';
					foreach ($ProtocolList as $p) $out.='<option value="'.$p.'" '.(($editdata['protocol'] == $p)?'selected="selected"':'').'>'.$p.'</option>';
					$out .='</select></td>
					<td>'.translate('TEXT_PHONE_LINE_PROTOCOL_NOTE_CONST').'</td>
				</tr>
				<tr>
				<td><B>'.translate('TEXT_PORT_CONST').' *</B></td>
				<td><input type="text" name="port" value="'.((@$editdata['port']=='')?'5060"':$editdata['port']).'" /></td>
				</tr>
				<tr>
					<td><B>'.translate('TEXT_DTMFMODE_CONST').'</B></td>
					<td><select name="dtmf">';
					foreach ($DTMFList as $d) $out.='<option value="'.$d.'" '.(($editdata['dtmfmode'] == $d)?'selected="selected"':'').'>'.$d.'</option>';
					$out .='</select></td>
				</tr>		
				<tr>
					<td><em>* '.translate('TEXT_REQUIRED_FIELDS_CONST').'</em></td>
					<td>&nbsp;</td>
					<td><input type="submit" class="button" name="submit" value="'.translate('TEXT_SAVE_CONST').'"> <input type="reset" class="button" name="cancelBtn1" value="'.translate('TEXT_CANCEL_CONST').'"></td>
				</tr>
			</table>
			<script>
				var frmvalidator = new formValidator("phoneLines");
				frmvalidator.addValidation("username","req","'.translate('TEXT_USERNAME_REQUIRED_CONST').'");			
				frmvalidator.addValidation("password","req","'.translate('TEXT_PASSWORD_REQUIRED_CONST').'");			
				frmvalidator.addValidation("phonenumber","req","'.translate('TEXT_PHONENUMBER_REQUIRED_CONST').'");			
				frmvalidator.addValidation("host","req","'.translate('TEXT_HOST_REQUIRED_CONST').'");			
			</script>
			';
		}
	
		$out.='</form>';
	}
	else{ // process area
		
		// add new phoneline entry and reload form in edit mode
		if(isset($action) && $action == 'add') {
			$SQL="INSERT INTO phonelines (enabled,protocol) VALUES ('no','SIP')";
			$res=$astADO->Execute($SQL);
			$res=$astADO->Execute('SELECT LAST_INSERT_ID();');
			$row=$res->FetchRow();
			$id=$row[0];
			$cmd='sudo -u root /usr/pluto/bin/db_create_dialplan.sh';
			exec_batch_command($cmd,1);
			if ($_REQUEST['isFirst'] == 'true') {
				$action = 'update';
				$_REQUEST['editedID'] = $id;
			} else {
				$suffix='&msg='.translate('TEXT_ADD_PHONE_LINE_CMD_SENT_CONST');
				header('Location: index.php?section=phoneLines&action=form&eid='.$id.@$suffix);
				exit();
			}
		}

		// edit existing phoneline
		if(isset($action) && $action == 'update') {
			$editedID=$_REQUEST['editedID'];
			$username=cleanString($_POST['username']);
			$password=cleanString($_POST['pass']);
			$faxmail=cleanString($_POST['faxmail']);
			if(isset($editedID)) {
				$SQL="UPDATE phonelines SET name='".$_POST['name']
					."',enabled='".(($_POST['enabled'])?'yes':'no')
					."',isfax='".(($_POST['isfax'])?'yes':'no')
					."',prefix='".$_POST['prefix']
					."',username='".$username
					."',password='".$password
					."',faxmail='".$faxmail
					."',phonenumber='".$_POST['phone']
					."',host='".$_POST['host']
					."',protocol='".$_POST['proto']
					."',port='".$_POST['port']
					."',dtmfmode='".$_POST['dtmf']
					."' WHERE id=".$editedID;
				$res=$astADO->Execute($SQL);
				$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh lines';
				exec_batch_command($cmd,1);
				$suffix='&msg='.translate('TEXT_PHONE_LINE_UPDATED_CONST');
				header('Location: index.php?section=phoneLines'.@$suffix);
			} else {
				$suffix='&msg='.translate('TEXT_ADD_PHONE_LINE_ERROR_CONST');
				header('Location: index.php?section=phoneLines'.@$suffix);
			}
			exit();
		}
		
		// delete existing phoneline 
		if(isset($action) && $action == 'del') {
			$SQL="DELETE FROM Line_HouseMode WHERE ID=".$id;
			$dbADO->Execute($SQL);
			$SQL="DELETE FROM phonelines WHERE id='".$id."'";
			$astADO->Execute($SQL);
			$SQL="ALTER TABLE phonelines AUTO_INCREMENT=1";
			$astADO->Execute($SQL);
			$SQL="DELETE FROM Line_HouseMode WHERE ID='".$id."'";
			$telecomADO->Execute($SQL);
			$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh lines';
			exec_batch_command($cmd,1);
			$cmd='sudo -u root /usr/pluto/bin/db_create_dialplan.sh';
			exec_batch_command($cmd,1);
			$suffix='&msg='.translate('TEXT_PHONE_LINE_DELETED_CONST');
			header('Location: index.php?section=phoneLines'.@$suffix);
			exit();
		}
		
		if(isset($action) && $action == 'update_emergency'){
			$val_296=$_POST['value_296'];
			$val_297=$_POST['value_297'];
			$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
			if(is_null($telecomPlugin)){
				header('Location: index.php?section=phoneLines&error='.translate('TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST'));
				exit();
			}
			
			$query='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			
			$dbADO->Execute($query,array($val_296,$telecomPlugin,$GLOBALS['TelecomEmergencyNumbers']));
			$dbADO->Execute($query,array($val_297,$telecomPlugin,$GLOBALS['TelecomEmergencyLine']));
			
			$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh lines';
			exec_batch_command($cmd,1);
			header('Location: index.php?section=phoneLines&msg='.translate('TEXT_EMERGENCY_SETTINGS_UPDATED_CONST'));
			exit();
		}

		if(isset($action) && $action == 'update_settings'){
			$val_141=(isset($_POST['ckb_141']))?$_POST['value_141']:0;
			$val_142=(isset($_POST['ckb_141']) && isset($_POST['ckb_142']))?$_POST['value_142']:-1;
			$val_143=$_POST['value_143'];
			$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
			if(is_null($telecomPlugin)){
				header('Location: index.php?section=phoneLines&error='.translate('TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST'));
				exit();
			}
			
			$query='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			
			$dbADO->Execute($query,array($val_141,$telecomPlugin,$GLOBALS['TelecomLocalPrefix']));
			$dbADO->Execute($query,array($val_142,$telecomPlugin,$GLOBALS['TelecomPrependDigit']));
			$dbADO->Execute($query,array($val_143,$telecomPlugin,$GLOBALS['TelecomLocalNumberLength']));
			
			$cmd='sudo -u root /usr/pluto/bin/db_phone_config.sh lines';
			exec_batch_command($cmd,1);
			header('Location: index.php?section=phoneLines&msg='.translate('TEXT_PHONE_SETTINGS_UPDATED_CONST'));
			exit();
		}
		header('Location: index.php?section=phoneLines');
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_PHONE_LINES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_PHONE_LINES_CONST')=>'index.php?section=phoneLines'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_PHONE_LINES_CONST'));
	$output->output();
}

function phoneLinesTable($astADO){
	global $editedID;
	global $editdata;
	$GLOBALS['count']=0;
	$res=$astADO->Execute("SELECT * FROM phonelines ORDER BY prefix");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td align="center"><B>'.translate('TEXT_ENABLED_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_PREFIX_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_NAME_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_PHONE_NUMBER_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_HOST_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_PROTOCOL_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_PORT_CONST').'<B></td>
			<td align="center"><B>'.translate('TEXT_USERNAME_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_STATUS_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_ACTION_CONST').'</B></td>
		</tr>	';

	while($row=$res->FetchRow()){
		if (isset($editedID) && $editedID == $row['id'])
			$editdata=$row;
		$GLOBALS['count']++;
		$color=($GLOBALS['count']%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td align="center">'.(($row['enabled']=='yes')
				?'<img src="/'.APPDIRECTORY.'/img/enabled.png" height="15" alt="[x]">'
				:'<img src="/'.APPDIRECTORY.'/img/disabled.png" height="15" alt="[&nbsp&nbsp]">')
			.(($row['isfax']=='yes')
				?'<img src="/'.APPDIRECTORY.'/img/fax.png" height="15" alt="FAX">'
				:'')
			.'</td>
			<td align="center">'.$row['prefix'].'</td>
			<td>'.$row['name'].'</td>
			<td>'.$row['phonenumber'].'</td>
			<td>'.$row['host'].'</td>
			<td align="center">'.$row['protocol'].'</td>
			<td align="enter">'.$row['port'].'</td>
			<td>'.$row['username'].'</td>
			<td>'.getLineState($row['protocol'],$row['username']).'</td>
			<td align="center">
				<a href="index.php?section=phoneLines&eid='.$row['id'].'">'.translate('TEXT_EDIT_CONST').'</a> 
				<a href="index.php?section=incomingCallsSettings&id='.$row['id'].'">'.translate('TEXT_PHONE_ROUTING_CONST').'</a> 
				<a href="javascript:if(confirm(\''.translate('TEXT_DELETE_PHONE_LINE_CONFIRMATION_CONST').'\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">'.translate('TEXT_DELETE_CONST').'</a>
			</td>
		</tr>';
	}
	// add phoneline button
	if(!isset($_REQUEST['eid'])) {
		$out.='<form action="index.php" method="POST" name="addLine">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="add">
			<tr><td>&nbsp</td></tr>
			<tr><td colspan="9" align="center"><input type="submit" class="button" value="'.translate('TEXT_ADD_LINE_CONST').'"></td></tr>	
		</form>';
	}
	$out.='</table>';
	return $out;
}

function phoneLinesLocalSettings($dbADO){
	
	$installationID = (int)@$_SESSION['installationID'];	
	$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
	if(is_null($telecomPlugin)){
		return '<span class="err">'.translate('TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST').'</span>';
	}
	
	$ddArray=getAssocArray('Device_DeviceData','FK_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$telecomPlugin);

	$out='
	<form action="index.php" method="POST" name="phoneSettings">
	<input type="hidden" name="section" value="phoneLines">
	<input type="hidden" name="action" value="update">
	
	<table align="center" cellpadding="3" cellspacing="0" bgcolor="#F0F3F8">
		<tr>
			<td colspan="2"><input type="checkbox" name="ckb_141" value="1" disabled="disabled" '.((@$ddArray['141']!=0 && @$ddArray['141']!='')?'checked':'').'> '.translate('TEXT_PREPEND_AREA_CODE_CONST').' <input type="text" name="value_141" value="'.@$ddArray['141'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="ckb_142" value="1" '.((@$ddArray['142']!=-1)?'checked':'').'> '.translate('TEXT_PREPEND_DIGIT_CONST').' <input type="text" name="value_142" value="'.((@$ddArray['142']==-1)?'':@$ddArray['142']).'" style="width:25px;"></td>
		</tr>
		<tr>
			<td>'.translate('TEXT_LOCAL_NUMBER_LENGTH_CONST').' <input type="text" name="value_143" value="'.@$ddArray['143'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td align="center"><input type="submit" class="button" name="update_settings" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>	
	</table>
	</form>';
	
	return $out;
}

function emergencySettings($dbADO,$astADO) {
	$installationID = (int)@$_SESSION['installationID'];	
	$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
	if(is_null($telecomPlugin)){
		return '<span class="err">'.translate('TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST').'</span>';
	}
	
	$ddArray=getAssocArray('Device_DeviceData','FK_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$telecomPlugin);

	$res=$astADO->Execute("SELECT id, name, phonenumber FROM phonelines ORDER BY id");
	while($row=$res->FetchRow()){
		$PhoneLinesList[$row['id']]=$row['name'].' - '.$row['phonenumber'];
	}

	$out='<h3 align="center">'.translate('TEXT_DIAL_EMERGENCY_NUMBERS_CONST').'</h3>
	<form action="index.php" method="POST" name="emergencySettings">
	<input type="hidden" name="section" value="phoneLines">
	<input type="hidden" name="action" value="update_emergency">
	<table align="center" cellpadding="3" cellspacing="0" bgcolor="#F0F3F8">
	<tr><td><B>'.translate('TEXT_DIAL_EMERGENCY_NUMBERS_CONST').'</B>:&nbsp<input type="text" name="value_296" value="'.@$ddArray['296'].'">
	&nbsp;<B>'.translate('TEXT_EMERGENCY_TRUNK_CONST').'</B>:&nbsp<select name="value_297"  style="width: auto">';
		foreach ($PhoneLinesList as $id => $name) $out.='<option value="'.$id.'" '.((@$ddArray['297'] == $id)?'selected="selected"':'').'>'.$name.'</option>';
		$out .='</select></td>
	</tr>
	<tr><td>'.translate('TEXT_EMERGENCY_HELP_CONST').'</td></tr>
	<tr>
		<td align="center"><input type="submit" class="button" name="update_emergency" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
	</tr>	
	</table>
	</form>';
	
	return $out;
}

function getLineState($protocol, $username){

	if($protocol == 'SIP') {
		$cmd='sudo -u root /usr/sbin/asterisk -rx "sip show registry" |  awk \'$3 == "'.$username.'" { print $5}\'';
		$response=exec_batch_command($cmd,1);
	}

	else if($protocol == 'SPA') {
		$cmd='sudo -u root /usr/sbin/asterisk -rx "sip show peers" |  awk \'$1 == "'.$username.'" { print $6}\'';
		$response=exec_batch_command($cmd,1);
		//$response='N/A';
	}

	else if($protocol == 'IAX') {
		$cmd='sudo -u root /usr/sbin/asterisk -rx "iax2 show registry" |  awk \'$3 == "'.$username.'" { print $6}\'';
		$response=exec_batch_command($cmd,1);
	}

	else if($protocol == 'GTALK') {
		$cmd='sudo -u root /usr/sbin/asterisk -rx "jabber show connections" |  awk \'/'.$username.'/ { print $4}\'';
		$response=exec_batch_command($cmd,1);
	}
	
	else { // should never happen
		$response="error";
	}
    return $response;
}

?>

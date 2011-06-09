<?php
function setLoglevels($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/installationSettings.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	$selHost=(isset($_REQUEST['host']))?$_REQUEST['host']:'';

	// from src/DCE/Logger.h
	$loglevels[1] = "LV_CRITICAL";
	$loglevels[5] = "LV_WARNING";
	$loglevels[6] = "LV_EVENTHANDLER";
	$loglevels[7] = "LV_EVENT";
	$loglevels[8] = "LV_ACTION";
	$loglevels[9] = "LV_CRITERIA";
	$loglevels[10] = "LV_STATUS";
	$loglevels[11] = "LV_EPG";
	$loglevels[12] = "LV_ALARM";
	$loglevels[13] = "LV_SOCKET";
	$loglevels[14] = "LV_LIGHTING";
	$loglevels[15] = "LV_CONTROLLER";
	$loglevels[16] = "LV_APPDAEMON";
	$loglevels[17] = "LV_CAMERA";
	$loglevels[18] = "LV_CORPCLIENT";
	$loglevels[19] = "LV_FILEREQUEST";
	$loglevels[20] = "LV_DATAGRID";
	$loglevels[21] = "LV_LOCKING";
	$loglevels[22] = "LV_REGISTRATION";
	$loglevels[23] = "LV_XINE";
	$loglevels[24] = "LV_BURNER";
	$loglevels[25] = "LV_1394";
	$loglevels[26] = "LV_SPHINX";
	$loglevels[27] = "LV_FESTIVAL";
	$loglevels[28] = "LV_IR";
	$loglevels[29] = "LV_START";
	$loglevels[30] = "LV_START_DEVICE";
	$loglevels[31] = "LV_STOP";
	$loglevels[32] = "LV_MOUSE_CLICKS";
	$loglevels[33] = "LV_EPG_UPDATE";
	$loglevels[34] = "LV_MEDIA";
	$loglevels[35] = "LV_DEBUG";
	$loglevels[36] = "LV_ZWAVE";
	$loglevels[37] = "LV_PROFILER";
	$loglevels[38] = "LV_PROCESSUTILS";
	$loglevels[39] = "LV_TV";
	$loglevels[40] = "LV_SEND_DATA";
	$loglevels[41] = "LV_RECEIVE_DATA";
	$loglevels[42] = "LV_INSTEON";
	$loglevels[99] = "LV_COLOR";

	// these are the default levels and will be bolded in the list
	$defaults[1] = true;
	$defaults[5] = true;
	$defaults[7] = true;
	$defaults[8] = true;

	// get a list of the current loglevels from pluto.conf
	if ($selHost > 1) {
		$plutoConf = "/usr/pluto/diskless/$selHost/etc/pluto.conf";
	} else {
		$plutoConf = "/etc/pluto.conf";
	}
	$cmdToSend = "sudo -u root cat $plutoConf | grep LogLevels";
	$temp = exec_batch_command($cmdToSend);
	$pieces = explode("=",$temp);
	$temp = explode(",",trim($pieces[1]));
	foreach ($temp as $level) {
		$currentLevels[$level] = true;
	}

	if ($action=='form') {		
		$out.=setLeftMenu($dbADO).'
		
		<form action="index.php" method="post" name="setLoglevels">
		<input type="hidden" name="section" value="setLoglevels">
		<input type="hidden" name="action" value="update">		
		
		<h3>'.$TEXT_SET_LOGLEVELS_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<table cellpadding="3" align="center" border="0">
			<tr>
				<td colspan="2" align="center" class="tablehead">'.$TEXT_CURRENT_LOGLEVELS_CONST.' <B></B></td>
			</tr>';
		$sql = "select PK_Device as id,Description as text from Device where FK_Installation=? and FK_Device_ControlledVia is null and FK_DeviceTemplate in (7,28)";
		$dbh = $dbADO->Execute($sql,$_SESSION['installationID']);
		if ($dbh) {
			$out .= "<tr><td><b>$TEXT_SYSTEM_CONST</b></td><td> <select name=host onChange=\"document.setLoglevels.action.value='form';document.setLoglevels.submit();\">";
			while ($row = $dbh->FetchRow()) {
				$out .= "<option value='".$row['id']."'".(($selHost==$row['id'])?' selected':'').">".$row['text']." (".$row['id'].")</option>";
			}
			$out .= "</select></td></tr>";
		}
		foreach ($loglevels AS $key => $value){
			$out.='<tr><td colspan=2><input name="loglevels['.$key.']" type="checkbox"';
			if ($currentLevels[$key]){ $out .= " checked"; }
			$out .= '> ';
			if ($defaults[$key]){ $out .= '<b>'; };
			$out .= $value;
			if ($defaults[$key]){ $out .= '</b>'; };
			$out .= '</td></tr>';
		}
		$out.='
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="reset" class="button" name="closeBtn" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
		</table>
		</form>
		
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setLoglevels&host=$selHost&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		// write out the new loglevels to pluto.conf
		$loglevels = $_REQUEST['loglevels'];
		$loglevels = "LogLevels = ".implode(",",array_keys($loglevels));
		$cmd = "sed -i -e 's/LogLevels.*/$loglevels/' $plutoConf";
		exec_batch_command("sudo -u root $cmd",1);

		$suffix=(isset($err))?'&error='.$err:'';
		$out='
		<script>
				opener.location.reload();
			 	self.location="index.php?section=setLoglevels&from='.$from.'&host='.$selHost.'&msg='.$TEXT_LOGLEVELS_UPDATED_CONST.'";
		</script>';
		die($out);
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SET_LOGLEVELS_CONST);			
	$output->output();
}
?>

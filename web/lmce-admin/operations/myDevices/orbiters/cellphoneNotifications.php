<?php
function cellphoneNotifications($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/cellphoneNotifications.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$scriptInHead='';	
	

	$deviceID=getDeviceFromDT($installationID,$GLOBALS['SecurityPlugin'],$dbADO);
	if(is_null($deviceID)){
		$_GET['error']=$TEXT_ERROR_CANNOT_FIND_SECURITY_PLUGIN_CONST;
	}

	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="POST" name="cellphoneNotifications">
	<input type="hidden" name="section" value="cellphoneNotifications">
	<input type="hidden" name="action" value="add">	
		
	<div align="center">
	<p>'.$TEXT_CELLPHONE_NOTIFICATION_INFO_CONST.'</p>
			<h3>'.$TEXT_MOBILE_ORBITER_NOTIFICATIONS_CONST.'</h3>
			<p>'.$TEXT_MOBILE_ORBITER_NOTIFICATIONS_INFO_CONST.'</p>
			';
			$monArray=array();
			if(!is_null($deviceID)){
				$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['MobileOrbiterNotification']));
				if($resDDD->RecordCount()>0){
					$rowDDD=$resDDD->FetchRow();
					$monArray=explode(',',$rowDDD['IK_DeviceData']);
				}
			}

			$phonesPulldown=getAssocArray('Device_DeviceData','FK_Device','Description',$dbADO,'INNER JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceData='.$GLOBALS['MobileOrbiterPhone'].' AND FK_Installation='.$installationID,'ORDER BY Description ASC');

		$out.='
			'.$TEXT_CALL_ORDER_CONST.': 
			<select name="mon_sequence" onchange="form_change()">
				<option value="0" '.(((int)@$monArray[0]==0)?'selected':'').'>'.$TEXT_ORBITERS_ALL_AT_ONCE_CONST.'</option>
				<option value="1" '.(((int)@$monArray[0]>0)?'selected':'').'>'.$TEXT_ORBITERS_ONE_AT_A_TIME_CONST.'</option>
			</select>
			<span id="span_mon_seconds" style="display:none">
				<input type="text" size="2" maxlength="2" name="mon_seconds" value="'.(((int)@$monArray[0]>0)?(int)@$monArray[0]:'10').'" > seconds each
			</span>
		</div>
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_ORBITER_PHONE_CONST.' #</B></td>
				<td align="center"><B>'.$TEXT_MONITOR_MODE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_SECURITY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_FIRE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_AIR_QUALITY_ALERTS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DOOR_INTERCOM_CONST.'</B></td>
			</tr>
			<input type="hidden" name="oldMon" value="'.@join(',',$monArray).'">';
			for($i=0;$i<5;$i++){
				$out.='
				<tr>
					<td align="center">'.pulldownFromArray($phonesPulldown,'tel_'.(6*$i+1),((!isset($monArray[6*$i+1]))?0:$monArray[6*$i+1]),'','key','- '.$TEXT_PLEASE_SELECT_CONST.' -','').'</td>
					<td align="center"><input type="checkbox" name="monitor_mode_'.(6*$i+2).'" value="1" '.(((int)@$monArray[6*$i+2]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="security_'.(6*$i+3).'" value="1" '.(((int)@$monArray[6*$i+3]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="fire_'.(6*$i+4).'" value="1" '.(((int)@$monArray[6*$i+4]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="air_'.(6*$i+5).'" value="1" '.(((int)@$monArray[6*$i+5]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="door_intercom_'.(6*$i+6).'" value="1" '.(((int)@$monArray[6*$i+6]>0)?'checked':'').'></td>
				</tr>';
			}
		$out.='
			<tr>
				<td colspan="6">* '.$TEXT_WARNING_ORBITER_NO_PHONE_NUMBER_CONST.'</td>
			</tr>
		</table>
		
		<div align="center">
		<h3>'.$TEXT_OTHER_PHONE_NOTIFICATIONS_CONST.'</h3>
		<p>'.$TEXT_OTHER_PHONE_NOTIFICATIONS_INFO_CONST.'</p>
		';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['OtherPhoneNotifications']));
			$opnArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$opnArray=explode(',',$rowDDD['IK_DeviceData']);
			}
			
			$out.='
			<select name="opn_order">
				<option value="0" '.(((int)@$opnArray[1]==0)?'selected':'').'>'.$TEXT_ORBITERS_THEN_OTHER_CONST.'</option>
				<option value="1" '.(((int)@$opnArray[1]==1)?'selected':'').'>'.$TEXT_OTHER_THEN_ORBITERS_CONST.'</option>
				<option value="2" '.(((int)@$opnArray[1]==2)?'selected':'').'>'.$TEXT_ORBITERS_AND_OTHER_CONST.'</option>
			</select>
			<select name="opn_sequence" onchange="form_change()">
				<option value="0" '.(((int)@$opnArray[0]==0)?'selected':'').'>'.$TEXT_OTHER_ALL_AT_ONCE_CONST.'</option>
				<option value="1" '.(((int)@$opnArray[1]>0)?'selected':'').'>'.$TEXT_OTHER_ONE_AT_A_TIME_CONST.'</option>
			</select>
			<span id="span_opn_seconds" style="display:none">
				<input type="text" size="2" maxlength="2" name="opn_seconds" value="'.(((int)@$opnArray[0]>0)?@$opnArray[0]:'10').'" > '.$TEXT_SECONDS_EACH_CONST.'
			</span>
		</div>
		<input type="hidden" name="oldOpn" value="'.@join(',',$opnArray).'">
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_ORBITER_PHONE_CONST.' #</B></td>
				<td align="center"><B>'.$TEXT_MONITOR_MODE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_SECURITY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_FIRE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_AIR_QUALITY_ALERTS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DOOR_INTERCOM_CONST.'</B></td>
			</tr>';
			for($i=0;$i<5;$i++){
				$out.='
				<tr>
					<td align="center"><input type="text" name="opn_tel_'.(6*$i+2).'" value="'.@$opnArray[6*$i+2].'"></td>
					<td align="center"><input type="checkbox" name="opn_monitor_mode_'.(6*$i+3).'" value="1" '.(((int)@$opnArray[6*$i+3]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="opn_security_'.(6*$i+4).'" value="1" '.(((int)@$opnArray[6*$i+4]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="opn_fire_'.(6*$i+5).'" value="1" '.(((int)@$opnArray[6*$i+5]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="opn_air_'.(6*$i+6).'" value="1" '.(((int)@$opnArray[6*$i+6]>0)?'checked':'').'></td>
					<td align="center"><input type="checkbox" name="opn_door_intercom_'.(6*$i+7).'" value="1" '.(((int)@$opnArray[6*$i+7]>0)?'checked':'').'></td>
				</tr>';
			}
		$out.='
		</table>
		<div align="center">
			<h3>'.$TEXT_NEIGHBOURS_TO_CALL_CONST.'</h3>
			<p>'.$TEXT_NEIGHBOURS_TO_CALL_INFO_CONST.'</p>
		</div>
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_NAME_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_NUMBER_CONST.'</B></td>
			</tr>';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['NeighborstoCall']));
			$ntcArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$ntcArray=explode(',',$rowDDD['IK_DeviceData']);
			}

			for ($i=0; $i < 4; $i++){
				$out.='
				<tr>
					<td><input type="text" name="nname_'.(2*$i).'" value="'.@$ntcArray[2*$i].'" ></td>
					<td><input type="text" name="nphone_'.(2*$i+1).'" value="'.@$ntcArray[2*$i+1].'" ></td>
				</tr>';
			}
		$out.='
		<input type="hidden" name="oldNtc" value="'.join(',',$ntcArray).'">
		</table>';

		$out.='
		<div align="center">
			<h3>'.$TEXT_EMERGENCY_CALLS_CONST.'</h3>
			<p>'.$TEXT_EMERGENCY_CALLS_INFO_CONST.'</p>
		</div>
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_NAME_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_NUMBER_CONST.'</B></td>
			</tr>';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['EmergencyCalls']));
			$ecArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$ecArray=explode(',',$rowDDD['IK_DeviceData']);
			}

			for ($i=0; $i < 4; $i++){
				$out.='
				<tr>
					<td><input type="text" name="ecname_'.(2*$i).'" value="'.@$ecArray[2*$i].'" ></td>
					<td><input type="text" name="ecphone_'.(2*$i+1).'" value="'.@$ecArray[2*$i+1].'" ></td>
				</tr>';
			}
		$out.='
		<input type="hidden" name="oldEc" value="'.join(',',$ecArray).'">
		</table>';
		
		if(!is_null($deviceID)){
			$out.='<div align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></div>';
		}
	$out.='</form>';
	$scriptInHead='
	<script language="javascript" type="text/javascript">
		function form_change()
		{
			try{
				document.getElementById(\'span_mon_seconds\').style.display = (document.cellphoneNotifications.mon_sequence.value===\'1\') ? \'\' : \'none\';
				document.getElementById(\'span_opn_seconds\').style.display = (document.cellphoneNotifications.opn_sequence.value===\'1\') ? \'\' : \'none\';
			}catch(e){
				// do nothing
			}
		}
		</script>';
	
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=cellphoneNotifications&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$oldMon=$_POST['oldMon'];
		$mon_sequence=((int)$_POST['mon_sequence']>0)?(int)$_POST['mon_seconds']:(int)$_POST['mon_sequence'];
		$MONDeviceData=$mon_sequence;
		for($i=0;$i<5;$i++){
			$tel=cleanString($_POST['tel_'.(6*$i+1)]);
			$tel=($tel=='0')?'':$tel;
			$monitor_mode=(int)@$_POST['monitor_mode_'.(6*$i+2)];
			$security=(int)@$_POST['security_'.(6*$i+3)];
			$fire=(int)@$_POST['fire_'.(6*$i+4)];
			$air=(int)@$_POST['air_'.(6*$i+5)];
			$door_intercom=(int)@$_POST['door_intercom_'.(6*$i+6)];
			if($tel!=''){
				$MONDeviceData.=",$tel,$monitor_mode,$security,$fire,$air,$door_intercom";
			}
		}

		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($MONDeviceData,$deviceID,$GLOBALS['MobileOrbiterNotification']));

		$oldOpn=$_POST['oldOpn'];
		$opn_sequence=((int)$_POST['opn_sequence']>0)?(int)$_POST['opn_seconds']:(int)$_POST['opn_sequence'];
		$opn_order=(int)$_POST['opn_order'];
		
		$OPNDeviceData=$opn_sequence.','.$opn_order;
		for($i=0;$i<5;$i++){
			$tel=cleanString($_POST['opn_tel_'.((6*$i+2))]);
			$monitor_mode=(int)@$_POST['opn_monitor_mode_'.((6*$i+3))];
			$security=(int)@$_POST['opn_security_'.((6*$i+4))];
			$fire=(int)@$_POST['opn_fire_'.((6*$i+5))];
			$air=(int)@$_POST['opn_air_'.((6*$i+6))];
			$door_intercom=(int)@$_POST['opn_door_intercom_'.((6*$i+7))];
			if($tel!=''){
				$OPNDeviceData.=",$tel,$monitor_mode,$security,$fire,$air,$door_intercom";
			}
		}
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($OPNDeviceData,$deviceID,$GLOBALS['OtherPhoneNotifications']));

		$oldNtc=$_POST['oldNtc'];
		$NTCDeviceDataArray=array();
		for($i=0;$i<4;$i++){
			$name=@$_POST['nname_'.(2*$i)];
			$nphone=@$_POST['nphone_'.(2*$i+1)];
			if($name!='' && $nphone!='')
				$NTCDeviceDataArray[]=$name.','.$nphone;
		}
		
		$NTCDeviceData=join(',',$NTCDeviceDataArray);
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($NTCDeviceData,$deviceID,$GLOBALS['NeighborstoCall']));

		
		$oldEc=$_POST['oldEc'];
		$ECDeviceDataArray=array();
		for($i=0;$i<4;$i++){
			$name=@$_POST['ecname_'.(2*$i)];
			$nphone=@$_POST['ecphone_'.(2*$i+1)];
			if($name!='' && $nphone!='')
				$ECDeviceDataArray[]=$name.','.$nphone;
		}
		
		$ECDeviceData=join(',',$ECDeviceDataArray);
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($ECDeviceData,$deviceID,$GLOBALS['EmergencyCalls']));
		
		# Run generate_users_voicemenu.pl so we update the phone system with the changes
		$commandToSend='sudo -u root /usr/pluto/bin/generate_users_voicemenu.pl';
		exec_batch_command($commandToSend);

		header("Location: index.php?section=cellphoneNotifications&msg=$TEXT_CELLPHONE_NOTIFICATION_UPDATED_CONST");	
		exit();	
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_CELLPHONE_NOTIFICATIONS_CONST);
	$output->setScriptInHead($scriptInHead);
	$output->setScriptInBody('onLoad="form_change()"');
	$output->setNavigationMenu(array($TEXT_CELLPHONE_NOTIFICATIONS_CONST=>'index.php?section=cellphoneNotifications'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CELLPHONE_NOTIFICATIONS_CONST);
	$output->output();
}
?>

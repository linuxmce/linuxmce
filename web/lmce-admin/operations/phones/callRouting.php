<?
function callRouting($output,$dbADO,$asteriskADO,$telecomADO) {
	global 	$stepsArray;

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$stepsArray=array(
		'ring'=>$TEXT_RING_EXTENSION_CONST,
		'user'=>$TEXT_TRANSFER_TO_ANOTHER_USER_CONST,
		'voicemail'=>$TEXT_GOTO_VOICEMAIL_CONST,
		'transfer'=>$TEXT_TRANSFER_TO_AN_OUTSIDE_NUMBER_CONST);
	
	$scriptInHead='
	<script>
	function changeOptions(val){
		eval("sel_step=document.callRouting.routing_"+val+".value");
		stepsArray=new Array("ring","user","voicemail","transfer");
		for(i=0;i<4;i++){
			new_visib=(sel_step==stepsArray[i])?"":"none";
			eval("document.getElementById(\'div_"+stepsArray[i]+"_"+val+"\').style.display=\'"+new_visib+"\'");
		}
	}
	</script>
	';
	

	if($action=='form'){
		$out=getCallBeforeForm($dbADO);
		$out.=getUsersRoutingTable($dbADO,$telecomADO);
	}elseif($action=='step'){
		$out=addStepForm($dbADO,$telecomADO);
	}elseif($action=='update_call_before'){
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=callRouting&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}			
		
		$timeout=(int)@$_POST['timeout'];
		$timeout=($timeout<0)?0:$timeout;
		$telecomPlugin=getTelecomPlugin($installationID,$dbADO);

		set_device_data($telecomPlugin,$GLOBALS['call_before_timeout'],$timeout,$dbADO);
		$cmd='sudo -u root /usr/pluto/bin/create_pluto_dialplan.pl';
		exec_batch_command($cmd);
	
		/*
		$phones=explode(',',$_POST['phones']);
		$phonesToRing=array();
		foreach ($phones As $phoneID){
			if(isset($_REQUEST['phone_'.$phoneID]) && (int)$_REQUEST['phone_'.$phoneID]!=0){
				$phonesToRing[]=(int)$_REQUEST['phone_'.$phoneID];
			}
		}
		$callBeforePhones=(count($phonesToRing)==0)?0:join(',',$phonesToRing);
	
		
		set_device_data($telecomPlugin,$GLOBALS['call_before_phones'],$callBeforePhones,$dbADO);
		*/
		
		header('Location: index.php?section=callRouting');
		exit();
		
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=callRouting&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}			
		
		if(isset($_REQUEST['add'])){
			processAddStep($telecomADO);
		}
		
		if(isset($_REQUEST['did']) && (int)$_REQUEST['did']!=0){
			deleteStep((int)$_REQUEST['did'],$telecomADO);
		}
		if(isset($_REQUEST['mdid']) && (int)$_REQUEST['mdid']!=0){
			moveStep((int)$_REQUEST['mdid'],$telecomADO);
		}
		
		if(isset($_REQUEST['update'])){
			updateSteps($telecomADO);
		}
		
		header('Location: index.php?section=callRouting');
	}

	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($TEXT_CALL_ROUTING_CONST);
	$output->setScriptInHead(@$scriptInHead);
	$output->setNavigationMenu(array($TEXT_CALL_ROUTING_CONST=>'index.php?section=callRouting'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}


function getUsersRoutingTable($dbADO,$telecomADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
	
	$out='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		';
	$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);	
	$userModes=getAssocArray('UserMode','PK_UserMode','Description',$dbADO,'','ORDER BY Description ASC');
	
	$out.='
	<table align="center" cellpadding="2" cellspacing="0">';
	foreach ($users AS $userID=>$userName){
		$out.='
		<tr>
			<td colspan="2" align="center" class="tablehead"><B>'.$userName.'</B></td>
		</tr>
		<tr bgcolor="#EEEEEE">
			<td><B>'.$TEXT_USER_MODE_CONST.'</B></td>
			<td><B>'.$TEXT_CALLER_TYPE_CONST.'</B></td>
		</tr>';
		$pos=0;
		foreach ($userModes AS $mode=>$modeName){
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			
			$out.='
			<tr bgcolor="'.$color.'">
				<td rowspan="2">'.$modeName.'</td>
				<td><a href="index.php?section=callRouting&userID='.$userID.'&mode='.$mode.'&pr=0&action=step">Normal caller</a></td>
			</tr>
			<tr bgcolor="'.$color.'">
				<td><a href="index.php?section=callRouting&userID='.$userID.'&mode='.$mode.'&pr=1&action=step">Priority caller</a></td>
			</tr>			';
		}
		$out.='	
		<tr>
			<td colspan="2" align="center">&nbsp;</td>
		</tr>
		
		';
	}
	$out.='	
	</table>';
	
	return $out;
}



function addStepForm($dbADO,$telecomADO){
	global $stepsArray;

	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
		
	$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);
	$userModes=getAssocArray('UserMode','PK_UserMode','Description',$dbADO,'','ORDER BY Description ASC');
	$callerType=array(0=>'normal caller',1=>'priority caller');


	$userID=$_REQUEST['userID'];
	$selectedUserMode=$_REQUEST['mode'];
	$selectedCaller=$_REQUEST['pr'];
	$selectedStep=(isset($_REQUEST['routing']))?$_REQUEST['routing']:'ring';
		
	$out='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<form action="index.php" method="POST" name="callRouting">
			<input type="hidden" name="section" value="callRouting">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="userID" value="'.$userID.'">
			<input type="hidden" name="mode" value="'.$selectedUserMode.'">
			<input type="hidden" name="pr" value="'.$selectedCaller.'">
	
			<table align="center" cellpadding="2" cellspacing="0">
				<tr bgcolor="#EEEEEE">
					<td><B>'.$TEXT_USER_CONST.': </B></td>
					<td>'.$users[$userID].'</td>
				</tr>
				<tr>
					<td><B>'.$TEXT_USER_STATUS_CONST.'</B></td>
					<td>'.$userModes[$selectedUserMode].'</td>
				</tr>		
				<tr>
					<td><B>'.$TEXT_CALLER_TYPE_CONST.'</B></td>
					<td>'.$callerType[$selectedCaller].'</td>
				</tr>
				<tr bgcolor="#EEEEEE">
					<td colspan="2" align="center"><B>'.$TEXT_CALL_ROUTING_STEPS_CONST.'</B></td>
				</tr>	
				<tr>
					<td colspan="2" align="center">'.stepsTable($userID,$selectedUserMode,$selectedCaller,$telecomADO,$dbADO).'</td>
				</tr>	
				<tr>
					<td colspan="2" align="center">&nbsp;</td>
				</tr>	
				<tr bgcolor="#EEEEEE">
					<td colspan="2" align="center"><B>'.$TEXT_ADD_CALL_ROUTING_STEP_CONST.'</B></td>
				</tr>	
				<tr>
					<td colspan="2"><input type="checkbox" name="SendStatusFirst" value="1"> '.$TEXT_TELL_USER_STATUS_VOICEMAIL_CONST.'</td>
				</tr>		
				<tr>
					<td><B>'.$TEXT_ADD_NEW_STEP_CONST.'</B></td>
					<td>'.pulldownFromArray($stepsArray,'routing_',$selectedStep,'onChange="changeOptions(\'\')"','key','').'</td>
				</tr>		
				<tr>
					<td align="right" width="100">&nbsp;</td>
					<td>'.step_options($selectedStep,$userID,$dbADO).'</td>
				</tr>	
				<tr bgcolor="#EEEEEE">
					<td colspan="2" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>			
			</table>
		
		</form>
		';
	
	return $out;
}

function stepsTable($userID,$selectedUserMode,$selectedCaller,$telecomADO,$dbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
			
	$res=$telecomADO->Execute('SELECT * FROM UserRouting WHERE EK_Users=? AND EK_UserMode=? AND IsPriorityCaller=? ORDER BY StepOrder ASC',array($userID,$selectedUserMode,$selectedCaller));
	if($res->RecordCount()==0){
		return 	$TEXT_NO_RECORDS_CONST;
	}
	$stepsArray=array(
		'ring'=>$TEXT_RING_EXTENSION_CONST,
		'user'=>$TEXT_TRANSFER_TO_ANOTHER_USER_CONST,
		'voicemail'=>$TEXT_GOTO_VOICEMAIL_CONST,
		'transfer'=>$TEXT_TRANSFER_TO_AN_OUTSIDE_NUMBER_CONST);

		
	$pos=0;
	$out='
	<table width="100%" cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td colspan="3" align="center"><B><B>STEP</B></td>
			<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
		</tr>
	';
	
	$userStepsArray=array();
	while($row=$res->FetchRow()){
		$userStepsArray[]=$row['PK_UserRouting'];
		$selectedStep=substr(@$row['Routing'],0,strpos(@$row['Routing'],','));	
		$oldValues=substr(@$row['Routing'],strpos(@$row['Routing'],',')+1);	
		
		$pos++;
		$moveDownBtn=($pos!=$res->RecordCount())?'<br><input type="button" class="button_fixed" name="move_down" value="'.$TEXT_MOVE_DOWN_CONST.'" onClick="self.location=\'index.php?section=callRouting&action=move&userID='.$userID.'&mode='.$selectedUserMode.'&pr='.$selectedCaller.'&mdid='.$row['PK_UserRouting'].'&oldOrder='.$row['StepOrder'].'\'">':'';
		$out.='
		<tr>
			<td width="50"><B>Step '.$pos.'</B></td>
			<td>'.pulldownFromArray($stepsArray,'routing_'.$row['PK_UserRouting'],$selectedStep,'onChange="changeOptions(\''.$row['PK_UserRouting'].'\')"','key','').'</td>
			<td>'.step_options($selectedStep,$row['EK_Users'],$dbADO,$row['PK_UserRouting'],$oldValues).'</td>
			<td align="center"><input type="button" class="button_fixed" name="del" value="'.$TEXT_DELETE_CONST.'" onclick="if(confirm(\''.$TEXT_DELETE_STEP_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=callRouting&action=del&userID='.$userID.'&mode='.$selectedUserMode.'&pr='.$selectedCaller.'&did='.$row['PK_UserRouting'].'\'"> '.$moveDownBtn.'</td>
		</tr>
		<tr>
			<td colspan="4" align="center"><hr></td>
		</tr>
		
		';
	}
	$out.='	
		<tr bgcolor="#EEEEEE">
			<td colspan="4" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
		</tr>	
	</table>
	<input type="hidden" name="userStepsArray" value="'.join(',',$userStepsArray).'">
	';
	
	return $out;
}

function step_options($selected,$user,$dbADO,$suffix='',$oldValues=''){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
	
	global 	$stepsArray;

	$out='';
	foreach ($stepsArray AS $currentStep=>$stepName){
		$styleHidden=($selected==$currentStep)?'':'style="display:none"';
		switch ($currentStep){
			case 'ring':
				$GLOBALS['Room_Name']=1;
				$phones=getDevicesArrayFromCategory($GLOBALS['rootPhones'],$dbADO);
				$GLOBALS['Room_Name']=0;
				$oldValuesArray=explode(',',$oldValues);
				$div_ring='';
				foreach ($phones AS $phoneID=>$phoneName){
					$div_ring.='<input type="checkbox" name="phone_'.$phoneID.'_'.$suffix.'" value="1" '.(($oldValues=='' || in_array($phoneID,$oldValuesArray))?'checked':'').'> '.$phoneName.'<br>';
				}
				$div_ring.='<input type="hidden" name="phones_'.$suffix.'" value="'.join(',',array_keys($phones)).'">';
				$out.='<div id="div_ring_'.$suffix.'" '.$styleHidden.'>'.$div_ring.'</div>';
			break;
			case 'user':
				$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID'].' AND PK_Users!='.$user);
				$div_user=pulldownFromArray($users,'user_'.$suffix,(int)@$oldValues,'','key','');
				$out.='<div id="div_user_'.$suffix.'" '.$styleHidden.'>'.$div_user.'</div>';
			break;		
			case 'voicemail':
				// if old value is 0, set it to userID to be displayed as selected
				$oldValues=((int)@$oldValues==0)?$user:(int)@$oldValues;
				$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);
				$div_voicemail=pulldownFromArray($users,'voicemail_'.$suffix,(int)@$oldValues,'','key','');
				$out.='<div id="div_voicemail_'.$suffix.'" '.$styleHidden.'>'.$div_voicemail.'<br>'.$TEXT_VOICEMAIL_INFO_CONST.'</div>';
			break;		
			case 'transfer':
				$div_transfer='<input type="text" name="transfer_phone_'.$suffix.'" value="'.@$oldValues.'">';
				$out.='<div id="div_transfer_'.$suffix.'" '.$styleHidden.'>'.$div_transfer.'</div>';
			break;		
			default:
				$out.='&nbsp;';
			break;
		}
	}
	
	
	return $out;
}


function processAddStep($telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
	
	$selectedUser=$_REQUEST['userID'];
	$userMode=$_REQUEST['mode'];
	$SendStatusFirst=(int)@$_REQUEST['SendStatusFirst'];
	$priorityCaller=(int)$_REQUEST['pr'];
	
	$mode_selected=$_REQUEST['routing_'];
	switch ($mode_selected) {
		case 'ring':
			$phones=explode(',',$_REQUEST['phones_']);
			$phonesToRing=array();
			foreach ($phones As $phoneID){
				if(isset($_REQUEST['phone_'.$phoneID.'_'])){
					$phonesToRing[]=$phoneID;
				}
			}
			$routingValue='ring,'.join(',',$phonesToRing);
		break;
		case 'user':
			$user=$_REQUEST['user_'];
			$routingValue='user,'.$user;
		break;
		case 'voicemail':
			$user=$_REQUEST['voicemail_'];
			$routingValue='voicemail,'.$user;
		break;
		case 'transfer':
			$routingValue='transfer,'.$_REQUEST['transfer_phone_'];
		break;
		default:
		break;
	}

	$StepOrder=getLastStep($selectedUser,$userMode,$priorityCaller,$telecomADO);
	// insert
	$telecomADO->Execute('
		INSERT INTO UserRouting 
			(EK_Users,EK_UserMode,SendStatusFirst,IsPriorityCaller,StepOrder,Routing) 
		VALUES 
			(?,?,?,?,?,?)',
	array($selectedUser,$userMode,$SendStatusFirst,$priorityCaller,$StepOrder,$routingValue));
	
	$cmd='sudo -u root /usr/pluto/bin/create_pluto_dialplan.pl';
	exec_batch_command($cmd);
	
	header("Location: index.php?section=callRouting&action=step&userID=$selectedUser&mode=$userMode&pr=$priorityCaller&msg=$TEXT_CALL_ROUTING_STEP_ADDED_CONST");
	exit();
}

// get last available step for selected user, usermode and priority caller
function getLastStep($selectedUser,$userMode,$priorityCaller,$telecomADO){
	$res=$telecomADO->Execute('SELECT StepOrder FROM UserRouting WHERE EK_Users=? AND EK_UserMode=? AND IsPriorityCaller=? ORDER BY StepOrder DESC LIMIT 0,1',array($selectedUser,$userMode,$priorityCaller));
	if($res->RecordCount()==0){
		return 1;
	}else{
		$row=$res->FetchRow();
		return ($row['StepOrder']+1);
	}
}

function deleteStep($did,$telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
		
	$selectedUser=$_REQUEST['userID'];
	$userMode=$_REQUEST['mode'];
	$SendStatusFirst=(int)@$_REQUEST['SendStatusFirst'];
	$priorityCaller=(int)$_REQUEST['pr'];
	
	$telecomADO->Execute('DELETE FROM UserRouting WHERE PK_UserRouting=?',$did);

	$cmd='sudo -u root /usr/pluto/bin/create_pluto_dialplan.pl';
	exec_batch_command($cmd);
		
	header("Location: index.php?section=callRouting&action=step&userID=$selectedUser&mode=$userMode&pr=$priorityCaller&msg=$TEXT_CALL_ROUTING_STEP_DELETED_CONST");
	exit();
	
}

function moveStep($mdid,$telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
		
	$selectedUser=$_REQUEST['userID'];
	$userMode=$_REQUEST['mode'];
	$SendStatusFirst=(int)@$_REQUEST['SendStatusFirst'];
	$priorityCaller=(int)$_REQUEST['pr'];
	$oldOrder=(int)$_REQUEST['oldOrder'];
	
	$toSwitch=getFirstStep($oldOrder,$selectedUser,$userMode,$priorityCaller,$telecomADO);
	
	if($toSwitch!==false){
		$oldID=$toSwitch[0];
		$newOrder=$toSwitch[1];
		$telecomADO->Execute('UPDATE UserRouting SET StepOrder=? WHERE PK_UserRouting=?',array($newOrder,$mdid));
		$telecomADO->Execute('UPDATE UserRouting SET StepOrder=? WHERE PK_UserRouting=?',array($oldOrder,$oldID));
		
		$cmd='sudo -u root /usr/pluto/bin/create_pluto_dialplan.pl';
		exec_batch_command($cmd);
				
		header("Location: index.php?section=callRouting&action=step&userID=$selectedUser&mode=$userMode&pr=$priorityCaller");
		exit();
	}
	
}

// get first step higher than current value
function getFirstStep($current,$selectedUser,$userMode,$priorityCaller,$telecomADO){
	$res=$telecomADO->Execute('SELECT PK_UserRouting,StepOrder FROM UserRouting WHERE EK_Users=? AND EK_UserMode=? AND IsPriorityCaller=? AND StepOrder>? ORDER BY StepOrder ASC LIMIT 0,1',array($selectedUser,$userMode,$priorityCaller,$current));
	if($res->RecordCount()==0){
		// is the last or the only one
		return false;
	}else{
		$row=$res->FetchRow();
		return array($row['PK_UserRouting'],$row['StepOrder']);
	}
}

function updateSteps($telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callRouting.lang.php');
	
	$selectedUser=$_REQUEST['userID'];
	$userMode=$_REQUEST['mode'];
	$SendStatusFirst=(int)@$_REQUEST['SendStatusFirst'];
	$priorityCaller=(int)$_REQUEST['pr'];

	$userStepsArray=explode(',',$_REQUEST['userStepsArray']);
	foreach ($userStepsArray AS $usrID){
		$mode_selected=$_REQUEST['routing_'.$usrID];
		switch ($mode_selected) {
			case 'ring':
				$phones=explode(',',$_REQUEST['phones_'.$usrID]);
				$phonesToRing=array();
				foreach ($phones As $phoneID){
					if(isset($_REQUEST['phone_'.$phoneID.'_'.$usrID])){
						$phonesToRing[]=$phoneID;
					}
				}
				$routingValue='ring,'.join(',',$phonesToRing);
			break;
			case 'user':
				$user=$_REQUEST['user_'.$usrID];
				$routingValue='user,'.$user;
			break;
			case 'voicemail':
				$user=$_REQUEST['voicemail_'.$usrID];
				$routingValue='voicemail,'.$user;
			break;
			case 'transfer':
				$routingValue='transfer,'.$_REQUEST['transfer_phone_'.$usrID];
			break;
			default:
			break;
		}
	
		// insert
		$telecomADO->Execute('UPDATE UserRouting SET Routing=? WHERE PK_UserRouting=?',	array($routingValue,$usrID));
	}
		
	$cmd='sudo -u root /usr/pluto/bin/create_pluto_dialplan.pl';
	exec_batch_command($cmd);
	
	header("Location: index.php?section=callRouting&action=step&userID=$selectedUser&mode=$userMode&pr=$priorityCaller&msg=$TEXT_CALL_ROUTING_STEP_UPDATE_CONST");
	exit();
}

function getCallBeforeForm($dbADO){
	$installationID = (int)@$_SESSION['installationID'];
	$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
	$timeout=getDeviceData($telecomPlugin,$GLOBALS['call_before_timeout'],$dbADO);
	
	/* removed
	$selectedPhones=getDeviceData($telecomPlugin,$GLOBALS['call_before_phones'],$dbADO);
	
	$phones=getDevicesArrayFromCategory($GLOBALS['rootPhones'],$dbADO);
	if(count($phones)>0){
		$extensions=getAssocArray('Device_DeviceData','FK_Device','IK_DeviceData',$dbADO,'WHERE FK_Device IN ('.join(',',array_keys($phones)).') AND FK_DeviceData='.$GLOBALS['PhoneNumber']);
	}
	
	$oldValuesArray=explode(',',$selectedPhones);
	
	$phoneList='';
	foreach ($phones AS $phoneID=>$phoneName){
		$phoneList.='<input type="checkbox" name="phone_'.$phoneID.'" value="'.$extensions[$phoneID].'" '.(($selectedPhones=='' || in_array($extensions[$phoneID],$oldValuesArray))?'checked':'').'> '.((!isset($extensions[$phoneID]) || (int)$extensions[$phoneID]==0)?'<span class="err">'.$phoneName.'</span>':$extensions[$phoneID].' - '.$phoneName).'<br>';
	}
	$phoneList.='<input type="hidden" name="phones" value="'.join(',',array_keys($phones)).'">';
	*/
	
	$out='
		<form action="index.php" method="POST" name="callRouting_callBefore">
			<input type="hidden" name="section" value="callRouting">
			<input type="hidden" name="action" value="update_call_before">
	
			<table align="center" cellpadding="2" cellspacing="0">
				<tr class="alternate_back">
					<td><B># of seconds to ring before ivr (0=none)</b></td>
					<td><input type="text" name="timeout" value="'.$timeout.'" size="3"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>				
			</table>
		
		</form>
		';
	
	return $out;
	
}
?>
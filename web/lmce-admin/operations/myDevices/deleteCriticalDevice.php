<?php
function deleteCriticalDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteCriticalDevice.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = isset($_REQUEST['deviceID'])?cleanInteger($_REQUEST['deviceID']):0;
	if($deviceID==0){
		header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=$TEXT_ERROR_INVALID_ID_CONST");
		exit();
	}
	
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
		exit();
	}

	if($action=='form'){
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="deleteCriticalDevice">
	<input type="hidden" name="section" value="deleteCriticalDevice">
	<input type="hidden" name="action" value="del">	
	<input type="hidden" name="deviceID" value="'.$deviceID.'">
		
		<font color="red">
		<h2 align="center">'.$TEXT_DELETE_CRITICAL_DEVICE_NOTICE_CONST.'</h2><br>
		<h3 align="center"><B>'.$TEXT_DELETE_CRITICAL_DEVICE_CONFIRMATION_CONST.'</B></h3>
		<table width="50%" align="center">
			<tr>
				<td align="center">'.$TEXT_CONFIRM_BY_PASSWORD_CONST.':<br><br> <input type="password" name="confirmPassword"><br><br></td>
			</tr>
			<tr>
				<td align="center"><input type="submit" class="button" name="del" value="'.$TEXT_CONFIRMATION_DELETE_CONST.'"> <input type="button" class="button" name="cancel" value="'.$TEXT_NO_DELETE_CONST.'" onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$deviceID.'\'"></td>
			</tr>
		</table>
		</font>
	</form>';
		
	}else{
		$confirmPassword=$_POST['confirmPassword'];
		if($confirmPassword!=$_SESSION['password']){
			header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=$TEXT_ERROR_PASSWORD_NOMATCH_CONST");
			exit();
		}
		$deviceFields=getFieldsAsArray('Device','FK_DeviceTemplate,IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$deviceID);
		if($deviceFields['FK_DeviceTemplate'][0]==$GLOBALS['rootMediaDirectorsID']){
			$cmd='sudo -u root /usr/pluto/bin/Diskless_DeleteFS.sh "'.$deviceFields['IPaddress'][0].'" "'.$deviceFields['MACaddress'][0].'"';
			exec($cmd);
		}
		
		deleteDevice($deviceID,$dbADO);
		$cmd='sudo -u root /usr/pluto/bin/DHCP_config.sh';
		exec($cmd);
		setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);

		$out.='
	<script>
		alert("'.$TEXT_CRITICAL_DEVICES_DELETED_CONST.'");
		top.frames["treeframe"].location="index.php?section=leftMenu";
		document.location.href="index.php?section=myDevices&action=showBasicInfo";
	</script>
	<noscript>
		Device deleted! Please <a href="index.php?section=myDevices&action=showBasicInfo">click here</a> to go to main area!
	</noscript>
	';
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DELETE_CRITICAL_DEVICE_CONST);
	$output->output();
}

<?
function restart($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/restart.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';

	
	if($action=='form'){
		$queryDevice='
			SELECT Device.*
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_DeviceCategory=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['CategoryCore'],$_SESSION['installationID']));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
		}

		$MDArray=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
		$out.=setLeftMenu($dbADO).'
		<script>
		function confirmAction()
		{
			if(confirm(\'Do you confirm the action?\')){
				return true;
			}else
				return false;
		}
		</script>
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<form action="index.php" method="POST" name="restart" onSubmit="return confirmAction();">
		<input type="hidden" name="section" value="restart">
		<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" align="center">
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="quick_reload" value="'.$TEXT_QUICK_RELOAD_ROUTER_CONST.'"></td>
			</tr>
			<tr>
				<td colspan="6" align="center">&nbsp;</td>
			</tr>		
			<tr bgcolor="#EEEEEE">
				<td colspan="3"><B>'.$TEXT_RESTART_CORE_CONST.'</B></td>
				<td><input type="submit" class="button" name="halt_core" value="'.$TEXT_HALT_CONST.'"></td>
				<td><input type="submit" class="button" name="suspend_core" value="'.$TEXT_SUSPEND_CONST.'"></td>
				<td><input type="submit" class="button" name="reboot_core" value="'.$TEXT_REBOOT_CONST.'"></td>
			</tr>
			<tr bgcolor="#DDDDDD">
				<td colspan="6" align="center"><B>'.$TEXT_MEDIA_DIRECTORS_CONST.'</B></td>
			</tr>		
			<tr bgcolor="#EEEEEE">
				<td>&nbsp;</td>
				<td colspan="2" align="center"><B>'.$TEXT_BOOT_FROM_CONST.'</B></td>
				<td colspan="3"></td>
			</tr>		
		
		';

		if(count($MDArray)>0){
			$mdParents=getAssocArray('Device','PK_Device','FK_Device_ControlledVia',$dbADO,'WHERE PK_Device IN ('.join(',',array_keys($MDArray)).')');
			foreach($MDArray AS $mdID=>$description){
				if(@$mdParents[$mdID]!=$coreID){
					$out.='
					<tr>
						<td><B>'.$description.'</B></td>
						<td><input type="radio" name="boot_'.$mdID.'" value="V" onClick="self.location=\'index.php?section=restart&action=V&device='.$mdID.'\'"> '.$TEXT_HDD_CONST.' </td>
						<td><input type="radio" name="boot_'.$mdID.'" value="net" onClick="self.location=\'index.php?section=restart&action=N&device='.$mdID.'\'"> '.$TEXT_NET_CONST.'</td>
						<td><input type="submit" class="button" name="halt_'.$mdID.'" value="'.$TEXT_HALT_CONST.'"></td>
						<td><input type="submit" class="button" name="suspend_'.$mdID.'" value="'.$TEXT_SUSPEND_CONST.'"></td>
						<td><input type="submit" class="button" name="reboot_'.$mdID.'" value="'.$TEXT_REBOOT_CONST.'"></td>
					</tr>			
					';
				}
			}
		}
		$out.='
		</table>
			<input type="hidden" name="coreID" value="'.$coreID.'">
			<input type="hidden" name="MDArray" value="'.join(',',array_keys($MDArray)).'">
		</form>';
	}else{
	// process area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation) {	
			header('Location: index.php?section=restart&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}
		$coreID=(int)@$_POST['coreID'];
		$MDArray=explode(',',@$_POST['MDArray']);

		if(isset($_POST['halt_core'])){
			$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 27 1 323 21 H 2 '.$coreID;
			exec_batch_command($command);
		}

		if(isset($_POST['suspend_core'])){
			$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 27 1 323 21 S 2 '.$coreID;
			exec_batch_command($command);
		}

		if(isset($_POST['reboot_core'])){
			$command='sudo -u root /sbin/reboot';
			exec_batch_command($command);
		}

		foreach ($MDArray AS $mdID){
			if(isset($_POST['halt_'.$mdID])){
				$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 27 1 323 21 H 2 '.$mdID;
				exec_batch_command($command);
			}
	
			if(isset($_POST['suspend_'.$mdID])){
				$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 27 1 323 21 S 2 '.$mdID;
				exec_batch_command($command);
			}
	
			if(isset($_POST['reboot_'.$mdID])){
				// extract IP
				$mdData=getAssocArray('Device','PK_Device','IPAddress',$dbADO,'WHERE PK_Device='.$mdID);
				$command="sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '$mdData[$mdID]' 'reboot'";
				exec_batch_command($command);
			}
		}
		
		if(isset($_REQUEST['device']) && (int)$_REQUEST['device']!=0){
			$device=(int)$_REQUEST['device'];
			$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 27 1 323 21 '.$action.' 2 '.$device;
			exec_batch_command($command);
		}

		if(isset($_POST['quick_reload'])){
			$command='/usr/pluto/bin/MessageSend localhost 0 -1000 7 1';
			exec_batch_command($command);
		}

		header('Location: index.php?section=restart&msg='.$TEXT_COMMAND_SENT_CONST.' '.$command);
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_RESTART_CONST);
	$output->setNavigationMenu(array($TEXT_RESTART_CONST=>'index.php?section=restart'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_RESTART_CONST);
	$output->output();
}


?>

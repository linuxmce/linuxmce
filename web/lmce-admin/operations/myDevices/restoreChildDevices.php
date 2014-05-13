<?php
function restoreChildDevices($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/restoreChildDevices.lang.php');

	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	if($deviceID==0){
		error_redirect('ERROR: Device ID not specified.');
	}

	

	
	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.@$_GET['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="restoreChildDevices" enctype="multipart/form-data">
			
	<input type="hidden" name="section" value="restoreChildDevices">
	<input type="hidden" name="action" value="restore">	
	<input type="hidden" name="deviceID" value="'.$deviceID.'">	
			
		<table width="400">
			<tr>
				<td align="center"><input type="file" name="restore_file" value=""></td>
			</tr>
			<tr>
				<td align="center"><input type="submit" name="restore" class="button" value="'.$TEXT_RESTORE_CHILD_DEVICES_CONST.'"></td>
			</tr>		
		</table>		
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=restoreChildDevices&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		if(isset($_POST['restore'])){
			$tmpfile=$_FILES['restore_file']['tmp_name'];
			$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h dcerouter -C '.$deviceID.' -m '.$tmpfile;
			$response=exec_batch_command($cmd,1);
			
			header("Location: index.php?section=restoreChildDevices&deviceID=$deviceID&msg=".urlencode(nl2br($response)));
			exit();
		}
		
		header("Location: index.php?section=restoreChildDevices&deviceID=$deviceID");
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_RESTORE_CHILD_DEVICES_CONST);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_RESTORE_CHILD_DEVICES_CONST=>'index.php?section=restoreChildDevices'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>

<?php
function unknownDevices($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/unknownDevices.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$res=$dbADO->Execute('SELECT * FROM UnknownDevices');
	
	if ($action == 'form') {
		
	$out.=setLeftMenu($dbADO).'
	<div class="err">'.@$_GET['error'].'</div>
	<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
	<form action="index.php" method="POST" name="unknownDevices">
				
	<input type="hidden" name="section" value="unknownDevices">
	<input type="hidden" name="action" value="add">	';
	if($res->RecordCount()==0){
		$out.=$TEXT_NO_RECORDS_CONST;
	}else{
		$out.='<table cellpadding="3" cellspacing="0">
			<tr class="tablehead">
				<td align="center"><B>#</B></td>
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td><B>'.$TEXT_MAC_ADDRESS_CONST.'</B></td>
				<td><B>'.$TEXT_IP_ADDRESS_CONST.'</B></td>
				<td><B>'.$TEXT_VENDOR_MODEL_ID_CONST.'</B></td>
				<td><B>'.$TEXT_SERIAL_NUMBER_CONST.'</B></td>
				<td><B>'.$TEXT_DEVICE_CONST.'</B></td>
				<td><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>		
		';
		$pos=0;
		while($row=$res->FetchRow()){
			$class=($pos%2==0)?'alternate_back':'';
			$out.='
			<tr class="'.$class.'">
				<td>'.$row['PK_UnknownDevices'].'</td>
				<td>'.$row['Description'].'</td>
				<td>'.$row['MacAddress'].'</td>
				<td>'.$row['IPAddress'].'</td>
				<td>'.$row['VendorModelId'].'</td>
				<td>'.$row['SerialNumber'].'</td>
				<td>'.$row['FK_Device_PC'].'</td>
				<td><input type="submit" class="button" name="unlock_'.$row['PK_UnknownDevices'].'" value="'.$TEXT_UNLOCK_CONST.'" onClick="if(!confirm(\''.$TEXT_UNLOCK_CONFIRMATION_CONST.'\'))return false;"></td>
			</tr>';
			$pos++;
		}
		$out.='<t/able>';
	}
	$out.='	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=unknownDevices&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		while($row=$res->FetchRow()){
			if(isset($_POST['unlock_'.$row['PK_UnknownDevices']])){
				$dbADO->Execute('DELETE FROM UnknownDevices WHERE PK_UnknownDevices=?',array($row['PK_UnknownDevices']));

				header("Location: index.php?section=unknownDevices&msg=$TEXT_DEVICE_UNLOCKED_CONST");
				exit(0);
			}
		}
		
		header("Location: index.php?section=unknownDevices&msg=".urlencode($msg));
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_UNKNOWN_DEVICES_CONST);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_UNKNOWN_DEVICES_CONST=>'index.php?section=unknownDevices'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>

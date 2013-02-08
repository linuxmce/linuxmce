<?
function editModules($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editModules.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$installationID = (int)$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = (int)@$_REQUEST['deviceID'];
	
	
	if ($action=='form') {
		$out.='
		<h3>'.$TEXT_EDIT_SOFTWARE_MODULES_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
				
	<div class="err"><br>'.@$_GET['error'].'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="editModules">
			<input type="hidden" name="section" value="editModules">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			
			'.getInstallWizardDeviceTemplates(6,$dbADO,$deviceID,1,1).'
			<div align="center"><input type="submit" class="button" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></div>
		</form>
	';	

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editModules&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		$orbiterMDChild=getMediaDirectorOrbiterChild($deviceID,$dbADO);


		$installOptionsArray=explode(',',@$_POST['displayedTemplatesRequired_'.$deviceID]);
		foreach($installOptionsArray AS $elem){
			$oldDevice=@$_POST['oldDevice_'.$deviceID.'_requiredTemplate_'.$elem];
			$optionalDevice=(isset($_POST['device_'.$deviceID.'_requiredTemplate_'.$elem]))?$_POST['device_'.$deviceID.'_requiredTemplate_'.$elem]:0;
			if($optionalDevice!=0){
				$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
				if($oldDevice==''){
					// hard-coded: for BlueTooth dongle I create the device as child of media director instead of osd
					$parent=($elem==$GLOBALS['BluetoothDongle'])?$deviceID:$orbiterMDChild;

					//$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$elem.' -i '.$installationID.' -C '.$parent;
					//$insertID=exec_batch_command($cmd);
					$insertID=createDevice($elem,$installationID,$parent,NULL,$dbADO);

					$dbADO->Execute('UPDATE Device SET Description=?,FK_Room=? WHERE PK_Device=?',array($OptionalDeviceName,$room,$insertID));
				}
			}else{
				if($oldDevice!=''){
					deleteDevice($oldDevice,$dbADO);
				}
			}
		}

		
		header("Location: index.php?section=editModules&from=$from&deviceID=$deviceID&from=$from&msg=$TEXT_MODULES_UPDATE_CONST");
	}
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_SOFTWARE_MODULES_CONST);
	$output->output();
}


?>
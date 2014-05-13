<?php
function editCameraRelated($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editCameraRelated.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$installationID = (int)$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = (int)@$_REQUEST['deviceID'];
	$type=@$_REQUEST['type'];

	$GLOBALS['Room_Name']=2; //specify order for getDevicesArrayFromCategory()

	if ($action=='form') {
		$fields=getFieldsAsArray('Device','Description',$dbADO,'WHERE PK_Device='.$deviceID);
		switch ($type){
			case 'lights':
				$devicesArray=getDevicesArrayFromCategory($GLOBALS['rootLights'],$dbADO);
			break;
			case 'sensors':
				$devicesArray=getDevicesArrayFromCategory($GLOBALS['rootSecurity'],$dbADO);
			break;
			default:
				die('Type not specified');
			break;
		}
		$devicesRelatedFrom=getAssocArray('Device_Device_Related','FK_Device','FK_Device',$dbADO,'WHERE FK_Device_Related='.$deviceID);
		$devicesRelatedTo=getAssocArray('Device_Device_Related','FK_Device_Related','FK_Device_Related',$dbADO,'WHERE FK_Device='.$deviceID);
		$checkedDevices=array_keys($devicesRelatedFrom+$devicesRelatedTo);
			

			
		$out.='
		<p><h3>'.$TEXT_ASSOCIATED_DEVICES_CONST.' '.$TEXT_DEVICE_CONST.' #'.$deviceID.' ['.$fields['Description'][0].']</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
				
	<div class="err"><br>'.@$_GET['error'].'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="editCameraRelated">
			<input type="hidden" name="section" value="editCameraRelated">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="type" value="'.$type.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">';
			if(count($devicesArray)>0){
				$saveBtn='<input type="submit" class="button" value="'.$TEXT_SAVE_CONST.'">';
				$out.='<table align="center" cellpadding="2" cellspacing="0">';
				foreach ($devicesArray AS $device=>$name ){
					$out.='
					<tr>
						<td><input type="checkbox" name="device_'.$device.'" value="1" '.((in_array($device,$checkedDevices))?'checked':'').'></td>
						<td>'.$name.'</td>
					</tr>';
				}
				$out.='</table>
				<input type="hidden" name="devices" value="'.join(',',array_keys($devicesArray)).'">';
			}else{
				$out.=$TEXT_NO_DEVICES_TO_ASSOCIATE_WITH_CONST;
			}
			
			$out.='
			<div align="center">'.$saveBtn.' <input type="button" class="button" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></div>
		</form>
	';	

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editCameraRelated&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$devices=explode(',',$_POST['devices']);
		foreach ($devices AS $device){
			if(isset($_POST['device_'.$device])){
				$dbADO->Execute('INSERT IGNORE INTO Device_Device_Related (FK_Device,FK_Device_Related) VALUES (?,?)',array($device,$deviceID));
			}else{
				$dbADO->Execute('DELETE FROM Device_Device_Related WHERE FK_Device=? AND FK_Device_Related=?',array($device,$deviceID));
				$dbADO->Execute('DELETE FROM Device_Device_Related WHERE FK_Device=? AND FK_Device_Related=?',array($deviceID,$device));
			}
		}
		
		$script='
		<script>
			opener.document.forms[0].action.value="form";
			opener.document.forms[0].submit();
			self.location="index.php?section=editCameraRelated&from='.$from.'&deviceID='.$deviceID.'&from='.$from.'&type='.$type.'&msg='.$TEXT_ASSOCIATED_DEVICES_UPDATED_CONST.'";
		</script>
		';
		die($script);
	}
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ASSOCIATED_DEVICES_CONST);
	$output->output();
}


?>

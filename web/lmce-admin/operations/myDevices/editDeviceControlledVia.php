<?
function editDeviceControlledVia($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDeviceControlledVia.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$installationID = (int)$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = (int)@$_REQUEST['deviceID'];
	$GLOBALS['DT_&_Room']=1;
	
	if ($action=='form') {
		$data=getFieldsAsArray('Device','Device.Description AS Description,FK_DeviceTemplate,FK_DeviceCategory,FK_Device_ControlledVia',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device='.$deviceID);
		$controlledByPulldown=controlledViaPullDown('controlledBy',$deviceID,$data['FK_DeviceTemplate'][0],$data['FK_DeviceCategory'][0],$data['FK_Device_ControlledVia'][0],$dbADO);
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
		</script>	
		<h3>'.$TEXT_EDIT_DEVICE_CONTROLLED_VIA_CONST.' #'.$deviceID.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
				
	<div class="err"><br>'.@$_GET['error'].'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="editDeviceControlledVia">
			<input type="hidden" name="section" value="editDeviceControlledVia">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">

			<table align="center">
				<tr class="tablehead">
					<td align="center">'.$TEXT_DEVICE_CONST.'</td>
					<td  align="center">'.$TEXT_PARENT_CONST.'</td>
				</tr>
				<tr>
					<td><B>'.$data['Description'][0].'</B></td>
					<td>'.$controlledByPulldown.'</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>				
			</table>
		</form>
	';	

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editDeviceControlledVia&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		if(isset($_POST['save'])){
			$controlledBy=((int)$_POST['controlledBy']>0)?(int)$_POST['controlledBy']:NULL;

			updateDeviceControlledBy($deviceID,$controlledBy,$dbADO);
		}
		
		$script='
		<script>
			opener.document.forms[0].action.value="form";
			opener.document.forms[0].submit();
			self.location="index.php?section=editDeviceControlledVia&from='.$from.'&deviceID='.$deviceID.'&msg='.$TEXT_PARENT_SAVED_CONST.'";
		</script>
		';
		die($script);		
	}
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DEVICE_CONTROLLED_VIA_CONST);
	$output->output();
}
?>
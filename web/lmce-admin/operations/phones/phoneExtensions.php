<?php
function phoneExtensions($output,$dbADO) {
	// obsolete file
		
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.=setLeftMenu($dbADO).'
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<h3 align="center">Phone Extensions</h3>
		<p align="center">LinuxMCE compatible telephones are normally plug-and-play.  Just plug them into any jack--and they will be added to this page automatically.</p>
		<form action="index.php" method="POST" name="phoneExtensions">
			<input type="hidden" name="section" value="phoneExtensions">
			<input type="hidden" name="action" value="update">
		<table align="center" cellpadding="3">
			<tr class="tablehead">
				<td><B>Phone Name</B></td>
				<td><B>Device Template</B></td>
				<td><B>Phone Number</B></td>
			</tr>';
		
		$deviceCategory=$GLOBALS['rootPhones'];

		$phonesDTArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO);
		if(count($phonesDTArray)==0)
			$phonesDTArray[]=0;
		
		$queryPhones='
			SELECT PK_Device,Device.Description AS DeviceName, DeviceTemplate.Description AS TemplateName, IK_DeviceData
			FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN Device_DeviceData ON FK_Device=PK_Device
			WHERE FK_DeviceTemplate IN ('.join(',',$phonesDTArray).') AND FK_Installation=? AND FK_DeviceData=?
			ORDER BY DeviceName ASC';
		$resPhones=$dbADO->Execute($queryPhones,array($installationID,$GLOBALS['PhoneNumber']));
		$pos=0;
		$displayedDevicesArray=array();
		while($rowPhones=$resPhones->FetchRow()){
			$displayedDevicesArray[]=$rowPhones['PK_Device'];
			$pos++;
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
					<td><B>'.$rowPhones['DeviceName'].'</B></td>
					<td>'.$rowPhones['TemplateName'].'</td>
					<td><input type="text" name="phoneNumber_'.$rowPhones['PK_Device'].'" value="'.$rowPhones['IK_DeviceData'].'"></td>
				</tr>
			';
		}
		if(count($displayedDevicesArray)>0){
			$out.='
				<tr>
					<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
			';
		}
		$out.='
		</table>
			<input type="hidden" name="displayedDevicesArray" value="'.join(',',$displayedDevicesArray).'">
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=phoneExtensions&error=You are not authorised to change the installation.");
			exit(0);
		}	
		if(isset($_POST['update']) || $action=='externalSubmit'){
			$displayedDevicesArray=explode(',',$_POST['displayedDevicesArray']);
			foreach ($displayedDevicesArray AS $deviceID){
				$phoneNo=@$_POST['phoneNumber_'.$deviceID];
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($phoneNo,$deviceID,$GLOBALS['PhoneNumber']));
			}
			$sync=exec('/usr/pluto/bin/SynchronizeAsterisk.sh',$tmpArray);

			if($sync!=0){
				header("Location: index.php?section=phoneExtensions&error=Synchronisation failed.");
				exit();
			}
			header("Location: index.php?section=phoneExtensions&msg=Phone numbers updated.");
			exit();

		}
		
		
		header('Location: index.php?section=phoneExtensions');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>

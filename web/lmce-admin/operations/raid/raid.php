<?
function raid($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/raid.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
	<form action="index.php" method="POST" name="raid">
	<input type="hidden" name="section" value="raid">
	<input type="hidden" name="action" value="update">	

	<table>
		<tr class="tablehead">
			<td align="center"><b>'.$TEXT_ID_CONST.'</b></td>
			<td align="center"><b>'.$TEXT_PARENT_CONST.'</b></td>
			<td align="center"><b>'.$TEXT_RAID_CONST.'</b></td>
			<td align="center"><b>'.$TEXT_RAID_TYPE_CONST.'</b></td>
			<td align="center"><B>'.$TEXT_BLOCK_DEVICE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_NO_OF_DRIVES_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_SIZE_CONST.'</B></td>
			<td align="center"><b>'.$TEXT_RAID_STATUS_CONST.'</b></td>
			<td align="center"><B>'.$TEXT_RAID_FORMAT_STATUS_CONST.'</B></td>
			<td align="center"><b>'.$TEXT_ACTION_CONST.'</b></td>
		</tr>';
		$pos=0;
		$computers=getDevicesArrayFromCategory($GLOBALS['rootComputerID'],$dbADO);
		$raidCategories=getDescendantsForCategory($GLOBALS['RaidCategory'],$dbADO);
		$raidTemplates=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_DeviceCategory IN ('.join(',',$raidCategories).')');
		
		$fields='
		Device.FK_DeviceTemplate,
		Device.PK_Device,
		Device.Description AS Description,
		DeviceTemplate.Description AS Type,
		DDD1.IK_DeviceData AS Status,
		DDD2.IK_DeviceData AS RAIDStatus,
		DDD3.IK_Devicedata AS NoOfDisks,
		DDD4.IK_DeviceData AS BlockDevice,
		DDD5.IK_DeviceData AS RaidSize,
		Parent.Description AS ParentName,
		Parent.IPAddress AS ParentIP';
				
		$join='
		INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN Device_DeviceData DDD1 on DDD1.FK_Device=Device.PK_Device AND DDD1.FK_DeviceData='.$GLOBALS['State'].' 
		LEFT JOIN Device_DeviceData DDD2 on DDD2.FK_Device=Device.PK_Device AND DDD2.FK_DeviceData='.$GLOBALS['RAIDStatus'].' 
		LEFT JOIN Device_DeviceData DDD3 on DDD3.FK_Device=Device.PK_Device AND DDD3.FK_DeviceData='.$GLOBALS['NoofDisks'].'
		LEFT JOIN Device_DeviceData DDD4 on DDD4.FK_Device=Device.PK_Device AND DDD4.FK_DeviceData='.$GLOBALS['BlockDevice'].'
		LEFT JOIN Device_DeviceData DDD5 on DDD5.FK_Device=Device.PK_Device AND DDD5.FK_DeviceData='.$GLOBALS['DriveSize'].'
		LEFT JOIN Device Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia';			
		$data=getFieldsAsArray('Device',$fields,$dbADO,$join.' WHERE FK_DeviceCategory IN ('.join(',',$raidCategories).')');
		if(count($data)==0){
			$out.='
			<tr class="alternate_back">
				<td colspan="9">'.$TEXT_NO_RECORDS_CONST.'</td>
			</tr>';
		}

		$raidDevices=array();
		for ($i=0;$i<count(@$data['PK_Device']);$i++){
			$refreshStatusButton=($data['RAIDStatus'][$i]==0)?'':'<input type="button" class="button_fixed" name="drives_'.$data['PK_Device'][$i].'" value="Refresh status"  onClick="self.location=\'index.php?section=raid&deviceID='.$data['PK_Device'][$i].'&action=refresh\';"><br>';
			$raidDevices[]=$data['PK_Device'][$i];
			$bgColor=($data['RAIDStatus'][$i]==0)?'bgcolor="lightgreen"':'class="alternate_back"';
			$out.='
			<input type="hidden" name="parentIP_'.$data['PK_Device'][$i].'" value="'.$data['ParentIP'][$i].'">
			<tr '.$bgColor.'>
				<td>'.$data['PK_Device'][$i].'</td>
				<td>'.$data['ParentName'][$i].'</td>
				<td>'.$data['Description'][$i].'</td>
				<td>'.$data['Type'][$i].'</td>
				<td align="center">'.$data['BlockDevice'][$i].'</td>
				<td align="center">'.$data['NoOfDisks'][$i].'</td>
				<td align="center">'.$data['RaidSize'][$i].'</td>
				<td align="center">'.$data['Status'][$i].'</td>
				<td align="center" style="width:200px"><iframe src="index.php?section=raidFormatStatus&raidID='.$data['PK_Device'][$i].'" style="width:210px;height:22px;border:0;"></iframe></td>
				<td>
				<input type="button" class="button_fixed" name="drives_'.$data['PK_Device'][$i].'" value="'.$TEXT_DRIVES_CONST.'"  onClick="self.location=\'index.php?section=raidDrives&deviceID='.$data['PK_Device'][$i].'\';"><br>
				'.$refreshStatusButton.'
				<input type="button" class="button_fixed" name="edit_'.$data['PK_Device'][$i].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$data['PK_Device'][$i].'\';"><br>
				<input type="submit" class="button_fixed" name="delete_'.$data['PK_Device'][$i].'" value="'.$TEXT_DELETE_CONST.'"  onClick="if(!confirm(\'Are you sure you want to delete this device?\'))return false;">
				
				</td>
			</tr>';
		}
		
		$usersArray=array(-1=>$TEXT_USE_PLUTO_DIRECTORY_STRUCTURE_CONST)+getAssocArray('Users','PK_Users','Username',$dbADO,'','ORDER BY Username ASC');
		
		$out.='
		</table>
			<input type="hidden" name="raidDevices" value="'.join(',',$raidDevices).'">
		</form>
	
		<form action="index.php" method="POST" name="addRaid">
		<input type="hidden" name="section" value="raid">
		<input type="hidden" name="action" value="update">	

		<em>'.$TEXT_GREEN_LABEL_INFO_CONST.'</em><br><br>
		<table cellpadding="2" cellspacing="0">
					<tr>
						<td colspan="2" class="tablehead" align="center"><B>'.$TEXT_ADD_RAID_CONST.'</B></td>
					</tr>
					<tr>
						<td><B>'.$TEXT_PARENT_CONST.'</B></td>
						<td>'.pulldownFromArray($computers,'computer',0).'</td>
					</tr>
					<tr>
						<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
						<td><input type="text" name="description" value=""></td>
					</tr>
					<tr>
						<td><B>'.$TEXT_TYPE_CONST.'</B></td>
						<td>'.pulldownFromArray($raidTemplates,'template',0).'</td>
					</tr>
					<tr>
						<td><B>'.$TEXT_USE_AUTOMATICALLY_CONST.'</B></td>
						<td><input type="checkbox" name="use_automaticly" value="1" checked></td>
					</tr>
					<tr>
						<td><B>'.$TEXT_USE_PLUTO_DIRECTORY_STRUCTURE_CONST.'</B></td>
						<td>'.pulldownFromArray($usersArray,'use_pluto_directory_structure',-1,'','key','Public').'</td>
					</tr>
					
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
					</tr>
				</table>
	</form>

	<script>
	 	var frmvalidator = new formValidator("addRaid"); 			
		frmvalidator.addValidation("description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
		frmvalidator.addValidation("computer","dontselect=0","'.$TEXT_PARENT_REQUIRED_CONST.'");
		frmvalidator.addValidation("template","dontselect=0","'.$TEXT_TYPE_REQUIRED_CONST.'");
	</script>
	';
	}elseif ($action=='refresh'){
		// send sommand to update raid status
		// /usr/pluto/bin/refresh_status_RAID.sh <DEVICE_ID>
		$deviceID=@$_REQUEST['deviceID'];
		$cmd='sudo -u root /usr/pluto/bin/refresh_status_RAID.sh  "'.$deviceID.'"';
		$ret=exec_batch_command($cmd,1);

		header("Location: index.php?section=raid");
		exit();
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=raid&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		if(isset($_POST['add']) && (int)@$_POST['template']!=0){
			$description=cleanString($_POST['description']);
			$parent=(int)$_REQUEST['computer'];
			$use_automaticly=(int)@$_POST['use_automaticly'];
			$use_pluto_directory_structure=(int)@$_POST['use_pluto_directory_structure'];
			
			$raidID=createDevice((int)@$_POST['template'],$installationID,$parent,NULL,$dbADO);
			$dbADO->Execute('UPDATE Device SET Description=? WHERE PK_Device=?',array($description,$raidID));
			set_device_data($raidID,$GLOBALS['use_automaticly_dd'],$use_automaticly,$dbADO);
			set_device_data($raidID,$GLOBALS['DD_PK_Users'],$use_pluto_directory_structure,$dbADO);
			
			header("Location: index.php?section=raidDrives&deviceID=$raidID&msg=".urlencode(@$TEXT_RAID_ADDED_CONST));
			exit();
		}
		
		$raidDevices=explode(',',$_POST['raidDevices']);
		foreach ($raidDevices AS $device){
			if(isset($_POST['delete_'.$device])){
				$parentIP=$_REQUEST['parentIP_'.$device];
				
				$cmd='sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '.$parentIP.' "/usr/pluto/bin/delete_raid.sh '.$device.'"';
				$ret=exec_batch_command($cmd,1);
				
				// delete moved in delete_raid.sh
				deleteDevice($device,$dbADO);
			}
		}
		
		header("Location: index.php?section=raid&msg=".urlencode(@$msg));
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_RAID_CONST);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_RAID_CONST=>'index.php?section=raid'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>

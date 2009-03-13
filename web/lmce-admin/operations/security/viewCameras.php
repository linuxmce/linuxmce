<?
function viewCameras($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/viewCameras.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootCameras'];

	$displayedDevices=array();
	
	$cameras=getDescendantsForCategory($deviceCategory,$dbADO);
	$queryDevice='
		SELECT Device.Description AS DeviceName, DeviceTemplate.Description AS Template, PK_Device, Room.Description AS RoomName, PK_Room
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room 
		WHERE Device.FK_Installation=? AND FK_DeviceCategory IN ('.join(',',$cameras).')
		ORDER BY RoomName ASC, DeviceName ASC';	
	$resDevice=$dbADO->Execute($queryDevice,array($installationID));	
	
	if($action=='form'){
	
		$camerasByRoom=array();
		$roomNames=array(0=>$TEXT_NO_ROOM_CONST);
		$devicesArray=array();
		while($row=$resDevice->FetchRow()){
			if(!is_null($row['PK_Room'])){
				$roomNames[$row['PK_Room']]=$row['RoomName'];
			}
			
			$devicesArray[]=$row['PK_Device'];
			$camerasByRoom[(int)$row['PK_Room']][$row['PK_Device']]['Description']=$row['DeviceName'];
			$camerasByRoom[(int)$row['PK_Room']][$row['PK_Device']]['Template']=$row['Template'];
		}

		$out.='
		<script>
		function selAllCheckboxes()
		{
		   val=(document.viewCameras.setAll.checked)?true:false;
		   for (i = 0; i < document.viewCameras.elements.length; i++)
		   {
		     if (document.viewCameras.elements[i].type == "checkbox")
		     {
		         document.viewCameras.elements[i].checked = val;
		     }
		   }
		}
		</script>
		
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="viewCameras">
	<input type="hidden" name="section" value="viewCameras">
	<input type="hidden" name="action" value="update">';
	if($resDevice->RecordCount()>0){

		$interval=(int)$_REQUEST['interval'];
		$interval=($interval<=0)?5:$interval;
		
	$out.='		
<table align="center" cellpading="3" cellspacing="3" border="0" width="100%">
<tr>
	<td width="200">
	<table align="center" cellpading="3" cellspacing="3" border="0">
		<tr>
			<td align="center" bgcolor="#D1D9EA"><B>'.$TEXT_REFRESH_INTERVAL_CONST.'</B></td>
		</tr>
		<tr>
			<td align="center"><input type="text" name="interval" value="'.$interval.'" style="width:30px;"> s</td>
		</tr>		
	';
	foreach ($camerasByRoom AS $roomID=>$data){
		$out.='
			<tr>
				<td align="center" bgcolor="#D1D9EA"><B>'.$roomNames[$roomID].'</B></td>
			</tr>';
		foreach ($data AS $deviceID=>$info){
			$out.='
			<tr>
				<td align="left"><input type="checkbox" name="camera_'.$deviceID.'" '.((isset($_POST['camera_'.$deviceID]))?'checked':'').'> <B>'.$info['Description'].'</B></td>
			</tr>';
		}
	}
	if(count($devicesArray)>0){
		$out.='
			<tr>
				<td><input type="checkbox" name="setAll" onClick="selAllCheckboxes()" '.((isset($_POST['setAll']))?'checked':'').'> <B>Check/Uncheck all</B></td>
			</tr>
			<tr>
				<td><input type="button" class="button" name="preview" onClick="document.viewCameras.action.value=\'form\';document.viewCameras.submit();" Value="'.$TEXT_PREVIEW_CHECKED_CAMERAS_CONST.'"></td>
			</tr>';
	}
	$out.='
	</table>
	<input type="hidden" name="devicesArray" value="'.join(',',$devicesArray).'">
	</td>
	<td align="center" class="alternate_back">';
	if(!isset($_POST['devicesArray']) || @$_POST['devicesArray']==''){
		$out.=$TEXT_NO_CAMERA_SELECTED_CONST;
	}else{
		$out.='<B>'.$TEXT_SELECTED_CAMERAS_CONST.':</B>
		<table>';
		$cameras=explode(',',$_POST['devicesArray']);
		foreach ($cameras AS $cameraid){
			if(isset($_POST['camera_'.$cameraid])){
				$out.='
				<tr>
					<td><fieldset><legend><B># '.$cameraid.'</B></legend><IFRAME src="viewCamera.php?cameraID='.$cameraid.'" style="width:660px; height:500px;"></IFRAME></td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>';
			}
		}
	}
	
	$out.='</td>
</tr>
</table>';
	}else{
		$out.=$TEXT_NO_CAMERA_DEVICES_IN_INSTALLATION_CONST;
	}
	$out.='
	</form>';
	}else{
		// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=viewCameras&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
	}

	$output->setMenuTitle($TEXT_SECURITY_CONST.' |');
	$output->setPageTitle($TEXT_VIEW_CAMERAS_CONST);
	$output->setNavigationMenu(array($TEXT_VIEW_CAMERAS_CONST=>'index.php?section=viewCameras'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_VIEW_CAMERAS_CONST);
	$output->output();
}
?>

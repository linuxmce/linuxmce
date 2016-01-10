<?php
function devicesAV($output,$dbADO) {
	// obsolete file
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	

	$queryRooms='SELECT * FROM Room ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		$roomArray[]=$rowRoom['Description'];
		$roomIDArray[]=$rowRoom['PK_Room'];
	}
	
	if ($action == 'form') {
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="devicesAV">
	<input type="hidden" name="section" value="devicesAV">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h3>Audio / Video</h3></div>
		<table align="center">
				<tr>
					<td align="center"><B>DeviceCategory</B></td>
					<td align="center"><B>DeviceTemplate</B></td>
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Room</B></td>
					<td align="center"><B>Audio/Video</B></td>
					<td align="center"><B>Controlled by</B></td>
					<td align="center">&nbsp;</td>
				</tr>';				
			$displayedDevices=array();
			$queryDevice='
				SELECT 
					Device.*,
					DeviceTemplate.Description AS TemplateName,
			 		DeviceCategory.Description AS CategoryName
				FROM Device
					INNER JOIN DeviceTemplate ON 
						FK_DeviceTemplate=PK_DeviceTemplate 
					INNER JOIN DeviceCategory
						ON FK_DeviceCategory=PK_DeviceCategory
				WHERE
					IsAVDevice=1 AND FK_Installation=?';
			$dbADO->debug=true;	
			$resDevice=$dbADO->Execute($queryDevice,$installationID);

			while($rowD=$resDevice->FetchRow()){
				$displayedDevices[]=$rowD['PK_Device'];
				$out.='
				<tr>
					<td align="center">'.$rowD['CategoryName'].'</td>
					<td align="center">'.$rowD['TemplateName'].'</td>
					<td align="center"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"></td>
					<td><select name="room_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
				foreach($roomIDArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}

			$out.='		</select></td>
					<td align="center"><B>Audio/Video</B></td>
					<td align="center"><B>Controlled by</B></td>
					<td align="center"><input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  ></td>
				</tr>';
			}
			$out.='
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="update" value="Update"  ></td>
				</tr>';
			}
			$out.='
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("devicesAV");
//			frmvalidator.addValidation("Description","req","Please enter a device description");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}

		header("Location: index.php?section=devices&type=$type");		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

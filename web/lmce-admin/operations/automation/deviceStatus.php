<?
function deviceStatus($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deviceStatus.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<form action="index.php" method="POST" name="deviceStatus">
			<input type="hidden" name="section" value="deviceStatus">
			<input type="hidden" name="action" value="update">
		<table cellpadding="4" cellspacing="1" border="0" align="center">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_DEVICE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_STATE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_STATUS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_IP_ADDRESS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_MAC_ADDRESS_CONST.'</B></td>
			</tr>';
		$queryDevices='
			SELECT PK_Device, Device.Description AS DeviceName, State,Status, IPaddress,MACaddress,Room.Description AS RoomName, 
			Manufacturer.Description AS ManufName, DeviceTemplate.Description AS TemplateName, DeviceCategory.Description AS CategoryName
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE Device.FK_Installation=?
			ORDER BY RoomName ASC, DeviceName ASC';
		$resDevices=$dbADO->Execute($queryDevices,$installationID);
		$initRoom='undefined';
		$pos=0;
		while($rowDevices=$resDevices->FetchRow()){
			$pos++;
			if($rowDevices['RoomName']!=$initRoom){
				$out.='
				<tr class="tablehead">
					<td colspan="5"><B>'.(($rowDevices['RoomName']=='')?$TEXT_UNASSIGNED_T_A_ROOM_CONST:$rowDevices['RoomName']).'</B></td>
				</tr>';
				$initRoom=$rowDevices['RoomName'];
			}
			$out.='
				<tr class="'.(($pos%2==0)?'alternate_back':'').'">
					<td title="'.$TEXT_DEVICE_TEMPLATE_CONST.': '.$rowDevices['TemplateName'].' '.$TEXT_MANUFACTURER_CONST.': '.$rowDevices['ManufName'].' Device Category: '.$rowDevices['CategoryName'].'"><a href="index.php?section=editDeviceParams&deviceID='.$rowDevices['PK_Device'].'">'.$rowDevices['DeviceName'].'</a></td>
					<td>'.$rowDevices['State'].'</td>
					<td>'.$rowDevices['Status'].'</td>
					<td>'.$rowDevices['IPaddress'].'</td>
					<td>'.$rowDevices['MACaddress'].'</td>
				</tr>
			';
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=deviceStatus');
	}

	$output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
	$output->setPageTitle($TEXT_DEVICE_STATUS_CONST);
	$output->setNavigationMenu(array($TEXT_DEVICE_STATUS_CONST=>'index.php?section=deviceStatus'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DEVICE_STATUS_CONST);
	$output->output();
}
?>

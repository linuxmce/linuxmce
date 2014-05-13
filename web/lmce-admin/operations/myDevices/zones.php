<?php
function zones($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/zones.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'security';	// default, security if no other is selected
	
	switch ($type){
		case 'security':
			$title=$TEXT_SECURITY_CONST;
			$typeInt=1;
			$deviceCategory=$GLOBALS['rootSecurity'];
		break;
		default:
			$title=$TEXT_SECURITY_ZONES_CONST;
			$typeInt=1;
			$deviceCategory=$GLOBALS['rootSecurity'];
		break;
	}
	// get device templates for selected category
	$DTArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO);
	if(count($DTArray)==0)
		$DTArray[]=0;	// to evoid error if there are no DT in category
	
	if ($action == 'form') {
		$res=$dbADO->Execute('SELECT * FROM DeviceGroup WHERE FK_Installation=? AND Type=?',array($installationID,$typeInt));
		$tableRowsArray=array();
		$deviceGroupArray=array();
		while($row=$res->FetchRow()){
			$deviceGroupArray[]=$row['PK_DeviceGroup'];
			$zoneDevices=$dbADO->Execute('
				SELECT Device.Description,PK_Device,FK_Device,Room.Description AS Room
				FROM Device
				LEFT JOIN Room ON FK_Room=PK_Room
				LEFT JOIN Device_DeviceGroup ON FK_Device=PK_Device AND FK_DeviceGroup=?
				WHERE FK_DeviceTemplate IN ('.join(',',$DTArray).')
				ORDER BY Device.Description ASC',$row['PK_DeviceGroup']);
			$inZoneArray=array();
			$outZoneArray=array();
			while($rowD=$zoneDevices->FetchRow()){
				if(is_null($rowD['FK_Device']))
					$outZoneArray[$rowD['PK_Device']]=$rowD['Description'].' ['.((is_null($rowD['Room']))?$TEXT_ZONES_NO_ROOM_CONST:$rowD['Room']).']';
				else
					$inZoneArray[$rowD['PK_Device']]='<a href="javascript:document.zones.did.value='.$rowD['PK_Device'].';document.zones.ddg.value='.$row['PK_DeviceGroup'].';document.zones.submit();">'.$rowD['Description'].'</a>';
			}
			$devicesInZone=(count($inZoneArray)>0)?join(', ',$inZoneArray):$TEXT_NO_DEVICES_IN_THIS_ZONE_CONST;
			
			$tableRowsArray[]='
				<tr>
					<td><input type="text" name="dg_'.$row['PK_DeviceGroup'].'" value="'.$row['Description'].'"></td>
					<td>'.$devicesInZone.'</td>
					<td>'.pulldownFromArray($outZoneArray,'addTo_'.$row['PK_DeviceGroup'],0).'</td>
					<td><input type="submit" class="button" name="zone_'.$row['PK_DeviceGroup'].'" value="'.$TEXT_DELETE_ZONE_CONST.'" onclick="if(!confirm(\''.$TEXT_DELETE_ZONE_CONFIRMATION_CONST.'\'))return false; else{document.zones.dzone.value='.$row['PK_DeviceGroup'].';}"></td>
				</tr>';
		}
		if($res->RecordCount()==0){
			$tableRowsArray[]='
				<tr>
					<td colspan="4">'.$TEXT_NO_ZONES_DEFINED_CONST.'</td>
				</tr>';
		}else{
			$tableRowsArray[]='
				<input type="hidden" name="deviceGroupArray" value="'.join(',',$deviceGroupArray).'">
				<tr>
					<td colspan="4" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
				</tr>';
		}
		$tableRows=join('',$tableRowsArray);
		
		$out.=setLeftMenu($dbADO).'
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="zones">
	<input type="hidden" name="section" value="zones">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="did" value="">
	<input type="hidden" name="ddg" value="">
	<input type="hidden" name="dzone" value="">
				
	<div align="center"><h3>'.$title.' '.$TEXT_ZONES_CONST.'</h3></div>
		
	<table align="center" cellpadding="2" cellspacing="1">
		<tr class="tablehead">
			<td align="center"><B>'.$TEXT_ZONE_NAME_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DEVICES_IN_ZONE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_ADD_DEVICE_TO_ZONE_CONST.'</B></td>
			<td align="center">&nbsp;</td>
		</tr>
		'.$tableRows.'
		<tr>
			<td colspan="3">&nbsp;</td>
		</tr>		
		<tr bgcolor="#F0F3F8">
			<td align="center"><B>'.$TEXT_ADD_ZONE_CONST.'</B></td>
			<td colspan="3"><input type="text" name="newDG" value=""> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_ZONE_CONST.'"></td>
		</tr>
	</table>
	</form>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=zones&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		$msg='';
		// update button
		$deviceGroupArray=explode(',',@$_POST['deviceGroupArray']);
		$updCount=0;
		foreach ($deviceGroupArray AS $dg){
			$device=(int)@$_POST['addTo_'.$dg];
			$description=stripslashes(@$_POST['dg_'.$dg]);
			$dbADO->Execute('UPDATE DeviceGroup SET Description=? WHERE PK_DeviceGroup=?',array($description,$dg));
			$updCount+=$dbADO->Affected_Rows();
			if( $device!=0){
				$dbADO->Execute('INSERT INTO Device_DeviceGroup (FK_Device,FK_DeviceGroup) VALUES (?,?)',array($device,$dg));
				$updCount++;
			}
		}
		$msg=($updCount>0)?$TEXT_ZONES_UPDATED_CONST.': '.$updCount:'';

		$did=(int)$_POST['did'];
		$ddg=(int)$_POST['ddg'];
		if($did!=0 && $ddg!=0){
			$dbADO->Execute('DELETE FROM Device_DeviceGroup WHERE FK_Device=? AND FK_DeviceGroup=?',array($did,$ddg));
			$msg.='<br>'.$TEXT_DEVICE_REMOVED_FROM_ZONE_CONST;
		}
		
		// add button
		if(isset($_POST['newDG']) && $_POST['newDG']!=''){
			$dbADO->Execute('
				INSERT INTO DeviceGroup 
					(Description, FK_Installation, Type) 
				VALUES
					(?,?,?)',array(stripslashes($_POST['newDG']),$installationID,$typeInt));
			$msg.='<br>'.$TEXT_NEW_ZONE_WAS_ADDED_CONST;	
		}
		
		$dzone=(int)$_POST['dzone'];
		if($dzone>0){
			$dbADO->Execute('DELETE FROM Device_DeviceGroup WHERE FK_DeviceGroup=?',array($dzone));
			$dbADO->Execute('DELETE FROM DeviceGroup WHERE PK_DeviceGroup=?',array($dzone));
			$msg=$TEXT_ZONE_DELETED_CONST;	
		}
		
		header("Location: index.php?section=zones&type=$type&msg=".urlencode($msg));		
	}

	$output->setNavigationMenu(array($TEXT_SECURITY_DEVICES_CONST=>'index.php?section=devices&type='.$type,$TEXT_ZONES_CONST=>'index.php?section=zones&type='.$type));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$title);
	$output->output();
}
?>

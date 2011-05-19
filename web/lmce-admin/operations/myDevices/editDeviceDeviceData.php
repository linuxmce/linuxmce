<?php
function editDeviceDeviceData($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDeviceDeviceData.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$installationID = (int)@$_SESSION['installationID'];
	$deviceID = (int)$_REQUEST['deviceID'];
	$dd = (int)$_REQUEST['dd'];
	
	if ($action=='form') {
		$query='
			SELECT 
				DeviceData.Description AS dd_Description, 
				Device_DeviceData.FK_DeviceData,
				ParameterType.Description AS typeParam, 
				Device_DeviceData.IK_DeviceData,
				ShowInWizard,ShortDescription,
				AllowedToModify,
				DeviceTemplate_DeviceData.Description AS Tooltip 
			FROM DeviceData 
			INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
			INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
			INNER JOIN Device ON FK_Device=PK_Device
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE FK_Device=? AND Device_DeviceData.FK_DeviceData=?';
		$res=$dbADO->Execute($query,array($deviceID,$dd));
		
		if($res->RecordCount()==0){
			$deviceDataBox="Device Data #$dd does not exist for device #$deviceID";
		}else{
			$rowDDforDevice=$res->FetchRow();
			$ddValue=$rowDDforDevice['IK_DeviceData'];
	
			if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='')){
				$deviceDataBox='<b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$rowDDforDevice['dd_Description']).'</b> '.((@$rowDDforDevice['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowDDforDevice['Tooltip'].'" border="0" style="vertical-align: middle;"> ':'');
				switch($rowDDforDevice['typeParam']){
					case 'int':
					if(in_array($rowDDforDevice['dd_Description'],$GLOBALS['DeviceDataLinkedToTables'])){
						$tableName=str_replace('PK_','',$rowDDforDevice['dd_Description']);
						$filterQuery='';
						switch($tableName){
							case 'Device':
							$filterQuery=" WHERE FK_Installation='".$installationID."'";
							break;
							case 'FloorplanObjectType':
							$filterQuery=" WHERE FK_FloorplanType='".@$specificFloorplanType."'";
						}
	
						$queryTable="SELECT * FROM $tableName $filterQuery ORDER BY Description ASC";
						$resTable=$dbADO->Execute($queryTable);
						$deviceDataBox.='<select name="IK_DeviceData" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
													<option value="0"></option>';
						while($rowTable=$resTable->FetchRow()){
							$itemStyle=($tableName=='FloorplanObjectType' && is_null(@$rowTable['FK_DesignObj_Control']))?' style="background-color:red;"':'';
							$deviceDataBox.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').' '.$itemStyle.'>'.$rowTable['dd_Description'].'</option>';
						}
						$deviceDataBox.='</select>';
					}
					else
						$deviceDataBox.='<input type="text" name="IK_DeviceData" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
					break;
					case 'bool':
						$deviceDataBox.='<input type="checkbox" name="IK_DeviceData" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
					break;
					default:
					if($dd==$GLOBALS['Port']){
						$deviceDataBox.=serialPortsPulldown('IK_DeviceData',@$ddValue,$rowDDforDevice['AllowedToModify'],getTopLevelParent($deviceID,$dbADO),$dbADO,$deviceID);
					}else{
						$deviceDataBox.='<textarea name="IK_DeviceData" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>'.@$ddValue.'</textarea>';
					}
				}
			}
			$deviceDataBox.='<br><input type="submit" class="button" name="submit" value="Update"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">';
		}
		
		$out.='

		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="editDeviceDeviceData">
		<input type="hidden" name="section" value="editDeviceDeviceData">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="dd" value="'.$dd.'">
		<input type="hidden" name="from" value="'.$from.'">
		
		<h2>'.$TEXT_DEVICE_DATA_FOR_DEVICE_CONST.' '.$deviceID.'</h2>
		'.$deviceDataBox.'										
		</form>
		';
		
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editDeviceDeviceData&deviceID=$deviceID&dd=$dd&from=$from&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$IK_DeviceData=$_POST['IK_DeviceData'];
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($IK_DeviceData,$deviceID,$dd));
		
		
		$out.='
		<script>
			opener.document.avWizard.action.value="form";
			opener.document.avWizard.submit();
			self.close();
		</script>';

	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DEVICE_DEVICE_DATA_CONST);			
	$output->output();
}
?>

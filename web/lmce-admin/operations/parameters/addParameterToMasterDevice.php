<?php
function addParameterToMasterDevice($output,$dbADO) {
	// obsolete function
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	
	
	
	
	
	if ($action=='form') {
		$alreadySelectedParameter = array();
		$alreadySelectedParameter[]=0;
		$querySelectParameterForDevice = "
			select FK_DeviceData
								FROM DeviceTemplate_DeviceData
								INNER JOIN  DeviceTemplate on FK_DeviceTemplate  = PK_DeviceTemplate
								WHERE FK_DeviceTemplate = ?			
			";
		$rs = $dbADO->Execute($querySelectParameterForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedParameter[]=$row['FK_DeviceData'];
		}
		$rs->Close();
		$out.='
		<script>
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}
		</script>
		<form action="index.php" method="post" name="addParameterToMasterDevice">
		<input type="hidden" name="section" value="addParameterToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Parameter:</td>
					<td>
						<select name="parameterID">
						<option value="0">-please select-</option>
						';
						$querySelectRemainingParameterForDevice = "
							select  * from DeviceData
							where PK_DeviceData NOT IN (".join(",",$alreadySelectedParameter).")
							order by Description asc";
						$rs = $dbADO->_Execute($querySelectRemainingParameterForDevice);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_DeviceData'].'">'.$row['Description'].'</option>';
						}
						$rs->Close();
					$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td>Comments:</td>
					<td><input type="text" name="Description" size="30" value=""></td>
				</tr>
				<tr>
					<td>Default value:</td>
					<td><input type="text" name="IK_DeviceData" size="30" value=""></td>
				</tr>
				<tr>
					<td>Required:</td>
					<td><input type="checkbox" name="Required" value="1"></td>
				</tr>
				<tr>
					<td>Allowed to modify:</td>
					<td><input type="checkbox" name="AllowedToModify" value="1"></td>
				</tr>
				<tr>
					<td>User Master Device List Defaults:</td>
					<td><input type="checkbox" name="UserDeviceTemplateDefault" value="1"></td>
				</tr>
				<tr>
					<td>Set By Device:</td>
					<td><input type="checkbox" name="SetByDevice" value="1"></td>
				</tr>	
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addParameterToMasterDevice");
 			frmvalidator.addValidation("parameterID","dontselect=0","Please select a parameter");
			frmvalidator.addValidation("Description","req","Please enter a description");
			frmvalidator.addValidation("IK_DeviceData","req","Please enter a default value");
		</script>
		<br />
		If the parameter you wish to add is not in the list,<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addParameter&from=addParameterToMasterDevice&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">click here to add a new parameter</a>
		';
		
	} else {
		
		$parameterID = isset($_POST['parameterID'])?cleanString($_POST['parameterID']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		
			$descriptionParam = cleanString(@$_POST['Description']);
			$defaultValueParam = cleanString(@$_POST['IK_DeviceData']);
			$requiredParam = cleanInteger(@$_POST['Required']);
			$allowedToModifyParam = cleanInteger(@$_POST['AllowedToModify']);
			$useDeviceTemplateDefaultParam = cleanInteger(@$_POST['UseDeviceTemplateDefault']);
			$setByDeviceParam = cleanInteger(@$_POST['SetByDevice']);
			
		if ((int)$parameterID!=0 && (int)$deviceID!=0 && $defaultValueParam!='' && $descriptionParam!='') {
			$insertObjToDevice = 'insert into DeviceTemplate_DeviceData
					(FK_DeviceTemplate, FK_DeviceData,Description,IK_DeviceData,
					Required,AllowedToModify,SetByDevice,UseDeviceTemplateDefault) 
					values(?,?,?,?,?,?,?,?)';
			$query = $dbADO->Execute($insertObjToDevice,array($deviceID,$parameterID,$descriptionParam,$defaultValueParam,
						$requiredParam,$allowedToModifyParam,$setByDeviceParam,$useDeviceTemplateDefaultParam)
						);
			
			$out.="
			<script>
				alert('Device data added!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='Data_Description_$parameterID';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			header("Location: index.php?section=addParameterToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

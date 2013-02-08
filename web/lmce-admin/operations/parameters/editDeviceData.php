<?php
function editDeviceData($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDeviceData.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
	$deviceDataID = (int)@$_REQUEST['deviceDataID'];
		
	$queryDeviceData = "select * from DeviceData where PK_DeviceData = $deviceDataID";
	$res = $dbADO->_Execute($queryDeviceData);
	 if ($res) {
	 	$row=$res->FetchRow();
	 	$ddDescription = $row['Description'];
	 	$ddParameterType = $row['FK_ParameterType'];
	 }
	 
	$queryParams = "select * from ParameterType order by Description";
	$resParams = $dbADO->_Execute($queryParams);
	$paramsSelect = '<option value="0">-please select-</option>';
	if ($resParams) {
		while ($rowParams = $resParams->FetchRow()) {
			$paramsSelect.='<option '.($ddParameterType==$rowParams['PK_ParameterType']?' selected="selected" ':'').' value="'.$rowParams['PK_ParameterType'].'">'.$rowParams['Description'].'</option>';
		}
	}
	$resParams->Close();
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="editDeviceData">
		<input type="hidden" name="section" value="editDeviceData">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="deviceDataID" value="'.$deviceDataID.'">
		<input type="hidden" name="from" value="'.$from.'">
		
		<h3>'.$TEXT_EDIT_DEVICE_DATA_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_PARAMETER_NAME_CONST.' *</td>
					<td><input type="text" size="15" name="ParameterName" value="'.$ddDescription.'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_PARAMETER_TYPE_CONST.' *</td>
					<td>
						<select name="ParameterType" >
						'.$paramsSelect.'
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editDeviceData"); 			
			frmvalidator.addValidation("ParameterName","req","'.$TEXT_PARAMETER_NAME_REQUIRED_CONST.'");
			frmvalidator.addValidation("ParameterType","dontselect=0","'.$TEXT_PARAMETER_TYPE__REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$parameterName = cleanString(@$_POST['ParameterName'],50);
		$parameterType = (int)@$_POST['ParameterType'];
		
		
		if ($parameterName!='' && $parameterType!=0 && $deviceDataID!=0) {
			$queryInsertParameter = 'update DeviceData set Description=?,FK_ParameterType=? where PK_DeviceData = ?';
			$dbADO->Execute($queryInsertParameter,array($parameterName,$parameterType,$deviceDataID));
			
			if($from=='editMasterDevice'){
				$out.="
				<script>
					alert('$TEXT_PARAMETER_MODIFIED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.lastAction.value='Data_Description_$deviceDataID';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			}else{
				$out.="
				<script>
					alert('$TEXT_PARAMETER_MODIFIED_CONST');
				    opener.location.reload();
					self.close();
				</script>
				";			
			}
		} else {
			header("Location: index.php?section=editDeviceData&from=$from&deviceID=$deviceID&deviceDataID=$deviceDataID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DEVICE_DATA_CONST);			
	$output->output();
}
?>

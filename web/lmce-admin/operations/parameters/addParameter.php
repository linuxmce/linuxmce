<?php
function addParameter($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addParameter.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
		
	$queryParams = "select * from ParameterType order by Description";
	$resParams = $dbADO->_Execute($queryParams);
	$paramsSelect = '<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
	if ($resParams) {
		while ($rowParams = $resParams->FetchRow()) {
			$paramsSelect.='<option value="'.$rowParams['PK_ParameterType'].'">'.$rowParams['Description'].'</option>';
		}
	}
	$resParams->Close();
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="addParameter">
		<input type="hidden" name="section" value="addParameter">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_ADD_PARAMETER_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_PARAMETER_NAME_CONST.' *</td>
					<td><input type="text" size="15" name="ParameterName" value=""></td>
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
		 	var frmvalidator = new formValidator("addParameter"); 			
			frmvalidator.addValidation("ParameterName","req","'.$TEXT_PARAMETER_NAME_REQUIRED_CONST.'");
			frmvalidator.addValidation("ParameterType","dontselect=0","'.$TEXT_PARAMETER_TYPE__REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$parameterName = cleanString(@$_POST['ParameterName'],50);
		$parameterType = (int)@$_POST['ParameterType'];
		
		
		if ($parameterName!='' && $parameterType!=0) {
			$queryInsertParameter = 'insert into DeviceData(Description,FK_ParameterType) values(?,?)';
			$dbADO->Execute($queryInsertParameter,array($parameterName,$parameterType));
			
			
				$out.="
				<script>
					alert('$TEXT_PARAMETER_ADDED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=addParameter&from=$from&deviceID=$deviceID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_PARAMETER_CONST);			
	$output->output();
}
?>

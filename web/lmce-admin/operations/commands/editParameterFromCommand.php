<?php
function editParameterFromCommand($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editParameterFromCommand.lang.php');
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$parameterID = @(int)$_REQUEST['commandParameterID'];
	
	if ($action=='form') {		
		$selValues = 'select * from CommandParameter where PK_CommandParameter = ?';
		$resValues = $dbADO->Execute($selValues,array($parameterID));
		 if ($resValues) {
		 	$row = $resValues->FetchRow();
		 	$descriptionParam = $row['Description'];		 	
		 	$parameterType = (int)$row['FK_ParameterType'];
		 }
		
		$query = "select Description,PK_ParameterType from ParameterType order by Description asc";
		$parameterTypes='';
		$res = $dbADO->Execute($query);
			if ($res) {
				while ($row = $res->FetchRow()) {
					$parameterTypes.="<option ".($row['PK_ParameterType']==$parameterType?" selected='selected'":"")." value='{$row['PK_ParameterType']}'>{$row['Description']}</option>\n";
				}
			}
		$out.='
		<form action="index.php" method="post" name="editParameterFromCommand">
		<input type="hidden" name="section" value="editParameterFromCommand">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="commandParameterID" value="'.$parameterID.'">	
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_EDIT_PARAMETER_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.' *</td>
					<td><input type="text" size="15" name="Description" value="'.stripslashes($descriptionParam).'"></td>
				</tr>				
				<tr>
					<td>'.$TEXT_PARAMETER_TYPE_CONST.' *</td>
					<td><select name="paramType">'.$parameterTypes.'</select></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editParameterFromCommand");
 			frmvalidator.addValidation("Description","req","'.$TEXT_PLEASE_ENTER_A_DESCRIPTION_CONST.'");
			frmvalidator.addValidation("paramType","dontselect=0","'.$TEXT_VALIDATE_PARAMETER_TYPE_CONST.'");			
		</script>
		';
		
	} else {
		
		
		
		$description = cleanString(@$_POST['Description'],50);
		
		$parameterType = cleanInteger(@$_POST['paramType']);
		
		if ($description!='' && $parameterType!=0) {
			$queryUpdateParameter = 'update CommandParameter set Description=?,FK_ParameterType=? where PK_CommandParameter = ?';
			$dbADO->Execute($queryUpdateParameter,array($description,$parameterType,$parameterID));
						
			$out.="
				<script>
					alert('$TEXT_PARAMETER_MODIFIED_CONST');					
				    opener.document.forms.{$from}.action.value='form';					
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=editParameterFromCommand&from=$from");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PARAMETER_CONST);			
	$output->output();
}
?>

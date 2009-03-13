<?php
function createParameterForCommand($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createParameterForCommand.lang.php');

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	if ($action=='form') {		
		$parameterTypes='<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
		$query = "select Description,PK_ParameterType from ParameterType order by Description asc";
		$res = $dbADO->Execute($query);
			if ($res) {
				while ($row = $res->FetchRow()) {
					$parameterTypes.="<option value='{$row['PK_ParameterType']}'>{$row['Description']}</option>\n";
				}
			}
		$out.='
		<form action="index.php" method="post" name="createParameterForCommand">
		<input type="hidden" name="section" value="createParameterForCommand">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_COMMAND_PARAMETER_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.' *</td>
					<td><input type="text" size="15" name="Description" value=""></td>
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
		 	var frmvalidator = new formValidator("createParameterForCommand");
 			frmvalidator.addValidation("Description","req","'.$TEXT_VALIDATE_DESCRIPTION_CONST.'");
			frmvalidator.addValidation("paramType","dontselect=0","'.$TEXT_VALIDATE_PARAMETER_TYPE_CONST.'");			
		</script>
		';
		
	} else {
		
		
		
		$description = cleanString(@$_POST['Description'],50);		
		$parameterType = cleanInteger(@$_POST['paramType']);
		
		if ($description!='' && $parameterType!=0) {
			$queryInsertParameter = 'insert into CommandParameter(Description,FK_ParameterType) values(?,?)';
			$dbADO->Execute($queryInsertParameter,array($description,$parameterType));
			$packageID = $dbADO->Insert_ID();
						
			$out.="
				<script>
					alert('$TEXT_PARAMETER_ADDED_CONST');					
				    opener.document.forms.{$from}.action.value='form';					
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=createParameterForCommand&from=$from");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_COMMAND_PARAMETER_CONST);			
	$output->output();
}
?>

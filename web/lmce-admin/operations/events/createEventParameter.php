<?php
function createEventParameter($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createEventParameter.lang.php');
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	
	if ($action=='form') {
		
		
		$querySelectParameterType = "select PK_ParameterType,Description from ParameterType ORDER BY Description asc";
		$resSelectEventParameters = $dbADO->_Execute($querySelectParameterType);
		
		$eventParametersTxt = '<option value="0">-please select-</option>';
		if ($resSelectEventParameters) {
			while ($rowSelectEventParams=$resSelectEventParameters->FetchRow()) {
				$eventParametersTxt.= '<option '.(0==$rowSelectEventParams['PK_ParameterType']?' selected="selected" ':'').' value="'.$rowSelectEventParams['PK_ParameterType'].'">'.$rowSelectEventParams['Description'].'</option>';
			}
		}
		$out.='
		
		<form action="index.php" method="post" name="createEventParameter">
		<input type="hidden" name="section" value="createEventParameter">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
		<h3>'.$TEXT_CREATE_EVENT_PARAMETER_CONST.'</h3>
			<table>	
				<tr>
					<td>
						'.$TEXT_EVENT_TYPE_CONST.':</td><td><select name="eventParameterType">'.$eventParametersTxt.'</select>							
					</td>
				</tr>		
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.':</td><td><input type="text" maxlength="50" name="eventParameterDescription" value=""></td>
				</tr>		
				<tr>
					<td>'.$TEXT_COMMENTS_CONST.':</td><td><textarea name="eventParameterComments" cols="40" rows="4"></textarea></td>
				</tr>		
				<tr>
					<td colspan="2" align="center">						
						<input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">
					</td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("createEventParameter");			
 			frmvalidator.addValidation("eventParameterDescription","req","'.$TEXT_PARAMETER_NAME_REQUIRED_CONST.'");
			frmvalidator.addValidation("eventParameterType","dontselect=0","'.$TEXT_PARAMETER_TYPE_REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		$eventParameterDescription = isset($_POST['eventParameterDescription'])?cleanString($_POST['eventParameterDescription']):'';
		$eventParameterComments = isset($_POST['eventParameterComments'])?cleanString($_POST['eventParameterComments']):'';
		$parameterType = cleanInteger($_POST['eventParameterType']);
		
		if ($parameterType!= 0 && $eventParameterDescription!='') {
			$insertEvent = 'insert into EventParameter (Description,Comments,FK_ParameterType) values(?,?,?)';
			$res=$dbADO->Execute($insertEvent,array($eventParameterDescription,$eventParameterComments,$parameterType));			
			$out.="
			<script>
				alert('$TEXT_EVENT_PARAMETER_CREATED_CONST');
			    opener.document.forms.{$from}.action.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createEventParameter&from=$from");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CREATE_EVENT_PARAMETER_CONST);	
	$output->output();
}
?>

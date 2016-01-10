<?php
function editEventParameterFromEvent($output,$dbADO) {
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$selectedEventParameter = (int)$_REQUEST['eventParameterID'];
	
	if ($action=='form') {
		
		$querySelectEventParameter = "SELECT * FROM EventParameter WHERE PK_EventParameter = $selectedEventParameter ";
		$resSelectEventParameter = $dbADO->_Execute($querySelectEventParameter);
		if ($resSelectEventParameter && $resSelectEventParameter->RecordCount()==1) {
			$row=$resSelectEventParameter->FetchRow();
			$eventParameter_Description = $row['Description'];
			$eventParameter_Comments = $row['Comments'];
			$eventParameter_ParameterType= $row['FK_ParameterType'];
		}
		
		$querySelectParameterType = "select PK_ParameterType,Description from ParameterType ORDER BY Description asc";
		$resSelectEventParameters = $dbADO->_Execute($querySelectParameterType);
		
		$eventParametersTxt = '<option value="0">-please select-</option>';
		if ($resSelectEventParameters) {
			while ($rowSelectEventParams=$resSelectEventParameters->FetchRow()) {
				$eventParametersTxt.= '<option '.($eventParameter_ParameterType==$rowSelectEventParams['PK_ParameterType']?' selected="selected" ':'').' value="'.$rowSelectEventParams['PK_ParameterType'].'">'.$rowSelectEventParams['Description'].'</option>';
			}
		}
		$out.='
		
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		
		<form action="index.php" method="post" name="editEventParameterFromEvent">
		<input type="hidden" name="section" value="editEventParameterFromEvent">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="eventParameterID" value="'.$selectedEventParameter.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>	
				<tr>
					<td>
						Parameter Type *</td><td><select name="eventParameterType">'.$eventParametersTxt.'</select>							
					</td>
				</tr>		
				<tr>
					<td>Description *</td><td><input type="text" maxlength="50" name="eventParameterDescription" value="'.$eventParameter_Description.'"></td>
				</tr>		
				<tr>
					<td>Comments:</td><td><textarea name="eventParameterComments" cols="40" rows="4">'.$eventParameter_Comments.'</textarea></td>
				</tr>		
				<tr>
					<td colspan="2" align="center">						
						<input type="submit" class="button" name="submitX" value="Save">
					</td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editEventParameterFromEvent");			
 			frmvalidator.addValidation("eventParameterDescription","req","Please enter a name for this Event Parameter !");
			frmvalidator.addValidation("eventParameterType","dontselect=0","Please select a parameter type!");
		</script>
		';
		
	} else {
		
		$eventParameterDescription = isset($_POST['eventParameterDescription'])?cleanString($_POST['eventParameterDescription']):'';
		$eventParameterComments = isset($_POST['eventParameterComments'])?cleanString($_POST['eventParameterComments']):'';
		$parameterType = cleanInteger($_POST['eventParameterType']);
		
		if ($parameterType!= 0 && $eventParameterDescription!='') {
			$insertEvent = 'update EventParameter set Description=?, Comments=?, FK_ParameterType = ? where PK_EventParameter = ?';
			$res=$dbADO->Execute($insertEvent,array($eventParameterDescription,$eventParameterComments,$parameterType,$selectedEventParameter));			
			$out.="
			<script>
				alert('Event Parameter modified!');
			    opener.document.forms.{$from}.action.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=editEventParameterFromEvent&eventParameterID=$selectedEventParameter&from=$from");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

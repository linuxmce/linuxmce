<?php
function createEvent($output,$dbADO) {
	// obsolete file
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	if ($action=='form') {
		
		$querySelectEventCategory = "select Description,PK_EventCategory from EventCategory where  FK_EventCategory_Parent IS NULL order by Description asc";
		$resSelectEventCategory = $dbADO->_Execute($querySelectEventCategory);
		
		$EventCategoryTxt = '<option value="0">-please select-</option>';
		if ($resSelectEventCategory) {
			while ($rowSelectEventCategory=$resSelectEventCategory->FetchRow()) {
				$EventCategoryTxt.= '<option '.(0==$rowSelectEventCategory['PK_EventCategory']?' selected="selected" ':'').' value="'.$rowSelectEventCategory['PK_EventCategory'].'">'.$rowSelectEventCategory['Description'].'</option>';
				$EventCategoryTxt.=getEventCategoryChildsArray($rowSelectEventCategory['PK_EventCategory'],$rowSelectEventCategory['Description'],0,$dbADO);
			}
		}
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="createEvent">
		<input type="hidden" name="section" value="createEvent">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
			<table>	
				<tr>
					<td>Event List Category:</td><td><select name="EventCategory">'.$EventCategoryTxt.'</select></td>
				</tr>		
				<tr>
					<td>Description:</td><td><input type="text" maxlength="50" name="EventDescription" value=""></td>
				</tr>				
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<br />If the events list category is not in the list, <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createEventCategory&from=createEvent\',\'status=1,resizable=1,width=500,height=250,toolbar=1\');">click here to create an Event List Category</a>
		<script>
		 	var frmvalidator = new formValidator("createEvent");
			frmvalidator.addValidation("EventCategory","dontselect=0","Please select a category!");
 			frmvalidator.addValidation("EventDescription","req","Please enter a name for this Event List!");
		</script>
		';
		
	} else {
		
		$EventDescription = isset($_POST['EventDescription'])?cleanString($_POST['EventDescription']):'';
		$category = cleanInteger($_POST['EventCategory']);
		if ($EventDescription!='' && $category!=0) {
			$insertEvent = 'insert into Event(Description, FK_EventCategory) values(?,?)';
			$query = $dbADO->Execute($insertEvent,array($EventDescription,$category));
			$out.="
			<script>
				alert('Event List added!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='newEventToMasterDevice';//
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createEvent");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

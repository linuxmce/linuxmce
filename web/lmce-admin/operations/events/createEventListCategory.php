<?php
function createEventCategory($output,$dbADO) {
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
		
		<form action="index.php" method="post" name="createEventCategory">
		<input type="hidden" name="section" value="createEventCategory">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
			<table>	
				<tr>
					<td>Parent Category:</td><td><select name="EventCategoryParent">'.$EventCategoryTxt.'</select></td>
				</tr>		
				<tr>
					<td>Event List Category Name:</td><td><input type="text" maxlength="50" name="EventCategoryDescription" value=""></td>
				</tr>				
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("createEventCategory");			
 			frmvalidator.addValidation("EventCategoryDescription","req","Please enter a name for this Event List Category!");
			frmvalidator.addValidation("EventCategoryParent","dontselect=0","Please select a parent category!");
		</script>
		';
		
	} else {
		
		$EventCategoryDescription = isset($_POST['EventCategoryDescription'])?cleanString($_POST['EventCategoryDescription']):'';
		$category = cleanInteger($_POST['EventCategoryParent']);
		if ($category == 0) {
			$insertEvent = 'insert into EventCategory(Description) values(?)';
		} else {
			$insertEvent = 'insert into EventCategory(Description, FK_EventCategory_Parent) values(?,'.$category.')';
		}
		
		if ($EventCategoryDescription!='') {
			
			$query = $dbADO->_Execute($insertEvent,array($EventCategoryDescription));
			$out.="
			<script>
				alert('Event List Category added!');
			    opener.document.forms.{$from}.action.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createEventCategory");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

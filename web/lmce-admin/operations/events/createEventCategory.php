<?php
function createEventCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createEventCategory.lang.php');
		
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
					<td>'.$TEXT_PARENT_CATEGORY_CONST.':</td><td><select name="EventCategoryParent">'.$EventCategoryTxt.'</select></td>
				</tr>		
				<tr>
					<td>'.$TEXT_EVENT_LIST_CATEGORY_NAME_CONST.':</td><td><input type="text" maxlength="50" name="EventCategoryDescription" value=""></td>
				</tr>				
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("createEventCategory");			
 			frmvalidator.addValidation("EventCategoryDescription","req","'.$TEXT_EVENT_CATEGORY_VALIDATION_CONST.'");
			//frmvalidator.addValidation("EventCategoryParent","dontselect=0","'.$TEXT_PARENT_CATEGORY_VALIDATION_CONST.'");
		</script>
		';
		
	} else {
		
		$EventCategoryDescription = isset($_POST['EventCategoryDescription'])?cleanString($_POST['EventCategoryDescription']):'';
		$category = cleanInteger($_POST['EventCategoryParent']);
		$insertEvent = 'insert into EventCategory(Description, FK_EventCategory_Parent) values(?,?)';
		
		if ($EventCategoryDescription!='') {
			$category=($category==0)?NULL:$category;
			$query = $dbADO->Execute($insertEvent,array($EventCategoryDescription,$category));
			$out.="
			<script>
				alert('$TEXT_EVENT_LIST_CATEGORY_ADDED_CONST');
				try{
			    	opener.location.reload();				
				}catch(e){
					// error
				}
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createEventCategory");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CREATE_EVENT_CATEGORY_CONST);			
	$output->output();
}
?>

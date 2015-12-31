<?php
function editCommandCategory($output,$dbADO) {
	// include languages files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editCommandCategory.lang.php');

	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$ccID=(int)$_REQUEST['ccID'];

	$resCC=$dbADO->Execute('SELECT * FROM CommandCategory WHERE PK_CommandCategory=?',$ccID);
	$rowCC=$resCC->FetchRow();

	$querySelectCategoryMainForDevice = "
		SELECT  PK_CommandCategory,Description FROM CommandCategory 
		WHERE  FK_CommandCategory_Parent IS NULL 
		ORDER BY Description ASC
	";
	$commandsCategsTxt = '<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
	$rs = $dbADO->_Execute($querySelectCategoryMainForDevice);
	while ($row = $rs->FetchRow()) {
				$commandsCategsTxt.='<option value="'.$row['PK_CommandCategory'].'" '.(($row['PK_CommandCategory']==$rowCC['FK_CommandCategory_Parent'])?'selected':'').'>'.$row['Description'].'</option>';
				$querySelectCategoryForDevice = "
					SELECT  PK_CommandCategory,Description FROM CommandCategory
					WHERE  
						FK_CommandCategory_Parent = {$row['PK_CommandCategory']} AND PK_CommandCategory!={$ccID}
					ORDER BY Description asc
						";
				$rs2 = $dbADO->_Execute($querySelectCategoryForDevice);				
					while ($row2 = $rs2->FetchRow()) {
						$commandsCategsTxt.='<option value="'.$row2['PK_CommandCategory'].'" '.(($row2['PK_CommandCategory']==$rowCC['FK_CommandCategory_Parent'])?'selected':'').'>'.$row['Description'].'-'.$row2['Description'].'</option>';
					}
			
	}
	$rs->Close();

	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="editCommandCategory">
		<input type="hidden" name="section" value="editCommandCategory">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="ccID" value="'.$ccID.'">		
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_EDIT_COMMAND_CATEGORY_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_COMMAND_CATEGORY_NAME_CONST.' *</td>
					<td><input type="text" size="15" name="CommandCategoryDescription" value="'.$rowCC['Description'].'"></td>
				</tr>				
				<tr>
					<td>'.$TEXT_COMMAND_CATEGORY_PARENT_CONST.'</td>
					<td>
						<select name="parentCateg" >
						'.$commandsCategsTxt.'
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
		 	var frmvalidator = new formValidator("editCommandCategory"); 			
			frmvalidator.addValidation("CommandCategoryDescription","req","'.$TEXT_VALIDATE_COMMAND_CATEGORY_NAME_CONST.'");
		</script>
		';
		
	} else {

		$commandCategoryDescription = cleanString(@$_POST['CommandCategoryDescription'],50);		
		$parentCateg = ((int)@$_POST['parentCateg']!=0)?(int)@$_POST['parentCateg']:NULL;

		
		if ($commandCategoryDescription!='') {
		
			$dbADO->Execute('UPDATE CommandCategory SET Description=?, FK_CommandCategory_Parent=? WHERE PK_CommandCategory=?',array($commandCategoryDescription,$parentCateg,$ccID));
			
				$out.="
				<script>
					alert('$TEXT_COMMAND_CATEGORY_UPDATED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=editCommandCategory&from=$from&ccID=$ccID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_COMMAND_CATEGORY_CONST);			
	$output->output();
}
?>

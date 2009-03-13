<?php
function addCommand($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addCommand.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$commandCateg=(int)@$_REQUEST['commandCateg'];
	
		
	$querySelectCategoryMainForDevice = "
		select  PK_CommandCategory,Description from CommandCategory 
		where  FK_CommandCategory_Parent IS NULL
		order by Description asc
	";
	$commandsCategsTxt = '<option value="0">-please select-</option>';
	$rs = $dbADO->_Execute($querySelectCategoryMainForDevice);
	while ($row = $rs->FetchRow()) {
				$commandsCategsTxt.='<option value="'.$row['PK_CommandCategory'].'" '.(($commandCateg==$row['PK_CommandCategory'])?'selected':'').'>'.$row['Description'].'</option>';
				$querySelectCategoryForDevice = "
					select  PK_CommandCategory,Description from CommandCategory
					where  
						FK_CommandCategory_Parent = {$row['PK_CommandCategory']}
					order by Description asc
						";
				$rs2 = $dbADO->_Execute($querySelectCategoryForDevice);				
					while ($row2 = $rs2->FetchRow()) {
						$commandsCategsTxt.='<option value="'.$row2['PK_CommandCategory'].'" '.(($commandCateg==$row2['PK_CommandCategory'])?'selected':'').'>'.$row['Description'].'-'.$row2['Description'].'</option>';
					}
			
	}
	$rs->Close();
						
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="addCommand">
		<input type="hidden" name="section" value="addCommand">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_ADD_COMMAND_CONST.'</h3>
			<table width="100%">
				<tr>
					<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
				</tr>
			</table>		
		
			<table>			
				<tr>
					<td>'.$TEXT_COMMAND_NAME_CONST.' *</td>
					<td><input type="text" size="15" name="CommandDescription" value=""></td>
				</tr>				
				<tr>
					<td>'.$TEXT_AV_COMMAND_CONST.'</td>
					<td><input type="checkbox" name="AVCommand" value="1"></td>
				</tr>
				<tr>
					<td>'.$TEXT_COMMAND_CATEGORY_CONST.' *</td>
					<td>
						<select name="commandCateg" >
						'.$commandsCategsTxt.'
						</select>
					</td>
				</tr>
				<tr>
							<td colspan="2">
							<em>'.$TEXT_REQUIRED_FIELDS_CONST.'</em>
								<fieldset>
									<legend>'.$TEXT_PIPES_THIS_COMMAND_USES_CONST.'</legend>
										<table>
				';
								
				$selectPipes = "SELECT * FROM Pipe Order BY Description ASC";
				$resPipes = $dbADO->_Execute($selectPipes);
				if ($resPipes) {
					while ($rowPipes = $resPipes->FetchRow()) {
						$out.='<tr><td><input type="checkbox" value="1" name="commandPipe_'.$rowPipes['PK_Pipe'].'"></td><td>'.$rowPipes['Description'].'</td></tr>';
					}
				}
				$out.='</table>
						</fieldset>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		
		<script>
		 	var frmvalidator = new formValidator("addCommand"); 			
			frmvalidator.addValidation("CommandDescription","req","'.$TEXT_VALIDATE_COMMAND_CONST.'");
			frmvalidator.addValidation("commandCateg","dontselect=0","'.$TEXT_VALIDATE_COMMAND_CATEGORY_CONST.'");
		</script>
		';
		
	} else {

		$commandDescription = cleanString(@$_POST['CommandDescription'],50);		
		$AVCommand = cleanInteger(@$_POST['AVCommand']);
		$commandCateg = (int)@$_POST['commandCateg'];

		
		if ($commandDescription!='' && $commandCateg!=0) {

			
		
		$queryInsertCmdGroup = 'insert into Command (Description,FK_CommandCategory,AVCommand) values(?,?,?)';
		$dbADO->Execute($queryInsertCmdGroup,array($commandDescription,$commandCateg,$AVCommand));

		$commandID = $dbADO->Insert_ID();	
		
		$selectPipes = "SELECT * FROM Pipe Order BY Description ASC";
		$resPipes = $dbADO->_Execute($selectPipes);
		
		if ($resPipes) {
			while ($rowPipes = $resPipes->FetchRow()) {
				if (isset($_POST['commandPipe_'.$rowPipes['PK_Pipe']]) && (int)$_POST['commandPipe_'.$rowPipes['PK_Pipe']] == 1) {
					$checkIfExists = "SELECT * FROM Command_Pipe WHERE FK_Pipe = ? AND FK_Command = ?";					
					$resCheckIfExists = $dbADO->Execute($checkIfExists,array($rowPipes['PK_Pipe'],$commandID));
					
					 if ($resCheckIfExists && $resCheckIfExists->RecordCount()==1) {
					 	//do nothing
					 } else {
					 	//insert this relation
					 	$insertCommandPipe = "INSERT INTO Command_Pipe (FK_Command,FK_Pipe) values(?,?)";
					 	
					 	$resInsertCommandPipe = $dbADO->Execute($insertCommandPipe,array($commandID,$rowPipes['PK_Pipe']));
					 	
					 }
				} else {
					$deleteExistingCommandPipe = "DELETE FROM Command_Pipe Where FK_Command = ? AND FK_Pipe = ? ";
					$resDeleteExistingCommandPipe = $dbADO->Execute($deleteExistingCommandPipe,array($commandID,$rowPipes['PK_Pipe']));
				}
			}
		}
		
		
		
			
			$insertID = $dbADO->Insert_ID();
				$out.="
				<script>
					alert('$TEXT_COMMAND_ADDED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					if(opener.document.forms.{$from}.lastAction){
						opener.document.forms.{$from}.lastAction.value='addNewCommandToDeviceCommandGroup';
					}
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=addCommand&from=$from");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_COMMAND_CONST);			
	$output->output();
}
?>

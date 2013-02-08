<?php
function editCommand($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editCommand.lang.php');
	
	$out='';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
						
	if ($action=='form') {		
		$commandID = (int)@$_REQUEST['commandID'];

		// get device templates that are using this command
		$res=$dbADO->Execute('
			SELECT DISTINCT FK_DeviceTemplate, DeviceTemplate.Description
			FROM DeviceCommandGroup_Command 
			INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup=DeviceCommandGroup_Command.FK_DeviceCommandGroup
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Command=?',$commandID);
		$deviceTemplatesWhoUseCommand=array();
		while($row=$res->FetchRow()){
			$deviceTemplatesWhoUseCommand[]='#'.$row['FK_DeviceTemplate'].' '.$row['Description'];
		}
		
		$querySelectCommand = "
			SELECT  * FROM
				Command 
			WHERE   PK_Command = ?";
		$rs = $dbADO->Execute($querySelectCommand,array($commandID));
		
		while ($row = $rs->FetchRow()) {
			$commandDescription = stripslashes($row['Description']);
			$commandAVCommand = (int)$row['AVCommand'];
			$commandCategory=(int)$row['FK_CommandCategory'];
		}
		$rs->Close();
		$querySelectCategoryMainForDevice = "
			SELECT  PK_CommandCategory,Description
				FROM CommandCategory 
			WHERE  FK_CommandCategory_Parent IS NULL
				ORDER BY Description ASC";
		$commandsCategsTxt = '<option value="0">-please select-</option>';
		$rs = $dbADO->_Execute($querySelectCategoryMainForDevice);
		while ($row = $rs->FetchRow()) {
					$commandsCategsTxt.='<option value="'.$row['PK_CommandCategory'].'" '.(($row['PK_CommandCategory']==$commandCategory)?'selected':'').'>'.$row['Description'].'</option>';
					$querySelectCategoryForDevice = "
						select  PK_CommandCategory,Description from CommandCategory
						where  
							FK_CommandCategory_Parent = {$row['PK_CommandCategory']}
						order by Description asc
							";
					$rs2 = $dbADO->_Execute($querySelectCategoryForDevice);				
						while ($row2 = $rs2->FetchRow()) {
							$commandsCategsTxt.='<option value="'.$row2['PK_CommandCategory'].'" '.(($row2['PK_CommandCategory']==$commandCategory)?'selected':'').'>'.$row['Description'].'-'.$row2['Description'].'</option>';
						}
				
		}
		$rs->Close();

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editCommand">
		<input type="hidden" name="section" value="editCommand">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="commandID" value="'.$commandID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_EDIT_COMMAND_CONST.' #'.$commandID.'</h3>
			<table width="100%">
				<tr>
					<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
				</tr>
			</table>
				
			<table>			
				<tr>
					<td><B>'.$TEXT_COMMAND_CONST.' *</B>	</td>
					<td><input type="text" size="25" name="commandDescription" value="'.stripslashes($commandDescription).'" class="input_big"></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_COMMAND_CATEGORY_CONST.' *</B></td>
					<td>
						<select name="commandCategs" class="input_big">
						'.$commandsCategsTxt.'
						</select>
					</td>
				</tr>
				<tr>
					<td><B>'.$TEXT_DEVICE_TEMPLATES_WHO_USE_COMMAND_CONST.' *</B></td>
					<td class="'.((count($deviceTemplatesWhoUseCommand)>0)?'err':'').'">
						'.join(', ',$deviceTemplatesWhoUseCommand).'
					</td>
				</tr>		
				<tr><td colspan="2">
				<fieldset>
					<legend>'.$TEXT_PARAMETERS_CONST.'</legend>
					<table>
							';
		$queryGetParams = "select Command_CommandParameter.FK_CommandParameter as PK_CP,IsOut,
							Command_CommandParameter.Description as CCP_Desc,
							CommandParameter.Description as CP_Desc,
							ParameterType.Description as PT_Desc
							 from 
						Command_CommandParameter 
							INNER JOIN CommandParameter on PK_CommandParameter = FK_CommandParameter
							INNER JOIN ParameterType on PK_ParameterType = FK_ParameterType
						where 
						FK_Command = ? order by CommandParameter.Description
						";
		$res = $dbADO->Execute($queryGetParams,array($commandID));
		$displayedCP = array();
		$displayedCP[] = 0;
		$jsValidationForCommandParameterDescription='';
				while ($row=$res->FetchRow()) {
					$out.="<tr ".(strlen(trim($row['CCP_Desc']))==0?" bgColor='lightgreen' ":" ")." ><td valign='top'>#{$row['PK_CP']}</td><td>{$row['CP_Desc']} ({$row['PT_Desc']})</td><td><textarea cols='40' rows='3' name='Command_ParameterCommand_{$row['PK_CP']}'>{$row['CCP_Desc']}</textarea></td><td nowrap align='center'><b>Is out</b><br />&nbsp; Yes <input type='radio' name='IsOut_{$row['PK_CP']}' value='1' ".($row['IsOut']==1?" checked='checked' ":'').">  &nbsp; No <input type='radio' name='IsOut_{$row['PK_CP']}' value='0' ".($row['IsOut']==0?" checked='checked' ":'')."></td><td><a href='javascript:void(0);' onClick=\"windowOpen('index.php?section=deleteParameterFromCommand&from=editCommand&commandParameterID={$row['PK_CP']}&commandID=$commandID','width=100,height=100,toolbar=0,resizable=0,scrollbars=0');\">Delete</a> <a href='javascript:void(0);' onClick=\"windowOpen('index.php?section=editParameterFromCommand&from=editCommand&commandParameterID={$row['PK_CP']}&commandID=$commandID','width=400,height=400,toolbar=0,resizable=0,scrollbars=0');\">Edit</a> </td></tr>";
					$jsValidationForCommandParameterDescription.="
						frmvalidator.addValidation(\"Command_ParameterCommand_{$row['PK_CP']}\",\"req\",\"$TEXT_VALIDATE_DESCRIPTION_CONST\");
					";
					$displayedCP[]=$row['PK_CP'];
				}
		
		$out.='		
					<input type="hidden" value="'.(join(",",$displayedCP)).'" name="displayedCommandParams">
				</table>
		';
		$querygetAllParameter = 'select CommandParameter.Description,
										CommandParameter.PK_CommandParameter,
										ParameterType.Description as PT_Desc
								FROM CommandParameter 
									INNER JOIN ParameterType on FK_ParameterType = PK_ParameterType
								WHERE PK_CommandParameter NOT IN ('.(join(",",$displayedCP)).')
								ORDER BY Description asc
								';
		$resAllParameters = $dbADO->_Execute($querygetAllParameter);
		$allParams='<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
			if ($resAllParameters) {
				while ($row=$resAllParameters->FetchRow()) {
					$allParams.='<option value="'.$row['PK_CommandParameter'].'">'.$row['Description'].'('.$row['PT_Desc'].')</option>';
				}
			}
		$resAllParameters->Close();
		$out.='
					'.$TEXT_ADD_NEW_PARAMETER_CONST.' <select name="addNewParameterToCommand">'.$allParams.'</select><input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'"><br />
					<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createParameterForCommand&from=editCommand\',\'width=400,height=400,toolbar=0,resizable=0,scrollbars=0\');">'.$TEXT_CREATE_NEW_PARAMETER_CONST.'</a>
				</fieldset>
				</td>
				</tr>
				<tr>
					<td colspan="2">
						<fieldset>
							<legend>'.$TEXT_PIPES_THIS_COMMAND_USES_CONST.'</legend>
								<table>
		';
		
		$selectedPipes = "SELECT * FROM Command_Pipe WHERE FK_Command = ?";
		$resSelectedPipes = $dbADO->Execute($selectedPipes,array($commandID));
		$selectedPipesArray = array();
		if ($resSelectedPipes) {
			while ($rowSelectedPiped = $resSelectedPipes->FetchRow()) {
				$selectedPipesArray[]=$rowSelectedPiped['FK_Pipe'];
			}
		}

		$selectPipes = "SELECT * FROM Pipe Order BY Description ASC";
		$resPipes = $dbADO->_Execute($selectPipes);
		if ($resPipes) {
			while ($rowPipes = $resPipes->FetchRow()) {
				$out.='<tr><td><input type="checkbox" value="1" '.(in_array($rowPipes['PK_Pipe'],$selectedPipesArray)?" checked='checked' ":'').' name="commandPipe_'.$rowPipes['PK_Pipe'].'"></td><td>'.$rowPipes['Description'].'</td></tr>';
			}
		}
		$out.='
							
							</table>
						</fieldset>
					</td>
				</tr>
				
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
		
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editCommand"); 			
			frmvalidator.addValidation("commandDescription","req","'.$TEXT_VALIDATE_COMMAND_GROUP_NAME_CONST.'");
			
			'.$jsValidationForCommandParameterDescription.'
		</script>
		';
		
	} else {
		
		$lastAction='';
		$commandID = isset($_POST['commandID'])?cleanString($_POST['commandID']):'0';
		$commandDescription = cleanString(@$_POST['commandDescription'],50);
		$commandCategs=cleanInteger($_POST['commandCategs']);
		
		$addNewParameterToCommand = isset($_POST['addNewParameterToCommand'])?cleanInteger($_POST['addNewParameterToCommand']):'0';
		
		if ($addNewParameterToCommand!=0) {
			$query = 'insert into Command_CommandParameter (FK_Command, FK_CommandParameter) values(?,?) ';
			$dbADO->Execute($query,array($commandID,$addNewParameterToCommand));
			$lastAction = "Command_ParameterCommand_".$addNewParameterToCommand;
		}
		
		$displayedCommandParams = @$_POST['displayedCommandParams'];
		$displayedCommandParamsArray = explode(",",$displayedCommandParams);
		if (!is_array($displayedCommandParamsArray)) {
			$displayedCommandParamsArray=array();
			$displayedCommandParamsArray[]=0;
		}
		
		foreach ($displayedCommandParamsArray as $elem) {
			$desc = cleanString(@$_POST['Command_ParameterCommand_'.$elem]);
			$isOut = cleanInteger(@$_POST['IsOut_'.$elem]);
			
			$selectOldValue = "select Description from Command_CommandParameter where FK_Command = ? and FK_CommandParameter = ?";
			$resOldValue= $dbADO->Execute($selectOldValue,array($commandID,$elem));
			 if ($resOldValue)  {
			 	$row = $resOldValue->FetchRow();
			 	 if ($desc != $row['Description'] && $desc!='') {
			 	 	$lastAction = 'Command_ParameterCommand_'.$elem;
			 	 }
			 	 if ($elem!=0) {
			 	 	$query = "update Command_CommandParameter set Description = ? ,IsOut=? where FK_Command = ? and FK_CommandParameter = ?";
			 	 	$dbADO->Execute($query,array($desc,$isOut,$commandID,$elem));
			 	 }
			 	 	
			 }
			
		}
		
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
		
		if ($commandDescription!='' && $commandID!=0) {
			
			$querySelectCommand = "
			select  FK_CommandCategory,Description FROM
				Command 
			where   PK_Command = ?";
			$res = $dbADO->Execute($querySelectCommand,array($commandID));
			if ($res) {
				$row=$res->FetchRow();
				$oldDescription = $row['Description']; 
				$oldCategory=(int)$row['FK_CommandCategory'];
				if ($oldDescription!=$commandDescription || $commandCategs!=$oldCategory) {
					$lastAction="commandDescription";
					$updateDb = "UPDATE Command SET Description = ?, FK_CommandCategory=? WHERE PK_Command = ?";
					$dbADO->Execute($updateDb,array($commandDescription,$commandCategs,$commandID));
				}
				
			}
			
			header("Location: index.php?section=editCommand&from=$from&lastAction=$lastAction&commandID=$commandID");
		}
				
	}
	
	$onLoad='';
	
	if ($lastAction!=''?$onLoad.="if (document.forms.editCommand.{$lastAction}) {document.forms.editCommand.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_COMMAND_CONST);			
	$output->output();
}
?>

<?php
function editControlledViaCategoryToMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editControlledViaCategoryToMasterDevice.lang.php');
		
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$objID = (int)$_REQUEST['objID'];
	
	
	if ($action=='form') {
		$queryGetValues = "select * from DeviceTemplate_DeviceCategory_ControlledVia where PK_DeviceTemplate_DeviceCategory_ControlledVia = ?";
		$rs = $dbADO->Execute($queryGetValues,array($objID));
		if ($rs->RecordCount()==1) {
			$row=$rs->FetchRow();			
			$reroute = (int)$row['RerouteMessagesToParent'];
			$autocreate = (int)$row['AutoCreateChildren'];			
			$controlledVia = (int)$row['FK_DeviceCategory'];
			$deviceID = (int)$row['FK_DeviceTemplate'];
			$rs->Close();
		}
		
		$alreadySelectedControlledViaCategory = array();
		$alreadySelectedControlledViaCategory[]=0;
		
		$querySelectControlledViaCategoryForDevice = "
			select MDL_CV.FK_DeviceCategory 
								FROM DeviceTemplate_DeviceCategory_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate
								INNER JOIN DeviceCategory DC on MDL1.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ?
			";
		$rs = $dbADO->Execute($querySelectControlledViaCategoryForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedControlledViaCategory[]=$row['FK_DeviceCategory'];
		}
		$rs->Close();
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editControlledViaCategoryToMasterDevice">
		<input type="hidden" name="section" value="editControlledViaCategoryToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="objID" value="'.$objID.'">
		<input type="hidden" name="from" value="'.$from.'">
		
			<table>			
				<tr>
					<td>'.$TEXT_CONTROLLED_VIA_CONST.' *</td>
					<td>
						<select name="controlledVia">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
						';
		
						
						$queryMasterDeviceCategories_parents = '
							select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null order by Description asc';
						$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row['PK_DeviceCategory']==$controlledVia?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].' #'.$row['PK_DeviceCategory'].'</option>';
								$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$row['Description'],$controlledVia,'',$dbADO);
							}
						$rs->Close();
						
						$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td>'.$TEXT_REROUTE_MESSAGES_TO_PARENT_CONST.':</td>
					<td>
						<input name="RerouteMessagesToParent" value="1" type="checkbox" '.($reroute==1?' checked ':'').'>
					</td>
				</tr>
				<tr>
					<td>'.$TEXT_AUTO_CREATE_CHILDREN_CONST.':</td>
					<td>
						<input name="AutoCreateChildren" value="1" type="checkbox" '.($autocreate==1?' checked ':'').'>
					</td>
				</tr>
				<tr>					
					<td colspan="2">
						<h2>'.$TEXT_PIPES_USED_CONST.'</h2>
							<table>
								<tr>
									<td>
										<table>
											';
							
							$selectInputs = 'SELECT * FROM Command WHERE FK_CommandCategory=22 Order By Description ASC';
							$resSelectInputs = $dbADO->Execute($selectInputs);
							
							$selectOutputs = 'SELECT * FROM Command WHERE FK_CommandCategory=27 Order By Description ASC';
							$resSelectOutputs = $dbADO->Execute($selectOutputs);
							
							$selectPipes = 'SELECT * FROM Pipe Order By Description ASC';
							$resSelectPipes = $dbADO->Execute($selectPipes);
							$pipesNames=array();
							while ($rowSelPipes = $resSelectPipes->FetchRow()) {
								$pipesNames[$rowSelPipes['PK_Pipe']]=$rowSelPipes['Description'];
							}
							
							$selectPipesUsed = 'SELECT DeviceTemplate_DeviceCategory_ControlledVia_Pipe.*,
														DT.Description as Desc_From,
														DT.PK_DeviceTemplate as FK_From,
														DC.Description as Desc_To,
														DC.PK_DeviceCategory as FK_To													
													 FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe 
														INNER JOIN DeviceTemplate_DeviceCategory_ControlledVia on FK_DeviceTemplate_DeviceCategory_ControlledVia = PK_DeviceTemplate_DeviceCategory_ControlledVia
														INNER JOIN DeviceTemplate DT ON DT.PK_DeviceTemplate = DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceTemplate
														INNER JOIN DeviceCategory DC ON DC.PK_DeviceCategory = DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory
												WHERE  FK_DeviceTemplate_DeviceCategory_ControlledVia = ?';
							$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($objID));
							$displayedPipes=array();
							
							if ($resSelectPipesUsed) {
								while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {
									$displayedPipes[]=$rowSelectedPipesUsed['FK_Pipe'];
									$out.='<tr bgcolor="#EEEEEE">
											<td align="center"><B>'.$rowSelectedPipesUsed['Desc_To']."</B> <input type='hidden' name='deviceTo_{$rowSelectedPipesUsed['FK_To']}'> &nbsp;&nbsp;&nbsp;&nbsp;</td>";
									
									$resSelectInputs->MoveFirst();			
									$selectInputsTxt='';
									while ($rowSelInputs = $resSelectInputs->FetchRow()) {
										$selectInputsTxt.= '<option '.($rowSelInputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Input']?" selected='selected' ":"").' value="'.$rowSelInputs['PK_Command'].'">'.$rowSelInputs['Description'].'</option>';
									}
									
									$out.='<td align="center"> <B>Input on '.($rowSelectedPipesUsed['ToChild']==0?$rowSelectedPipesUsed['Desc_To']:$rowSelectedPipesUsed['Desc_From']).'</B> <select name="input_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">-please select-</option>'.$selectInputsTxt.'</select></td>';
									
									$resSelectOutputs->MoveFirst();			
									$selectOutputsTxt='';
									while ($rowSelOutputs = $resSelectOutputs->FetchRow()) {
										$selectOutputsTxt.= '<option '.($rowSelOutputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Output']?" selected='selected' ":"").' value="'.$rowSelOutputs['PK_Command'].'">'.$rowSelOutputs['Description'].'</option>';
									}
									
									$out.='<td align="center"> <B>Output on '.($rowSelectedPipesUsed['ToChild']==0?$rowSelectedPipesUsed['Desc_From']:$rowSelectedPipesUsed['Desc_To']).'</B> <select name="output_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">-please select-</option>'.$selectOutputsTxt.'</select></td>';
									
						

									
									$out.='<td valign="top" align="center"> <B>Pipe</B> '.@$pipesNames[$rowSelectedPipesUsed['FK_Pipe']].'<input type="hidden" name="pipe_'.$rowSelectedPipesUsed['FK_To'].'" value="'.$rowSelectedPipesUsed['FK_Pipe'].'"></td>';
									$out.='<td align="center" valign="top"> <B>Flip In/Out:</B> <input type="checkbox" '.($rowSelectedPipesUsed['ToChild']==1?" checked='checked'":'').' name="toChild_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'" value="1"></td>';
									$out.='<td><input value="Delete" type="button" class="button" onClick="if (confirm(\'Are you sure you want to delete this pipe?\')) {windowOpen(\'index.php?section=deletePipeFromDeviceTemplateControlledViaCategory&objID='.$rowSelectedPipesUsed['FK_DeviceTemplate_DeviceCategory_ControlledVia'].'&from=editControlledViaCategoryToMasterDevice&pipe='.$rowSelectedPipesUsed['FK_Pipe'].'\',\'width=100,height=100,toolbar=1,scrollbars=1,resizable=1\');}"></td>';
									
									$out.='</tr>';
								}
							}
							$out.='
										</table>
									</td>
								</tr>							    
							</table>
					</td>
				</tr>';
												
		$out.='	<tr>
					<td colspan="2" align="center">'.$TEXT_NEW_PIPE_CONST.' <select name="newPipe">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
		$resSelectPipes->MoveFirst();			
		while ($rowSelPipes = $resSelectPipes->FetchRow()) {
			if(!in_array($rowSelPipes['PK_Pipe'],$displayedPipes))
				$out.= '<option value="'.$rowSelPipes['PK_Pipe'].'">'.$rowSelPipes['Description'].'</option>';
		}

		$out.='		</select> <input type="submit" class="button" name="addPipe" value="'.$TEXT_ADD_CONST.'">
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editControlledViaCategoryToMasterDevice");
 			frmvalidator.addValidation("controlledVia","dontselect=0","'.$TEXT_PLEASE_SELECT_A_DEVICE_CONST.'");			
		</script>
		';
		
	} else {
		
		$controlledVia = isset($_POST['controlledVia'])?cleanString($_POST['controlledVia']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';		
		$objID = isset($_POST['objID'])?cleanString($_POST['objID']):'0';				
		$reroute = cleanInteger(@$_POST['RerouteMessagesToParent']);
		$autocreate = cleanInteger(@$_POST['AutoCreateChildren']);
		
		if(isset($_POST['addPipe']) && (int)$_POST['newPipe']!=0){
			$newPipe=(int)$_POST['newPipe'];
			$insertPipe = "INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia_Pipe (FK_DeviceTemplate_DeviceCategory_ControlledVia,FK_Pipe) values (?,?)";
			$dbADO->Execute($insertPipe,array($objID,$newPipe));				
			header("Location: index.php?section=editControlledViaCategoryToMasterDevice&from=$from&objID=$objID");
			exit();
		}
		
		
		$selectPipesUsed = 'SELECT DeviceTemplate_DeviceCategory_ControlledVia_Pipe.*,
														DT.Description as Desc_From,
														DT.PK_DeviceTemplate as FK_From,
														DC.Description as Desc_To,
														DC.PK_DeviceCategory as FK_To													
													 FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe 
														INNER JOIN DeviceTemplate_DeviceCategory_ControlledVia on FK_DeviceTemplate_DeviceCategory_ControlledVia = PK_DeviceTemplate_DeviceCategory_ControlledVia
														INNER JOIN DeviceTemplate DT ON DT.PK_DeviceTemplate = DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceTemplate
														INNER JOIN DeviceCategory DC ON DC.PK_DeviceCategory = DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory
												WHERE  FK_DeviceTemplate_DeviceCategory_ControlledVia = ?
						';
		$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($objID));
		
		$affectedPipes = 'Pipes not updated!';
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {	
			$input=($_POST['input_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe']]!='0')?cleanInteger(@$_POST['input_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe']]):NULL;
			$outputs=(@$_POST['output_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe']]!='0')?cleanInteger(@$_POST['output_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe']]):NULL;
			$pipe=cleanInteger(@$_POST['pipe_'.$rowSelectedPipesUsed['FK_To']]);
			$toChild = cleanInteger(@$_POST['toChild_'.$rowSelectedPipesUsed['FK_To'].'_'.$rowSelectedPipesUsed['FK_Pipe']]);
			$deviceTo=$rowSelectedPipesUsed['FK_To'];

			$updateDevicePipe = 'UPDATE DeviceTemplate_DeviceCategory_ControlledVia_Pipe SET FK_Command_Input=?,FK_Command_Output=?,ToChild=? WHERE FK_Pipe=? AND FK_DeviceTemplate_Devicecategory_ControlledVia = ?';
			$res=$dbADO->Execute($updateDevicePipe,array($input,$outputs,$toChild,$pipe,$objID));
			$affectedPipes = $dbADO->Affected_Rows()==1 && $affectedPipes == 'Pipes not updated!'?"Pipes updated!":"Pipes not updated!";
		}
		
		
		if ((int)$controlledVia!=0 && (int)$deviceID!=0 ) {
			$updateObjToDevice = 'UPDATE DeviceTemplate_DeviceCategory_ControlledVia
					SET 
						FK_DeviceCategory=?,
						RerouteMessagesToParent=?,AutoCreateChildren=?
					WHERE  PK_DeviceTemplate_DeviceCategory_ControlledVia = ? 
					';			
			$query = $dbADO->Execute($updateObjToDevice,array($controlledVia,$reroute,$autocreate,$objID));
			$out.="
			<script>
				alert('Controlled via ".($dbADO->Affected_Rows()==1?"":"not")." modified! {$affectedPipes}');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
				//document.location.href='index.php?section=editControlledViaCategoryToMasterDevice&from=$from&objID=$objID';
			</script>
			";			
		} else {
			header("Location: index.php?section=editControlledViaCategoryToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

<?php
function editControlledViaToMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editControlledViaToMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$objID = (int)$_REQUEST['objID'];
	
	
	if ($action=='form') {
		$queryGetValues = "select * from DeviceTemplate_DeviceTemplate_ControlledVia where PK_DeviceTemplate_DeviceTemplate_ControlledVia = ?";
		$rs = $dbADO->Execute($queryGetValues,array($objID));
		if ($rs->RecordCount()==1) {
			$row=$rs->FetchRow();			
			$reroute = (int)$row['RerouteMessagesToParent'];
			$autocreate = (int)$row['AutoCreateChildren'];
			$controlledVia = (int)$row['FK_DeviceTemplate_ControlledVia'];
			$deviceID = (int)$row['FK_DeviceTemplate'];
			$rs->Close();
		}
		
		
		
		$alreadySelectedControlledVia = array();
		$alreadySelectedControlledVia[]=$deviceID;
		$querySelectControlledViaForDevice = "
			select MDL_CV.FK_DeviceTemplate_ControlledVia
								FROM DeviceTemplate_DeviceTemplate_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 
								INNER JOIN DeviceTemplate MDL2 on FK_DeviceTemplate_ControlledVia  = MDL2.PK_DeviceTemplate
								INNER JOIN DeviceCategory DC on MDL2.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ? AND FK_DeviceTemplate_ControlledVia != $controlledVia
			";
		$rs = $dbADO->Execute($querySelectControlledViaForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedControlledVia[]=$row['FK_DeviceTemplate_ControlledVia'];
		}
		$rs->Close();
		
		
		/* @var $dbADO ADOConnection */
		/* @var $rs ADORecordSet */
				
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editControlledViaToMasterDevice">
		<input type="hidden" name="section" value="editControlledViaToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="objID" value="'.$objID.'">
		'.(@(int)$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID']!=0?'<input type="hidden" name="controlledVia" value="'.$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'].'">':'').'
		<input type="hidden" name="from" value="'.$from.'">
		
			<table>			
				<tr>
					<td>'.$TEXT_CONTROLLED_VIA_CONST.':</td>
					<td>
						'.(@(int)$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID']!=0
								?'You have selected : <b>'.$_SESSION['editControlledViaToMasterDevice']['deviceTemplateName'].'</b>&nbsp;&nbsp;'
								:(
									isset($_GET['msg'])
									?'<b>'.strip_tags($_GET['msg']).'</b>'
									:'')
								 ).' 
								 <input type="text" name="ControlledViaDeviceTemplateID" size="20" value="'.(isset($_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'])?$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID']:$controlledVia).'">
								 <input type="submit" class="button" name="searchDeviceTemplate" value="Search [...]" >
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
							
							$selectPipesUsed = 'SELECT DeviceTemplate_DeviceTemplate_ControlledVia_Pipe.*,
														DT1.Description as Desc_From,
														DT1.PK_DeviceTemplate as FK_From,
														DT2.Description as Desc_To,
														DT2.PK_DeviceTemplate as FK_To													
													 FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
														INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia on FK_DeviceTemplate_DeviceTemplate_ControlledVia = PK_DeviceTemplate_DeviceTemplate_ControlledVia
														INNER JOIN DeviceTemplate DT1 ON DT1.PK_DeviceTemplate = FK_DeviceTemplate
														INNER JOIN DeviceTemplate DT2 ON DT2.PK_DeviceTemplate = FK_DeviceTemplate_ControlledVia
												WHERE  FK_DeviceTemplate_DeviceTemplate_ControlledVia = ?';
							$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($objID));
							$displayedPipes=array();
							
							if ($resSelectPipesUsed) {
								while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {
									$displayedPipes[]=$rowSelectedPipesUsed['FK_Pipe'];
									$out.='<tr><td>'.$rowSelectedPipesUsed['Desc_To']." <input type='hidden' name='deviceTo_{$rowSelectedPipesUsed['FK_To']}'> &nbsp;&nbsp;&nbsp;&nbsp;</td>";
									
									$resSelectInputs->MoveFirst();			
									$selectInputsTxt='';
									while ($rowSelInputs = $resSelectInputs->FetchRow()) {
										$selectInputsTxt.= '<option '.($rowSelInputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Input']?" selected='selected' ":"").' value="'.$rowSelInputs['PK_Command'].'">'.$rowSelInputs['Description'].'</option>';
									}
									
									$out.='<td> Input on '.($rowSelectedPipesUsed['ToChild']==0?$rowSelectedPipesUsed['Desc_To']:$rowSelectedPipesUsed['Desc_From']).' <select name="input_'.$rowSelectedPipesUsed['FK_To'].'"><option value="0">-please select-</option>'.$selectInputsTxt.'</select></td>';
									
									$resSelectOutputs->MoveFirst();			
									$selectOutputsTxt='';
									while ($rowSelOutputs = $resSelectOutputs->FetchRow()) {
										$selectOutputsTxt.= '<option '.($rowSelOutputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Output']?" selected='selected' ":"").' value="'.$rowSelOutputs['PK_Command'].'">'.$rowSelOutputs['Description'].'</option>';
									}
									
									$out.='<td> Output on '.($rowSelectedPipesUsed['ToChild']==0?$rowSelectedPipesUsed['Desc_From']:$rowSelectedPipesUsed['Desc_To']).' <select name="output_'.$rowSelectedPipesUsed['FK_To'].'"><option value="0">-please select-</option>'.$selectOutputsTxt.'</select></td>';
									
						
									$resSelectPipes->MoveFirst();			
									$selectPipesTxt='';
									while ($rowSelPipes = $resSelectPipes->FetchRow()) {
										$selectPipesTxt.= '<option '.($rowSelPipes['PK_Pipe']==$rowSelectedPipesUsed['FK_Pipe']?" selected='selected' ":"").' value="'.$rowSelPipes['PK_Pipe'].'">'.$rowSelPipes['Description'].'</option>';
									}
									
									$out.='<td> Pipe <select name="pipe_'.$rowSelectedPipesUsed['FK_To'].'"><option value="0">-please select-</option>'.$selectPipesTxt.'</select></td>';
									$out.='<td> Flip In/Out: <input type="checkbox" '.($rowSelectedPipesUsed['ToChild']==1?" checked='checked'":'').' name="toChild_'.$rowSelectedPipesUsed['FK_To'].'" value="1"></td>';
									$out.='<td><input value="Delete" type="button" class="button" onClick="if (confirm(\'Are you sure you want to delete this pipe?\')) {windowOpen(\'index.php?section=deletePipeFromDeviceTemplateControlledVia&objID='.$rowSelectedPipesUsed['FK_DeviceTemplate_DeviceTemplate_ControlledVia'].'&from=editControlledViaToMasterDevice\',\'width=100,height=100,toolbar=1,scrollbars=1,resizable=1\');}"></td>';
									
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
				</tr>';
								
				$out.='<tr>
							<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editControlledViaToMasterDevice");
 			//frmvalidator.addValidation("controlledVia","dontselect=0","'.$TEXT_PLEASE_SELECT_A_DEVICE_CONST.'"); 						
		</script>
		';
		
	} else {
		$controlledViaDeviceTemplateID = cleanInteger($_POST['ControlledViaDeviceTemplateID']);		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';		
		$objID = isset($_POST['objID'])?cleanString($_POST['objID']):'0';				
		$reroute = cleanInteger(@$_POST['RerouteMessagesToParent']);
		$autocreate = cleanInteger(@$_POST['AutoCreateChildren']);
		
		
		
		
		if(isset($_POST['addPipe']) && (int)$_POST['newPipe']!=0){
			$newPipe=(int)$_POST['newPipe'];
			$insertPipe = "INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe (FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe) values (?,?)";
			$query = $dbADO->Execute($insertPipe,array($objID,$newPipe));
					
			header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$objID");
			exit();
		}
		
		$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'] = 0;
		$_SESSION['editControlledViaToMasterDevice']['deviceTemplateName'] = '';
		
		
			$querySelectRemainingControlledViaForDevice = "
							select  PK_DeviceTemplate,Description from DeviceTemplate 
							where PK_DeviceTemplate = ?";
						$rs = $dbADO->Execute($querySelectRemainingControlledViaForDevice,array($controlledViaDeviceTemplateID));
						while ($row = $rs->FetchRow()) {
								$_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'] = $row['PK_DeviceTemplate'];
								$_SESSION['editControlledViaToMasterDevice']['deviceTemplateName'] = $row['Description'];
						}
			$rs->Close();
		
			
		if (isset($_POST['searchDeviceTemplate'])) {
			if ($_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'] !=0) {
				header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$objID&deviceID=$deviceID");
				exit();
			} else {
				header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$objID&deviceID=$deviceID&msg=Invalid+ID");
				exit();				
			}
		} 
		
			
		//pipes
		$selectPipesUsed = 'SELECT DeviceTemplate_DeviceTemplate_ControlledVia_Pipe.*,
														DT1.Description as Desc_From,
														DT1.PK_DeviceTemplate as FK_From,
														DT2.Description as Desc_To,
														DT2.PK_DeviceTemplate as FK_To													
													 FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
														INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia on FK_DeviceTemplate_DeviceTemplate_ControlledVia = PK_DeviceTemplate_DeviceTemplate_ControlledVia
														INNER JOIN DeviceTemplate DT1 ON DT1.PK_DeviceTemplate = FK_DeviceTemplate
														INNER JOIN DeviceTemplate DT2 ON DT2.PK_DeviceTemplate = FK_DeviceTemplate_ControlledVia
												WHERE  FK_DeviceTemplate_DeviceTemplate_ControlledVia = ?
						';
		$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($objID));
		
		$affectedPipes = 'Pipes not updated!';
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {	
			$input=(cleanInteger(@$_POST['input_'.$rowSelectedPipesUsed['FK_To']])!=0)?cleanInteger(@$_POST['input_'.$rowSelectedPipesUsed['FK_To']]):NULL;
			$outputs=(cleanInteger(@$_POST['output_'.$rowSelectedPipesUsed['FK_To']])!=0)?cleanInteger(@$_POST['output_'.$rowSelectedPipesUsed['FK_To']]):NULL;
			$pipe=cleanInteger(@$_POST['pipe_'.$rowSelectedPipesUsed['FK_To']]);
			$toChild = cleanInteger(@$_POST['toChild_'.$rowSelectedPipesUsed['FK_To']]);
			$deviceTo=$rowSelectedPipesUsed['FK_To'];
			
			if($pipe!=0){
				$updateDevicePipe = 'UPDATE DeviceTemplate_DeviceTemplate_ControlledVia_Pipe SET FK_Command_Input=?,FK_Command_Output=?,FK_Pipe=?,ToChild=? WHERE FK_DeviceTemplate_DeviceTemplate_ControlledVia = ? ';
				$res=$dbADO->Execute($updateDevicePipe,array($input,$outputs,$pipe,$toChild,$objID));
			}
				$affectedPipes = $dbADO->Affected_Rows()==1 && $affectedPipes == 'Pipes not updated!'?"Pipes updated!":"Pipes not updated!";
		}
		
		
		//RerouteMessagesToParent AutoCreateChildren
		
		if ((int)$controlledViaDeviceTemplateID!=0) {
			$insertObjToDevice = 'update DeviceTemplate_DeviceTemplate_ControlledVia
					set FK_DeviceTemplate_ControlledVia=?,
					RerouteMessagesToParent=?,AutoCreateChildren=?
			where  PK_DeviceTemplate_DeviceTemplate_ControlledVia = ? 
					';
			$query = $dbADO->Execute($insertObjToDevice,array($_SESSION['editControlledViaToMasterDevice']['deviceTemplateID'],$reroute,$autocreate,$objID));
			
			$out.="
			<script>
				alert('Controlled via ".($dbADO->Affected_Rows()==1?"":"not")." modified! {$affectedPipes}');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				//self.close();
				document.location.href='index.php?section=editControlledViaToMasterDevice&from=$from&objID=$objID';
			</script>
			";
			$_SESSION['editControlledViaToMasterDevice']=array();		
		} else {			
			header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$objID&deviceID=$deviceID");
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

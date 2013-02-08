<?php
function wizardScenarios($output,$dbADO) {
	// obsolete file
	
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/wizardScenarios.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

//	$dbADO->debug=true;			
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$commandGroupID = (int)$_REQUEST['cgID'];
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
		
	if ($action=='form') {
		$query = "SELECT * FROM CommandGroup WHERE PK_CommandGroup = ? AND FK_Installation = ?";
		$resCommandGroupDetails = $dbADO->Execute($query,array($commandGroupID,$installationID));
		
		
		if (!$resCommandGroupDetails || $resCommandGroupDetails->RecordCount()!=1) {
			$out='This command group does not exists or you don\'t have access to it!';
		} else {
			$rowCommandGroupDetails = $resCommandGroupDetails->FetchRow();
			
			$out.='
			<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
			</script>
			<form action="index.php" method="POST" name="wizardScenarios">
				<input type="hidden" name="section" value="wizardScenarios">
				
				<input type="hidden" name="action" value="add">	
				<input type="hidden" name="cgID" value="'.$commandGroupID.'">
				<h2>Edit My Scenario</h2> 
				<table>
					<tr>
						<td>Description</td>
						<td><input type="text" size="20" name="description" value="'.$rowCommandGroupDetails['Description'].'"></td>
					</tr>
					<tr>
						<td>Hint</td>
						<td><input type="text" size="20" name="Hint" value="'.$rowCommandGroupDetails['Hint'].'"></td>
					</tr>

			
				<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="Save"  ></td></tr>
				';
						
				$selectCommandsAssigned = "
				SELECT CommandGroup_Command.* FROM 
					CommandGroup_Command				
					INNER JOIN 
						Device ON CommandGroup_Command.FK_Device = Device.PK_Device	
					INNER JOIN
						Command ON CommandGroup_Command.FK_Command=Command.PK_Command
					WHERE 
						Device.FK_Installation = ?
					AND CommandGroup_Command.FK_CommandGroup = ?
				";
				$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($installationID,$commandGroupID));

				if ($resCommandAssigned->RecordCount()>0) {
				$out.='<tr>
					<td colspan="2">
							<fieldset>
								<legend>Commands</legend>
								<table>
									';
				
				while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {
					$out.='
						<tr><td valign="top">
							<select name="device_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">
								<option value="0">-please select-</option>
						';
							
							$query = 'SELECT * From Device WHERE FK_Installation = ? order by Description ASC';
							$resDevices=$dbADO->Execute($query,array($installationID));
							
							if ($resDevices) {
								while ($rowDevice = $resDevices->FetchRow()) {
									$out.='<option '.($rowDevice['PK_Device']==$rowCommandAssigned['FK_Device']?' selected="selected" ':'').' value="'.$rowDevice['PK_Device'].'">'.$rowDevice['Description'].'</option>';
								}
							}
						
						$out.='
					 		</select></td>
						<td>
						';
						
						$query = "SELECT PK_Command,Command.Description
												FROM Device 
													INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
													INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
													INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
										   WHERE PK_Device=?";
						$resNewCommand = $dbADO->Execute($query,array($rowCommandAssigned['FK_Device']));
						if ($resNewCommand) {
							$out.='<select name="deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">
								<option value="0">-please select-</option>';
									while ($rowNewCommand = $resNewCommand->FetchRow()) {
										$out.='<option '.($rowCommandAssigned['FK_Command']==$rowNewCommand['PK_Command']?'selected="selected"':'').' value="'.$rowNewCommand['PK_Command'].'">'.$rowNewCommand['Description'].'</option>';
									}
									if ($resNewCommand->RecordCount()==0) {
										$out.='<option value="0">-no command-</option>';
									}
						$out.='</select>';
						}
						
						
							
						
					$out.='
					
							</td>
						<td valign="top">
						<input type="button" class="button" name="editA" value="Remove" onClick="windowOpen(\'index.php?section=deleteCommandFromCommandGroup_Command&from=wizardScenarios&cgID='.$rowCommandAssigned['PK_CommandGroup_Command'].'\',\'width=100,height=100,toolbar=1,resizable=1,scrollbars=1\');"">
						</td>						
					</tr>
					';
				}
				
				$out.='
									<tr><td colspan="3" align="center"><input type="submit" class="button" name="addNewDeviceButton" value="Save changes"  ></td></tr>
								</table>
							</fieldset>
					</td>
				</tr>
				';
				}

				$out.='
					</td>
				</tr>
				</table>	
			</form>
			<script>
		 		var frmvalidator = new formValidator("wizardScenarios");
 				frmvalidator.addValidation("description","req","Please enter a description");			
			</script>
			';
		}
		
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if ($canModifyInstallation) {
			// processing form
				
		}			

			$out.="
			<script>
			    //opener.document.forms.{$from}.action.value='form';
				//opener.document.forms.{$from}.submit();
				//self.close();
				document.location.href='index.php?section=wizardScenarios&from=$from&cgID=$commandGroupID&useWizard=$useWizard';
			</script>
			";
			$_SESSION['wizardScenarios']=array();
	}
	
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();
}
?>

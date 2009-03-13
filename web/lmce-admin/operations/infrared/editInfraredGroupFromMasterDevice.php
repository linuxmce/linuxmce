<?php
function editInfraredGroupFromMasterDevice($output,$dbADO) {
	// obsolete file
//	$dbADO->debug=true;
	$out='';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	
	if ($action=='form') {
		
		$infraredGroupID = (int)$_REQUEST['infraredGroupID'];
		$deviceID = (int)$_REQUEST['deviceID'];
	
		$queryGetValues = "SELECT * FROM InfraredGroup where PK_InfraredGroup = ?";
		$rs = $dbADO->Execute($queryGetValues,array($infraredGroupID));
		if ($rs->RecordCount()==1) {
			$row=$rs->FetchRow();
			$deviceCategoryID = (int)$row['FK_DeviceCategory'];
			$infraredGroupName = $row['Description'];
			$rs->Close();
		}

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="POST" name="editInfraredGroupFromMasterDevice">
		<input type="hidden" name="section" value="editInfraredGroupFromMasterDevice">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Device Category:</td>
					<td>
						<select name="deviceCategory">
						<option value="0">-please select-</option>
						';
		
						$queryMasterDeviceCategories_parents = 'SELECT PK_DeviceCategory,Description FROM DeviceCategory WHERE FK_DeviceCategory_Parent IS NULL ORDER BY Description ASC';
						$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row['PK_DeviceCategory']==$deviceCategoryID?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].'</option>';
								$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$row['Description'],$deviceCategoryID,'',$dbADO);
							}
						$rs->Close();
					$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td>Description:</td>
					<td>
						<input name="Description" value="'.stripslashes($infraredGroupName).'" type="text" size="20">						
					</td>
				</tr>		
				<tr>
					<td colspan="2">		
						<fieldset>
							<legend>Commands:</legend>							
							<table>
							';
							$query = "SELECT InfraredGroup_Command.*, Command.Description						
											FROM InfraredGroup_Command 
												INNER JOIN Command on FK_Command = PK_Command
									  WHERE FK_InfraredGroup = $infraredGroupID
									  order by Command.Description asc";
							$res = $dbADO->Execute($query);
							$commandsDisplayed = array();
							$commandsDisplayed[]=0;
								if ($res) {
									while ($row=$res->FetchRow()) {
										$out.='<tr><td>#'.$row['Description'].' #'.$row['FK_Command'].'</td><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=editInfraredGroupFromMasterDevice&commandID='.$row['FK_Command'].'\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">Edit</a> ';
										$out.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteInfraredCommandFromMasterDevice&from=editInfraredGroupFromMasterDevice&infraredGroupID='.$infraredGroupID.'&commandID='.$row['FK_Command'].'\',\'width=100,height=100,toolbar=1,resizable=1,scrollbars=1\');">Delete</a>';
										$out.='</td></tr>';
										$commandsDisplayed[]=$row['FK_Command'];
									}
								}
							$queryGetCommands = "select PK_Command,Description from Command where PK_Command NOT IN (".join(",",$commandsDisplayed).") order by Description asc";
							$otherCommands = '<option value="0">-please select-</option>';
							$res = $dbADO->Execute($queryGetCommands);
							if ($res->RecordCount()==0) {
								$otherCommands = '<option value="0">No commands left</option>';
							}
								if ($res) {
									while ($row=$res->FetchRow()) {
										$otherCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';			
									}
								}
					$out.='
							</table>
						<br />Add this command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherCommands.'</select><input type="submit" class="button" name="submitX" value="Add">';
						$out.='<br /><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommand&from=editInfraredGroupFromMasterDevice\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">Create new command</a>';
						$out.='<input type="hidden" name="displayedCommands" value="'.(join(",",$commandsDisplayed)).'">
						</fieldset>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editInfraredGroupFromMasterDevice");
 			frmvalidator.addValidation("deviceCategory","dontselect=0","Please select a category"); 			
			frmvalidator.addValidation("Description","req","Please enter a description");
		</script>
		';
		
	} else {
		$lastAction='';
		$deviceCategory = isset($_POST['deviceCategory'])?cleanString($_POST['deviceCategory']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';				
		$description = cleanString($_POST['Description'],25);
		$infraredGroupID = cleanInteger($_POST['infraredGroupID']);
		
		$addNewCommandToThisCommandGroup = (int)$_POST['addNewCommandToDeviceCommandGroup'];
		if ($addNewCommandToThisCommandGroup!=0) {
			$query = "INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command) values(?,?)";
			$dbADO->Execute($query,array($infraredGroupID,$addNewCommandToThisCommandGroup));			
			$lastAction='CommandDescription_'.$addNewCommandToThisCommandGroup;
		}
		
		if ((int)$deviceCategory!=0 && (int)$deviceID!=0 && $description!='') {
			$insertObjToDevice = 'UPDATE InfraredGroup 
					SET FK_DeviceCategory=?,
						Description=?
					WHERE   PK_InfraredGroup = ? 
					';			
			$query = $dbADO->Execute($insertObjToDevice,array($deviceCategory,stripslashes($description),$infraredGroupID));
			$out.="";
			header("Location: index.php?section=editInfraredGroupFromMasterDevice&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=$lastAction");
		}		
	}
	
	$onLoad='';
	

	if ($lastAction!=''?$onLoad.="if (document.forms.editInfraredGroupFromMasterDevice.{$lastAction}) {document.forms.editInfraredGroupFromMasterDevice.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Infrared Group');			
	$output->output();
}
?>

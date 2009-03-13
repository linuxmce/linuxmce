<?php
function editCommandGroupFromMasterDevice($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editCommandGroupFromMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	
	if ($action=='form') {
		
		$commandGroupID = (int)$_REQUEST['commandGroupID'];
		$deviceID = (int)$_REQUEST['deviceID'];
	
		$queryGetValues = "select * from DeviceCommandGroup where PK_DeviceCommandGroup = ?";
		$rs = $dbADO->Execute($queryGetValues,array($commandGroupID));
		if ($rs->RecordCount()==1) {
			$row=$rs->FetchRow();
			$deviceCategoryID = (int)$row['FK_DeviceCategory'];
			$commandGroupName = $row['Description'];
			$rs->Close();
		}

		$otherTemplates=getAssocArray('DeviceTemplate_DeviceCommandGroup','PK_DeviceTemplate','CONCAT(\'#\',PK_DeviceTemplate,\' \',DeviceTemplate.Description) AS Description',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCommandGroup='.$commandGroupID.' AND FK_DeviceTemplate!='.$deviceID,'ORDER BY DeviceTemplate.Description ASC');
		$warning=(count($otherTemplates)>0)?$TEXT_CHANGING_CAN_BREAK_OTHERS_CONST:'';
		$otherTemplatesText=(count($otherTemplates)>0)?join(', ',array_values($otherTemplates)):'N/A';
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editCommandGroupFromMasterDevice">
		<input type="hidden" name="section" value="editCommandGroupFromMasterDevice">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="commandGroupID" value="'.$commandGroupID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_EDIT_COMMAND_GROUP_CONST.' #'.$commandGroupID.'</h3>
			<table width="100%">
				<tr>
					<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
				</tr>
			</table>
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<table>			
				<tr>
					<td><B>'.$TEXT_DEVICE_CATEGORY_CONST.'</B></td>
					<td>
						<select name="deviceCategory">
						<option value="0">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>
						';
		
						$queryMasterDeviceCategories_parents = 'select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null order by Description asc';
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
					<td><B>'.$TEXT_DESCRIPTION_CONST.' *</B></td>
					<td>
						<input name="Description" value="'.stripslashes($commandGroupName).'" type="text" size="20"> <input type="button" class="button" name="button" value="'.$TEXT_ADD_REMOVE_COMMANDS_FOR_CG_CONST.'" onClick="windowOpen(\'index.php?section=commandsPicker&dtID='.$deviceID.'&commandGroupID='.$commandGroupID.'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');">
					</td>
				</tr>		
				<tr>
					<td colspan="2"><B>'.$TEXT_OTHER_DEVICE_TEMPLATES_WHO_USE_DCG_CONST.': '.$otherTemplatesText.'</td>
				</tr>					
				<tr>
					<td colspan="2" class="err"><B>'.$warning.'</b></td>
				</tr>					
				<tr>
					<td colspan="2">		
						<fieldset>
							<legend>'.$TEXT_COMMANDS_CONST.':</legend>							
							<table>
							';
							$query = "SELECT DeviceCommandGroup_Command.*, Command.Description as C_Description						
											FROM DeviceCommandGroup_Command 
												INNER JOIN Command on FK_Command = PK_Command
									  WHERE FK_DeviceCommandGroup = $commandGroupID
									  order by Command.Description asc";
							$res = $dbADO->Execute($query);
							$commandsDisplayed = array();
							$commandsDisplayed[]=0;
								if ($res) {
									while ($row=$res->FetchRow()) {
										$out.="<tr ".(strlen(trim($row['Description']))==0?" bgColor='lightgreen' ":"")."><td>#{$row['C_Description']}<br />#{$row['FK_Command']}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$row['FK_Command']}\">".stripslashes($row['Description']).'</textarea></td><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=editCommandGroupFromMasterDevice&commandID='.$row['FK_Command'].'\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">Edit</a> ';
										$out.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteCommandFromMasterDevice&from=editCommandGroupFromMasterDevice&commandGroupID='.$commandGroupID.'&commandID='.$row['FK_Command'].'\',\'width=100,height=100,toolbar=1,resizable=1,scrollbars=1\');">Delete</a>';
										$out.='</td></tr>';
										$commandsDisplayed[]=$row['FK_Command'];
									}
								}
							$queryGetCommands = "select PK_Command,Description from Command where PK_Command NOT IN (".join(",",$commandsDisplayed).") order by Description asc";
							$otherCommands = '<option value="0">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
							$res = $dbADO->Execute($queryGetCommands);
							if ($res->RecordCount()==0) {
								$otherCommands = '<option value="0">'.$TEXT_NO_COMMANDS_LEFT_CONST.'</option>';
							}
								if ($res) {
									while ($row=$res->FetchRow()) {
										$otherCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';			
									}
								}
							
					$out.='
							</table>
						<br />'.$TEXT_ADD_COMMAND_TO_GROUP_CONST.': <select name="addNewCommandToDeviceCommandGroup">'.$otherCommands.'</select><input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'">';
						$out.='<br /><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommand&from=editCommandGroupFromMasterDevice\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">'.$TEXT_CREATE_NEW_COMMAND_CONST.'</a>';
						$out.='<input type="hidden" name="displayedCommands" value="'.(join(",",$commandsDisplayed)).'">
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
		 	var frmvalidator = new formValidator("editCommandGroupFromMasterDevice");
 			frmvalidator.addValidation("deviceCategory","dontselect=0","'.$TEXT_CATEGORY_REQUIRED_CONST.'"); 			
			frmvalidator.addValidation("Description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		$lastAction='';
		$deviceCategory = isset($_POST['deviceCategory'])?cleanString($_POST['deviceCategory']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';				
		$description = cleanString($_POST['Description'],25);
		$commandGroupID = cleanInteger($_POST['commandGroupID']);
		
		$displayedCommands = explode(",",@$_POST['displayedCommands']);
		if (!is_array($displayedCommands)) {$displayedCommands=array();}
		foreach ($displayedCommands as $elem) {
			$elem = (int)$elem;
			$commandAssocDescription = cleanString(@$_POST['CommandDescription_'.$elem]);
			if ($commandAssocDescription!='') {
				//get old value
				$query = "select Description from DeviceCommandGroup_Command where FK_DeviceCommandGroup = ? and FK_Command = ?";
				$resD = $dbADO->Execute($query,array($commandGroupID,$elem));
				if ($resD && $resD->RecordCount()==1) {
					$rowD = $resD->FetchRow();
					if ($commandAssocDescription!=$rowD['Description']) {
						$lastAction='CommandDescription_'.$elem;
					}
				}
				$query = "update DeviceCommandGroup_Command set Description = ? where FK_DeviceCommandGroup = ? and FK_Command = ?";
				$dbADO->Execute($query,array(stripslashes($commandAssocDescription),$commandGroupID,$elem));
			}
		}
		
		$addNewCommandToThisCommandGroup = (int)$_POST['addNewCommandToDeviceCommandGroup'];
		if ($addNewCommandToThisCommandGroup!=0) {
			$query = "insert into DeviceCommandGroup_Command(FK_DeviceCommandGroup,FK_Command) values(?,?)";
			$dbADO->Execute($query,array($commandGroupID,$addNewCommandToThisCommandGroup));			
			$lastAction='CommandDescription_'.$addNewCommandToThisCommandGroup;
		}
		
		if ((int)$deviceCategory!=0 && (int)$deviceID!=0 && $description!='') {
			$insertObjToDevice = 'UPDATE DeviceCommandGroup 
					SET FK_DeviceCategory=?,
						Description=?
					WHERE   PK_DeviceCommandGroup = ? 
					';			
			$query = $dbADO->Execute($insertObjToDevice,array($deviceCategory,stripslashes($description),$commandGroupID));
			$out.="";
			header("Location: index.php?section=editCommandGroupFromMasterDevice&from=$from&deviceID=$deviceID&commandGroupID=$commandGroupID&lastAction=$lastAction&msg=$TEXT_COMMAND_GROUP_UPDATED_CONST");
		}		
	}
	
	$onLoad='';
	
	if ($lastAction!=''?$onLoad.="if (document.forms.editCommandGroupFromMasterDevice.{$lastAction}) {document.forms.editCommandGroupFromMasterDevice.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_COMMAND_GROUP_CONST);			
	$output->output();
}
?>

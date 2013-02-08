<?php
function createDeviceGroup($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createDeviceGroup.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = isset($_REQUEST['deviceID'])?cleanInteger($_REQUEST['deviceID']):NULL;
	$installationID = (int)$_SESSION['installationID'];
	
	$selectPossibleParents = 'SELECT Description,PK_DeviceGroup FROM DeviceGroup Order By Description ASC';
	$resPossibleParents = $dbADO->_Execute($selectPossibleParents);
	
		
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="createDeviceGroup">
		<input type="hidden" name="section" value="createDeviceGroup">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<h3>'.$TEXT_CREATE_DEVICE_GROUP_CONST.'</h3>		
			<table>			
				<tr>
					<td>Description *</td>
					<td><input type="text" size="15" name="'.$TEXT_DESCRIPTION_CONST.'" value=""></td>
				</tr>
				<tr>
					<td>'.$TEXT_PARENT_CONST.'</td>
					<td>
						<select name="parentID" >
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
						';
		if ($resPossibleParents) {
			while ($rowPossibleParents = $resPossibleParents->fetchRow()) {
				$out.='<option value="'.$rowPossibleParents['PK_DeviceGroup'].'">'.$rowPossibleParents['Description'].'</option>';
			}
		}
		$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>		
		<script>
		 	var frmvalidator = new formValidator("createDeviceGroup");
 			frmvalidator.addValidation("Description","req","'.$TEXT_DEVICE_GROUP_DESCRIPTION_CONFIRMATION_CONST.'");			
		</script>
		';
		
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			$out="
				<script>
			 		opener.top.frames[1].location='index.php?section=editDeviceParams&deviceID=$deviceID&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST;
					self.close();
				</script>
				";		
		}
		else{
		
			$description = cleanString(@$_POST['Description'],50);
			$parentID = isset($_POST['parentID'])?cleanInteger(@$_POST['parentID']):null;
			$deviceID = cleanInteger(@$_POST['deviceID']);
			
			if ($description!='') {
				
				$parentID=$parentID===0?$parentID=NULL:$parentID;
				if (!is_null($parentID)) {
					$queryUpdate = 'INSERT INTO DeviceGroup 
								(Description,FK_Installation,FK_DeviceGroup_Parent)
						  		values(?,?,?)';
					$dbADO->Execute($queryUpdate,array($description,$installationID,$parentID));
				} else {
					$queryUpdate = 'INSERT INTO DeviceGroup 
								(Description,FK_Installation)
						  		values(?,?)';
					$dbADO->Execute($queryUpdate,array($description,$installationID));
					
				}
				
				
				$lastInsert = $dbADO->Insert_ID();
				
				if ($deviceID!=0 && $lastInsert!=0) {
					$insertDeviceToGroup = "
						INSERT INTO Device_DeviceGroup(FK_Device,FK_DeviceGroup)
							VALUES(?,?)
						";
					$query=$dbADO->Execute($insertDeviceToGroup,array($deviceID,$lastInsert));				
					
				}
				$out.="
					<script>
						alert('Group created!');
				 		opener.document.forms.{$from}.action.value='form';
						opener.document.forms.{$from}.submit();
						self.close();
					</script>
					";				
				
			} else {
				header("Location: index.php?section=createDeviceGroup&saved=no description&deviceID=$deviceID");
			}
		}				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CREATE_DEVICE_GROUP_CONST);			
	$output->output();
}
?>

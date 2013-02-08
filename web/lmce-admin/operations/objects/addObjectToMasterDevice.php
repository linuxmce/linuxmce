<?php
function addObjectToMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addObjectToMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	
	
	
	
	
	if ($action=='form') {
		$alreadySelectedObjects = array();
		$alreadySelectedObjects[]=0;
		$querySelectObjectForDevice = "select FK_DesignObj from DeviceTemplate_DesignObj where FK_DeviceTemplate = ?";
		$rs = $dbADO->Execute($querySelectObjectForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedObjects[]=$row['FK_DesignObj'];
		}
		$rs->Close();
		$out.='
		<form action="index.php" method="post" name="addObjectToMasterDevice">
		<input type="hidden" name="section" value="addObjectToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<h3>'.$TEXT_ADD_DESIGN_OBJECTS_TO_USE_AS_REMOTES_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_OBJECTS_CONST.':</td>
					<td>
						<select name="objID">
						<option value="0">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>
						';
						$querySelectRemainingObjectForDevice = "select PK_DesignObj,Description from DesignObj where PK_DesignObj NOT IN (".join(",",$alreadySelectedObjects).")";
						$rs = $dbADO->_Execute($querySelectRemainingObjectForDevice);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_DesignObj'].'">'.cleanString($row['Description']).'</option>';
						}
						$rs->Close();
					$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addObjectToMasterDevice");
 			frmvalidator.addValidation("objID","dontselect=0","'.$TEXT_OBJECT_REQUIRED_CONST.'"); 			
		</script>
		';
		
	} else {
		
		$objID = isset($_POST['objID'])?cleanString($_POST['objID']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		
		if ((int)$objID!=0 && (int)$deviceID!=0) {
			$insertObjToDevice = 'insert into DeviceTemplate_DesignObj(FK_DeviceTemplate,FK_DesignObj) values(?,?)';
			$query = $dbADO->Execute($insertObjToDevice,array($deviceID,$objID));
			$out.="
			<script>
				alert('$TEXT_OBJECT_ADDED_TO_DEVICE_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			header("Location: index.php?section=addObjectToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_DESIGN_OBJECTS_TO_USE_AS_REMOTES_CONST);			
	$output->output();
}
?>

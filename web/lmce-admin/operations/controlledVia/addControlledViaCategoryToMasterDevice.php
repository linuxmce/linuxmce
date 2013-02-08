<?php
function addControlledViaCategoryToMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addControlledViaCategoryToMasterDevice.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
		
	if ($action=='form') {
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
		<form action="index.php" method="post" name="addControlledViaCategoryToMasterDevice">
		<input type="hidden" name="section" value="addControlledViaCategoryToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		
			<table>			
				<tr>
					<td>'.$TEXT_CONTROLLED_VIA_CATEGORY_CONST.' *</td>
					<td>
						<select name="controlledVia">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
						';
						$notIn = join(",",$alreadySelectedControlledViaCategory);

						$queryMasterDeviceCategories_parents = '
							select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null 
							and PK_DeviceCategory NOT IN ('.$notIn.')
							order by Description asc';
						$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row['PK_DeviceCategory']==0?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.cleanString($row['Description']).'</option>';
								$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],cleanString($row['Description']),0,$notIn,$dbADO);
							}
						$rs->Close();
						
						
					$out.='
						</select>
					</td>
				</tr>
				
				<tr>
					<td>'.$TEXT_REROUTE_MESSAGES_TO_PARENT_CONST.':</td>
					<td>
						<input name="RerouteMessagesToParent" value="1" type="checkbox">
					</td>
				</tr>
				<tr>
					<td>'.$TEXT_AUTO_CREATE_CHILDREN_CONST.':</td>
					<td>
						<input name="AutoCreateChildren" value="1" type="checkbox">
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>					
		<script>
		 	var frmvalidator = new formValidator("addControlledViaCategoryToMasterDevice");
 			frmvalidator.addValidation("controlledVia","dontselect=0","'.$TEXT_PLEASE_SELECT_A_DEVICE_CONST.'"); 
		</script>
		';
		
	} else {
		$controlledVia = isset($_POST['controlledVia'])?cleanString($_POST['controlledVia']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';		
		$reroute = cleanInteger(@$_POST['RerouteMessagesToParent']);
		$autocreate = cleanInteger(@$_POST['AutoCreateChildren']);
		
		if ((int)$controlledVia!=0 && (int)$deviceID!=0 ) {
			$insertObjToDevice = 'insert into DeviceTemplate_DeviceCategory_ControlledVia
					(FK_DeviceTemplate, FK_DeviceCategory,RerouteMessagesToParent,AutoCreateChildren) 
					values(?,?,?,?)';
			$query = $dbADO->Execute($insertObjToDevice,array($deviceID,$controlledVia,$reroute,$autocreate));
			
			$lastInsert = $dbADO->Insert_ID();
			
			if ($autocreate==1) {
//				$insertPipe = "INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia_Pipe (FK_DeviceTemplate_DeviceCategory_ControlledVia) values (?)";
//				$query = $dbADO->Execute($insertPipe,array($lastInsert));
				
				header("Location: index.php?section=editControlledViaCategoryToMasterDevice&from=$from&objID=$lastInsert");
				exit();
			}
			
			$out.="
			<script>
				alert('$TEXT_CONTROLLED_VIA_CATEGORY_ADDED_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			header("Location: index.php?section=addControlledViaCategoryToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_CONTROLLED_VIA_DEVICE_CATEGORY_CONST);			
	$output->output();
}
?>

<?php
function addInfraredGroup($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
	
	$queryMasterDeviceCategories_parents = '
							select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null order by Description asc';
	$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
	$deviceCategsTxt = '<option value="0">-please select-</option>';
		while ($row = $rs->FetchRow()) {
			$deviceCategsTxt.='<option '.($row['PK_DeviceCategory']==0?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].'</option>';
			$deviceCategsTxt.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$row['Description'],0,'',$dbADO);
		}
	$rs->Close();
						
	
						
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="addInfraredGroup">
		<input type="hidden" name="section" value="addInfraredGroup">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Infrared group name:</td>
					<td><input type="text" size="15" name="InfraredGroupDescription" value=""></td>
				</tr>
				<tr>
					<td>Device Category:</td>
					<td>
						<select name="deviceCategs" >
						'.$deviceCategsTxt.'
						</select>
					</td>
				</tr>
				<tr>
					<td>Manufacturer:</td>
					<td>
						<select name="Manufacturer">
						<option value="0">-please select-</option>
		';
		$queryManufacturers = 'select Description, PK_Manufacturer from Manufacturer order by Description asc';
		$rs = $dbADO->_Execute($queryManufacturers);
							while ($row = $rs->FetchRow()) {								
									$out.='<option value="'.$row['PK_Manufacturer'].'">'.stripslashes($row['Description']).'</option>';
							}
		$out.='			</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addInfraredGroup"); 			
			frmvalidator.addValidation("InfraredGroupDescription","req","Please enter a name for this Command group!");
			frmvalidator.addValidation("deviceCategs","dontselect=0","Please select a device category!");
			frmvalidator.addValidation("Manufacturer","dontselect=0","Please select a manufacturer!");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$InfraredGroupDescription = cleanString(@$_POST['InfraredGroupDescription'],50);
		$deviceCategs = (int)@$_POST['deviceCategs'];
		$FK_Manufacturer = (int)@$_POST['Manufacturer'];		
		
		if ($InfraredGroupDescription!='' && $deviceCategs!=0) {
			$queryInsertCmdGroup = 'INSERT INTO InfraredGroup (FK_Manufacturer, Description,FK_DeviceCategory) values(?,?,?)';
			$dbADO->Execute($queryInsertCmdGroup,array($FK_Manufacturer,$InfraredGroupDescription,$deviceCategs));
			$insertID = $dbADO->Insert_ID();
			
			$queryDevicesTemplates='SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory=?';
			$resDeviceTemplates=$dbADO->Execute($queryDevicesTemplates,array($deviceCategs));
			while($row = $resDeviceTemplates->FetchRow()){
				$ins=$dbADO->Execute('INSERT INTO DeviceTemplate_InfraredGroup (FK_DeviceTemplate , FK_InfraredGroup) VALUES(?,?)',array($row['PK_DeviceTemplate'],$insertID));
			}
				$out.="
				<script>
					alert('Infrared group added!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.lastAction.value='InfraredGroup_{$insertID}';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=addInfraredGroup&from=$from&deviceID=$deviceID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Add Infrared Group');			
	$output->output();
}
?>

<?php
function confirmDevicePackages($output,$dbADO) {
	// obsolete file
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
	$DeviceTemplate=(int)@$_REQUEST['DeviceTemplate'];
	$Package=(int)@$_REQUEST['Package'];
	
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="confirmDevicePackages">
		<input type="hidden" name="section" value="confirmDevicePackages">
		<input type="hidden" name="action" value="display">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="DeviceTemplate" value="'.$DeviceTemplate.'">
		<input type="hidden" name="Package" value="'.$Package.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>
				<tr>
					<td>Directory</td>
					<td>Path</td>
					<td>Verified</td>
				</tr>
		';
			$displayedPackages=array();
			$query='SELECT * FROM Package_Directory
						INNER JOIN Directory ON FK_Directory=PK_Directory
						LEFT JOIN Device_Package_Directory ON FK_Package_Directory = PK_Package_Directory
					WHERE FK_Package =?';
			$res=$dbADO->Execute($query,array($Package));
			while ($row = $res->FetchRow()){
				$displayedPackages[]=$row['PK_Package_Directory'];
				$out.='
					<tr>
						<td>'.$row['Description'].'</td>
						<td><input type="text" name="path_'.$row['PK_Package_Directory'].'" value="'.$row['Path'].'"></td>
						<td><input type="checkbox" '.(($row['Verified']==1)?'checked':'').' disabled></td>
					</tr>
				';
			}
			$out.='			
				<tr>
					<td colspan="3" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
			<input type="hidden" name="displayedPackages" value="'.join(",",$displayedPackages).'">
		</form>
		';
		
	} else{
	// processing area

			
		$displayedPackages = cleanString(@$_POST['displayedPackages']);
		$packagesArray=explode(",",$displayedPackages);
		foreach($packagesArray as $value){
			$path=cleanString($_POST['path_'.$value]);
			$query='SELECT * FROM Device_Package_Directory WHERE FK_Device=? AND FK_Package_Directory=?';
			$res=$dbADO->Execute($query,array($deviceID,$value));
			if($res->RecordCount()==0){
		
				$insertDevice_Package_Directory='INSERT INTO Device_Package_Directory (FK_Device, FK_Package_Directory,Path,Verified)
				VALUES(?,?,?,?)';
				if($path!='')
					$dbADO->Execute($insertDevice_Package_Directory,array($deviceID,$value,$path,1));
			}else{
				$updateDevice_Package_Directory='UPDATE Device_Package_Directory SET Path=? WHERE FK_Device=? AND FK_Package_Directory=?';
				if($path!='')
					$dbADO->Execute($updateDevice_Package_Directory,array($path,$deviceID,$value));
			
			}
		}
		
		
		$out.="
			<script>
		
				self.location='index.php?section=confirmDevicePackages&deviceID=".$deviceID."&from=confirmPackages&DeviceTemplate=".$DeviceTemplate."&Package=".$Package."&action=form';
			</script>
			";		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

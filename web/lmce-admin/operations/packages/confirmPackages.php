<?php
function confirmPackages($output,$dbADO) {
	// obsolete file
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
	$DeviceTemplate=(int)@$_REQUEST['DeviceTemplate'];
	
	$out='<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>';		
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="confirmPackages">
		<input type="hidden" name="section" value="confirmPackages">
		<input type="hidden" name="action" value="display">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="DeviceTemplate" value="'.$DeviceTemplate.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Need configure:</td>';
			$queryDeviceDeviceData='SELECT NeedConfigure FROM Device WHERE PK_Device=?';
			$res=$dbADO->execute($queryDeviceDeviceData,array($deviceID));
			$row=$res->FetchRow();
			$out.='	<td><input type="checkbox" size="15" name="needConfigure" value="1" '.(($row['NeedConfigure']==1)?'checked':'').' onClick="javascript:this.form.submit();"></td>
					<td>Is development:</td>';
			$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData 
										LEFT JOIN DeviceData ON FK_DeviceData=PK_DeviceData
									WHERE FK_Device=? AND Description=?';
			$res=$dbADO->execute($queryDeviceDeviceData,array($deviceID,'Development'));
			$isDevelopment=($res->RecordCount()==0)?0:1;
			$out.='	<td><input type="checkbox" size="15" name="isDevelopment" value="1" '.(($isDevelopment==1)?'checked':'').' onClick="javascript:this.form.submit();"></td>
					<td>Distro:</td>';
			$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData 
										LEFT JOIN DeviceData ON FK_DeviceData=PK_DeviceData
									WHERE FK_Device=? AND Description=?';
			$res=$dbADO->execute($queryDeviceDeviceData,array($deviceID,'PK_Distro'));
			if($res->RecordCount()!=0){
				$row=$res->FetchRow();
				$selectedDistro=$row['IK_DeviceData'];
				$oldSelectedDistro=$row['IK_DeviceData'];
			}else{
				$oldSelectedDistro='none';
			}
			$out.='<input type="hidden" name="oldSelectedDistro" value="'.$oldSelectedDistro.'">';
			$out.='<input type="hidden" name="oldIsDevelopment" value="'.$isDevelopment.'">';
			
			$out.=	'<td><select name="FK_Distro" onChange="javascript:this.form.submit();">
						<option value="0">- Any -</option>
						';
						$queryDistroOperatingSystem = "
							SELECT  Distro.*, OperatingSystem.Description AS OS 
								FROM Distro,OperatingSystem 
							WHERE Distro.FK_OperatingSystem= OperatingSystem.PK_OperatingSystem 
								ORDER BY Distro.Description ASC";
						$rs = $dbADO->_Execute($queryDistroOperatingSystem);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_Distro'].'" '.(($row['PK_Distro']==@$selectedDistro)?'selected':'').'>'.$row['Description']." / {$row['OS']} </option>";
						}
						$rs->Close();
					$out.='
						</select>
					</td>
					<td align="left"><input type="submit" class="button" name="submitX" value="Display"></td>
				</tr>
				<tr>';
		$out.='
				<table>
					<tr>
						<td><b>Package name</b></td>
						<td><b>Name</b></td>
						<td><b>Repository</b></td>
						<td><b>Version</b></td>
						<td><b>Source</b></td>
					</tr>
				';
				$displayedPackagesArray=array();
				if(isset($selectedDistro)&& $selectedDistro!=0)
					$query='SELECT DeviceTemplate_Package .  * , Package.Description AS PackageName, Package.PK_Package, Package_Distro.Name AS PackageDistroName, Package_Distro.Repository, Package_Distro.Version, Package_Distro.FK_Package AS PackageDistroFK_Package, RepositorySource.Description AS RepositorySource
								FROM DeviceTemplate_Package
							INNER  JOIN Package ON DeviceTemplate_Package.FK_Package = PK_Package
							LEFT  JOIN Package_Distro ON DeviceTemplate_Package.FK_Package = Package_Distro.FK_Package
							LEFT  JOIN RepositorySource ON FK_RepositorySource = PK_RepositorySource
								WHERE DeviceTemplate_Package.FK_DeviceTemplate=? AND DeviceTemplate_Package.DevelopmentOnly=? AND (DeviceTemplate_Package.FK_Distro='.$selectedDistro.' OR DeviceTemplate_Package.FK_Distro IS NULL)';
				else
					$query='SELECT DeviceTemplate_Package .  * , Package.Description AS PackageName, Package.PK_Package, Package_Distro.Name AS PackageDistroName, Package_Distro.Repository, Package_Distro.Version, Package_Distro.FK_Package AS PackageDistroFK_Package, RepositorySource.Description AS RepositorySource
								FROM DeviceTemplate_Package
							INNER JOIN Package ON DeviceTemplate_Package.FK_Package = PK_Package
							LEFT  JOIN Package_Distro ON DeviceTemplate_Package.FK_Package = Package_Distro.FK_Package
							LEFT  JOIN RepositorySource ON FK_RepositorySource = PK_RepositorySource

						WHERE DeviceTemplate_Package.FK_DeviceTemplate=? AND DeviceTemplate_Package.DevelopmentOnly=?';
				$res=$dbADO->Execute($query,array($DeviceTemplate,$isDevelopment));
				while ($row = $res->FetchRow()){
					$displayedPackagesArray[]=$row['FK_Package'];
					$out.='
					<tr>
						<td><b>'.$row['PackageName'].'<b></td>
						<td>'.$row['PackageDistroName'].'</td>
						<td>'.$row['Repository'].'</td>
						<td>'.$row['Version'].'</td>
						<td>'.$row['RepositorySource'].'</td>
					</tr>
					<tr>
						<td colspan="5">';
						$displayedPackagesDirectory=array();
						$query='SELECT * FROM Package_Directory
									INNER JOIN Directory ON FK_Directory=PK_Directory
									LEFT JOIN Device_Package_Directory ON FK_Package_Directory = PK_Package_Directory
								WHERE FK_Package =?';
						$resPackageDirectory=$dbADO->Execute($query,array($row['PK_Package']));
						while ($rowPackageDirectory = $resPackageDirectory->FetchRow()){
							$displayedPackagesDirectory[]=$rowPackageDirectory['PK_Package_Directory'];
							$out.='
								<tr>
									<td>&nbsp;</td>
									<td>'.$rowPackageDirectory['Description'].'</td>
									<td colspan="2"><input type="text" name="path_'.$rowPackageDirectory['PK_Package_Directory'].'" value="'.$rowPackageDirectory['Path'].'"></td>
									<td><input type="checkbox" '.(($rowPackageDirectory['Verified']==1)?'checked':'').' disabled></td>
								</tr>
							';
						}
			$out.='		
						<input type="hidden" name="displayedPackagesDirectory" value="'.join(",",$displayedPackagesDirectory).'">	
						</td>
					</tr>
					';
				}

		$out.='	</tr>
					<input type="hidden" name="displayedPackagesArray" value="'.join(",",$displayedPackagesArray).'">
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		';
		
	} elseif($action=='display') {
		$needConfigure=((isset($_POST['needConfigure']) && $_POST['needConfigure']==1)?1:0);
		$updateDevice='UPDATE Device SET NeedConfigure=? WHERE PK_Device=?';
		$dbADO->execute($updateDevice,array($needConfigure,$deviceID));
		
		$isDevelopment=(isset($_POST['isDevelopment']) && $_POST['isDevelopment']==1)?1:0;
		$queryDeviceData='SELECT PK_DeviceData FROM DeviceData WHERE Description=?';
		$res=$dbADO->execute($queryDeviceData,array('Development'));
		$row=$res->fetchRow();
		$isDevelopmentDeviceDataPK=$row['PK_DeviceData'];
		$oldIsDevelopment=$_POST['oldIsDevelopment'];
		
		if($oldIsDevelopment!=$isDevelopment){
			if($isDevelopment==1){
			
				$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
				$dbADO->execute($insertDeviceDeviceData,array($deviceID,$isDevelopmentDeviceDataPK,1));
			}else{
				$delDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$dbADO->execute($delDeviceDeviceData,array($deviceID,$isDevelopmentDeviceDataPK));
			}
		}
		
		$queryDeviceData='SELECT PK_DeviceData FROM DeviceData WHERE Description=?';
		$res=$dbADO->execute($queryDeviceData,array('PK_Distro'));
		$row=$res->fetchRow();
		$selectedDistroDeviceDataPK=$row['PK_DeviceData'];

		$FK_Distro=(int)$_POST['FK_Distro'];
		$oldSelectedDistro=$_POST['oldSelectedDistro'];
		
		if($oldSelectedDistro=='none'){
			// insert
			$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
			$dbADO->execute($insertDeviceDeviceData,array($deviceID,$selectedDistroDeviceDataPK,$FK_Distro));
		}else {
			$updateDeviceDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			$dbADO->execute($updateDeviceDeviceData,array($FK_Distro,$deviceID,$selectedDistroDeviceDataPK));
		}
		
		$displayedPackagesDirectory = cleanString(@$_POST['displayedPackagesDirectory']);
		$packagesArray=explode(",",$displayedPackagesDirectory);
		foreach($packagesArray as $value){
			$path=cleanString(@$_POST['path_'.$value]);
			$query='SELECT * FROM Device_Package_Directory WHERE FK_Device=? AND FK_Package_Directory=?';
			$res=$dbADO->Execute($query,array($deviceID,$value));
			if($res->RecordCount()==0){
		
				$insertDevice_Package_Directory='INSERT INTO Device_Package_Directory (FK_Device, FK_Package_Directory,Path,Verified)
				VALUES(?,?,?,?)';
				if($path!='')
					$dbADO->Execute($insertDevice_Package_Directory,array($deviceID,$value,$path,0));
			}else{
				$updateDevice_Package_Directory='UPDATE Device_Package_Directory SET Path=? WHERE FK_Device=? AND FK_Package_Directory=?';
				if($path!='')
					$dbADO->Execute($updateDevice_Package_Directory,array($path,$deviceID,$value));
			
			}
		}

		$out.="
			<script>
				self.location='index.php?section=confirmPackages&deviceID=".$deviceID."&from=confirmPackages&DeviceTemplate=".$DeviceTemplate."&action=form';
			</script>
		";
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

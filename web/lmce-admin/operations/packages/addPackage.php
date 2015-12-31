<?php
function addPackage($output,$dbADO) {
	// obsolete file
	
	//$dbADO->debug=true;
	$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
	';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$subAction = isset($_REQUEST['subAction'])?cleanString($_REQUEST['subAction']):'add';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
		
	if ($action=='form' && $subAction!='edit') {		
		$out.='
		<form action="index.php" method="post" name="addPackage">
		<input type="hidden" name="section" value="addPackage">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="subAction" value="edit">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Package Description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="addDescription" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addPackage");
 			frmvalidator.addValidation("Description","req","Please enter a package description");
		</script>
		';
		
	} elseif($subAction=='edit'){
		$description = cleanString(@$_REQUEST['Description'],30);
		$PK_Package = cleanInteger(@$_REQUEST['PK_Package']);
		
		if(isset($_REQUEST['addDescription'])){
				
			$queryInsertPackage = 'INSERT INTO Package(Description) values(?)';
			$dbADO->Execute($queryInsertPackage,array($description));		
			$PK_Package = $dbADO->Insert_ID();
		}
		
		$out.='
				<form action="index.php" method="post" name="addPackage">
		<input type="hidden" name="section" value="addPackage">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="subAction" value="update">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="PK_Package" value="'.$PK_Package.'">
			<table>			
				<tr>
					<td>Package Description: <input type="text" size="15" name="Description" value="'.$description.'"></td>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2"><fieldset><legend>Where to get</legend>';
			$queryDistroPackage='SELECT * FROM Package_Distro
									INNER JOIN Distro ON FK_Distro=PK_Distro
								WHERE FK_Package=?';
			$resDistroPackage=$dbADO->Execute($queryDistroPackage,array($PK_Package));
			if($resDistroPackage->RecordCount()==0)
				$out.='No distro selected';
			$out.='<table>
					<tr>
						<td>Description</td>
						<td>Name</td>
						<td>Repository</td>
						<td>Version</td>
						<td>RepositorySource</td>
					</tr>';
			$displayedDistro=array();
			while ($row = $resDistroPackage->FetchRow()) {
				$displayedDistro[]=$row['FK_Distro'];
				$out.='
					<tr>
						<td><input type="text" name="DistroDescription_'.$row['FK_Distro'].'" value="'.$row['Description'].'"></td>
						<td><input type="text" name="PackageDistroName_'.$row['FK_Distro'].'" value="'.$row['Name'].'"></td>
						<td><input type="text" name="PackageDistroRepository_'.$row['FK_Distro'].'" value="'.$row['Repository'].'"></td>
						<td><input type="text" name="PackageDistroVersion_'.$row['FK_Distro'].'" value="'.$row['Version'].'"></td>
						<td>
						<select name="FK_RepositorySource_'.$row['FK_Distro'].'">
						<option value="0">-please select-</option>
						';
						$queryRepository = "
							SELECT  RepositorySource.*, RepositorySource.Description AS RepositoryDescription FROM RepositorySource	
								INNER JOIN Distro ON FK_Distro=PK_Distro
							WHERE FK_Distro='".$row['FK_Distro']."' AND (RepositorySource.FK_OperatingSystem='".$row['FK_OperatingSystem']."' OR RepositorySource.FK_OperatingSystem IS NULL) ORDER BY RepositorySource.Description ASC";
						$rs = $dbADO->_Execute($queryRepository);
						
						while ($rowRepository = $rs->FetchRow()) {
								$out.='<option value="'.$rowRepository['PK_RepositorySource'].'" '.(($row['FK_RepositorySource']==$rowRepository['PK_RepositorySource'])?'selected':'').'>'.$rowRepository['RepositoryDescription']."</option>";
						}
						$rs->Close();
					$out.='
						</select>';
					if($row['FK_RepositorySource']!=0)
						$out.=' <a href="javascript:void(0);" title="Edit Repository Source" onClick="windowOpen(\'index.php?section=addRepositorySource&from=addPackage&RepositorySource='.$row['FK_RepositorySource'].'\',\'status=0,resizable=1,width=500,height=350,toolbar=1\');">Edit</a>';
					$out.=' <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deletePackageDistro&from=addPackage&Package='.$PK_Package.'&Distro='.$row['FK_Distro'].'\',\'status=0,resizable=1,width=500,height=350,toolbar=1\');">Delete</a>';
					$out.='
						</td>
					</tr>';
			}
			$out.='<input type="hidden" name="displayedDistro" value="'.join(",",$displayedDistro).'">';
			$out.='</table></fieldset>';			
					$out.='
						<select name="FK_Distro">
						<option value="0">-please select-</option>
						';
						$filterDistro=(count($displayedDistro)>0?'AND Distro.PK_Distro NOT IN ('.join(",",$displayedDistro).')':'');
						$queryDistroOperatingSystem = "
							SELECT  Distro.*, OperatingSystem.Description AS OS 
								FROM Distro,OperatingSystem 
							WHERE Distro.FK_OperatingSystem= OperatingSystem.PK_OperatingSystem $filterDistro
								ORDER BY Distro.Description ASC";
						$rs = $dbADO->_Execute($queryDistroOperatingSystem);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_Distro'].'">'.$row['Description']." / {$row['OS']} </option>";
						}
						$rs->Close();
					$out.='
						</select>
					<input type="submit" class="button" name="submitX" value="Add source"> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDistro&from=addPackage&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">Add Distro</a>
					<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addRepositorySource&from=addPackage&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">Add RepositorySource</a>
					';
					$out.='
					</td>
				<tr>
					<td colspan="2"><fieldset><legend>Requested directories and files</legend>';
			$queryDirectoryPackage='SELECT * FROM Package_Directory
									INNER JOIN Directory ON FK_Directory=PK_Directory
								WHERE FK_Package=? ORDER BY Description ASC';
			$resDirectoryPackage=$dbADO->Execute($queryDirectoryPackage,array($PK_Package));
			if($resDirectoryPackage->RecordCount()==0)
				$out.='No Directory selected';
			$displayedDirectory=array();
			while ($row = $resDirectoryPackage->FetchRow()) {
				$displayedDirectory[]=$row['PK_Directory'];
				$out.='<fieldset><legend>'.$row['Description'].'</legend>';
				
				$queryDirectoryFiles='SELECT * FROM Package_Directory_File WHERE FK_Package_Directory=?';
				$resDirectoryFiles=$dbADO->Execute($queryDirectoryFiles,array($row['PK_Package_Directory']));
				$out.='<table>';
				if($resDirectoryFiles->RecordCount()==0)
					$out.='<tr>
								<td colspan="2">No files selected.</td>
							</tr>';
				while ($rowDirectoryFiles = $resDirectoryFiles->FetchRow()){
					$out.='
					<tr>
						<td>'.$rowDirectoryFiles['File'].'</td>
						<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editDirectoryFile&from=addPackage&deviceID='.$deviceID.'&Package_Directory='.$row['PK_Package_Directory'].'&oldFile='.$rowDirectoryFiles['File'].'\',\'status=0,resizable=1,width=400,height=350,toolbar=1\');">Edit</a> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteDirectoryFile&from=addPackage&deviceID='.$deviceID.'&Package_Directory='.$row['PK_Package_Directory'].'&oldFile='.$rowDirectoryFiles['File'].'\',\'status=0,resizable=1,width=100,height=100,toolbar=1\');">Delete</a></td>		
					</tr>';
				}

				$out.='
					<tr>
						<td colspan="2"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDirectoryFile&from=addPackage&deviceID='.$deviceID.'&Package_Directory='.$row['PK_Package_Directory'].'\',\'status=0,resizable=1,width=500,height=350,toolbar=1\');">Add file</a></td>
					</tr>
				</table>';
				$out.='</fieldset>';
			}
						
					$out.='
						<select name="FK_Directory">
						<option value="0">-please select-</option>
						';
						$filterDirectory=((count($displayedDirectory)>0)?' WHERE PK_Directory NOT IN ('.join(",",$displayedDirectory).')':'');
						$queryDirectory = "
							SELECT  Directory.* FROM Directory $filterDirectory ORDER BY Directory.Description ASC";
						$rs = $dbADO->_Execute($queryDirectory);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_Directory'].'">'.$row['Description']."</option>";
						}
						$rs->Close();
					$out.='
						</select> <input type="submit" class="button" name="submitX" value="Select"> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDirectory&from=addPackage&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">Add Directory</a>';
					$out.='</fieldset></td>
				</tr>					
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>

		';	
	}else {
		$FK_Distro = cleanInteger(@$_REQUEST['FK_Distro']);
		$FK_Directory = cleanInteger(@$_REQUEST['FK_Directory']);
		$PK_Package = cleanInteger(@$_REQUEST['PK_Package']);
		$description = cleanString(@$_REQUEST['Description'],30);
		
		// update package description
		$updatePackage='UPDATE Package SET Description=? WHERE PK_Package=?';
		$dbADO->Execute($updatePackage,array($description,$PK_Package));
		
		//distro added to package
		if ($FK_Distro!=0) {
			$query = "INSERT INTO Package_Distro (FK_Package , FK_Distro, Name, Repository, Version)
					VALUES (?,?,?,?,?)";
			$dbADO->Execute($query,array($PK_Package,$FK_Distro,'','',''));
		}
		// update Package_Distro
		$displayedDistroArray=explode(",",@$_REQUEST['displayedDistro'],100);
		foreach ($displayedDistroArray as $value){
			$DistroDescription = cleanString(@$_REQUEST['DistroDescription_'.$value],50);
			$PackageDistroName = cleanString(@$_REQUEST['PackageDistroName_'.$value],50);
			$PackageDistroRepository = cleanString(@$_REQUEST['PackageDistroRepository_'.$value],50);
			$PackageDistroVersion = cleanString(@$_REQUEST['PackageDistroVersion_'.$value],50);		
			$PackageDistroFK_RepositorySource = cleanInteger(@$_REQUEST['FK_RepositorySource_'.$value]);		
			
			$updateDistro='UPDATE Distro SET Description=? WHERE PK_Distro=?';
			$dbADO->Execute($updateDistro,array($DistroDescription,$value));
			
			$updatePackage_Distro='UPDATE Package_Distro SET Name=?, Repository=?, Version=?, FK_RepositorySource=? WHERE FK_Package=? AND FK_Distro=?';
			$dbADO->Execute($updatePackage_Distro,array($PackageDistroName,$PackageDistroRepository,$PackageDistroVersion,$PackageDistroFK_RepositorySource,$PK_Package,$value));
		}
		
		//directory added to package
		if ($FK_Directory!=0) {
			$query = "INSERT INTO Package_Directory (FK_Package , FK_Directory)
					VALUES ($PK_Package,$FK_Directory)";
			$dbADO->Execute($query);
		}
		$out.="
		<script>
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				document.location.href='index.php?section=addPackage&from=addPackageToMasterDevice&deviceID=$deviceID&subAction=edit&PK_Package=$PK_Package&Description=$description';
		</script>";
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

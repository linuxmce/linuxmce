<?php
function editRepositorySource($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$PK_RepositorySource = cleanInteger(@$_REQUEST['RepositorySource']);
	
	$deviceID = (int)@$_REQUEST['deviceID'];

	$queryRepositorySource = "SELECT * FROM RepositorySource
										INNER JOIN Distro ON FK_Distro=PK_Distro 
								   WHERE RepositorySource.PK_RepositorySource='$PK_RepositorySource'";
	$rs = $dbADO->_Execute($queryRepositorySource);
	$rowRepositorySource = $rs->FetchRow();

	$countryDescriptionArray=array();
	$countryIDArray=array();
	$queryCountry = "SELECT * FROM Country ORDER BY Description ASC";
	$rs = $dbADO->_Execute($queryCountry);
	while ($row = $rs->FetchRow()) {
		$countryDescriptionArray[]=$row['Description'];
		$countryIDArray[]=$row['PK_Country'];
	}
	$rs->Close();
	
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editRepositorySource">
		<input type="hidden" name="section" value="editRepositorySource">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="RepositorySource" value="'.$PK_RepositorySource.'">
			<table>			
				<tr>
					<td>Distro:</td>
					<td>
						';
						$queryDistroOperatingSystem = "
							SELECT  Distro.*, OperatingSystem.Description AS OS 
								FROM Distro,OperatingSystem 
							WHERE Distro.FK_OperatingSystem= OperatingSystem.PK_OperatingSystem AND PK_Distro='".$rowRepositorySource['FK_Distro']."'
								ORDER BY Distro.Description ASC";
						$rs = $dbADO->_Execute($queryDistroOperatingSystem);
						$row = $rs->FetchRow();
						$out.=$row['Description']." / {$row['OS']} ";
					$out.='
					</td>
				</tr>
				<tr>
					<td>Description:</td>
					<td><input type="text" size="15" name="Description" value="'.$rowRepositorySource['Description'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="left">Existing URLs:</td>
				</tr>';
				$queryUrl='SELECT * FROM RepositorySource_URL WHERE FK_RepositorySource=?';
				$resUrl=$dbADO->Execute($queryUrl,array($PK_RepositorySource));
				$displayingUrls=array();
				while($rowUrl = $resUrl->FetchRow()){
					$displayingUrls[]=$rowUrl['PK_RepositorySource_URL'];
					$out.='
				<tr>
					<td><input type="text" size="15" name="URL_'.$rowUrl['PK_RepositorySource_URL'].'" value="'.$rowUrl['URL'].'"></td>
					<td>
						<select name="Country_'.$rowUrl['PK_RepositorySource_URL'].'">
						<option value="0">-please select-</option>';
						for($i=0;$i<count($countryDescriptionArray);$i++)
							$out.='<option value="'.$countryIDArray[$i].'" '.(($rowUrl['FK_Country']==$countryIDArray[$i])?'selected':'').'>'.$countryDescriptionArray[$i].'</option>';
						$out.='</select>
						<input type="submit" class="button" name="delete_'.$rowUrl['PK_RepositorySource_URL'].'" value="Delete">
					</td>
				</tr>';
				}
				$out.='
				<input type="hidden" name="displayingUrls" value="'.join(",",$displayingUrls).'">
				<tr>
					<td colspan="2" align="left">Add URL:</td>
				</tr>
				<tr>
					<td><input type="text" size="15" name="newURL" value=""></td>
					<td>
						<select name="newCountry'.$rowUrl['PK_RepositorySource_URL'].'">
						<option value="0">-please select-</option>';
						for($i=0;$i<count($countryDescriptionArray);$i++)
							$out.='<option value="'.$countryIDArray[$i].'">'.$countryDescriptionArray[$i].'</option>';
						$out.='</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editRepositorySource");
			frmvalidator.addValidation("Description","req","Please enter a package description");
			//frmvalidator.addValidation("URL","req","Please enter an URL for the Repository Source");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$description = cleanString(@$_POST['Description'],30);
		$newURL = cleanString(@$_POST['newURL'],30);
		$editedUrls=cleanString($_REQUEST['displayingUrls']);
		$newCountry=cleanInteger($_REQUEST['newCountry']);
		$urlIdArray=explode(",",$editedUrls);
			
		if ($description!='') {
			$updateRepositorySource='UPDATE RepositorySource SET Description=? WHERE PK_RepositorySource=?';
			$dbADO->Execute($updateRepositorySource,array($description,$PK_RepositorySource));
			foreach($urlIdArray as $value){
				$URL=$_POST['URL_'.$value];
				$FK_Country=$_POST['Country_'.$value];
				$updateRepositorySourceUrl = 'UPDATE RepositorySource_URL SET URL=?,FK_Country=? WHERE PK_RepositorySource_URL=?';
				$dbADO->Execute($updateRepositorySourceUrl,array($URL,$FK_Country,$value));
				
				if(isset($_POST['delete_'.$value])){
					$delUrl='DELETE FROM RepositorySource_URL WHERE PK_RepositorySource_URL=?';
					$dbADO->Execute($delUrl,array($value));
				}
			}
			if($newURL!=''){
				$insertRepositorySourceUrl = 'INSERT INTO RepositorySource_URL(FK_RepositorySource,URL,FK_Country) values(?,?,?)';
				$dbADO->Execute($insertRepositorySourceUrl,array($PK_RepositorySource,$newURL,$newCountry));
			}
			$out.="
				<script>
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.subAction.value='edit';
					opener.document.forms.{$from}.submit();
					self.location='index.php?section=editRepositorySource&from=addPackage&RepositorySource=".$PK_RepositorySource."';
				</script>
				";		
		} else {
			header("Location: index.php?section=addPackage&from=$from&deviceID=$deviceID&subAction=edit");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

<?php
function editRepositorySource($output,$dbADO) {
        // include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editRepositorySource.lang.php');

	//$dbADO->debug=true;
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$PK_RepositorySource = isset($_REQUEST['RepositorySource'])?cleanInteger($_REQUEST['RepositorySource']):0;

	$deviceID = (int)@$_REQUEST['deviceID'];

	// load repository source if one is provided
	if ($PK_RepositorySource!=0) {
		$queryRepositorySource = "SELECT * FROM RepositorySource
					WHERE PK_RepositorySource='$PK_RepositorySource'";
		$rs = $dbADO->Execute($queryRepositorySource);
		$rowRepositorySource = $rs->FetchRow();

		$FK_OperatingSystem=$rowRepositorySource['FK_OperatingSystem'];
		$FK_Distro=$rowRepositorySource['FK_Distro'];
		$FK_RepositoryType=$rowRepositorySource['FK_RepositoryType'];
	}

	if ($action=='form') {
		// load country data
		$countryDescriptionArray=array();
		$countryIDArray=array();
		$queryCountry = "SELECT * FROM Country ORDER BY Description ASC";
		$rs = $dbADO->_Execute($queryCountry);
		while ($row = $rs->FetchRow()) {
			$countryDescriptionArray[]=$row['Description'];
			$countryIDArray[]=$row['PK_Country'];
		}
		$rs->Close();

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<fieldset><legend>'.$TEXT_ADD_EDIT_REPOSITORY_SOURCE_CONST.'</legend>
		<form action="index.php" method="post" name="editRepositorySource">
		<input type="hidden" name="section" value="editRepositorySource">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="RepositorySource" value="'.$PK_RepositorySource.'">
		<table>
			<tr>
				<td>
					'.$TEXT_DESCRIPTION_CONST.':
					<input type="text" size="30" name="Description" value="'.$rowRepositorySource['Description'].'">
				</td>
			</tr>
			<tr>
				<td>
					<b>'.$TEXT_COMPATIBLE_WITH_CONST.'</b>
				</td>
			</tr>
		        <tr>
		                <td>
				<select name="FK_OperatingSystem" onChange="">
		                        <option value="0" >-'.$TEXT_ANY_CONST.'-</option>
		';
		                        $queryOperatingSystem = "SELECT * FROM OperatingSystem ORDER BY Description ASC";
	                        	$rs = $dbADO->Execute($queryOperatingSystem);
		                        while ($row = $rs->FetchRow()) {
		                                $selected=($FK_OperatingSystem==$row['PK_OperatingSystem'])?'selected':'';
		                                $out.='
					        <option value="'.$row['PK_OperatingSystem'].'" '.$selected.' >'.$row['Description'].'</option>
		                                ';
		                        }
	                        $rs->Close();
	        $out.='
        		        </select>
				<select name="FK_Distro" onChange="">
	                        	<option value="0" >-'.$TEXT_ANY_CONST.'-</option>
		';
					$queryDistro = "SELECT Distro.*, OperatingSystem.Description AS OS
							FROM Distro
							LEFT JOIN OperatingSystem ON Distro.FK_OperatingSystem=OperatingSystem.PK_OperatingSystem
							ORDER BY OperatingSystem.Description, Distro.Description ASC";
					$rs = $dbADO->Execute($queryDistro);
	                        while ($row = $rs->FetchRow()) {
	                                $selected=($FK_Distro==$row['PK_Distro'])?'selected':'';
	                                $out.='
	                                <option value="'.$row['PK_Distro'].'" '.$selected.' >'.$row['Description'].' / '.$row['OS'].'</option>";
	                                ';
	                        }
	                        $rs->Close();
	        $out.='
	                        </select>
		                </td>
			        </tr>
		';

		if ($PK_RepositorySource!=0) {
			$out.='
				<tr>
					<td colspan="2" align="left">'.$TEXT_REPOSITORY_TYPE_CONST.':
					<select name="FK_RepositoryType" onChange="">
		                        <option value="0" >-'.$TEXT_ANY_CONST.'-</option>
			';
		                        $queryRepositoryType = "SELECT * FROM RepositoryType ORDER BY PK_RepositoryType";
	                        	$rs = $dbADO->Execute($queryRepositoryType);
		                        while ($row = $rs->FetchRow()) {
		                                $selected=($FK_RepositoryType==$row['PK_RepositoryType'])?'selected':'';
		                                $out.='
					        <option value="'.$row['PK_RepositoryType'].'" '.$selected.' >'.$row['Description'].'</option>
		                                ';
		                        }
		                        $rs->Close();
		        $out.='
	        		        </select>
					</td>
				</tr>
			';

			$out.='
				<tr>
					<td>'.$TEXT_CURRENT_URLS_CONST.':</td>
					<td>'.$TEXT_COUNTRY_CONST.':</td>
				</tr>
			';
			$queryUrl='SELECT * FROM RepositorySource_URL WHERE FK_RepositorySource=?';
			$resUrl=$dbADO->Execute($queryUrl,array($PK_RepositorySource));
			$displayingUrls=array();
			while($rowUrl = $resUrl->FetchRow()) {
				$displayingUrls[]=$rowUrl['PK_RepositorySource_URL'];
				$out.='
				<tr>
					<td><input type="text" size="50" name="URL_'.$rowUrl['PK_RepositorySource_URL'].'" value="'.$rowUrl['URL'].'"></td>
					<td>
						<select name="Country_'.$rowUrl['PK_RepositorySource_URL'].'">
						<option value="0">-'.$TEXT_ANY_CONST.'-</option>
					';
					for($i=0;$i<count($countryDescriptionArray);$i++)
						$out.='<option value="'.$countryIDArray[$i].'" '.(($rowUrl['FK_Country']==$countryIDArray[$i])?'selected':'').'>'.$countryDescriptionArray[$i].'</option>';
					$out.='
						</select>
						<input type="submit" class="button" name="delete_'.$rowUrl['PK_RepositorySource_URL'].'" value="Delete">
					</td>
				</tr>
				';
			}
			$out.='
				<input type="hidden" name="displayingUrls" value="'.join(",",$displayingUrls).'">
				<tr>
					<td>'.$TEXT_ADD_URL_CONST.':</td>
					<td>'.$TEXT_COUNTRY_CONST.':</td>
				</tr>
				<tr>
					<td><input type="text" size="50" name="newURL" value=""></td>
					<td>
						<select name="newCountry'.$rowUrl['PK_RepositorySource_URL'].'">
						<option value="0">-'.$TEXT_ANY_CONST.'-</option>
						';
						for($i=0;$i<count($countryDescriptionArray);$i++)
							$out.='<option value="'.$countryIDArray[$i].'">'.$countryDescriptionArray[$i].'</option>';
						$out.='
						</select>
					</td>
				</tr>
			';
		}
		$out.='
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		</fieldset>
		<script>
		 	var frmvalidator = new formValidator("editRepositorySource");
			frmvalidator.addValidation("Description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
		';
		if ($PK_RepositorySource!=0) {
			$out.='
			frmvalidator.addValidation("FK_RepositoryType","req","'.$TEXT_REPOSITORY_TYPE_REQUIRED_CONST.'");
//			frmvalidator.addValidation("URL","req","'.$TEXT_REPOSITORY_SOURCE_URL_REQUIRED_CONST.'");
			';
		}
		$out.='
		</script>
		';

	} else {

		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$description = isset($_REQUEST['Description'])?cleanString($_REQUEST['Description'],30):'';
		$FK_OperatingSystem=cleanInteger($_REQUEST['FK_OperatingSystem']);
		$FK_Distro=cleanInteger($_REQUEST['FK_Distro']);

		if ($FK_OperatingSystem==0) {
			$FK_OperatingSystem=NULL;
		}

		if ($FK_Distro==0) {
			$FK_Distro=NULL;
		}

		if ($description!='') {
			if ($PK_RepositorySource==0) {
				// insert
				$insertRepositorySource = 'INSERT INTO RepositorySource(Description,FK_OperatingSystem,FK_Distro) values(?,?,?)';
				$dbADO->Execute($insertRepositorySource,array($description,$FK_OperatingSystem,$FK_Distro));
				$PK_RepositorySource=$dbADO->Insert_ID();
			} else {
				//update
				$newURL = cleanString(@$_POST['newURL'],30);
				$editedUrls=cleanString($_REQUEST['displayingUrls']);
				$newCountry=cleanInteger($_REQUEST['newCountry']);
				$urlIdArray=explode(",",$editedUrls);

				$updateRepositorySource='UPDATE RepositorySource 
					SET Description=?, FK_OperatingSystem=?, FK_Distro=?, FK_RepositoryType=?
						WHERE PK_RepositorySource=?';
				$dbADO->Execute($updateRepositorySource,
					array($description,$FK_OperatingSystem,$FK_Distro,$FK_RepositoryType,
						$PK_RepositorySource));
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
			}
			$out.="
			<script>
 				opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.OperatingSystem='{$FK_OperatingSystem}';
				opener.document.forms.{$from}.Distro='{$FK_Distro}'
				opener.document.forms.{$from}.submit();
 				self.location='index.php?section=editRepositorySource&from={$from}&RepositorySource={$PK_RepositorySource}';
			</script>
			";
		} else {
			header("Location: index.php?section=editRepositorySource&from={$from}&deviceID={$deviceID}&RepositorySource={$PK_RepositorySource}");
		}
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>

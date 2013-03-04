<?php
function addDistro($output,$dbADO) {
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addDistro.lang.php');

	//$dbADO->debug=true;
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$subAction = isset($_REQUEST['subAction'])?cleanString($_REQUEST['subAction']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$deviceID = isset($_REQUEST['deviceID'])?cleanInteger($_REQUEST['deviceID']):0;

	if ($action=='form' && $subAction!='edit') {
		$out.='
		<form action="index.php" method="post" name="addDistro">
		<input type="hidden" name="section" value="addDistro">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="subAction" value="edit">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_ADD_DISTRIBUTION_CONST.'</h3>
			<table>
				<tr>
					<td>'.$TEXT_DISTRIBUTION_CONST.' *</td>
					<td><input type="text" size="30" name="Description" value="'.$rowDistro['Description'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="addDescription" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addDistro");
 			frmvalidator.addValidation("Description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
		</script>
		';

	} elseif ($subAction=='edit') {
       	        $PK_Distro = cleanInteger($_REQUEST['PK_Distro']);

		if(isset($_REQUEST['addDescription'])){
	                $Description = cleanString($_REQUEST['Description'],30);
			$queryInsertDistro = 'INSERT INTO Distro(Description) values(?)';
			$dbADO->Execute($queryInsertDistro,array($Description));
			$PK_Distro = $dbADO->Insert_ID();
		}

		$queryDistro='SELECT * FROM Distro WHERE PK_Distro=?';
		$resDistro=$dbADO->Execute($queryDistro,array($PK_Distro));
		$rowDistro = $resDistro->FetchRow();
		$out.='
		<form action="index.php" method="post" name="addDistro">
		<input type="hidden" name="section" value="addDistro">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="subAction" value="update">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="PK_Distro" value="'.$PK_Distro.'">

			<h3>'.$TEXT_EDIT_DISTRIBUTION_CONST.'</h3>
			<table>
				<tr>
					<td>'.$TEXT_DISTRIBUTION_CONST.' *</td>
					<td><input type="text" size="30" name="Description" value="'.$rowDistro['Description'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_DEFINE_CONST.'</td>
					<td><input type="text" size="25" name="Define" value="'.$rowDistro['Define'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_OPERATING_SYSTEM_CONST.':</td>
					<td><select name="FK_OperatingSystem" >
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
		';
		$queryOperatingSystem = "SELECT * FROM OperatingSystem ORDER BY Description ASC";
		$rs = $dbADO->Execute($queryOperatingSystem);
		while ($row = $rs->FetchRow()) {
	$selected = ($rowDistro['FK_OperatingSystem']==$row['PK_OperatingSystem'])?'selected':'';
	$out.='<option value="'.$row['PK_OperatingSystem'].'" '.$selected.'>'.$row['Description']."</option>";
		}
		$rs->Close();
		$out.='
					</select></td>
				</tr>
				<tr>
					<td>'.$TEXT_INSTALLER_CONST.'</td>
					<td><input type="text" size="30" name="Installer" value="'.$rowDistro['Installer'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_KICKSTARTCD_CONST.'</td>
					<td><input type="text" size="30" name="KickStartCD" value="'.$rowDistro['KickStartCD'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_BINARIES_CONST.'</td>
					<td><input type="text" size="30" name="Binaries" value="'.$rowDistro['Binaries'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_SOURCECODE_CONST.'</td>
					<td><input type="text" size="30" name="SourceCode" value="'.$rowDistro['SourceCode'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_INSTALLER_URL_CONST.'</td>
					<td><input type="text" size="30" name="InstallerURL" value="'.$rowDistro['InstallerURL'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="left"><input type="checkbox" name="Confirmed" value="1" '.(($rowDistro['Confirmed']==1)?'checked':'').'>'.$TEXT_CONFIRMED_CONST.'</td><br>
				</tr>
				<tr>
					<td colspan="2" align="left"><input type="checkbox" name="Core" value="1" '.(($rowDistro['Core']==1)?'checked':'').'>'.$TEXT_CORE_CONST.'</td><br>
				</tr>
				<tr>
					<td colspan="2" align="left"><input type="checkbox" name="MediaDirector" value="1" '.(($rowDistro['MediaDirector']==1)?'checked':'').'>'.$TEXT_MEDIA_DIRECTOR_CONST.'</td><br>
				</tr>
				<tr>
					<td colspan="2" align="left"><input type="checkbox" name="Orbiter" value="1" '.(($rowDistro['Orbiter']==1)?'checked':'').'>'.$TEXT_ORBITER_CONST.'</td><br>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addDistro");
 			frmvalidator.addValidation("Description","req","'.$TEXT_DISTRIBUTION_REQUIRED_CONST.'");
			frmvalidator.addValidation("FK_OperatingSystem","req","'.$TEXT_OPERATING_SYSTEM_REQUIRED_CONST.'");
		</script>
		';
	} else {
		$Description = cleanString($_REQUEST['Description'],30);
		$Define = cleanString($_REQUEST['Define'],25);
		$FK_OperatingSystem = cleanInteger($_REQUEST['FK_OperatingSystem']);
		$Installer = cleanString($_REQUEST['Installer'],100);
		$KickStartCD = cleanString($_REQUEST['KickStartCD'],100);
		$Binaries = cleanString($_REQUEST['Binaries'],100);
		$SourceCode = cleanString($_REQUEST['SourceCode'],100);
		$Confirmed = cleanInteger($_REQUEST['Confirmed']);
		$Core = cleanInteger($_REQUEST['Core']);
		$MediaDirector = cleanInteger($_REQUEST['MediaDirector']);
		$Orbiter = cleanInteger($_REQUEST['MediaDirector']);
		$InstallerURL = cleanString($_REQUEST['InstallerURL'],100);
		$PK_Distro = cleanInteger($_REQUEST['PK_Distro']);

		if ($Description!='') {
			$queryUpdateDistro = 'UPDATE Distro SET Description=?, Define=?,
				FK_OperatingSystem=?, Installer=?, KickStartCD=?,
				Binaries=?, SourceCode=?, Confirmed=?, Core=?,
				MediaDirector=?, Orbiter=?, InstallerURL=? WHERE PK_Distro=?';
			$dbADO->Execute($queryUpdateDistro,
					array($Description, $Define, $FK_OperatingSystem, $Installer,
						$KickStartCD, $Binaries, $SourceCode, $Confirmed,
						$Core, $MediaDirector, $Orbiter, $InstallerURL,
						$PK_Distro
					));

			$out.="
				<script>
					opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.location='index.php?section=addDistro&subAction=edit&from={$from}&PK_Distro={$PK_Distro}';
				</script>
				";
		} else {
			header("Location: index.php?section=addDistro&from={$from}&deviceID={$deviceID}&subAction=edit&PK_Distro={$PK_Distro}");
		}

	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DISTRIBUTION_CONST);
	$output->output();
}
?>

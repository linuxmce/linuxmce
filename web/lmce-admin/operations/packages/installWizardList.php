<?php
function installWizardList($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/installWizardList.lang.php');


	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	// get array for operating system pull down
	$queryOperatingSystem='SELECT * FROM OperatingSystem ORDER BY Description ASC';
	$rs = $dbADO->Execute($queryOperatingSystem);
	$arrayIdOperatingSystem=array();
	$arrayDescriptionOperatingSystem=array();
	while ($row = $rs->FetchRow()) {
		$arrayIdOperatingSystem[]=$row['PK_OperatingSystem'];
		$arrayDescriptionOperatingSystem[]=$row['Description'];
	}
	$rs->Close();

	// get array for distro pull down
	$arrayIdDistro=array();
	$arrayDescriptionDistro=array();
	$queryDistroOperatingSystem = "
					SELECT  Distro.*, OperatingSystem.Description AS OS FROM Distro,OperatingSystem 
					WHERE Distro.FK_OperatingSystem= OperatingSystem.PK_OperatingSystem ORDER BY Distro.Description ASC";
	$rs = $dbADO->_Execute($queryDistroOperatingSystem);
	while ($row = $rs->FetchRow()) {
		$arrayIdDistro[]=$row['PK_Distro'];
		$arrayDescriptionDistro[]=$row['Description']." / ".$row['OS'];
	}
	$rs->Close();

	$arrayIdStep=array();
	$arrayDescriptionStep=array();
	$arrayIdStep[]=0;
	$arrayDescriptionStep[]=$TEXT_DISABLED_CONST.' / '.$TEXT_NONE_CONST;
	$arrayIdStep[]=5;
	$arrayDescriptionStep[]=$TEXT_CORE_CONST;
	$arrayIdStep[]=6;
	$arrayDescriptionStep[]=$TEXT_MEDIA_DIRECTOR_CONST;

	if ($action=='form') {
		$DataFormValidation='';
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="installWizardList">
		<input type="hidden" name="section" value="installWizardList">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">

		'.$TEXT_INSTAL_WIZARD_ENTRIES_CONST.'
		<table>	

			<tr>
				<td colspan="2">
				<fieldset><legend>blah</legend>
				<table>
		';
			$compatArray=array();
			$queryWizardList='SELECT * FROM InstallWizard ORDER BY Step ASC';
			$resWizardList=$dbADO->execute($queryWizardList);
			$displayedWizard=array();
			while($rowWizard=$resWizardList->FetchRow()){
				$displayedWizard[]=$rowWizard['PK_InstallWizard'];
				$DataFormValidation.= 'frmvalidator.addValidation("wizard_'.$rowWizard['PK_InstallWizard'].'","req","Please enter a source name!");';

				$out.='
					<tr bgcolor="#F0F3F8">
						<td>'.$TEXT_NAME_CONST.'</td>
						<td>'.$TEXT_STEP_CONST.'</td>
						<td colspan="2">'.$TEXT_DEVICE_TEMPLATE_CONST.'</td>
					</tr>
					<tr bgcolor="#F0F3F8">
						<td><input type="text" name="comments_'.$rowWizard['PK_InstallWizard'].'" value="'.$rowWizard['Comments'].'"></td>

<!--						<td><input type="text" name="step_'.$rowWizard['PK_InstallWizard'].'" value="'.$rowWizard['Step'].'"></td> -->
						<td><select name="step_'.$rowWizard['PK_InstallWizard'].'">
					';
					foreach($arrayDescriptionStep as $key => $value){
						$out.='
							<option value="'.$arrayIdStep[$key].'" '.(($arrayIdStep[$key]==$rowWizard['Step'])?'selected':'').'>'.$value.'</option>
						';
					}
					$out.='
						</select></td>

						<td colspan="2"><input type="text" name="deviceTemplate_'.$rowWizard['PK_InstallWizard'].'" value="'.$rowWizard['FK_DeviceTemplate'].'"></td>

<!--						<td><input type="text" name="default_'.$rowWizard['PK_InstallWizard'].'" value="'.$rowWizard['Default'].'"></td> -->
						<td><input name="default_'.$rowWizard['PK_InstallWizard'].'" type="checkbox" value="1" '.(($rowWizard['Default']==1)?'checked':'').'>  '.$TEXT_DEFAULT_CONST.'</td>

						<td><input type="submit" class="button" name="deleteWizard_'.$rowWizard['PK_InstallWizard'].'" value="'.$TEXT_DELETE_INSTALL_WIZARD_CONST.'"></td>
					</tr>
					<input type="hidden" name="displayedWizard" value="'.join(",",$displayedWizard).'">

					<tr bgcolor="#F0F3F8">
						<td>&nbsp;</td>
						<td>'.$TEXT_OPERATING_SYSTEM_CONST.'</td>
						<td>'.$TEXT_DISTRO_CONST.'</td>
						<td>'.$TEXT_DEVICE_TEMPLATE_CONST.'</td>
						<td>'.$TEXT_COMMENTS_CONST.'</td>
					</tr>
				';
				$queryWizardCompatibility='SELECT * FROM InstallWizard_Distro WHERE FK_InstallWizard=?';
				$resWizardCompatibility=$dbADO->Execute($queryWizardCompatibility,$rowWizard['PK_InstallWizard']);
				while($rowCompatibility=$resWizardCompatibility->fetchRow()){
					$compatArray[]=$rowCompatibility['PK_InstallWizard_Distro'];
					$out.='
					<tr>
						<td>&nbsp;</td>
						<td><select name="OperatingSystem_'.$rowCompatibility['PK_InstallWizard_Distro'].'">
							<option value="0">-'.$TEXT_ANY_CONST.'-</option>
					';
					foreach($arrayDescriptionOperatingSystem as $key => $value){
						$out.='
							<option value="'.$arrayIdOperatingSystem[$key].'" '.(($arrayIdOperatingSystem[$key]==$rowCompatibility['FK_OperatingSystem'])?'selected':'').'>'.$value.'</option>
						';
					}
					$out.='
						</select></td>
						<td><select name="distro_'.$rowCompatibility['PK_InstallWizard_Distro'].'">
							<option value="0">-'.$TEXT_ANY_CONST.'-</option>
					';
					foreach($arrayDescriptionDistro as $key => $value){
						$out.='
							<option value="'.$arrayIdDistro[$key].'" '.(($arrayIdDistro[$key]==$rowCompatibility['FK_Distro'])?'selected':'').'>'.$value.'</option>
						';
					}

					$out.='
						</select></td>
						<td><input type="text" name="compatDeviceTemplate_'.$rowCompatibility['PK_InstallWizard_Distro'].'" value="'.$rowCompatibility['FK_DeviceTemplate'].'"></td>
						<td><textarea cols="180" name="compatComments_'.$rowCompatibility['PK_InstallWizard_Distro'].'">'.$rowCompatibility['Comments'].'</textarea></td>
						<td><input type="submit" class="button" name="deleteCompatibility_'.$rowCompatibility['PK_InstallWizard_Distro'].'" value="'.$TEXT_DELETE_COMPATIBILITY_CONST.'"></td>
					</tr>
					<input type="hidden" name="compatArray" value="'.join(",",$compatArray).'">

				';

				}
				$out.='
					<tr>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td colspan="6" align="centre"><input type="submit" class="button" name="addCompatibility_'.$rowWizard['PK_InstallWizard'].'" value="'.$TEXT_ADD_OTHER_COMPATIBILITY_CONST.'"></td>
					</tr>
					<tr>
						<td colspan="6" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"></td>
					</tr>
					<tr>
						<td colspan=6><hr></td>
					</tr>
				';

			}
			$out.='
					<tr>
						<td colspan="7">
							<input type="submit" class="button" name="addWizard" value="'.$TEXT_ADD_INSTALL_WIZARD_CONST.'">
						</td>
					</tr>
				</table>
				</fieldset>
				</td>
			</tr>
		</table>
		</form>
			';
	} else {
		$displayedWizardArray=explode(",",@$_POST['displayedWizard']);
		$compatArray=explode(",",@$_POST['compatArray']);

		if(isset($_POST['addWizard'])){
			// add new wizard in table InstallWizard
			$insertInstallWizard='INSERT INTO InstallWizard (InstallWizard.Default) VALUES (0)';
			$dbADO->Execute($insertInstallWizard);
		}

		foreach($compatArray AS $value){
			$compatOperatingSystem=(cleanInteger(@$_POST['OperatingSystem_'.$value]))?@$_POST['OperatingSystem_'.$value]:NULL;
			$compatDistro=(cleanInteger(@$_POST['distro_'.$value]))?@$_POST['distro_'.$value]:NULL;
			$compatDeviceTemplate=(cleanInteger(@$_POST['compatDeviceTemplate_'.$value]))?@$_POST['compatDeviceTemplate_'.$value]:NULL;
			$compatComments=cleanString(@$_POST['compatComments_'.$value]);
			if(isset($_POST['deleteCompatibility_'.$value])) {
				$deleteInstallWizardDistro='DELETE FROM InstallWizard_Distro WHERE PK_InstallWizard_Distro=?';
				$dbADO->Execute($deleteInstallWizardDistro,$value);
			} else {
				$updateInstallWizardDistro='
					UPDATE InstallWizard_Distro SET
						FK_OperatingSystem=?,
						FK_Distro=?,
						FK_DeviceTemplate=?,
						Comments=?
					WHERE PK_InstallWizard_Distro=?';
				$dbADO->Execute($updateInstallWizardDistro,array($compatOperatingSystem,$compatDistro,$compatDeviceTemplate,$compatComments,$value));
			}
		}

		foreach($displayedWizardArray AS $value) {
			$wizardDeviceTemplate=(cleanInteger(@$_POST['deviceTemplate_'.$value])!='0')?@$_POST['deviceTemplate_'.$value]:0;
			$wizardStep=(cleanInteger(@$_POST['step_'.$value])!='0')?@$_POST['step_'.$value]:0;
			$wizardDefault=(cleanInteger(@$_POST['default_'.$value])!='0')?1:0;
			$wizardComments=(@$_POST['comments_'.$value]!='')?@$_POST['comments_'.$value]:NULL;

			$updateInstallWizard='
				UPDATE InstallWizard SET
					FK_DeviceTemplate=?,
					Step=?,
					InstallWizard.Default=?,
					Comments=?
				WHERE PK_InstallWizard=?';
			$dbADO->Execute($updateInstallWizard,array($wizardDeviceTemplate,$wizardStep,$wizardDefault,$wizardComments,$value));

			if(isset($_POST['deleteWizard_'.$value])) {
				$deleteInstallWizardDistro='DELETE FROM InstallWizard_Distro WHERE FK_InstallWizard=?';
				$dbADO->Execute($deleteInstallWizardDistro,$value);
				$deleteInstallWizard='DELETE FROM InstallWizard WHERE PK_InstallWizard=?';
				$dbADO->Execute($deleteInstallWizard,$value);
			}

			// add compatibility
			if(isset($_POST['addCompatibility_'.$value])) {
				$insertInstallWizardDistro='INSERT INTO InstallWizard_Distro (FK_InstallWizard) VALUES (?)';
				$dbADO->Execute($insertInstallWizardDistro,$value);
			}

		}
		$out.='<script>';

		$out.="self.location='index.php?section=installWizardList&from=".$from."';";
		$out.='</script>';
	}

//$output->setMenuTitle($TEXT_DEVELOPERS_CONST.' |');
//$output->setPageTitle($TEXT_INSTALL_WIZARD_CONST);
//$output->setNavigationMenu(array($TEXT_DISTRO_CONST=>'index.php?section=installWizardList'));
//$output->setBody($out);
//$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DISTRO_CONST);
//$output->output();

	$output->setBody($out);
	$output->setTitle(@$rowPackage['Description'].' - '.APPLICATION_NAME.' :: '.$TEXT_CREATE_NEW_INSTALL_WIZARD_CONST);
	$output->output();
}
?>

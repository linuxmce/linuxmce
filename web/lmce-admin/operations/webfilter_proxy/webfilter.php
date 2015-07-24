<?PHP
function webfilterSettings($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');

	
$out='';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
if ($action == 'form') {
	//grep webfilter devicedata
	$res=$dbADO->Execute("SELECT * FROM Device as Device INNER JOIN DeviceTemplate_DeviceData as Template on Device.FK_DeviceTemplate = Template.FK_DeviceTemplate WHERE Device.FK_DeviceTemplate='2290'");
	if($res->RecordCount()!=0){
		$row=$res->FetchRow();
		$DisableWebfilter=$row['Disabled'];
		$WebfilterPort=$row['IK_DeviceData'];
		$notinstalled='';
	}
}else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=webfilter&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
}}
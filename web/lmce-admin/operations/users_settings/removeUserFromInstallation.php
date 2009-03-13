<?php
function removeUserFromInstallation($output,$dbADO) {
	// obsolete file
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['userID'];

	$installationID = cleanInteger($_SESSION['installationID']);
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {			
		
		//check if is the only one user assigned to that installation who could modify installation
		$checkIsTheOnlyUser = "SELECT FK_Users FROM Installation_Users where FK_Installation = ? AND userCanModifyInstallation=1 AND FK_Users!=?";
		$resIsTheOnlyUser = $dbADO->Execute($checkIsTheOnlyUser,array($installationID,$objID));
		
		if ($resIsTheOnlyUser && $resIsTheOnlyUser->RecordCount()>0) {	
			if ((int)$objID!=0 && $installationID!=0) {
				$deleteObjFromDevice = 'delete from Installation_Users where FK_Installation = ? and FK_Users = ?';
				$query = $dbADO->Execute($deleteObjFromDevice,array($installationID,$objID));
				
				$out.="
				<script>
					alert('User removed from installation!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {
				$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
			}		
			
		} else {
			$out = 'This is the only user who could modify installation!You can not delete it! <a href="javascript:window.close();">Close</a>';
		}
	
	} else {
			$out = 'You are not allowed to do that!<a href="javascript:window.close();">Close</a>';
	}
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
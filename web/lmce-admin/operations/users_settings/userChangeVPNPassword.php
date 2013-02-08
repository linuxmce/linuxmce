<?php
//Rob Woodward - Modified to allow Admin users to set / change passwords without knowing original password
function userChangeVPNPassword($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('userChangePassword.lang.php');

	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;

	$installationID = cleanInteger($_SESSION['installationID']);
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);

	if ($action=='form') {

		$out.='
		<script>
			funerChangeVPNPassword.phption windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="userChangeVPNPassword">
		<input type="hidden" name="section" value="userChangeVPNPassword">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">

		<h3>'.translate('TEXT_CHANGE_VPN_PASSWORD_CONST').'</h3>
			<table>	
					<tr valign="top">
						<td>'.translate('TEXT_YOUR_PASSWORD_CONST').'</td>
						<td>
							';
							//If the user is able to modify the configuration, there's no need for the old password
							if ($canModifyInstallation) {
								$out.=translate('TEXT_PASSWORD_CHANGE_ADMIN_USER');
							} else {
								$out.='<input type="password" name="oldVPNPassword" value="">';
							}
	$out.='					</td>
					</tr>
					<tr valign="top">
						<td>'.translate('TEXT_PASSWORD_CONST').'</td>
						<td>
							<input type="password" name="VPNPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.translate('TEXT_USER_CONFIRM_PASSWORD_CONST').'</td>
						<td>
							<input type="password" name="VPNPassword2" value="">
						</td>
					</tr>					
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  > <input type="button" class="button" name="update" value="'.translate('TEXT_CLOSE_CONST').'" onClick="self.close();"></td>
					</tr>
			</table>
			</form>
			';

			$out.='
				<script>
		 			var frmvalidator = new formValidator("userChangeVPNPassword"); 		
					frmvalidator.addValidation("VPNPassword","req","'.translate('TEXT_PASSWORD_REQUIRED_CONST').'");
					frmvalidator.addValidation("VPNPassword2","req","'.translate('TEXT_CONFIRMED_PASSWORD_REQUIRED_CONST').'");';
			$out.='
				</script>
			';

	} else {
		if ($canModifyInstallation) {		
			$VPNPassword = cleanString($_POST['VPNPassword']);
			$VPNPassword2 = cleanString($_POST['VPNPassword2']);

			$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
			$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
			$rowUser = $queryUserInst->FetchRow(); 
			$username = $rowUser['UserName'];

			//If the user is permitted to modify the installation, they don't need to know the old password in case it's a forgotten password
			$oldVPNPassword = exec("awk '$1==\"$username\" {print $3}' /etc/ppp/chap-secrets");

			//make the change
			$out .= makeChange($queryUserInst, $from, $userID, $username, $oldVPNPassword, $VPNPassword, $VPNPassword2);
		} else {
			//Normal user, can modify their own password ONLY!
			if ( $_SESSION['userID']==$username ) {
				$oldVPNPassword = cleanString($_POST['oldVPNPassword']);
				$VPNPassword = cleanString($_POST['VPNPassword']);
				$VPNPassword2 = cleanString($_POST['VPNPassword2']);

				$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
				$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
				$rowUser = $queryUserInst->FetchRow(); 
				$username = $rowUser['UserName'];

				//make the change
				$out .= makeChange($queryUserInst, $from, $userID, $username, $oldVPNPassword, $VPNPassword, $VPNPassword2);
			}else{
				header("Location: index.php?section=userChangeVPNPassword&userID=$userID&from=$from&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
				exit();
			}
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_CHANGE_VPN_PASSWORD_CONST'));			
	$output->output();
}

function makeChange($queryUserInst, $from, $userID, $username, $oldVPNPassword, $VPNPassword, $VPNPassword2) {

	if(exec("awk '$1==\"$username\" {print $3}' /etc/ppp/chap-secrets") != $oldVPNPassword){	
		header('Location: index.php?section=userChangeVPNPassword&from=users&userID='.$userID.'&error='.translate('TEXT_ERROR_OLD_PASSWORD_DO_NOT_MATCH_CONST'));
		exit();
	}

	if ($VPNPassword!=$VPNPassword2) {
		header("Location: index.php?section=userChangeVPNPassword&error=".translate('TEXT_ERROR_PASSWORD_DO_NOT_MATCH_CONST')."&userID=$userID&from=$from");
		exit();
	}	

	//check if is allowed to change password!
	if ($VPNPassword!='' && ($queryUserInst && $queryUserInst->RecordCount()==1)) {
		exec("sudo -u root /usr/pluto/bin/Network_VPN.sh password $username $oldVPNPassword $VPNPassword");
		$out.="
		<script>
			alert('".translate('TEXT_PASSWORD_CHANGED_CONST')."');
			    opener.document.forms.{$from}.action.value='form';
			opener.document.forms.{$from}.submit();
			self.close();
		</script>
		";			
	} else {				
		header("Location: index.php?section=userChangeVPNPassword&userID=$userID&from=$from");
		exit();
	}
	return $out;	
}
?>

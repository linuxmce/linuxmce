<?php
function userChangeVPNPassword($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userChangePassword.lang.php');
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="userChangeVPNPassword">
		<input type="hidden" name="section" value="userChangeVPNPassword">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">
		
		<h3>'.$TEXT_CHANGE_VPN_PASSWORD_CONST.'</h3>
			<table>	
					<tr valign="top">
						<td>'.$TEXT_YOUR_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="oldVPNPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.$TEXT_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="VPNPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.$TEXT_USER_CONFIRM_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="VPNPassword2" value="">
						</td>
					</tr>					
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
					</tr>
			</table>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("userChangeVPNPassword"); 		
					frmvalidator.addValidation("VPNPassword","req","'.$TEXT_PASSWORD_REQUIRED_CONST.'");
					frmvalidator.addValidation("VPNPassword2","req","'.$TEXT_CONFIRMED_PASSWORD_REQUIRED_CONST.'");';
			$out.='
				</script>
			';
		
	} else {
		
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if ($canModifyInstallation) {		
		
			$oldVPNPassword = cleanString($_POST['oldVPNPassword']);
			$VPNPassword = cleanString($_POST['VPNPassword']);
			$VPNPassword2 = cleanString($_POST['VPNPassword2']);
			
			$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
			$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
			$rowUser = $queryUserInst->FetchRow(); 
			$username = $rowUser['UserName'];
			
			if(exec("awk '$1==\"$username\" {print $3}' /etc/ppp/chap-secrets") != $oldVPNPassword){	
				header('Location: index.php?section=userChangeVPNPassword&from=users&userID='.$userID.'&error='.$TEXT_ERROR_OLD_PASSWORD_DO_NOT_MATCH_CONST);
				exit();
			}
			

			if ($VPNPassword!=$VPNPassword2) {
				header("Location: index.php?section=userChangeVPNPassword&error=$TEXT_ERROR_PASSWORD_DO_NOT_MATCH_CONST&userID=$userID&from=$from");
				exit();
			}	

			//check if is allowed to change password!

			
			if ($VPNPassword!='' && ($queryUserInst && $queryUserInst->RecordCount()==1)) {

								
				// reload daemon
				//$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
				//exec($commandToSend);

				
				$out.="
				<script>
					alert('$TEXT_PASSWORD_CHANGED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {				
				header("Location: index.php?section=userChangeVPNPassword&userID=$userID&from=$from");
				exit();
			}		
		} else {
			header("Location: index.php?section=userChangeVPNPassword&userID=$userID&from=$from&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CHANGE_VPN_PASSWORD_CONST);			
	$output->output();
}
?>

<?php
function userChangePassword($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userChangePassword.lang.php');
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	$selectTheSamePassword ='SELECT PK_Users FROM Users WHERE PK_Users =?';
	$resTheSamePassword = $dbADO->Execute($selectTheSamePassword,array($userID));
	$rowTheSamePassword = $resTheSamePassword->FetchRow();
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="userChangePassword">
		<input type="hidden" name="section" value="userChangePassword">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">
		
		<h3>'.$TEXT_CHANGE_PASSWORD_CONST.'</h3>
			<table>	
					<tr valign="top">
						<td>'.$TEXT_YOUR_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="oldPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.$TEXT_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="userPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.$TEXT_USER_CONFIRM_PASSWORD_CONST.'</td>
						<td>
							<input type="password" name="userPassword2" value="">
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
		 			var frmvalidator = new formValidator("userChangePassword"); 		
					frmvalidator.addValidation("userPassword","req","'.$TEXT_PASSWORD_REQUIRED_CONST.'");
					frmvalidator.addValidation("userPassword2","req","'.$TEXT_CONFIRMED_PASSWORD_REQUIRED_CONST.'");';
			$out.='
				</script>
			';
		
	} else {
		
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if ($canModifyInstallation) {		
			if($_REQUEST['oldPassword']!=$_SESSION['password']){
				header('Location: index.php?section=userChangePassword&from=users&userID='.$userID.'&error='.$TEXT_ERROR_PASSWORD_DO_NOT_MATCH_CONST);
				exit();
			}
			
			$userPassword = $_SESSION['userChangePassword']['userPassword'] = cleanString($_POST['userPassword']);
			$userPassword2 = $_SESSION['userChangePassword']['userPassword2'] = cleanString($_POST['userPassword2']);
	
			if ($userPassword!=$userPassword2) {
				header("Location: index.php?section=userChangePassword&error=$TEXT_ERROR_PASSWORD_DO_NOT_MATCH_CONST&userID=$userID&from=$from");
				exit();
			}	

			$passMd5=md5($userPassword);
		
			//check if is allowed to change password!if  is within that installation 

			$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
			$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
			
			if ($userPassword!='' && (($queryUserInst && $queryUserInst->RecordCount()==1) || $_SESSION['userID']==$userID)) {
				//(only the creator and the owner can change the password

                $SambaPass = @exec("/usr/pluto/bin/smbpass.pl $userPassword", $outputSamba, $retcode);
                if ($retcode != 0)
                    $SambaPass=''; // we can't issue an error here, can we?
                $LinuxSalt = '$1$Pluto$'; // should we generate this? :)
                $LinuxPass = crypt($userPassword, $LinuxSalt);

				$insertUser = '
						UPDATE Users set Password = ?,Password_Unix=?,Password_Samba=? WHERE PK_Users = ?';
				$query = $dbADO->Execute($insertUser,array(md5($userPassword),$LinuxPass,$SambaPass,$userID));
				$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
				exec($commandToSend);

				
				$out.="
				<script>
					alert('$TEXT_PASSWORD_CHANGED_CONST');
				    opener.document.forms.{$from}.action.value='form';
					//opener.document.forms.{$from}.lastAction.value='xxx';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {				
				header("Location: index.php?section=userChangePassword&userID=$userID&from=$from");
				exit();
			}		
		} else {
			header("Location: index.php?section=userChangePassword&userID=$userID&from=$from&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CHANGE_PASSWORD_CONST);			
	$output->output();
}
?>

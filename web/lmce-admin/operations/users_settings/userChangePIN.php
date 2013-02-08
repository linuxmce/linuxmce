<?php
function userChangePIN($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userChangePIN.lang.php');
	
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
		<form action="index.php" method="post" name="userChangePIN">
		<input type="hidden" name="section" value="userChangePIN">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">
		
		<h3>'.$TEXT_CHANGE_PIN_CODE_CONST.'</h3>
		<table>	
		';
			$out.='
					<tr valign="top">
						<td>'.$TEXT_OLD_PASSWORD_OR_PIN_CONST.'</td>
						<td>
							<input type="password" name="passOrPIN" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>'.$TEXT_NEW_PIN_CONST.' *</td>
						<td>
							<input type="text" name="newPIN" value="">
						</td>
					</tr>					
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
					</tr>
			</table>
			<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("userChangePIN"); 		
					frmvalidator.addValidation("passOrPIN","req","'.$TEXT_ENTER_OLD_PASSWORD_OR_PIN_CONST.'");
					frmvalidator.addValidation("newPIN","req","'.$TEXT_PINCODE_REQUIRED_CONST.'");
			 		frmvalidator.addValidation("newPIN","numeric","'.$TEXT_PINCODE_REQUIRED_NUMERICAL_CONST.'");
				</script>
			';
		
	} else {
		
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		$passOrPIN=$_POST['passOrPIN'];
		$newPIN=$_POST['newPIN'];
		
		if (!$canModifyInstallation) {		
			header("Location: index.php?section=userChangePIN&userID=$userID&from=$from&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
		
		$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
		$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
		if($queryUserInst->RecordCount()==0){
			header("Location: index.php?section=userChangePIN&error=$TEXT_ERROR_INVALID_USER_SPECIFIED_CONST&userID=$userID&from=$from");
			exit();
		}
		$row=$queryUserInst->FetchRow();
		if(md5($passOrPIN)==$row['Password'] || md5($passOrPIN)==$row['PINCode']){
			$dbADO->Execute('UPDATE Users SET PINCode = ? WHERE PK_Users = ?',array(md5($newPIN),$userID));
			$out.="
				<script>
					alert('PIN code was changed!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
		}else{
			header("Location: index.php?section=userChangePIN&error=$TEXT_ERROR_NO_MATCH_PIN_OR_PASSWORD_CONST&userID=$userID&from=$from");
			exit();
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CHANGE_PIN_CODE_CONST);
	$output->output();
}
?>

<?php
function createUser($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createUser.lang.php');

	global  $dbPlutoMainDatabase;
	
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$installationID = cleanInteger(@$_SESSION['installationID']);

	$out='<h3>'.$TEXT_ADD_NEW_USER_CONST.'</h3>
	'.$TEXT_ADD_USER_INFO_CONST;
	
	if ($action=='form') {
		$lastExtensionArr=array_values(getAssocArray('Users','PK_Users','Extension',$dbADO,'','ORDER BY Extension DESC LIMIT 0,1'));
		$_SESSION['createUser']['userExtension']=(isset($lastExtensionArr[0]))?'30'.((int)substr($lastExtensionArr[0],2)+1):'301';
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
			
		</script>
		<form action="index.php" method="post" name="createUser">
		<input type="hidden" name="section" value="createUser">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<table>	
				
		
		';
		
		$i=0;

		$languagesTxt = '';
		$selectLanguages = 'SELECT * FROM Language ORDER BY Description ASC';
		$resLanguages = $dbADO->_Execute($selectLanguages);
		
		$queryHisInstallations = 'SELECT * FROM Installation 
		INNER JOIN Installation_Users on FK_Installation = PK_Installation
		WHERE FK_Users = ? and PK_Installation != ?';
		
		$resHisInstallations = $dbADO->Execute($queryHisInstallations,array($_SESSION['userID'],$installationID));
		
			$out.='
					<tr valign="top">
						<td colspan="2"><h3 class="err">'.(isset($_GET['error'])?$_GET['error']:'').'</h3></td>
					</tr>
					<tr valign="top" bgcolor="#EEEEEE">
						<td><B>'.$TEXT_USERNAME_CONST.'</B>*</td>
						<td>
							<input type="text" name="userUserName" value="'.@$_SESSION['createUser']['userUserName'].'">
						</td>
					</tr>
					<tr valign="top">
						<td><B>'.$TEXT_PASSWORD_CONST.'</B>*</td>
						<td>
							<input type="password" name="userPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td><B>'.$TEXT_CONFIRM_PASSWORD_CONST.'</B>*</td>
						<td>
							<input type="password" name="userPassword2" value="">
						</td>
					</tr>
					<tr valign="top" bgcolor="#EEEEEE">
						<td><B>'.$TEXT_PINN_CODE_CONST.'</B>* ('.$TEXT_NUMERIC_ONLY_CONST.')</td>
						<td>
							<input type="text" name="pinCode" value="">
						</td>
					</tr>			
					<tr valign="top">
						<td><B>'.$TEXT_CREATE_VOICEMAIL_INQUIRY_CONST.'</B><br>'.$TEXT_CREATE_VOICEMAIL_INFO_CONST.'</td>
						<td><input type="checkbox" name="userHasMailbox" value="1" '.@($_SESSION['createUser']['userHasMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td><B>'.$TEXT_ACCESS_GENERAL_MAILBOX_INQUIRY_CONST.'</B><br>'.$TEXT_ACCESS_GENERAL_MAILBOX_INFO_CONST.'</td>
						<td><input type="checkbox" name="userAccessGeneralMailbox" value="1" '.@($_SESSION['createUser']['userAccessGeneralMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td><B>Extension for intercom</B><br>Dial this number from any phone in the house to quickly reach the user.  Outside callers can dial it also to be directly connected.</td>
						<td><input type="text" name="userExtension" value="'.@$_SESSION['createUser']['userExtension'].'"></td>
					</tr>
					<tr valign="top">
						<td><B>'.$TEXT_NAME_CONST.'</B></td>
						<td>
							<B>'.$TEXT_FIRSTNAME_CONST.':</B><input type="text" name="userFirstName" value="'.@$_SESSION['createUser']['userFirstName'].'"><br />
							<B>'.$TEXT_LASTNAME_CONST.':</B> <input type="text" name="userLastName" value="'.@$_SESSION['createUser']['userLastName'].'"><br />
							<B>'.$TEXT_NICKNAME_CONST.':</B> <input type="text" name="userNickname" value="'.@$_SESSION['createUser']['userNickname'].'">
						</td>
					</tr>
					<tr valign="top">
						<td><B>'.$TEXT_CAN_MODIFY_CONFIGURATION_CONST.'</B><br>'.$TEXT_CAN_MODIFY_CONFIGURATION_INFO_CONST.'</td>
						<td><input type="checkbox" name="userCanModifyInstallation" value="1" '.(@$_SESSION['createUser']['userCanModifyInstallation']==1?" checked='checked'":'').'></td>
					</tr>
					
			';
			
			if ($resLanguages) {
					$resLanguages->MoveFirst();
					$languagesTxt='';
					while ($rowLanguages=$resLanguages->fetchRow()) {
						$languagesTxt.='<option '.(@$_SESSION['createUser']['userLanguage']==$rowLanguages['PK_Language']?"selected='selected'":'').' value="'.$rowLanguages['PK_Language'].'">'.$rowLanguages['Description'].'</option>';
					}
			}
			$out.='
						<tr>
							<td>
								<B>'.$TEXT_LANGUAGE_CONST.'</B>
							</td>
							<td>
								<select name="userLanguage">
								'.$languagesTxt.'
								</select>
							</td>
						</tr>
			';
			
		
			$out.='
					<tr>
						<td colspan="2" align="left">* Required fields.</td>
					</tr>			
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  > <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
					</tr>
			</table>
			<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("createUser"); 		
					frmvalidator.addValidation("userUserName","req","'.$TEXT_USERNAME_REQUIRED_CONST.'");
					frmvalidator.addValidation("userPassword","req","'.$TEXT_PASSWORD_REQUIRED_CONST.'");
					frmvalidator.addValidation("userPassword2","req","'.$TEXT_RETYPE_PASSWORD_REQUIRED_CONST.'");
					//frmvalidator.addValidation("userFirstName","req","'.$TEXT_FIRSTNAME_REQUIRED_CONST.'");
					//frmvalidator.addValidation("userLastName","req","'.$TEXT_LASTNAME_REQUIRED_CONST.'");
					frmvalidator.addValidation("pinCode","req","'.$TEXT_PINN_CODE_REQUIRED_CONST.'");
			 		frmvalidator.addValidation("pinCode","numeric","'.$TEXT_PINN_CODE_NUMERIC_REQUIRED_CONST.'");
				</script>
			';
		
	} else {
		
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	$users=getAssocArray('Users','PK_Users','Username',$dbADO);
	
	if ($canModifyInstallation || count($users)==0) {		
		
		$username = $_SESSION['createUser']['userUserName'] = cleanString($_POST['userUserName']);
		
		$hasMailbox = $_SESSION['createUser']['userHasMailbox'] =  cleanInteger(@$_POST['userHasMailbox']);
		$userAccessGeneralMailbox = $_SESSION['createUser']['userAccessGeneralMailbox'] = cleanInteger(@$_POST['userAccessGeneralMailbox']);
		
		$userFirstName =  $_SESSION['createUser']['userFirstName'] = cleanString($_POST['userFirstName']);
		$userLastName = $_SESSION['createUser']['userLastName'] = cleanString($_POST['userLastName']);
		$userNickname = $_SESSION['createUser']['userNickname'] = cleanString($_POST['userNickname']);
		
		$userExtension = $_SESSION['createUser']['userExtension'] = cleanString($_POST['userExtension']);
		
		$userCanModifyInstallation = $_SESSION['createUser']['userCanModifyInstallation'] = cleanInteger(@$_POST['userCanModifyInstallation']);
		
		$userLanguage = $_SESSION['createUser']['userLanguage'] = cleanInteger(@$_POST['userLanguage']);
		
	
		$userPassword = $_SESSION['createUser']['userPassword'] = cleanString($_POST['userPassword']);
		$userPassword2 = $_SESSION['createUser']['userPassword2'] = cleanString($_POST['userPassword2']);
		$passMd5=md5($userPassword);
		
		$masterPassMd5=$passMd5;
		$userMasterPassword=$userPassword;

		$pinCode=$_POST['pinCode'];
		$pinCodeMd5=md5($pinCode);
		
		if ($userPassword!=$userPassword2) {
			header("Location: index.php?section=createUser&error=$TEXT_ERROR_PASSWORD_NO_MATCH_CONST&from=$from");
			exit();
		}	

		$selectUsername = "SELECT UserName from Users where UserName = ?";
		$resUsername = $dbADO->Execute($selectUsername,array($username));
		if ($resUsername && $resUsername->RecordCount()>0) {
			header("Location: index.php?section=createUser&error=$TEXT_ERROR_USERNAME_TAKEN_CONST&from=$from");
			exit();
		}

		if ($username!='' && $userPassword!='') {
			
			// insert the user into local table 
			
			$SambaPass = @exec("/usr/pluto/bin/smbpass.pl $userPassword", $outputSamba, $retcode);
            if ($retcode != 0)
            	$SambaPass=''; // we can't issue an error here, can we?
			$LinuxSalt = '$1$Pluto$'; // should we generate this? :)
            $LinuxPass = crypt($userPassword, $LinuxSalt);
            
            $cmd='sudo -u root /usr/pluto/bin/CreateDevice  -D '.$dbPlutoMainDatabase.' -U "'.$username.'"';
            writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\t".$cmd."\n",'a+');
            
            // this will actually create an user !!!
            $insertID=exec($cmd,$retMessage);
            
            if(@$insertID>0){
				$dbADO->Execute('
					UPDATE Users SET
						Password=?,
						HasMailbox=?,
						AccessGeneralMailbox=?,
						FirstName=?,
						LastName=?,
						Nickname=?,
						Extension=?,
						FK_Language=?,
						PINCode=?,
						Password_Unix=?,
						Password_Samba=?
					WHERE PK_Users=?',
				array($passMd5, $hasMailbox, $userAccessGeneralMailbox, $userFirstName, $userLastName, $userNickname, $userExtension, $userLanguage, $pinCodeMd5, $LinuxPass, $SambaPass,$insertID));
			
					
				$insertUserToInstallation = "
					UPDATE Installation_Users
						SET userCanModifyInstallation=?
					WHERE FK_Installation=? AND FK_Users=?";
				$_SESSION['createUser']=array();
				$query=$dbADO->Execute($insertUserToInstallation,array($userCanModifyInstallation,$installationID,$insertID));

				// removed SetupUsers since it is executed from CreateDevice with -U
				$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
				exec_batch_command($commandToSend);
            			
				$out="
				<script>
					alert('$TEXT_USER_CREATED_CONST');
					try{
				    	opener.document.forms.{$from}.action.value='form';
						opener.document.forms.{$from}.submit();
					}catch(e){
						// error
						opener.location.reload();
					}
					self.close();
				</script>
				";	
				die($out);		
            }else{
				header("Location: index.php?section=createUser&error=".urlencode(join("<br>",$retMessage)));
				exit();
            }
		} else {				
			header("Location: index.php?section=createUser&from=$from");
			exit();
		}		
		
	} else {
			header("Location: index.php?section=createUser&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
	}
	
}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_NEW_USER_CONST);
	$output->output();
}
?>

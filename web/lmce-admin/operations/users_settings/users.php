<?
function users($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/users.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $resUserTypes ADORecordSet */
	$out='';
	$locationGoTo='';
	//$dbADO->debug=true;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login&from=users");
		exit();
	}

	$installationID = cleanInteger($_SESSION['installationID']);

	$out.='<p>'.$TEXT_USERS_NOTE_CONST.'</p>';


	if ($action=='form') {
		$queryUsers = 'SELECT Users.*,Installation_Users.userCanModifyInstallation as canModifyInstallation,userCanChangeHouseMode
		FROM Users 
		INNER JOIN Installation_Users on FK_Users = PK_Users
		WHERE FK_Installation = ?';
		$resUsers = $dbADO->Execute($queryUsers,array($installationID));


		if ($resUsers) {
			$out.=setLeftMenu($dbADO)."
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,'',attributes);
			}
		</script>
		
		<div class=\"confirm\" align=\"center\"><B>".@$_GET['msg']."</B></div>
		<div class=\"err\" align=\"center\">".@$_GET['error']."</div>
			
		<table border='0' cellpadding='3' cellspacing='1'>
			<form action='index.php' method='post' name='users'>
			<input type='hidden' name='section' value='users'>
			<input type='hidden' name='action' value='add'>
			<input type='hidden' name='lastAction' value=''>
		";

			$usersFormValidation='';
			$out.='<tr bgcolor="#DFDFDF">
				<td align="center"><B>'.$TEXT_USERNAME_CONST.'</B></td>
				<td align="center" colspan="5"><B>'.$TEXT_DETAILS_CONST.'</b></td>
				</tr>';
			$displayedUsers=array();
			$i=0;

			$languagesTxt = '';
			$selectLanguages = 'SELECT * FROM Language ORDER BY Description ASC';
			$resLanguages = $dbADO->_Execute($selectLanguages);


			while ($rowUser = $resUsers->FetchRow()) {
				if ($resLanguages) {
					$resLanguages->MoveFirst();
					$languagesTxt='';
					while ($rowLanguages=$resLanguages->fetchRow()) {
						$languagesTxt.='<option value="'.$rowLanguages['PK_Language'].'" '.($rowUser['FK_Language']==$rowLanguages['PK_Language']?"selected = 'selected'":"").'>'.$rowLanguages['Description'].'</option>';
					}
				}

				$filePath=$GLOBALS['usersPicsPath'].$rowUser['PK_Users'].'.png';
				if(file_exists($filePath)){
					$randNumber=rand(0,99999);
					$userImage='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'">';
				}else{
					$userImage='';
				}

				$displayedUsers[]=$rowUser['PK_Users'];
				$color=($i%2==1?"F0F3F8":"EEEEEE");
				$out.='<tr valign="top" bgcolor="#'.$color.'">
						<td>
							<table width="100%">
								<tr class="tablehead">
									<td align="center"><B>'.$rowUser['UserName'].'</B></td>
								</tr>
								<tr>
									<td align="center">
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePassword&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=400,toolbar=1,resizable=1\');">'.$TEXT_USER_CHANGE_PASSWORD_CONST.'</a><br>
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePIN&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=200,toolbar=1,resizable=1\');">'.$TEXT_USER_CHANGE_PIN_CONST.'</a><br>
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userPic&from=users&userID='.$rowUser['PK_Users'].'\',\'width=600,height=400,toolbar=1,resizable=1,scrollbars=1\');">'.$TEXT_UPLOAD_PICTURE_CONST.'</a><br>
										<a href="javascript:if(confirm(\''.$TEXT_DELETE_USER_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=users&action=delete&did='.$rowUser['PK_Users'].'\'">'.$TEXT_DELETE_USER_CONST.'</a>
									</td>
								</tr>			
							</table>			
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_VOICEMAIL_CONST.'<br>+'.$TEXT_EMAIL_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userHasMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['HasMailbox']?" checked='checked' ":'').'></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_ACCESS_GENERAL_MAILBOX_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userAccessGeneralMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['AccessGeneralMailbox']?" checked='checked' ":'').'></td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_EXTENSION_FOR_INTERCOM_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userExtension_'.$rowUser['PK_Users'].'" value="'.$rowUser['Extension'].'"></td>
								</tr>	
							</table>
						</td>
						<td>
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_FIRSTNAME_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userFirstName_'.$rowUser['PK_Users'].'" value="'.$rowUser['FirstName'].'"></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_LASTNAME_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userLastName_'.$rowUser['PK_Users'].'" value="'.$rowUser['LastName'].'"></td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_NICKNAME_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userNickname_'.$rowUser['PK_Users'].'" value="'.$rowUser['Nickname'].'"></td>
								</tr>	
			
							</table>			
						</td>
						
						<td>
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_LANGUAGE_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><select name="userLanguage_'.$rowUser['PK_Users'].'">
									'.$languagesTxt.'
									</select>
									</td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_HIDE_FROM_ORBITER_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="HideFromOrbiter_'.$rowUser['PK_Users'].'" value="1" '.(($rowUser['HideFromOrbiter']==1)?" checked='checked' ":'').'></td>
								</tr>	
							</table>			
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_CAN_MODIFY_CONFIGURATION_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userCanModifyInstallation_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['canModifyInstallation']?" checked='checked' ":'').'></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_CAN_SET_HOUSE_SECURITY_MODE_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userCanChangeHouseMode_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['userCanChangeHouseMode']?" checked='checked' ":'').'></td>
								</tr>			
							</table>
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.$TEXT_PICTURE_CONST.'</B></td>
								</tr>
								<tr>
									<td align="center">'.@$userImage.'</td>
								</tr>			
							</table>
						</td>			
			</tr>
			';
				$i++;
			}



			$out.='
			<tr>
				<td colspan="9"><span class="err">'.stripslashes(@$_REQUEST['error']).'</span></td>
			</tr>
			<tr>
				<td colspan="9"><p><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=users\',\'width=600,height=650,toolbar=1, resizable=1\');">'.$TEXT_CREATE_NEW_USER_CONST.'</a><p>
				</td>
			</tr>
			<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td></tr>
				<input type="hidden" name="displayedUsers" value="'.join(",",$displayedUsers).'">
			</form>

		<script>
		 	var frmvalidator = new formValidator("users"); 		
			'.$usersFormValidation.'
		</script>
			
		</table>
		<br />
		';
		}
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);

		if ($canModifyInstallation) {
			//process
			// delete user if $_REQUEST['did'] is set
			if(isset($_REQUEST['did']) && (int)$_REQUEST['did']>0){
				$toDel=(int)$_REQUEST['did'];
				
				// check if you try to delete your own user
				if($toDel==$_SESSION['userID']){
					header('Location: index.php?section=users&error='.$TEXT_ERROR_DELETE_YOUR_USER_CONST);
					exit();
				}
				
				$cmd='sudo -u root /usr/pluto/bin/RemoveUser.sh -d '.$toDel;
				exec($cmd);

				$dbADO->Execute('DELETE FROM Installation_Users WHERE FK_Users=?',$toDel);
				$dbADO->Execute('DELETE FROM Users WHERE PK_Users=?',$toDel);
				
				$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
				exec_batch_command($commandToSend);

				header("Location: index.php?section=users&msg=$TEXT_USER_DELETED_CONST");
				exit();
			}
			
			$displayedUsers = cleanString($_POST['displayedUsers']);
			$displayedUsersArray = explode(",",$displayedUsers);

			if (!is_array($displayedUsersArray) || $displayedUsersArray===array()) {
				$displayedUsersArray=array();
			}

			foreach ($displayedUsersArray as $user) {


				$hasMailbox =   cleanInteger(@$_POST['userHasMailbox_'.$user]);
				$userAccessGeneralMailbox = cleanInteger(@$_POST['userAccessGeneralMailbox_'.$user]);

				$userFirstName = cleanString($_POST['userFirstName_'.$user]);
				$userLastName = cleanString($_POST['userLastName_'.$user]);
				$userNickname = cleanString($_POST['userNickname_'.$user]);

				$userExtension = cleanString($_POST['userExtension_'.$user]);


				$userCanModifyInstallation = cleanInteger(@$_POST['userCanModifyInstallation_'.$user]);
				$userCanChangeHouseMode= cleanInteger(@$_POST['userCanChangeHouseMode_'.$user]);

				$userLanguage = cleanInteger(@$_POST['userLanguage_'.$user]);
				$HideFromOrbiter = cleanInteger(@$_POST['HideFromOrbiter_'.$user]);


				$query = 'UPDATE Users set
									HasMailbox =?,
									AccessGeneralMailbox =?,
									Extension=?,
									FirstName=?,
									LastName=?,
									Nickname=?,
									FK_Language=?,
									HideFromOrbiter=?
							WHERE PK_Users = ?';
				$resUpdUser = $dbADO->Execute($query,array($hasMailbox,$userAccessGeneralMailbox,
				$userExtension,$userFirstName,$userLastName,$userNickname,
				$userLanguage,$HideFromOrbiter,$user));

				// if the user is current user, update session variables
				if($user==$_SESSION['userID']){
					$_SESSION['MyExtension'] = $userExtension;
					$_SESSION['AccessGeneralMailbox'] = $userAccessGeneralMailbox;
				}
				
				// check if is the last user who could modify this installation
				$resLastUser=$dbADO->Execute('SELECT FK_Users FROM Installation_Users WHERE userCanModifyInstallation=1 AND FK_Installation =?',array($installationID));
				$rowLastUser=$resLastUser->FetchRow();
				if($resLastUser && $resLastUser->RecordCount()==1 && $userCanModifyInstallation==0 && $rowLastUser['FK_Users']==$user){
					header("Location: index.php?section=users&error=At least one user could be able to modify installation!&lastAction=".$locationGoTo);
					exit();
				}

				$updateInstallationUserCanModify = 'UPDATE Installation_Users set userCanModifyInstallation = '.$userCanModifyInstallation.',userCanChangeHouseMode='.$userCanChangeHouseMode.' WHERE FK_Users = ? and FK_Installation = ? ';
				$dbADO->Execute($updateInstallationUserCanModify,array($user,$installationID));
				$locationGoTo = "userDesc_".$user;


			}
			$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
			exec_batch_command($commandToSend);

			if (strstr($locationGoTo,"#")) {
				header("Location: index.php?section=users&msg=$TEXT_USERS_PAGE_SAVED_CONST".$locationGoTo);
			} else {
				header("Location: index.php?section=users&msg=$TEXT_USERS_PAGE_SAVED_CONST&lastAction=".$locationGoTo);
			}
		} else {
			header("Location: index.php?section=users&error=$TEXT_NOT_ALLOWED_CONST&lastAction=".$locationGoTo);
		}

	}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.users.{$lastAction}) {document.forms.users.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_USERS_CONST);
	$output->setNavigationMenu(array($TEXT_SETTINGS_CONST=>'index.php?section=installationSettings',$TEXT_USERS_CONST=>'index.php?section=users'));

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_USERS_CONST);
	$output->output();
}
?>

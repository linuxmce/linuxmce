<?php
//Rob Woodward - Modified to stop enable / disable from re-setting password
function checkVPNaccess($username) {
	$username=exec("echo $username | tr '[:upper:]' '[:lower:]'");
	return (exec("awk '$1==\"$username\" {print $1}' /etc/ppp/pap-secrets") == $username);
}

function setVPNaccess($username, $access, $userVPNIP) {
	//Rob Woodward - Check if the user already exists in the chap-secrets file

	if($access) // enable user's VPN access
	{
		exec("sudo -u root /usr/pluto/bin/Network_VPN.sh enable pluto_$username $userVPNIP");
	}
	else // disable user's VPN access
	{
		exec("sudo -u root /usr/pluto/bin/Network_VPN.sh disable pluto_$username $userVPNIP");
	}
}

function changeVPN($username, $userVPNIP, $userVPNAccess){
	echo "$username";
	exec("sudo -u root /usr/pluto/bin/Network_VPN.sh change pluto_$username $userVPNIP $userVPNAccess");
}


function users($output,$dbADO) {
	
	includeLangFile('common.lang.php');
	includeLangFile('users.lang.php');
	
	//ini_set('display_errors', 1); 
	//error_reporting(E_ALL);
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

	// extract VPN settings from DeviceData
        $resVPN=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=295');
        $rowVPN=$resVPN->FetchRow();
        $VPN_data=explode(",", $rowVPN['IK_DeviceData']);
        $enableVPN = ($VPN_data[0]=='on')?1:0;
        $VPNRange=explode('-',$VPN_data[1]);
        $VPNCountStart=explode('.',$VPNRange[0]);
        $VPNCountStart=$VPNCountStart[3];
        $VPNCountEnd=explode('.',$VPNRange[1]);
        $VPNCountEnd=$VPNCountEnd[3];
		$VPNNet=explode('.',$VPNRange[0]);
        $VPNNet=$VPNNet[0].'.'.$VPNNet[1].'.'.$VPNNet[2];
		$VPNCount=$VPNCountStart;

		while ($VPNCount <= $VPNCountEnd) {
			$VPN_IP[]=$VPNNet.'.'.$VPNCount;
			$VPNCount++;
		}
	
	$installationID = cleanInteger($_SESSION['installationID']);

	$out.='<p>'.translate('TEXT_USERS_NOTE_CONST').'</p>';

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
				<td align="center"><B>'.translate('TEXT_USERNAME_CONST').'</B></td>
				<td align="center" colspan="5"><B>'.translate('TEXT_DETAILS_CONST').'</b></td>
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
				$out.='<input type="hidden" name="userName_'.$rowUser['PK_Users'].'" value="'.$rowUser['UserName'].'">';
				$filePath=$GLOBALS['usersPicsPath'].$rowUser['PK_Users'].'.png';
				if(file_exists($filePath)){
					$randNumber=rand(0,99999);
					$userImage='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'">';
				}else{
					$userImage='';
				}

				$displayedUsers[]=$rowUser['PK_Users'];
				$color=($i%2==1?"F0F3F8":"EEEEEE");
				$hasVPNaccess=checkVPNaccess("pluto_".$rowUser['UserName']);
				$out.='<tr valign="top" bgcolor="#'.$color.'">
						<td>
							<table width="100%">
								<tr class="tablehead">
									<td align="center"><B>'.$rowUser['UserName'].'</B></td>
								</tr>
								<tr>
									<td align="center">
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePassword&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=400,toolbar=1,resizable=1\');">'.translate('TEXT_USER_CHANGE_PASSWORD_CONST').'</a><br>
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePIN&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=200,toolbar=1,resizable=1\');">'.translate('TEXT_USER_CHANGE_PIN_CONST').'</a><br>
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userPic&from=users&userID='.$rowUser['PK_Users'].'\',\'width=600,height=400,toolbar=1,resizable=1,scrollbars=1\');">'.translate('TEXT_UPLOAD_PICTURE_CONST').'</a><br>
										<a href="javascript:if(confirm(\''.translate('TEXT_DELETE_USER_CONFIRMATION_CONST').'\'))self.location=\'index.php?section=users&action=delete&did='.$rowUser['PK_Users'].'\'">'.translate('TEXT_DELETE_USER_CONST').'</a>
									</td>
								</tr>			
							</table>			
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_VOICEMAIL_CONST').' + '.translate('TEXT_EMAIL_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userHasMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['HasMailbox']?" checked='checked' ":'').'></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_ACCESS_GENERAL_MAILBOX_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userAccessGeneralMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['AccessGeneralMailbox']?" checked='checked' ":'').'></td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_EXTENSION_FOR_INTERCOM_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userExtension_'.$rowUser['PK_Users'].'" value="'.$rowUser['Extension'].'"></td>
								</tr>
                                                                <tr bgcolor="#DFDFDF">
                                                                        <td align="center"><B>'.translate('TEXT_EMAIL_ADDRESS_CONST').'</B></td>
                                                                </tr>
                                                                <tr>
				                                        <td align="center"><input type="text" name="userEmailAddress_'.$rowUser['PK_Users'].'" value="'.$rowUser['ForwardEmail'].'"></td>
                                                                </tr>

							</table>
						</td>
						<td>
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_FIRSTNAME_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userFirstName_'.$rowUser['PK_Users'].'" value="'.$rowUser['FirstName'].'"></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_LASTNAME_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userLastName_'.$rowUser['PK_Users'].'" value="'.$rowUser['LastName'].'"></td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_NICKNAME_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="text" name="userNickname_'.$rowUser['PK_Users'].'" value="'.$rowUser['Nickname'].'"></td>
								</tr>	
			
							</table>			
						</td>
						
						<td>
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_LANGUAGE_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><select name="userLanguage_'.$rowUser['PK_Users'].'">
									'.$languagesTxt.'
									</select>
									</td>
								</tr>	
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_HIDE_FROM_ORBITER_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="HideFromOrbiter_'.$rowUser['PK_Users'].'" value="1" '.(($rowUser['HideFromOrbiter']==1)?" checked='checked' ":'').'></td>
								</tr>	
							</table>			
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_CAN_MODIFY_CONFIGURATION_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userCanModifyInstallation_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['canModifyInstallation']?" checked='checked' ":'').'></td>
								</tr>			
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_CAN_SET_HOUSE_SECURITY_MODE_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userCanChangeHouseMode_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['userCanChangeHouseMode']?" checked='checked' ":'').'></td>
								</tr>
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_CAN_CONNECT_TO_VPN_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center"><input type="checkbox" name="userCanConnectToVPN_'.$rowUser['PK_Users'].'" value="1" '.($hasVPNaccess ? " checked='checked' ":'').'>';
									$username='pluto_'.$rowUser['UserName'];
									$username=exec("echo $username | tr '[:upper:]' '[:lower:]'");
									$uservpn=exec("awk '$1==\"$username\" {print $4}' /etc/ppp/pap-secrets");
									$Select_IP = '<option name="userVPNIP_'.$rowUser['PK_Users'].'" value="Auto">Automatic</option>';
									foreach ($VPN_IP as $IP => $User_IP) {
										$selected=$uservpn == $User_IP?'selected':'';
										$Select_IP .= '<option name="userVPNIP_'.$rowUser['PK_Users'].'" value="'.$User_IP.'" '.$selected.' >'.$User_IP.'</option>';
										if ($uservpn == $User_IP) {
											$VPN_IP=array_diff($VPN_IP, array($User_IP)); 
										};
									};
									$out.='<select name="userVPNIP_'.$rowUser['PK_Users'].'">'.$Select_IP.'
									</select>';
									$uservpnaccess=exec("awk '$1==\"$username\" {print $5}' /etc/ppp/pap-secrets | cut -d\"#\" -f2");
									$out.='
									<select name="userVPNAccess_'.$rowUser['PK_Users'].'">
									<option name="userVPNAccess_'.$rowUser['PK_Users'].'" value="Core '.(($uservpnaccess=="Core")?'selected':'').'">Core Only</option>
									<option name="userVPNAccess_'.$rowUser['PK_Users'].'" value="LAN" '.(($uservpnaccess=="LAN")?'selected':'').'>Core and Local Network</option>
									<option name="userVPNAccess_'.$rowUser['PK_Users'].'" value="All" '.(($uservpnaccess=="All")?'selected':'').'>Everything</option>
								        </select>
									</td>
								</tr>
							</table>
						</td>
						<td align="center">
							<table width="100%">
								<tr bgcolor="#DFDFDF">
									<td align="center"><B>'.translate('TEXT_PICTURE_CONST').'</B></td>
								</tr>
								<tr>
									<td align="center">'.@$userImage.'</td>
								</tr>			
							</table>
						</td>			
			</tr>
			<tr><td colspan="6"><hr /></td></tr>';
				unset($Select_IP);
				$i++;
			}



			$out.='
			<tr>
				<td colspan="9"><span class="err">'.stripslashes(@$_REQUEST['error']).'</span></td>
			</tr>
			<tr>
				<td colspan="9"><p><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=users\',\'width=600,height=650,toolbar=1, resizable=1\');">'.translate('TEXT_CREATE_NEW_USER_CONST').'</a><p>
				</td>
			</tr>
			<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_SAVE_CONST').'"  > <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td></tr>
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
					header('Location: index.php?section=users&error='.translate('TEXT_ERROR_DELETE_YOUR_USER_CONST'));
					exit();
				}
				
				$cmd='sudo -u root /usr/pluto/bin/RemoveUser.sh -d '.$toDel;
				exec($cmd);

				$res=$dbADO->Execute('select Username from Users where PK_Users=?',$toDel);
				while ($row=$res->FetchRow()) {
					$username=$row['Username'];
				}
				exec_batch_command("sudo -u root /usr/pluto/bin/Network_VPN.sh delete pluto_$username");

				$dbADO->Execute('DELETE FROM Installation_Users WHERE FK_Users=?',$toDel);
				$dbADO->Execute('DELETE FROM Users WHERE PK_Users=?',$toDel);
				
				$commandToSend='sudo -u root /usr/pluto/bin/SetupUsers.sh';
				exec_batch_command($commandToSend);

				header("Location: index.php?section=users&msg=".translate('TEXT_USER_DELETED_CONST'));
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

				$userName = cleanString($_POST['userName_'.$user]);
				$userFirstName = cleanString($_POST['userFirstName_'.$user]);
				$userLastName = cleanString($_POST['userLastName_'.$user]);
				$userNickname = cleanString($_POST['userNickname_'.$user]);

				$userExtension = cleanString($_POST['userExtension_'.$user]);
				$userEmailAddress = cleanString($_POST['userEmailAddress_'.$user]);

				$userCanModifyInstallation = cleanInteger(@$_POST['userCanModifyInstallation_'.$user]);
				$userCanChangeHouseMode= cleanInteger(@$_POST['userCanChangeHouseMode_'.$user]);
				$userCanConnectToVPN= cleanInteger(@$_POST['userCanConnectToVPN_'.$user]);
				$userVPNIP = cleanString($_POST['userVPNIP_'.$user]);
				$userVPNAccess = cleanString($_POST['userVPNAccess_'.$user]);

				$userLanguage = cleanInteger(@$_POST['userLanguage_'.$user]);
				$HideFromOrbiter = cleanInteger(@$_POST['HideFromOrbiter_'.$user]);


				$query = 'UPDATE Users set
									HasMailbox =?,
									AccessGeneralMailbox =?,
									Extension=?,
									ForwardEmail=?,
									FirstName=?,
									LastName=?,
									Nickname=?,
									FK_Language=?,
									HideFromOrbiter=?
							WHERE PK_Users = ?';
				$resUpdUser = $dbADO->Execute($query,array($hasMailbox,$userAccessGeneralMailbox,
				$userExtension,$userEmailAddress,$userFirstName,$userLastName,$userNickname,
				$userLanguage,$HideFromOrbiter,$user));

				// modify user's VPN access if needed
				setVPNaccess($userName, $userCanConnectToVPN, $userVPNIP);
				changeVPN($userName, $userVPNIP, $userVPNAccess);

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
				header("Location: index.php?section=users&msg=".translate('TEXT_USERS_PAGE_SAVED_CONST').$locationGoTo);
			} else {
				header("Location: index.php?section=users&msg=".translate('TEXT_USERS_PAGE_SAVED_CONST')."&lastAction=".$locationGoTo);
			}
		} else {
			header("Location: index.php?section=users&error=".translate('TEXT_NOT_ALLOWED_CONST')."&lastAction=".$locationGoTo);
		}

	}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.users.{$lastAction}) {document.forms.users.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_USERS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_SETTINGS_CONST')=>'index.php?section=installationSettings',translate('TEXT_USERS_CONST')=>'index.php?section=users'));

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_USERS_CONST'));
	$output->output();
}
?>

<?
function login($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/login.lang.php');
	global $wikiHost;
	
	/* @var $dbADO ADOConnection */
	$out='';
	$actionX = cleanString(@$_REQUEST['action']);

	$users=getAssocArray('Users','PK_Users','Username',$dbADO);
	
	$loginFormBig= '
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>

	<form name="form1" id="form1" method="post" action="'.$_SERVER['PHP_SELF'].'">
	<input type="hidden" name="section" value="login">
	  <table border="0" align="center" cellpadding="5" cellspacing="0">
		<tr>
			<td colspan="2" align="center" height="133" class="left_frame_logo">
				<img src="include/images/spacer.gif" border="0" width="220" height="90">
			</td>
		</tr>
		<tr>
			<td colspan="2" align="center">
				<B>'.((isset($_SESSION['logout_msg']))?$_SESSION['logout_msg']:$TEXT_WELCOME_MSG_CONST).'</B>
			</td>
		</tr>';
	if(count($users)==0){
		$loginFormBig.='
		<tr> 
	      <td colspan="2" align="center"><span class="err">'.$TEXT_NO_USERS_CONST.'</span><br><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=login\',\'width=600,height=650,toolbars=true, resizable=1\');">'.$TEXT_CREATE_NEW_ADMIN_USER_CONST.'</a><br><br><br></td>
	    </tr>';
	}else{
		$loginFormBig.='		
	    <tr> 
	      <td>'.$TEXT_USERNAME_CONST.'</td>
	      <td>
	      	<input type="text" name="username" value="'.@$_POST['username'].'" /></td>
	    </tr>
	    <tr> 
	      <td>'.$TEXT_PASSWORD_CONST.'</td>
	      <td><input type="password" name="password" value="" /></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_LOGIN_CONST.'" /></td>
	    </tr>';
	}
	
	$loginFormBig.='
	    <tr>
	    &nbsp;&nbsp;
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><a href="'.$wikiHost.'/index.php/<-mkr_t_name_mixed->_Admin_Website" target="_top">'.$TEXT_HELP_WITH_ADMIN_SITE_CONST.'</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center">Learn about <-mkr_t_name_mixed-> at <a href="http://www.<-mkr_t_main_url->" target="_top">www.<-mkr_t_main_url-></a><br><br>'.$TEXT_DOWNLOAD_CONST.' <a href="index.php?section=orbitersWin">'.$TEXT_ORBITER_WIN_INSTALLER_CONST.'</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center">'.$TEXT_VERSION_CONST.': <=version=></td>
	    </tr>
	  </table>
	</form>
	<p align="center" class="err"><B>'.(isset($_GET['error'])?strip_tags(stripslashes($_GET['error'])):'').'</B></p>	
	';
	unset($_SESSION['logout_msg']);

	if (isset($_POST['submitX'])) {

		$usernameForm = cleanString($_POST['username']);
		$passwordForm = cleanString($_POST['password']);


		$messages = '';

		if ($usernameForm=='')  {
			$messages.=$TEXT_LOGIN_USERNAME_REQUIRED_CONST;
		}

		if (isset($_POST['submitX']))
		{
			if ($passwordForm == '') {
				header("Location: index.php?section=login&error=$TEXT_LOGIN_PASSWORD_REQUIRED_CONST");
				exit();
			} else {
				$_SESSION['password']=$passwordForm;
				$passwordForm = md5($passwordForm);

				if ($messages==='')	{
					/* @var $res_users ADORecordSet */
					$res_users = $dbADO->Execute("select * from Users WHERE UserName = ? and Password = ?",array($usernameForm,$passwordForm));
					if (!$res_users || $res_users->RecordCount()==0) {
						if($usernameForm=='remote'){

							// remote acces: if you allowed remote acces in Wizard/Security/Outside Access the website can be accesed
							// using username remote and the password you setup in Outside access page

							exec('cat '.$GLOBALS['pluto.conf'].' | grep -v -E "^#|^$" ',$retArray);
							foreach ($retArray as $comf){
								parse_str(preg_replace('/ ?= ?/','=',$comf));
							}
							if(isset($remote) && $remote==$_SESSION['password']){
								// search for an user who can modify installation
								$resRemote=$dbADO->Execute('
											SELECT Users.* 
											FROM Users 
											INNER JOIN Installation_Users ON FK_Users=PK_Users
											WHERE userCanModifyInstallation=1 LIMIT 0,1');
								if($resRemote->RecordCount()>0){
									$rowRemote=$resRemote->FetchRow();

									$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ? ORDER BY FK_Installation ASC";
									$res_installations = $dbADO->Execute($query_installation,array((int)$rowRemote['PK_Users']));
									if($res_installations->RecordCount()==0){
										header('Location: index.php?section=login&error='.$TEXT_ERROR_NO_INSTALLATION_CONST);
										exit();
									}

									$_SESSION['userID'] = (int)$rowRemote['PK_Users'];
									$_SESSION['MyExtension'] = (int)$rowRemote['Extension'];
									$_SESSION['AccessGeneralMailbox'] = (int)$rowRemote['AccessGeneralMailbox'];
									$_SESSION['userLoggedIn'] = true;

									$_SESSION['username'] = $rowRemote['UserName'];


									$installations=array();
									while ($rowInstallations=$res_installations->FetchRow()) {
										$installations[]=$rowInstallations['FK_Installation'];
									}
									if ($installations===array()) {
										//the user has no installation!!!
										$messages=$TEXT_ERROR_NO_INSTALLATION_CONST;
										header("Location: index.php?section=login&error=urlencode($messages)");
										exit();
									} else {
										$_SESSION['installationIDs'] = $installations;
										$_SESSION['installationID'] = $installations[0];
									}

									header("Location: index.php");
									exit();
								}else{
									$_SESSION['password'] = null;
									$_SESSION['userLoggedIn'] = false;
									header("Location: index.php?section=login&error=".$TEXT_ERROR_REMOTE_ACCES_FAILED_CONST);
									exit();
								}
							}
						}

						$messages.=$TEXT_ERROR_INVALID_USER_OR_PASSWORD_CONST;
						header("Location: index.php?section=login&error=".$TEXT_ERROR_INVALID_USER_OR_PASSWORD_CONST);
						exit(0);
					} else {

						$row_users = $res_users->FetchRow();

						$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ? ORDER BY FK_Installation ASC";
						$res_installations = $dbADO->Execute($query_installation,array((int)$row_users['PK_Users']));
						if($res_installations->RecordCount()==0){
							header('Location: index.php?section=login&error='.$TEXT_ERROR_NO_INSTALLATION_CONST);
							exit();
						}

						$_SESSION['userID'] = (int)$row_users['PK_Users'];
						$_SESSION['userLoggedIn'] = true;

						$_SESSION['hh_username'] = $row_users['FirstName'].' '.$row_users['LastName'];
						$_SESSION['username'] = $usernameForm;
						$_SESSION['MyExtension'] = (int)$row_users['Extension'];
						$_SESSION['AccessGeneralMailbox'] = (int)$row_users['AccessGeneralMailbox'];

						$installations=array();
						while (!$res_installations->EOF) {
							$installations[]=$res_installations->fields['FK_Installation'];
							$res_installations->MoveNext();
						}

						if ($installations===array()) {
							//the user has no installation!!!
							$messages=$TEXT_ERROR_NO_INSTALLATION_CONST;
							header("Location: index.php?section=login&error=urlencode($messages)");
						} else {
							$_SESSION['installationIDs'] = $installations;
							$_SESSION['installationID'] = $installations[0];
						}

						die('<script>top.location=\'index.php\';</script>');

					}
				} else {
					$out.=$messages;
					$_SESSION['password'] = null;
					$_SESSION['userLoggedIn'] = false;
					header("Location: index.php?section=login&error=$messages");
				}

			}

		}
	} elseif ($actionX=='logout') {
		$_SESSION = array();
		session_destroy();
		$_SESSION['logout_msg']=$TEXT_LOGOUT_MSG_CONST;
		
		$scriptInHead='
		<script>
			top.location.href="index.php?section=login"
		</script>';
		
		$output->setScriptInHead($scriptInHead);
	} elseif (@$_SESSION['userLoggedIn']==false) {
		$output->setScriptInBody("onload=\"document.form1.username.focus();\"");
		$out.=$loginFormBig;
	} else {
		$scriptInHead='
		<script>
			top.location.href=\'index.php\';
		</script>
	';
		$output->setScriptInHead($scriptInHead);
		exit(0);
	}


	$output->setScriptInBody('style="background:#FFFFFF"');
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>

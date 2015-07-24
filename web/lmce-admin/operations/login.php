<?php
function login($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('login.lang.php');

	global $wikiHost;
	
	/* @var $dbADO ADOConnection */
	$out='';
	$actionX = cleanString(@$_REQUEST['action']);

	$users=getAssocArray('Users','PK_Users','Username',$dbADO);
	
	//See if a web orbiter is installed
	$query_weborbiter = "SELECT * FROM Device WHERE FK_DeviceTemplate=1749";
	$res_weborbiter = $dbADO->Execute($query_weborbiter);
	if($res_weborbiter->RecordCount()==0){
		//Web orbiter not installed
		$weborbiterInstalled=false;
	} else {
		//Web orbiter is installed
		$weborbiterInstalled=true;
	}
	$authorize="NONE";
	$app_name= $_GET['client_id'];
	$app_secret=$_GET['client_secret'];
	$authorize = @$_REQUEST["authorize"];
	$version = exec("dpkg -s pluto-website-admin|grep 'Version: '|cut -d ' ' -f 2");
	$loginFormBig= '
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>

	<form name="form1" id="form1" method="post" action="'.$_SERVER['PHP_SELF'].'">
	<input type="hidden" name="section" value="login">
	
	<input type="hidden" name="authorize" value="'.$authorize.'">
        <input type="hidden" name="client_id" value="'.$app_name.'">
        <input type="hidden" name="client_secret" value="'.$app_secret.'">
	<input type="hidden" name="state" value="'.$authorize.'">
	  <table border="0" align="center" cellpadding="5" cellspacing="0">
		<tr>
			<td colspan="2" align="center" height="133" class="left_frame_logo">
				<img src="include/images/spacer.gif" border="0" width="220" height="90">
			</td>
		</tr>
		<tr>
			<td colspan="2" align="center">
				<B>'.((isset($_SESSION['logout_msg']))?$_SESSION['logout_msg']:translate('TEXT_WELCOME_MSG_CONST')).'</B>
 			</td>
		</tr>';
	if(count($users)==0){
		$loginFormBig.='
		<tr> 
	      <td colspan="2" align="center"><span class="err">'.translate('TEXT_NO_USERS_CONST').'</span><br><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=login\',\'width=600,height=650,toolbar=1, resizable=1\');">'.translate('TEXT_CREATE_NEW_ADMIN_USER_CONST').'</a><br><br><br></td>
	    </tr>';
	}else{
		$loginFormBig.='		
	    <tr> 
	      <td>'.translate('TEXT_USERNAME_CONST').'</td>
	      <td>
	      	<input type="text" name="username" value="'.@$_POST['username'].'" /></td>
	    </tr>
	    <tr> 
	      <td>'.translate('TEXT_PASSWORD_CONST').'</td>
	      <td><input type="password" name="password" value="" /></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.translate('TEXT_LOGIN_CONST').'" /></td>
	    </tr>';
	}
	
	$loginFormBig.='
	    <tr>
	    &nbsp;&nbsp;
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><a href="'.$wikiHost.'/index.php/LinuxMCE_Admin_Website" target="_top">'.translate('TEXT_HELP_WITH_ADMIN_SITE_CONST').'</a></td>
	    </tr>
	    <tr>
	      <td>&nbsp;</td>
	    </tr>';
	if($weborbiterInstalled) {
		$loginFormBig.='
		<tr>
		  <td colspan="2" align="left">'.translate('TEXT_WEBORBITER_LOGIN').' <a href="weborbiter.php" target="_top">'.translate('TEXT_WEBORBITER').'</a></td>
		</tr>';
	}
	$loginFormBig.='
	    <tr>
	      <td colspan="2" align="left">Learn about LinuxMCE at <a href="http://www.linuxmce.org" target="_top">www.linuxmce.org</a><br><br>'.translate('TEXT_DOWNLOAD_CONST').' <a href="index.php?section=orbitersWin">'.translate('TEXT_ORBITER_WIN_INSTALLER_CONST').'</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="left">'.translate('TEXT_VERSION_CONST').': '.$version.'</td>
	    </tr>
	  </table>
	</form>
	<p align="center" class="err"><B>'.(isset($_GET['error'])?strip_tags(stripslashes($_GET['error'])):'').'</B></p>	
	';
	unset($_SESSION['logout_msg']);

	if (isset($_POST['submitX'])) {

		$usernameForm = cleanString($_POST['username']);
		$passwordForm = cleanString($_POST['password']);

		$authorize=$_POST["authorize"];
		$messages = '';

		if ($usernameForm=='')  {
			$messages.=translate('TEXT_LOGIN_USERNAME_REQUIRED_CONST');
		}

		if (isset($_POST['submitX']))
		{
			if ($passwordForm == '') {
				header("Location: index.php?section=login&error=".translate('TEXT_LOGIN_PASSWORD_REQUIRED_CONST'));
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
										header('Location: index.php?section=login&error='.translate('TEXT_ERROR_NO_INSTALLATION_CONST'));
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
										$messages=translate('TEXT_ERROR_NO_INSTALLATION_CONST');
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
									header("Location: index.php?section=login&error=".translate('TEXT_ERROR_REMOTE_ACCES_FAILED_CONST'));
									exit();
								}
							}
						}

						$messages.=translate('TEXT_ERROR_INVALID_USER_OR_PASSWORD_CONST');
						//log user login failed
						$postuser=$usernameForm;
						$today = date("F j H:i:s");
						$IPClient= $_SERVER['REMOTE_ADDR'];
						$logAuth = fopen('/var/log/lmceweb/auth.log', 'a+');
						fputs($logAuth, $today . " \tServer name " . " lmce-admin: login failure". " as user : " . $postuser . " \t" . $IPClient . "\n");
						fclose($logAuth);
						header("Location: index.php?section=login&error=".translate('TEXT_ERROR_INVALID_USER_OR_PASSWORD_CONST'));
						exit(0);
					} else {

						$row_users = $res_users->FetchRow();

						$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ? ORDER BY FK_Installation ASC";
						$res_installations = $dbADO->Execute($query_installation,array((int)$row_users['PK_Users']));
						if($res_installations->RecordCount()==0){
							header('Location: index.php?section=login&error='.translate('TEXT_ERROR_NO_INSTALLATION_CONST'));
							exit();
						}

						$_SESSION['userID'] = (int)$row_users['PK_Users'];
						$_SESSION['userLoggedIn'] = true;

						$_SESSION['hh_username'] = $row_users['FirstName'].' '.$row_users['LastName'];
						$_SESSION['username'] = $usernameForm;
						$_SESSION['MyExtension'] = (int)$row_users['Extension'];
						$_SESSION['AccessGeneralMailbox'] = (int)$row_users['AccessGeneralMailbox'];
						
						// set the user's language for web admin
						if ($row_users['FK_Language'] != NULL) {
							$res_language = $dbADO->Execute("select Description from Language WHERE PK_Language = ".$row_users['FK_Language']);
							$row_language = $res_language->FetchRow();						
							$_SESSION['UserLanguage'] = $row_language['Description'];
						} else {
							$_SESSION['UserLanguage'] = $GLOBALS['fallbacklang'];
						}

						$installations=array();
						while (!$res_installations->EOF) {
							$installations[]=$res_installations->fields['FK_Installation'];
							$res_installations->MoveNext();
						}

						if ($installations===array()) {
							//the user has no installation!!!
							$messages=translate('TEXT_ERROR_NO_INSTALLATION_CONST');
							header("Location: index.php?section=login&error=urlencode($messages)");
						} else {
							$_SESSION['installationIDs'] = $installations;
							$_SESSION['installationID'] = $installations[0];
						}
						
						;
						if($authorize!=""){
						 header("Location: index.php?section=authorizeApp&state='.authorize.'");
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
		$_SESSION['logout_msg']=translate('TEXT_LOGOUT_MSG_CONST');
		
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

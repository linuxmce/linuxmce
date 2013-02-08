<?php
function emailSetup($output,$dbADO) {
        // include language files
 		includeLangFile('common.lang.php');
		includeLangFile('emailSetup.lang.php');

		// initialize some variables		
		$out='';
		$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
		$installationID = cleanInteger($_SESSION['installationID']);

		// get core ID
		$queryDevice='SELECT Device.PK_Device FROM Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['CategoryCore'],$installationID));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
		}

		// fetch current mail settings from database
		$query='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$res=$dbADO->Execute($query,array($coreID,$GLOBALS['EmailSettings']));
		$row=$res->FetchRow();
		
		$emailSettings=explode(',',$row['IK_DeviceData']);
		
		$emailEnabled=$emailSettings[0];			// Email subsystem enabled
		$emailFromName=$emailSettings[1];			// From Name
		$emailFromMail=$emailSettings[2];			// From email address
		$emailProvider=$emailSettings[3];			// SMTP provider
		$emailServer=$emailSettings[4];				// SMTP server
		$emailPort=$emailSettings[5];				// SMTP port
		$emailTLSEnabled=$emailSettings[6];			// Use TLS
		$emailAuthEnabled=$emailSettings[7];		// SMTP auth
		$emailAuthUsername=$emailSettings[8];		// SMTP auth username
		$emailAuthPassword=$emailSettings[9];		// SMTP auth password

		// put some defaults into empty config variables
		if($emailFromName=='') $emailFromName='LinuxMCE';
		if($emailFromMail=='') {
			$domain=getDeviceData($coreID,$GLOBALS['ComputerDomain'],$dbADO);
			$cname=getDeviceData($coreID,$GLOBALS['ComputerName'],$dbADO);
			$emailFromMail=$cname.'@'.$domain;
		}
		if($emailProvider=='') $emailProvider='custom';
		if($emailPort=='') $emailPort='25';
		if($emailServer=='') $emailServer='smtp.provider.com';
		
		// Debugging
		//print_array($emailSettings);

	// Show form
	if ($action == 'form') {
		if(!isset($fatalError)){					
			$out.='
			<script type="text/javascript">
			function changeProvider()
				{
					provider=document.emailSetup.emailProvider.value;
					
					// disabled input fields
					document.emailSetup.emailServer.disabled = true;
					document.emailSetup.emailPort.disabled = true;
					document.emailSetup.emailTLSEnabled.disabled = true;
					document.emailSetup.emailAuthEnabled.disabled = true;
									
					switch(provider) {
					case "gmail":
						document.emailSetup.emailServer.value = "smtp.gmail.com";
						document.emailSetup.emailPort.value = "587";
						document.emailSetup.emailTLSEnabled.checked = true;
						document.emailSetup.emailAuthEnabled.checked = true;
						break;
					case "hotmail":
						document.emailSetup.emailServer.value = "smtp.live.com";
						document.emailSetup.emailPort.value = "587";
						document.emailSetup.emailTLSEnabled.checked = true;
						document.emailSetup.emailAuthEnabled.checked = true;
						break;
					case "yahoo":
						document.emailSetup.emailServer.value = "smtp.mail.yahoo.com";
						document.emailSetup.emailPort.value = "25";
						document.emailSetup.emailTLSEnabled.checked = false;
						document.emailSetup.emailAuthEnabled.checked = true;
						break;
					default:
						// enabled input fields if not preconfigured provider choosen
						document.emailSetup.emailServer.disabled = false;
						document.emailSetup.emailPort.disabled = false;
						document.emailSetup.emailTLSEnabled.disabled = false;
						document.emailSetup.emailAuthEnabled.disabled = false;

						document.emailSetup.emailServer.value = "";
						document.emailSetup.emailPort.value = "25";
						document.emailSetup.emailTLSEnabled.checked = false;
						document.emailSetup.emailAuthEnabled.checked = false;
						break;
					}
				}
				
			function submit_form() {
				// enable fields before submit, else values will not be posted
				document.emailSetup.emailServer.disabled = false;
				document.emailSetup.emailPort.disabled = false;
				document.emailSetup.emailTLSEnabled.disabled = false;
				document.emailSetup.emailAuthEnabled.disabled = false;

				document.getElementById("loading").style.display = "block";
				document.emailSetup.submit();
			}
			</script>
			<div id="loading">
  				<div id="loading_picture">
					<img src="img/loader.gif" /> 
				</div>
				<div id="loading_text">
					'.translate('TEXT_PLEASE_WAIT_CONST').'
				</div>
			</div>
			<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
			<form action="index.php" method="POST" name="emailSetup">
			<input type="hidden" name="section" value="emailSetup">
			<input type="hidden" name="action" value="update">
			<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>

			<table border="0">
				<!-- Email sender configuration -->
				<tr>
					<td colspan="4" class="tablehead"><B>'.translate('TEXT_MAIl_SENDER_CONST').':</B></td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SENDER_NAME_CONST').'</B></td>
					<td><input type="text"  size="50" name="emailFromName" value="'.$emailFromName.'"></td>
					<td>'.translate('TEXT_MAIL_SENDER_NAME_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SENDER_EMAIL_CONST').'</B></td>
					<td><input type="text" size="50" name="emailFromMail" value="'.$emailFromMail.'"></td>
					<td>'.translate('TEXT_MAIL_SENDER_EMAIL_COMMENT_CONST').'</td>
				</tr>

				<tr><td colspan="4">&nbsp;</td></tr>
				
				<!-- Email server configuration (smarthost) -->
				<tr>
					<td colspan="4" class="tablehead"><B>'.translate('TEXT_MAIL_SERVER_CONST').':</B></td>
				</tr>					
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_PROVIDER_CONST').'</B></td>
					<td><select name="emailProvider" onChange="changeProvider()";>
							<option value = "custom" '.($emailProvider=='custom'?'selected':'').'>'.translate('TEXT_MAIL_PROVIDER_CUSTOM_CONST').'</option>
							<option value = "gmail" '.($emailProvider=='gmail'?'selected':'').'>'.translate('TEXT_MAIL_PROVIDER_GMAIL_CONST').'</option>
							<option value = "hotmail" '.($emailProvider=='hotmail'?'selected':'').'>'.translate('TEXT_MAIL_PROVIDER_HOTMAILMSN_CONST').'</option>
							<option value = "yahoo" '.($emailProvider=='yahoo'?'selected':'').'>'.translate('TEXT_MAIL_PROVIDER_YAHOO_CONST').'</option>
						</select>
					</td>
					<td>'.translate('TEXT_MAIL_PROVIDER_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_SERVER_CONST').'</B></td>
					<td><input type="text"  size="50" name="emailServer" value="'.$emailServer.'"></td>
					<td>'.translate('TEXT_MAIL_SMTP_SERVER_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_PORT_CONST').'</B></td>
					<td><input type="text"  size="3" name="emailPort" value="'.$emailPort.'"></td>
					<td>'.translate('TEXT_MAIL_SMTP_PORT_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_TLS_CONST').'</B></td>
					<td><input type="checkbox" name="emailTLSEnabled" value="1" '.(($emailTLSEnabled=='yes')?'checked':'').' onclick="setIPRange();"></td>
					<td>'.translate('TEXT_MAIL_SMTP_TLS_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_AUTH_CONST').'</B></td>
					<td><input type="checkbox" name="emailAuthEnabled" value="1" '.(($emailAuthEnabled=='yes')?'checked':'').' onclick="setIPRange();"></td>
					<td>'.translate('TEXT_MAIL_SMTP_AUTH_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_USERNAME_CONST').'</B></td>
					<td><input type="text" size="50" name="emailAuthUsername" value="'.$emailAuthUsername.'"></td>
					<td>'.translate('TEXT_MAIL_SMTP_USERNAME_COMMENT_CONST').'</td>
				</tr>
				<tr>
					<td>&nbsp</td>
					<td><B>'.translate('TEXT_MAIL_SMTP_PASSWORD_CONST').'</B></td>
					<td><input type="password" size="50" name="emailAuthPassword" value="'.$emailAuthPassword.'"></td>
					<td>'.translate('TEXT_MAIL_SMTP_PASSWORD_COMMENT_CONST').'</td>
				</tr>
				
				<tr><td colspan="4"><hr></td></tr>
				<tr>
					<td>&nbsp;</td>
					<td><input type="button" onClick="submit_form()" value="'.translate('TEXT_UPDATE_CONST').'" class="button"/></td>
					</form>
					<form action="index.php" method="POST" name="emailTest">
						<input type="hidden" name="section" value="emailSetup">
						<input type="hidden" name="action" value="test">
						<input type="hidden" name="emailFromName" value="'.$emailFromName.'">
						<input type="hidden" name="emailFromMail" value="'.$emailFromMail.'">
						<td align="right"><B>'.translate('TEXT_MAIL_SMTP_TEST_CONST').'</B></td>
						<td>
							<input type="text" size="50" name="emailTestAddress" value="'.$emailFromMail.'">
							&nbsp;<input type="button" onClick="document.getElementById(\'loading\').style.display = \'block\';submit();"; value="'.translate('TEXT_TEST_CONST').'" class="button"/>
						</td>
					</form>		
					
				</tr>
			</table>';
		}else{
			$out.='<h2 style="color:red;">'.$fatalError.'</h2>';
		}
	// process form after submit
	} else if ($action == 'test') { 
		$emailTestAddress=$_POST["emailTestAddress"];
		$emailFromName=$_POST["emailFromName"];
		$emailFromMail=$_POST["emailFromMail"];
		$cmd="sudo -u root /usr/pluto/bin/Test_Postfix.sh '$emailFromName' '$emailFromMail' '$emailTestAddress'";
		$response=exec_batch_command($cmd,0);

		$msg=urlencode('Response:'.$response);
		header("Location: index.php?section=emailSetup&msg=".$msg);
	} else {
		$emailFromName = $_POST["emailFromName"];
		$emailFromMail = $_POST["emailFromMail"];
		$emailProvider = $_POST["emailProvider"];
		$emailServer = $_POST["emailServer"];
		$emailPort = $_POST["emailPort"];
		$emailTLSEnabled = $_POST["emailTLSEnabled"];
		$emailAuthEnabled = $_POST["emailAuthEnabled"];
		$emailAuthUsername = $_POST["emailAuthUsername"];
		$emailAuthPassword = $_POST["emailAuthPassword"];

		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=emailSettings&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}

		// save our new settings
		$emailSettings[0]='yes';							// Email subsystem enabled
		$emailSettings[1]=$emailFromName;					// From Name
		$emailSettings[2]=$emailFromMail;					// From email address
		$emailSettings[3]=$emailProvider;					// SMTP provider
		$emailSettings[4]=$emailServer;						// SMTP smarthost
		$emailSettings[5]=$emailPort;						// SMTP port
		$emailSettings[6]=($emailTLSEnabled?'yes':'no');	// TLS enabled
		$emailSettings[7]=($emailAuthEnabled?'yes':'no');	// SMTP auth
		$emailSettings[8]=$emailAuthUsername;				// SMTP auth username
		$emailSettings[9]=$emailAuthPassword;				// SMTP auth password
		
		$token=join(',',$emailSettings);
		$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData='".$token."' WHERE FK_Device=1 AND FK_DeviceData=".$GLOBALS['EmailSettings']) 
	 		or die('ERROR: Invalid query: '.mysql_error());

		// Lookup country
		$query = "SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version WHERE PK_Installation = ?";
		$selectInstallation = $dbADO->Execute($query,array($installationID));
		if ($selectInstallation) $rowInstallation = $selectInstallation->FetchRow();			
		$selectedCountry=(isset($_POST['countryID']))?(int)$_POST['countryID']:$rowInstallation['FK_Country'];	
		$query="SELECT Country.country_code FROM Country WHERE PK_Country=$selectedCountry";
		$selectedCountry=$dbADO->Execute($query);
		list($country_code, $selectedCountry) = explode("country_code", $selectedCountry);
		$selectedCountry=trim($selectedCountry);

		// Lookup the state
		$state = ((isset($_REQUEST['State']))?$_REQUEST['State']:$rowInstallation['State']);
	
		// set certificate organisation
		$org = 'LinuxMCE';
		
		// Call our script to setup postfix using our settings
		$cmd="sudo -u root /usr/pluto/bin/Configure_Postfix_interactive.sh '$emailFromName' '$emailFromMail' '$emailAuthUsername' '$emailAuthPassword' '$emailServer' '$emailPort' '".($emailTLSEnabled?'yes':'no')."' '$org' '$state' '$selectedCountry' '$emailProvider'";
		$response=exec_batch_command($cmd,0);
		$msg=urlencode("Email settings updated.");	
		header("Location: index.php?section=emailSetup&msg=".$msg);

	}


	// Setup our page environment
	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_EMAILSETUP_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_EMAILSETUP_CONST')=>"index.php?section=emailSetup"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_EMAILSETUP_CONST'));
	$output->output();
}
?>

<?php
function dynamicDNS($output, $dbADO) {

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/dynamicDNS.lang.php');
	$installationID = (int)@$_SESSION['installationID'];
	
	$DynamicDnsProviders=array(
		"- None -"				=> "",
		"DynDNS (dynamic)"			=> "protocol=dyndns2,server=members.dyndns.org",
		"DynDNS (static)"			=> "protocol=dyndns2,server=members.dyndns.org,static=yes",
		"DynDNS (custom)"			=> "protocol=dyndns2,server=members.dyndns.org,custom=yes",
		"ZoneEdit (zoneedit.com)"		=> "protocol=zoneedit1,server=www.zoneedit.com",
		"EasyDNS (easydns.com)"			=> "protocol=easydns,server=members.easydns.com",
		"OrgDNS.org account-configuration"	=> "protocol=dyndns2,server=www.orgdns.org,use=web,web=members.orgdns.org/nic/ip",
		"DnsPark.com"				=> "protocol=dnspark,use=web,web=ipdetect.dnspark.com,web-skip='Current Address:'",
		"NameCheap (namecheap.com)"		=> "protocol=namecheap,server=dynamicdns.park-your-domain.com",
		"No-IP (noip.com)"		=> "protocol=noip,server=dynupdate.no-ip.com/nic/update"
	);

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	if ($action == 'form') {
		
		exec('sudo -u root /usr/pluto/bin/DynamicDNS.sh get', $retArray);
		$ddLogin = $retArray[0];
		$ddProvider = $retArray[1];
		$ddUse = $retArray[2];
		$ddDomains = $retArray[3];

		$ddProvider_options='';
		foreach ( $DynamicDnsProviders as $providerName => $providerString ) {
			$selected=$ddProvider==$providerString?'selected':'';
			$ddProvider_options .= "<option value='{$providerString}' {$selected} >{$providerName}</option>\n";
		}
		
		$out='
		<form action="index.php" method="POST" name="dynamicDNS">
			<input type="hidden" name="section" value="dynamicDNS">
			<input type="hidden" name="action" value="add">
			<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
			<table border="0">
				<tr>
					<td colspan="2" class="tablehead">
						<b> '.$TEXT_DYNAMIC_DNS_CONST.' </b>
					</td>
				</tr>
				<tr>
					<td width="150"><b>'.$TEXT_DYNAMIC_DNS_PROVIDER_CONST.'</b></td>
					<td>
					<select name="ddProvider" STYLE="width:250px">
						'.$ddProvider_options.'
					</select>
					</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_DYNAMIC_DNS_DOMAINS_CONST.'</b></td>
					<td>
					<input type="text" name="ddDomains" value="'.$ddDomains.'" STYLE="width:250px">
					</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_DYNAMIC_DNS_LOGIN_CONST.'</b></td>
					<td>
					<input type="text" name="ddLogin" value="'.$ddLogin.'" STYLE="width:250px">
					</td>
				</tr>
				<tr>
					<td><b>'.$TEXT_DYNAMIC_DNS_PASSWORD_CONST.'</b></td>
					<td>
					<input type="password" name="ddPassword" STYLE="width:250px">
					</td>
				</tr>
				<tr>
					<td><b>&nbsp;</b></td>
					<td>
						<input type="checkbox" name="ddUse" '.($ddUse=="web"?'checked':'').'>'.$TEXT_DYNAMIC_DNS_USE_CONST.'<font color="FF0000"><b>*</b></font>
					</td>
				</tr>
				<tr>
					<td>&nbsp;</td>
					<td>
					<input type="submit" class="button" name="Save" value="Save">
					</td>
				</tr>
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2">'.$TEXT_DYNAMIC_DNS_NOTE_CONST.'</td>
				</tr>
			</table>
		</form>
		';

	} else {
                // check if the user has the right to modify installation
                $canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
                if (!$canModifyInstallation){
                        header("Location: index.php?section=networkSettings&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
                        exit(0);
                }
																
		$ddLogin=$_POST['ddLogin'];
		$ddProvider=$_POST['ddProvider'];
		$ddPassword=$_POST['ddPassword'];
		$ddDomains=$_POST['ddDomains'];
		$ddUse=$_POST['ddUse'];
		$ddUse=$ddUse?'web':'if';

		exec("sudo -u root /usr/pluto/bin/DynamicDNS.sh set '{$ddLogin}' '{$ddPassword}' '{$ddProvider}' '{$ddUse}' '{$ddDomains}' ");

		header("Location: index.php?section=dynamicDNS&msg={$TEXT_DYNAMIC_DNS_UPDATED_CONST}");
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_DYNAMIC_DNS_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_DYNAMIC_DNS_CONST=>'index.php?section=dynamicDNS'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DYNAMIC_DNS_CONST);
	$output->output();
								
}	
?>

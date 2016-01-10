<?
function proxySettings($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('proxy.lang.php');
	
$out='';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$accessFile=$GLOBALS['pluto.conf'];

	// check if proxy is disabled
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	if(count($retArray)==0){
		$_GET['error'].='Insuffient rights: pluto.conf file cannot be opened.';
	}
	
	$res=$dbADO->Execute('SELECT var,val FROM webfilter_proxy WHERE conf="proxy" AND conf_type="global"');
	$row=$res->FetchRow();

if ($action == 'form') {
$out.='
	<script>
	function confirmDisableproxy()
	{
		if(document.proxy.change_proxy_status.checked){
			if(confirm("'.translate('TEXT_DISABLE_PROXY_CONFIRMATION_CONST').'")){
				document.proxy.submit();
			}else{
			document.proxy.change_proxy_status.checked=false;
			}
		}else{
			document.proxy.submit();
		}
		</script>
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="POST" name="proxy">
	<input type="hidden" name="section" value="proxy">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
<table border="0" align="center">
		<tr>
			<td colspan="7" align="center"><h3 class="err">'.((@$DisableProxy!=1)?'':'- '.translate('TEXT_PROXY_DISABLED_CONST').' - ').'</h3></td>
		</tr>
		<tr>
			<td colspan="7" align="center"><input type="checkbox" name="change_proxy_status" value="1" '.((@$DisableProxy!=1)?'':'checked').' onClick="confirmDisableproxy()"> '.translate('TEXT_PROXY_DISABLED_CONST').'</td>
		</tr>
		<tr>
		<td>Port and networking options</td>
		</tr>
		<tr>
		<td>proxy address and ports:</td>
		<td><input type="radio" name"default" />default <input type="radio" name="manual" />listed below...</td>
		</tr>
		<tr>
		<td>
		<table>
		<tr>
		<td>Port</td><td>ip/hostname</td><td>address</td><td>options for port</td>
		</tr>
		<tr>
		<td><input type="text" name="port_1" value="3128"></td><td><input type="radio" name="all_1" />All<input type="radio" /></td><td><input type="text" name="address_1" /></td><td><input type="text" name="port_options_1" /></td>
		</tr>
		<tr>
		<td><input type="text" name="port_2" value=""></td><td><input type="radio" name="all_1" />All<input type="radio" /></td><td><input type="text" name="address_2" /></td><td><input type="text" name="port_options_2" /></td>
		</tr>
		</table>
		</td>
		</tr>
		<tr>
		<td>SSL Address and ports</td>
		<td><input type="radio" name"ssl_default" />default <input type="radio" name="ssl_manual" />listed below...</td>
		</tr>
		<tr>
		<td>
		<table>
		<tr>
		<td>Port</td><td>ip/hostname</td><td>address</td><td>options for port</td>
		</tr>
		<tr>
		<td><input type="text" name="ssl_port" value="3128"></td><td><input type="radio" name="ssl_all" />All<input type="radio" /></td><td><input type="text" name="ssl_address" /></td><td><input type="text" name="ssl_port_options" /></td>
		</tr>
		<tr>
		<td>ICP port:</td>
		<td><input type="radio" name="icp_default" />default <input type="radio" name="icp_manual" /></td>
		<td><input type="text" name="icp_port" value="" /></td>
		<td>Outgoing TCP address</td>
		<td><input type="radio" name="icp_default" />default <input type="radio" name="icp_manual" /><input type="text" name="icp_out_port" value="" /></td>
		</tr>
		<td>Outgoing UDP address:</td>
		<td><input type="radio" name="out_udp_any" />Any <input type="radio" name="out_udp_manual" /></td>
		<td><input type="text" name="out_udp" value="" /></td>
		<td>Incomming UDP address</td>
		<td><input type="radio" name="in_udp_any" />Any <input type="radio" name="in_udp_manual" /><input type="text" name="icp_in_port" value="" /></td>
		</tr>
		<tr>
		<td>Multicast Groups</td>
		<td><input type="textarea" name="multicast_groups" /></td>
		<td>TCP reseive buffer</td>
		<td><input type="text" name="tcp_buff" value="" /></td>
		</tr>
		<tr>
		<td>Validate hostnames in URL\'s?</td>
		<td><input type="radio" name="val_hostname" />Yes <input type="radio" name="val_hostname" />No</td>
		<td>Allow underscores in hostnames?</td>
		<td><input type="radio" name="underscore_hostname" />Yes <input type="radio" name="underscore_hostname" />No</td>
		</tr>
		<tr>
		<td>Do unclean SSL shutdowns?</td>
		<td><input type="radio" name="unclean_ssl" />Yes <input type="radio" name="unclean_ssl" />No</td>
		</tr>
		</table>
		</td>
		</tr>
		<tr>
		<td>
		<a href="index.php?section=proxy_acl&action=list">proxy groepen (ACL)</a>
		#<br />
# Add this to the bottom of the ACL section of squid.conf<br />
#<br />
acl home_network src 192.168.1.0/24<br />
acl business_hours time M T W H F 9:00-17:00<br />
acl RestrictedHost src 192.168.1.23<br />
<br />
#<br />
# Add this at the top of the http_access section of squid.conf<br />
#<br />
http_access deny RestrictedHost<br />
http_access allow home_network business_hours<br />
<br />
Or, you can allow morning access only:<br />
<br />
#<br />
# Add this to the bottom of the ACL section of squid.conf<br />
#<br />
acl mornings time 08:00-12:00<br />
<br />
#<br />
# Add this at the top of the http_access section of squid.conf<br />
#<br />
http_access allow mornings<br />
		</td>
		</tr>
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>
		</table>
		</form>';
} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=proxy&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		if(isset($_POST['add'])){
			/*$Protocol=@$_POST['protocol'].'-'.@$_POST['IPVersion'];
			$SourcePort=@$_POST['SourcePort'];
			$SourcePortEnd=@$_POST['SourcePortEnd'];
			$DestinationPort=isset($_POST['DestinationPort'])?$_POST['DestinationPort']:0;
			$DestinationIP=isset($_POST['DestinationIP'])?$_POST['DestinationIP']:0;
			$RuleType=@$_POST['RuleType'];
			$SourceIP=@$_POST['SourceIP'];
			
			$insert='INSERT INTO webfilter_proxy (conf, conf_type, var, val) VALUES (?,?,?,?)';
			$dbADO->Execute($insert,array($Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$RuleType,$SourceIP));*/
		}

		if(isset($_REQUEST['delid'])){
			/*$delid=$_REQUEST['delid'];
			$dbADO->Execute('DELETE FROM Proxy WHERE PK_Firewall=?',$delid);*/
		}else{
			if(@$_REQUEST['change_proxy_status']==1) 
				writeConf($accessFile, 'DisableProxy',@$DisableProxy,1);
				/*writeProxyConf('Disabled');*/
			else 
				writeConf($accessFile, 'DisableProxy',@$DisableProxy,0);
				/*writeProxyConf('Enabled');*/
		}
		
		//exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
		
		header("Location: index.php?section=proxy&msg=".translate('TEXT_PROXY_RULES_UPDATED_CONST'));
}
$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_PROXY_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_PROXY_RULES_CONST')=>'index.php?section=proxy'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_PROXY_RULES_CONST'));
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=proxy&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('proxy.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=proxy&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
		exit();
	}
	$oldFile=implode('',$oldFileArray);
	$stringToReplace=$variable.'='.$oldValue;
	if(ereg($stringToReplace,$oldFile)){
		$newFile=str_replace($stringToReplace,$variable.'='.$newValue,$oldFile);
	}
	else
		$newFile=$oldFile.$variable.'='.$newValue."\n";
	if(!is_writable($accessFile)){
		header("Location: index.php?section=proxy&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=proxy&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	fclose($handle);
}

/*writeProxyConf($Status) {
$accessFile='/etc/squid/squid.conf';
if($Status=='Disabled') {

} else {
cache_access_log /var/log/squid/access.log
cache_log /var/log/squid/cache.log
http_port 3128 transparent
http_port 192.168.81.1:80 vhost
visible_hostname DCERouter
acl lan src 192.168.81.0/24
acl localhost src 127.0.0.1
acl all src 0.0.0.0/0.0.0.0
http_access allow localhost
http_access allow lan
http_access deny all
#acl nocache src 192.168.80.1
cache deny nocache
#acl

}

if(!is_writable($accessFile)){
		header("Location: index.php?section=proxy&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=proxy&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	fclose($handle);

}*/
?>
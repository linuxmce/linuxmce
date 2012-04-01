<?
function firewall($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('firewall.lang.php');
	
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$accessFile=$GLOBALS['pluto.conf'];

	// check if firewall is disabled
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	if(count($retArray)==0){
		$_GET['error'].='Insuffient rights: pluto.conf file cannot be opened.';
	}
	
	if ($action == 'form') {

		$out.='
	<script>
	function enableDestination()
	{
		if(document.firewall.RuleType.value=="port_forward"){
			document.firewall.DestinationPort.disabled=false;
			document.firewall.DestinationIP.disabled=false;
		}else{
			document.firewall.DestinationPort.disabled=true;
			document.firewall.DestinationIP.disabled=true;
		}
	}
		
	function confirmDisableFirewall(ver)
	{
		if((document.firewall.change_ipv4_firewall_status.checked && ver=="ipv4")
			|| (document.firewall.change_ipv6_firewall_status.checked && ver=="ipv6")){
			if(confirm("("+ver+") '.translate('TEXT_DISABLE_FIREWALL_CONFIRMATION_CONST').'")){
				document.firewall.submit();
			}else{
				if(ver=="ipv4") document.firewall.change_ipv4_firewall_status.checked=false;
				else if(ver=="ipv6") document.firewall.change_ipv6_firewall_status.checked=false;
			}
		}else{
			document.firewall.submit();
		}
	}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="firewall">
	<input type="hidden" name="section" value="firewall">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		
	<table border="0" align="center">
		<tr>
			<td colspan="7" align="center"><h3 class="err">'.((@$DisableFirewall!=1)?'':'- '.translate('TEXT_FIREWALL_DISABLED_WARNING_CONST').' - ')
					.((@$DisableIPv6Firewall!=1)?'':' - '.translate('TEXT_IPV6_FIREWALL_DISABLED_WARNING_CONST').' -').'</h3></td>
		</tr>
		<tr>
			<td colspan="7" align="center"><input type="checkbox" name="change_ipv4_firewall_status" value="1" '.((@$DisableFirewall!=1)?'':'checked').' onClick="confirmDisableFirewall(\'ipv4\');"> IPv4 '.translate('TEXT_FIREWALL_DISABLED_CONST').'    <input type="checkbox" name="change_ipv6_firewall_status" value="1" '.((@$DisableIPv6Firewall!=1)?'':'checked').' onClick="confirmDisableFirewall(\'ipv6\');"> IPv6 '.translate('TEXT_FIREWALL_DISABLED_CONST').'</td>
		</tr>
		<tr class="tablehead">
			<td align="center"><B>'.translate('TEXT_PROTOCOL_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_IPVERSION_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_SOURCE_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_IP_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_RULE_TYPE_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_LIMIT_TO_IP_CONST').'</B></td>
			<td>&nbsp;</td>
		</tr>';
		$res=$dbADO->Execute('SELECT * FROM Firewall ORDER BY SourcePort, Protocol');
		$pos=0;
		while($row=$res->FetchRow()){
			++$pos;
			$protocol=explode("-",$row['Protocol']);
			$out.='
				<tr class="'.(($pos%2==0)?'alternate_back':'').'">
					<td align="center">'.$protocol[0].'</td>
					<td align="center">'.$protocol[1].'</td>
					<td align="center">'.($protocol[0] == 'ip' ? 'protocol: ' : '').
					$row['SourcePort'].($row['SourcePortEnd'] > 0 ? ' to '.$row['SourcePortEnd'] : '').'</B></td>
					<td align="center">'.($row['DestinationPort'] > 0 ? $row['DestinationPort']:'').'</td>
					<td align="center">'.($row['DestinationIP'] > 0 ? $row['DestinationIP']:'').'</td>
					<td align="center">'.$row['RuleType'].'</td>
					<td align="center">'.$row['SourceIP'].'</td>
					<td align="center"><a href="index.php?section=firewall&action=del&delid='.$row['PK_Firewall'].'">'.translate('TEXT_DELETE_CONST').'</a></td>
				</tr>';
		}
		$out.='
		<tr>
			<td colspan="7" align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><B>'.translate('TEXT_ADD_NEW_FIREWALL_RULE_CONST').'</B></td>
		</tr>
		<tr class="tablehead">
			<td align="center"><B>'.translate('TEXT_PROTOCOL_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_IPVERSION_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_SOURCE_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_IP_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_RULE_TYPE_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_LIMIT_TO_IP_CONST').'*</B></td>
			<td>&nbsp;</td>
		</tr>				
		<tr>
			<td align="center"><select name="protocol" STYLE="width:70px">
				<option value="tcp">tcp</option>
				<option value="udp">udp</option>
				<option value="ip">ip</option>
			</select></td>
			<td align="center"><select name="IPVersion" STYLE="width:70px">
				<option value="ipv4">IPv4</option>
				<option value="ipv6">IPv6</option>
				<option value="both">both</option>
			</select></td>
			<td align="center"><input type="text" name="SourcePort" size="2"> to <input type="text" name="SourcePortEnd" size="2"></td>
			<td align="center"><input type="text" name="DestinationPort" size="2" disabled></td>
			<td align="center"><input type="text" name="DestinationIP" size="8" disabled></td>
			<td align="center"><select name="RuleType" onChange="enableDestination()">
				<option value="core_input">core_input</option>
				<option value="port_forward">port_forward</option>
			</select></td>
			<td align="center"><input type="text" name="SourceIP" size="8"></td>
			<td align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>		
		<tr>
			<td colspan="7" align="left">* '.translate('TEXT_OPTIONAL_FIELD_CONST').'</td>
		</tr>		
	</table>	
	</form>
		<script>
		 	var frmvalidator = new formValidator("firewall");
// 			frmvalidator.addValidation("internalCoreIP_1","req","'.translate('TEXT_IP_REQUIRED_CONST').'");
		</script>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=firewall&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		if(isset($_POST['add'])){
			$Protocol=@$_POST['protocol'].'-'.@$_POST['IPVersion'];
			$SourcePort=@$_POST['SourcePort'];
			$SourcePortEnd=@$_POST['SourcePortEnd'];
			$DestinationPort=isset($_POST['DestinationPort'])?$_POST['DestinationPort']:0;
			$DestinationIP=isset($_POST['DestinationIP'])?$_POST['DestinationIP']:0;
			$RuleType=@$_POST['RuleType'];
			$SourceIP=@$_POST['SourceIP'];
			
			$insert='INSERT INTO Firewall (Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP) VALUES (?,?,?,?,?,?,?)';
			$dbADO->Execute($insert,array($Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$RuleType,$SourceIP));
		}

		if(isset($_REQUEST['delid'])){
			$delid=$_REQUEST['delid'];
			$dbADO->Execute('DELETE FROM Firewall WHERE PK_Firewall=?',$delid);
		}else{
			if(@$_REQUEST['change_ipv4_firewall_status']==1) 
				writeConf($accessFile, 'DisableFirewall',@$DisableFirewall,1);
			else 
				writeConf($accessFile, 'DisableFirewall',@$DisableFirewall,0);
			
			if(@$_REQUEST['change_ipv6_firewall_status']==1) 
				writeConf($accessFile, 'DisableIPv6Firewall',@$DisableIPv6Firewall,1);
			else 
				writeConf($accessFile, 'DisableIPv6Firewall',@$DisableIPv6Firewall,0);

		}
				
		exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
		
		header("Location: index.php?section=firewall&msg=".translate('TEXT_FIREWALL_RULES_UPDATED_CONST'));
	}

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_FIREWALL_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_FIREWALL_RULES_CONST')=>'index.php?section=firewall'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_FIREWALL_RULES_CONST'));
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=firewall&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('firewall.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
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
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>

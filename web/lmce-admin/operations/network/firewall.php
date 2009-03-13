<?
function firewall($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/firewall.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
//	$dbADO->debug=true;
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
		
	function confirmDisableFirewall()
	{
		if(document.firewall.change_firewall_status.checked){
			if(confirm("'.$TEXT_DISABLE_FIREWALL_CONFIRMATION_CONST.'")){
				document.firewall.submit();
			}else{
				document.firewall.change_firewall_status.checked=false;
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
			<td colspan="7" align="center">'.((@$DisableFirewall!=1)?'':'<h3 class="err">'.$TEXT_FIREWALL_DISABLED_WARNING_CONST.'</h3>').'</td>
		</tr>
		<tr>
			<td colspan="7" align="center"><input type="checkbox" name="change_firewall_status" value="1" '.((@$DisableFirewall!=1)?'':'checked').' onClick="confirmDisableFirewall();"> '.$TEXT_FIREWALL_DISABLED_CONST.'</td>
		</tr>
		<tr bgcolor="#EEEEEE">
			<td align="center"><B>'.$TEXT_PROTOCOL_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_SOURCE_PORT_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DESTINATION_PORT_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DESTINATION_IP_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_RULE_TYPE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_LIMIT_TO_IP_CONST.'</B></td>
			<td>&nbsp;</td>
		</tr>';
		$res=$dbADO->Execute('SELECT * FROM Firewall');
		while($row=$res->FetchRow()){
			$out.='
				<tr>
					<td align="center">'.$row['Protocol'].'</td>
					<td align="center">'.$row['SourcePort'].' to '.$row['SourcePortEnd'].'</B></td>
					<td align="center">'.$row['DestinationPort'].'</td>
					<td align="center">'.$row['DestinationIP'].'</td>
					<td align="center">'.$row['RuleType'].'</td>
					<td align="center">'.$row['SourceIP'].'</td>
					<td align="center"><a href="index.php?section=firewall&action=del&delid='.$row['PK_Firewall'].'">'.$TEXT_DELETE_CONST.'</a></td>
				</tr>';
		}
		$out.='
		<tr>
			<td colspan="7" align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><B>'.$TEXT_ADD_NEW_FIREWALL_RULE_CONST.'</B></td>
		</tr>
		<tr bgcolor="#EEEEEE">
			<td align="center"><B>'.$TEXT_PROTOCOL_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_SOURCE_PORT_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DESTINATION_PORT_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DESTINATION_IP_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_RULE_TYPE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_LIMIT_TO_IP_CONST.'*</B></td>
			<td>&nbsp;</td>
		</tr>				
		<tr>
			<td align="center"><select name="protocol">
				<option value="tcp">tcp</option>
				<option value="udp">udp</option>
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
			<td colspan="7" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>		
		<tr>
			<td colspan="7" align="left">* '.$TEXT_OPTIONAL_FIELD_CONST.'</td>
		</tr>		
	</table>	
	</form>
		<script>
		 	var frmvalidator = new formValidator("firewall");
// 			frmvalidator.addValidation("internalCoreIP_1","req","'.$TEXT_IP_REQUIRED_CONST.'");
		</script>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=firewall&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		if(isset($_POST['add'])){
			$Protocol=@$_POST['protocol'];
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
			if(@$_REQUEST['change_firewall_status']==1){
				writeConf($accessFile, 'DisableFirewall',@$DisableFirewall,1);
			}else{
				writeConf($accessFile, 'DisableFirewall',@$DisableFirewall,0);
			}
		}
				
		exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
		
		header("Location: index.php?section=firewall&msg=$TEXT_FIREWALL_RULES_UPDATED_CONST");
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_FIREWALL_RULES_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_FIREWALL_RULES_CONST=>'index.php?section=firewall'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_FIREWALL_RULES_CONST);
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/firewall.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=firewall&error=$TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST $accessFile");
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
		header("Location: index.php?section=firewall&error=$TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=firewall&error=$TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>

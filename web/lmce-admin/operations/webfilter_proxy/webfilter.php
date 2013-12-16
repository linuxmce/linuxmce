<?
function webfilterSettings($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');
	
	
$out='';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$accessFile=$GLOBALS['pluto.conf'];

	// check if webfilter is disabled
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
	function confirmDisablewebfilter()
	{
		if(document.webfilter.change_webfilter_status.checked){
			if(confirm("'.translate('TEXT_DISABLE_WEBFILTER_CONFIRMATION_CONST').'")){
				document.webfilter.submit();
			}else{
			document.webfilter.change_webfilter_status.checked=false;
			}
		}else{
			document.webfilter.submit();
		}
		</script>
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="POST" name="webfilter">
	<input type="hidden" name="section" value="webfilter">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
<table border="0" align="center">
		<tr>
			<td colspan="7" align="center"><h3 class="err">'.((@$DisableWebfilter!=1)?'':'- '.translate('TEXT_WEBFILTER_DISABLED_CONST').' - ').'</h3></td>
		</tr>
		<tr>
			<td colspan="7" align="center"><input type="checkbox" name="change_webfilter_status" value="1" '.((@$DisableWebfilter!=1)?'':'checked').' onClick="confirmDisablewebfilter()"> '.translate('TEXT_WEBFILTER_DISABLED_CONST').'</td>
		</tr>
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>
		</table>';
} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=webfilter&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
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
			
			$insert='INSERT INTO Proxy (Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP) VALUES (?,?,?,?,?,?,?)';
			$dbADO->Execute($insert,array($Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$RuleType,$SourceIP));*/
		}

		if(isset($_REQUEST['delid'])){
			/*$delid=$_REQUEST['delid'];
			$dbADO->Execute('DELETE FROM Proxy WHERE PK_Firewall=?',$delid);*/
		}else{
			if(@$_REQUEST['change_webfilter_status']==1) 
				writeConf($accessFile, 'DisableWebfilter',@$DisableWebfilter,1);
				/*writeProxyConf('Disabled');*/
			else 
				writeConf($accessFile, 'DisableWebfilter',@$DisableWebfilter,0);
				/*writeProxyConf('Enabled');*/
		}
		
		exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
		
		header("Location: index.php?section=webfilter&msg=".translate('TEXT_PROXY_WEBFILTER_UPDATED_CONST'));
}
	
$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_WEBFILTER_RULES_CONST')=>'index.php?section=webfilter'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=webfilter&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')."nr:1 ".$accessFile);
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
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')."nr:1 ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>
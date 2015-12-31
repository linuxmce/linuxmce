<?PHP
function proxyAcl($output,$dbADO) {
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
	$out.='<table border="0" align="center">
	<tr>
	<td>test</td>
	</tr>
	</table>';
}elseif ($action == 'list') {
	$out.='<table border="0" align="center">
	<tr>
	<td>list</td>
	</tr>
	</table>';
} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=proxy&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
}
$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_PROXY_ACL_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_PROXY_RULES_CONST')=>'index.php?section=proxy',translate('TEXT_PROXY_ACL_CONST')=>'index.php?section=proxy'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_PROXY_ACL_CONST'));
	$output->output();
}
?>
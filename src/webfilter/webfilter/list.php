<?PHP
function webfilterblacklist($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');
	
	$out='UPCOMMING TODO!!!!';
	if (!isset($_GET['list'])) {
	$out='<table>
		<tr>
		<td>Lists</td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=bannedphraselist&group='.$_GET['group'].'">bannedphraselist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=weightedphraselist&group='.$_GET['group'].'">weightedphraselist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=exceptionphraselist&group='.$_GET['group'].'">exceptionphraselist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=bannedsitelist&group='.$_GET['group'].'">bannedsitelist</a></td>
		</tr>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=greysitelist&group='.$_GET['group'].'">greysitelist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=exceptionsitelist&group='.$_GET['group'].'">exceptionsitelist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=bannedurllist&group='.$_GET['group'].'">bannedurllist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=greyurllist&group='.$_GET['group'].'">greyurllist</a></td>
		</tr>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=exceptionurllist&group='.$_GET['group'].'">exceptionurllist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=exceptionregexpurllist&group='.$_GET['group'].'">exceptionregexpurllist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=bannedregexpurllist&group='.$_GET['group'].'">bannedregexpurllist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=picsfile&group='.$_GET['group'].'">picsfile</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=contentregexplist&group='.$_GET['group'].'">contentregexplist</a></td>
		</tr>
		<tr>
		<td><a href="index.php?section=webfilter_list&list=urlregexplist&group='.$_GET['group'].'">urlregexplist</a></td>
		</tr>
		</table>';
	} else {
	}
	/*
	if (isset($_POST['action'])) {
	// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=webfilter&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		if ($newgroups == 2) {
			$dbADO->Execute('INSERT INTO webfilter_proxy (conf, conf_type, var, val) VALUES ("webfilter","'.$list.'f'.$newgroups.'","","/etc/dansguardian/lists/f'.$newgroups.'/exceptionsitelistf'.$newgroups.'")');
		} else {
			$dbADO->Execute('INSERT INTO webfilter_proxy (conf, conf_type, var, val) VALUES ("webfilter","'.$list.'f'.$newgroups.'","","/etc/dansguardian/lists/f'.$oldgroups.'/exceptionsitelistf'.$oldgroups.'")');
			$dbADO->Execute('INSERT INTO webfilter_proxy (conf, conf_type, var, val) VALUES ("webfilter","'.$list.'f'.$newgroups.'","","/etc/dansguardian/lists/f'.$newgroups.'/exceptionsitelistf'.$oldgroups.'")');
			exec_batch_command('sudo -u root /usr/pluto/bin/webfilter_proxy.sh global');
			exec_batch_command('sudo -u root dansguardian -g');
		}
	
	}*/
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_WEBFILTER_RULES_CONST')=>'index.php?section=webfilter'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->output();
}
?>
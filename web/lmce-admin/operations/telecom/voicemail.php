<?
function voicemail($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

	if(isset($_REQUEST['general'])){
		if($_SESSION['AccessGeneralMailbox']==0){
			$error=$TEXT_NO_ACCESS_TO_GENERAL_MAILBOX_CONST;
		}else{
			$_SESSION['Extension']=100;
		}
	}else{
		$_SESSION['Extension']=$_SESSION['MyExtension'];
	}

	exec_batch_command('sudo -u root /usr/pluto/bin/Asterisk_FixVoicemailRights.sh');
	
	if(isset($error)){
		$out.='<span class="err">'.$TEXT_NO_ACCESS_TO_GENERAL_MAILBOX_CONST.'</span>';
	}else{
		$out.=setLeftMenu($dbADO).'<iframe src="/recordings/index.php?s=voicemail" style="width:98%;height:600px;"></iframe>';
	}

	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($TEXT_VOICEMAIL_CONST);
	$output->setNavigationMenu(array($TEXT_VOICEMAIL_CONST=>'index.php?section=voicemail'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_VOICEMAIL_CONST);
	$output->output();
}
?>

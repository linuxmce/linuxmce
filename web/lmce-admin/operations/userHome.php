<?php
function userHome($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userHome.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

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

	$scriptInHead='
<script>
	'.($reloadTree===true?'
	if (parent.frames[\'treeframe\']) {
		top.frames[\'treeframe\'].location=\'index.php?section=wizard\';
	}':'').'
</script>
';
	$content=(isset($_REQUEST['voicemail']))?'<iframe src="amp/recordings/index.php?s=voicemail" style="width:98%;height:85%"></iframe>':$TEXT_USER_HOME_TEXT_CONST;
	
	if($weborbiterInstalled) {
		$out.=$TEXT_USER_HOME_WEBORBITER;
	}
	$out.=$content;
	
	$output->setMenuTitle($TEXT_WELCOME_MESSAGE_CONST);
	$output->setHelpSrc('/wiki/index.php/LinuxMCE_Admin_Website');
	$output->setBody($out);
	$output->setScriptInHead($scriptInHead);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_USER_HOME_CONST);
	$output->output();
}
?>

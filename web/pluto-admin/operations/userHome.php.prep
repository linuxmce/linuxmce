<?
function userHome($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userHome.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

	$scriptInHead='
<script>
	'.($reloadTree===true?'
	if (parent.frames[\'treeframe\']) {
		top.frames[\'treeframe\'].location=\'index.php?section=wizard\';
	}':'').'
</script>
';
	$content=(isset($_REQUEST['voicemail']))?'<iframe src="amp/recordings/index.php?s=voicemail" style="width:98%;height:85%"></iframe>':$TEXT_USER_HOME_TEXT_CONST;
	
	$out.=$content;

	$output->setMenuTitle($TEXT_WELCOME_MESSAGE_CONST);
	$output->setHelpSrc('/wiki/index.php/<-mkr_t_name_mixed->_Admin_Website');
	$output->setBody($out);
	$output->setScriptInHead($scriptInHead);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_USER_HOME_CONST);
	$output->output();
}
?>
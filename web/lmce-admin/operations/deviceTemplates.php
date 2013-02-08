<?
function deviceTemplates($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$out='';
	$dbADO->debug=false;
	unset($_SESSION['parentID']);
	
	$firstColLinks='';

	$_SESSION['from']='deviceTemplates';

	$_SESSION['categoryID']=(isset($_SESSION['categoryID']))?$_SESSION['categoryID']:NULL;
	$_SESSION['categoryID']=(isset($_REQUEST['categoryID']))?cleanInteger($_REQUEST['categoryID']):$_SESSION['categoryID'];
	$_SESSION['categoryID']=($_SESSION['categoryID']==0)?NULL:$_SESSION['categoryID'];
	
	$mID=(!isset($_REQUEST['manufacturerID']))?1:(int)$_REQUEST['manufacturerID'];
	
	$out.=pickDeviceTemplate($_SESSION['categoryID'],$mID,0,1,'deviceTemplates',$dbADO,1);
	
	$output->setNavigationMenu(array("Device Templates"=>'index.php?section=deviceTemplates'));
	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_DEVICE_TEMPLATES_CONST);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DEVICE_TEMPLATES_CONST);			
	$output->output();  		
}
?>
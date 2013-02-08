<?
function addModel($output,$dbADO,$mediadbADO)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addModel.lang.php');

	/* @var $dbADO ADOConnection */
	
	$deviceID=(int)@$_REQUEST['deviceID'];
	$userID=(int)@$_SESSION['userID'];
	$from=@$_REQUEST['from'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$step=$_REQUEST['step'];
	
	$publicADO=$dbADO;

	$mID=@$_REQUEST['mID'];
	$dcID=@$_REQUEST['dcID'];

	$avDeviceCategories=getDescendantsForCategory($GLOBALS['rootAVEquipment'],$dbADO);
	
	if(isset($_REQUEST['dcID']) && !in_array($dcID,$avDeviceCategories)){
		include('operations/addBasicModel.php');
	}else{
		if(file_exists('operations/infrared/addModel_step'.$step.'.php'))
			include('operations/infrared/addModel_step'.$step.'.php');
		else
			Header('Location: index.php');
	}		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_DEVICE_TEMPLATE_CONST);
	$output->output();
}
?>
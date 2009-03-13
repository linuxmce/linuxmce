<?
function deviceTemplatePicker($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deviceTemplatePicker.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$dbADO->debug=false;
	
	
	$_SESSION['from']=(isset($_REQUEST['from']))?cleanString(@$_REQUEST['from']):@$_SESSION['from'];

	$_SESSION['categoryID']=(isset($_SESSION['categoryID']))?$_SESSION['categoryID']:NULL;
	$_SESSION['categoryID']=(isset($_REQUEST['categoryID']))?cleanInteger($_REQUEST['categoryID']):$_SESSION['categoryID'];
	$_SESSION['categoryID']=(isset($_REQUEST['categoryID']))?cleanInteger($_REQUEST['categoryID']):$_SESSION['categoryID'];
	$_SESSION['categoryID']=($_SESSION['categoryID']==0)?NULL:$_SESSION['categoryID'];
	$allowAddDT=((int)@$_REQUEST['allowAdd']==1)?1:0;
	
	$_SESSION['parentID']=(isset($_REQUEST['parentID']))?(int)$_REQUEST['parentID']:@$_SESSION['parentID'];
	
	$genericSerialDevices=(isset($_REQUEST['from']) && $_REQUEST['from']=='genericSerialDevices')?1:0;
	
	$out='<br>
		<h3>'.$TEXT_PICK_DEVICE_TEMPLATE_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>';

	// new
	$out.=pickDeviceTemplate($_SESSION['categoryID'],(int)@$_REQUEST['manufacturerID'],1,1,'deviceTemplatePicker',$dbADO,0);
	//$out.=pickDeviceTemplate($_SESSION['categoryID'],1,0,$allowAddDT,1,1,'deviceTemplatePicker','',$dbADO,0);
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PICK_DEVICE_TEMPLATE_CONST);			
	$output->output();  		
}
?>
<?
function displayCode($output,$publicADO)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/displayCode.lang.php');

	
	/* @var $dbADO ADOConnection */
	global $LinuxMCEHost;
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='<p><b>'.$TEXT_IRDATA_FOR_INFRARED_CODE_CONST.'</b><br>';
	$irgcID=(int)$_REQUEST['irgcID'];

	if($irgcID==0){
		$out.='<p class="normaltext">'.$TEXT_ERROR_INVALID_CODE_SPECIFIED_CONST.'<br><br>';
	}else{
		$data=getFieldsAsArray('InfraredGroup_Command','InfraredGroup.Description AS Description,IRData,Command.Description AS Command',$publicADO,'LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup INNER JOIN Command ON FK_Command=PK_Command WHERE PK_InfraredGroup_Command='.$irgcID);
		if(isset($_REQUEST['deviceID']) && (int)$_REQUEST['deviceID']!=0){
			$deviceID=$_REQUEST['deviceID'];
			$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia',$publicADO,'WHERE PK_Device='.$deviceID);
			$parentDevice=$deviceInfo['FK_Device_ControlledVia'][0];
			
			$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$parentDevice.' 1 191 9 "'.$data['IRData'][0].'"';
			print $commandToTest;
			exec($commandToTest);
			$message=$TEXT_COMMAND_WAS_SENT_TO_DEVICE_CONST.' #'.$parentDevice;		
		}
		
		if(!isset($data['IRData'][0])){
			$out.=$TEXT_ERROR_MISSING_IRDATA_CONST;
		}else{
			$out.='<p class="normaltext"><b>'.$TEXT_INFRARED_GROUP_CONST.':</b> '.$data['Description'][0].'<br>
			<p class="normaltext"> <b>'.$TEXT_COMMAND_CONST.': </b>'.$data['Command'][0].' <span class="confirm"><B>'.@$message.'</B></span>
			<div class="normaltext" style="padding:10px; border-bottom: 1px solid #557DD2; border-top : 1px solid #557DD2; border-left : 1px solid #557DD2; border-right: 1px solid #557DD2;">'.$data['IRData'][0].'</div><br><br>
			<div align="center"><input type="button" class="button" name="button" value="'.$TEXT_CLOSE_CONST.'" onClick="javascript:self.close();"></div>';
		}
	}
		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_IRDATA_FOR_INFRARED_CODE_CONST);
	$output->output();
}
?>
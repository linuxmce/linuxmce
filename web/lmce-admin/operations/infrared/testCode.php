<?
function testCode($output,$publicADO)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/testCode.lang.php');

	/* @var $dbADO ADOConnection */
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$sender=$_REQUEST['sender'];
	$code=$_REQUEST['irData'];
	
	if(isset($_REQUEST['deviceID']) && (int)$_REQUEST['deviceID']!=0){
		if($code==''){
			$alert=$TEXT_ERROR_EMPTY_CONTENT_CONST;
		}else{
			$deviceID=$_REQUEST['deviceID'];
			$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia',$publicADO,'WHERE PK_Device='.$deviceID);
			if((int)@$deviceInfo['FK_Device_ControlledVia'][0]>0 || $sender=='rubyCodes'){
				$parentDevice=(int)@$deviceInfo['FK_Device_ControlledVia'][0];
					
				$deviceToReceive=($sender=='rubyCodes')?$deviceID:$parentDevice;
				$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceToReceive.' 1 191 9 "'.$code.'"';

				exec($commandToTest);
				$alert=$TEXT_COMMAND_SENT_CONFIRMATION_CONST.' #'.$deviceToReceive;		
			}else{
				$alert=$TEXT_ERROR_NOT_CONTROLLED_BY_IR_INTERFACE_CONST;
			}
		}
	}else{
		$alert=$TEXT_ERROR_NO_DEVICE_SPECIFIED_CONST;
	}
	
	/*
	// old section who test the code stored in database
	if($irgcID==0){
		$alert='Invalid code specified.<br>';
	}else{
		$data=getFieldsAsArray('InfraredGroup_Command','InfraredGroup.Description AS Description,IRData,Command.Description AS Command',$publicADO,'LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup INNER JOIN Command ON FK_Command=PK_Command WHERE PK_InfraredGroup_Command='.$irgcID);
		if(isset($_REQUEST['deviceID']) && (int)$_REQUEST['deviceID']!=0){
			if(@$data['IRData'][0]==''){
				$alert='ERROR: Specified command had empty content.';
			}else{

				$deviceID=$_REQUEST['deviceID'];
				$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia',$publicADO,'WHERE PK_Device='.$deviceID);
				if((int)@$deviceInfo['FK_Device_ControlledVia'][0]>0 || $sender=='rubyCodes'){
					$parentDevice=(int)@$deviceInfo['FK_Device_ControlledVia'][0];
					
					$deviceToReceive=($sender=='rubyCodes')?$deviceID:$parentDevice;
					$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceToReceive.' 1 191 9 "'.$data['IRData'][0].'"';

					exec($commandToTest);
					$alert='The command was sent to device #'.$deviceToReceive;		
				}else{
					$alert='ERROR: the device it\'s not controlled via infrared interface.';
				}
			}
		}else{
			$alert='ERROR: No device specified;';
		}
	}
	*/
	
	$out='
	<script>
		alert(\''.addslashes($alert).'\');
	</script>';
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
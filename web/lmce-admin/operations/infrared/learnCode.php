<?php
function learnCode($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/learnCode.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = ((int)$_REQUEST['deviceID']>0)?(int)$_REQUEST['deviceID']:NULL;
	$commandID = (int)$_REQUEST['commandID'];
	$dtID=(int)$_REQUEST['dtID'];
	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	$noRefresh=(isset($_REQUEST['noRefresh']))?1:0;

	if ($action=='form') {	
		if(!isset($_REQUEST['error'])){
			$res=$dbADO->Execute('SELECT psc_mod AS lastTime,PK_InfraredGroup_Command FROM InfraredGroup_Command ORDER BY psc_mod DESC LIMIT 0,1');
			$row=$res->FetchRow();
			if(isset($_SESSION['LastInfraredGroup_CommandTime']) && $_SESSION['LastInfraredGroup_CommandTime']!=$row['lastTime']){
				$out='
				<script>
					alert(\''.$TEXT_IR_CODE_LEARNED_CONST.'\');
					opener.location=\'index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID.'&from=avWizard%26type%3DavEquipment&rand='.rand(1000,9999).'#code'.$row['PK_InfraredGroup_Command'].'\';
					self.close();
				</script>';
			}
			else{
				$selectDevice='
					SELECT DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer,Device.Description
					FROM Device
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
					WHERE PK_DeviceTemplate=?';
				$resDevice=$dbADO->Execute($selectDevice,$dtID);
	
				$rowDevice=$resDevice->FetchRow();
				
				$out.='
			<script>
			function disableManually(val)
			{
				document.learnCode.irData.disabled=val;
				document.learnCode.add.disabled=val;
				if(val===false){
					document.learnCode.action.value="cancel";
					window.clearInterval(newInterval);
					document.learnCode.submit();
				}
				else{
					document.learnCode.submit();
				}
					
			}
			</script>
			<form action="index.php" method="post" name="learnCode">
			<input type="hidden" name="section" value="learnCode">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandID" value="'.$commandID.'">
			<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
				
			<table width="100%">
				<tr>
					<td align="center" colspan="2" height="60">'.$TEXT_DEVICE_CONST.' <B>'.$rowDevice['Description'].'</B>, '.strtolower($TEXT_DEVICE_TEMPLATE_CONST).' <B>'.$rowDevice['Template'].'</B>, '.strtolower($TEXT_DEVICE_CATEGORY_CONST).' <B>'.$rowDevice['Category'].'</B>, '.strtolower($TEXT_MANUFACTURER_CONST).' <B>'.$rowDevice['Manufacturer'].'</B></td>
				</tr>
				<tr>
					<td align="center"><input type="radio" name="newCodeType" value="0" '.(($noRefresh==0)?'checked':'').' onClick="disableManually(true);"></td>
					<td align="left"><B>'.$TEXT_LEARN_CODE_CONST.'</B> ('.$TEXT_LEARN_CODE_COND_CONST.')</td>
				</tr>
				<tr>
					<td align="center"><input type="radio" name="newCodeType" value="1" '.(($noRefresh==1)?'checked':'').' onClick="disableManually(false);"></td>
					<td align="left"><B>'.$TEXT_TYPE_CODE_MANNUALY_CONST.'</B> ('.$TEXT_IN_PRONTO_FORMAT_CONST.')</td>
				</tr>
				<tr>
					<td align="center">&nbsp;</td>
					<td align="left"><textarea name="irData" rows="3" cols="100" '.(($noRefresh==0)?'disabled':'').' style="width:100%"></textarea></td>
				</tr>
				
				<tr>
					<td align="center">&nbsp;</td>
					<td align="center">
						<input type="submit" class="button" name="add" value="'.$TEXT_ADD_CODE_CONST.'" '.(($noRefresh==0)?'disabled':'').'> 
						<input type="submit" class="button" name="cancel" value="'.$TEXT_CANCEL_CONST.'">
					</td>
				</tr>
			</table>			
			</form>';
				if($noRefresh==0){
					$out.='	
					<script>
						var newInterval=setInterval("self.location.reload()",2000);
					</script>';
				}
			}	
		}else{
			$out.='<div class="err" style="padding:50px;" align="center"><B>'.$_REQUEST['error'].'</B><br><br><br>
				<input type="button" class="button" name="close" value="Close" onClick="self.close();"></div>
			';
		}
	} else {
		if(isset($_REQUEST['cancel']) || $action=='cancel'){
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 245 8 0';
			exec($commandToSend);
			if($action=='cancel'){
				$out.="
				<script>
					self.location='index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=$commandID&noRefresh=1';
				</script>
				";
			}
			else{
				$out.='
				<script>
					self.close();
				</script>
			';
			}
		}elseif(isset($_POST['add'])){

			$irData=stripslashes($_POST['irData']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;

			$isOtherCustomCode=$dbADO->Execute('SELECT * FROM InfraredGroup_Command WHERE FK_InfraredGroup=? AND FK_Command=?',array($infraredGroupID,$commandID));
			if($isOtherCustomCode->RecordCount()>0){
				$rowOther=$isOtherCustomCode->FetchRow();
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$rowOther['PK_InfraredGroup_Command']));
				$igcID=$rowOther['PK_InfraredGroup_Command'];
			}else{
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command, IRData) VALUES (?,?,?)',array($infraredGroupID,$commandID, $irData));
				$igcID=$dbADO->Insert_ID();
			}
			
			$script='<script>
					opener.location=\'index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID.'&from=avWizard%26type%3DavEquipment&rand='.rand(1000,9999).'#code'.$igcID.'\';
					self.close();
				</script>
			';
			die($script);
		}else{
			if($deviceID>0){
				$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia',$dbADO,'WHERE PK_Device='.$deviceID);
				if((int)@$deviceInfo['FK_Device_ControlledVia'][0]>0){
					$parentDevice=$deviceInfo['FK_Device_ControlledVia'][0];
					$resLastCommand=$dbADO->Execute('SELECT psc_mod AS lastTime FROM InfraredGroup_Command ORDER BY psc_mod DESC LIMIT 0,1');
					$rowLastCommand=$resLastCommand->FetchRow();
					$_SESSION['LastInfraredGroup_CommandTime']=$rowLastCommand['lastTime'];
					
					$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$parentDevice.' 1 245 8 1 154 '.$commandID.' 2 '.$deviceID;
					exec($commandToSend);
				
					header("Location: index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID);
					exit();
				}else{
					header("Location: index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID.'&error='.$TEXT_ERROR_IR_INTERFACE_NOT_PARENT_CONST.'&noRefresh=1');
					exit();
				}
			}else{
				header("Location: index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID.'&noRefresh=1');
				exit();
			}
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_LEARN_CODE_CONST);			
	$output->output();
}
?>

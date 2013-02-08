<?php
function newRubyCode($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/newRubyCode.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = ((int)$_REQUEST['deviceID']>0)?(int)$_REQUEST['deviceID']:NULL;
	$commandID = (int)$_REQUEST['commandID'];
	$dtID=(int)$_REQUEST['dtID'];
	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	
	if ($action=='form') {		

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
		<form action="index.php" method="post" name="newRubyCode">
		<input type="hidden" name="section" value="newRubyCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="dtID" value="'.$dtID.'">
		<input type="hidden" name="commandID" value="'.$commandID.'">
		<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
		
		<table width="100%">
			<tr>
				<td align="center" colspan="2" height="60">Device <B>'.$rowDevice['Description'].'</B>, device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B>, manufacturer <B>'.$rowDevice['Manufacturer'].'</B></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><B>'.$TEXT_TYPE_RUBY_CODE_CONST.' </B></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><textarea name="irData" rows="30" style="width:100%"></textarea></td>
			</tr>
			
			<tr>
				<td align="center">&nbsp;</td>
				<td align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_RUBY_CODE_CONST.'"> <input type="button" class="button" name="cancel" value="'.$TEXT_CANCEL_CONST.'" onclick="self.close();"></td>
			</tr>
		</table>			
		</form>';
	
	} else {

		if(isset($_POST['add'])){
			$irData=stripslashes($_POST['irData']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
			$isOtherCustomCode=$dbADO->Execute('SELECT * FROM InfraredGroup_Command WHERE FK_InfraredGroup=? AND FK_Command=?',array($infraredGroupID,$commandID));
			if($isOtherCustomCode->RecordCount()>0){
				$rowOther=$isOtherCustomCode->FetchRow();
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$rowOther['PK_InfraredGroup_Command']));
				$igcID=$rowOther['PK_InfraredGroup_Command'];
			}else{
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,IRData) VALUES (?,?,?)',array($infraredGroupID,$commandID, $irData));
				$igcID=$dbADO->Insert_ID();
			}
			
			$script='<script>
					opener.location.reload();
					opener.location=\'index.php?section=rubyCodes&dtID='.$dtID.'&deviceID='.$deviceID.'&from=genericSerialDevices&rand='.rand(1000,9999).'#code'.$igcID.'\';
					self.close();
				</script>
			';
			die($script);
		}	
		else{
			header("Location: index.php?section=newRubyCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID);
			exit();
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_NEW_RUBY_CODE_CONST);			
	$output->output();
}
?>

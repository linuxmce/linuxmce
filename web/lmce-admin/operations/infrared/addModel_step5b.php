<?
	$multiInputsCategs=array(103,77,126,98,109);
	$mediaTypesDT=array(106,104,107,105,108,135);
	
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	if($return==0){
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step=5&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step=6&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$submitLabel=$TEXT_NEXT_CONST;
	}else{
		$submitLabel=$TEXT_SAVE_CONST;
	}	
		
	$whereClause='
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
		LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate 
		WHERE PK_DeviceTemplate='.$dtID;
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,FK_MediaType,ToggleInput',$publicADO,$whereClause);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}
	
	
	$is=(isset($_REQUEST['is']))?(int)$_REQUEST['is']:1;
	
	
	if($action=='form'){
		
		$connectorsArray=getAssocArray('ConnectorType','PK_ConnectorType','Description',$publicADO,'','ORDER BY Description ASC');
		

		$queryInput="
			SELECT 
				DeviceTemplate_Input.*,
				Command.Description as Input_Desc, 
				PK_Command
			FROM Command 	
			INNER JOIN DeviceTemplate_Input ON PK_Command = FK_Command AND FK_DeviceTemplate='$dtID'
			WHERE FK_CommandCategory=22
			ORDER BY OrderNo ASC";
		$checkedCommands=array();
		$checkedCommands=array();
		$checkedCommandsAssoc=array();

		$resInput= $publicADO->Execute($queryInput);
		if ($resInput) {
			while ($row=$resInput->FetchRow()) {
				$checkedCommands[$row['PK_Command']]=$row['Input_Desc'];
			}
		}

		$output->setScriptInHead(reorderMultiPulldownJs());
		
		$out='
		<script>
			
		function setOrder()
		{
			saveOrder="";
			for( var i = 0; i < document.addModel.orderItem.length; i++ ) { 
				if(i==0){
					saveOrder=document.addModel.orderItem[i].value;
				}else{
					saveOrder=saveOrder+","+document.addModel.orderItem[i].value;
				}
				document.addModel.commandsOrder.value=saveOrder;
			}
			return true;
		}
		</script>
		
		<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q5_ORDER_TITLE_CONST.'</B><br><br>
		
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandsOrder" value="">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">
		
			<table align="center" border="0" width="400">
				<tr>
					<td colspan="2" class="normaltext"><B>'.$TEXT_Q5_CHANGE_ORDER_CONST.'</B></td>
				</tr>
				<tr>
					<td colspan="2" class="normaltext">'.$TEXT_Q5_CONFIRM_TOGGLE_INPUTS_CONST.'</td>
				</tr>
				<tr>				
					<td valign="top" align="right" width="50%">'.pulldownFromArray($checkedCommands,'orderItem',0,'size="10"','key','').'</td>
					<td valign="middle" align="left"><input type="button" class="button" value="U" onClick="moveUp(\'document.addModel.orderItem\');"> <br><input type="button" class="button" value="D" onClick="moveDown(\'document.addModel.orderItem\');"></td>
				</tr>
				<tr>				
					<td valign="top" align="center" colspan="2"><input type="checkbox" value="2" name="send_input_twice" '.((sendInputTwice($dtID,$dbADO)?'checked':'')).'> '.$TEXT_SEND_INPUT_TWICE_CONST.'</td>
				</tr>				
				<tr>
					<td colspan="2" class="normaltext"><input type="hidden" name="checkedCommands" value="'.urlencode(serialize($checkedCommands)).'"></td>
				</tr>
				<tr>
					<td align="center" colspan="2"><input type="submit" class="button" name="next" value="'.$submitLabel.'"></td>
				</tr>
			</table>
		<br>
		</form>
		';
		
	}else{
		// process

		if($dtID!=0){
			$send_input_twice=(int)@$_POST['send_input_twice'];
			$send_input_twice=($send_input_twice==0)?1:$send_input_twice;
			$dbADO->Execute('UPDATE DeviceTemplate_AV SET ToggleInput=? WHERE FK_DeviceTemplate=?',array($send_input_twice,$dtID));
			
			$checkedCommands=unserialize(urldecode($_POST['checkedCommands']));
			$commandOrder=array_flip(explode(',',$_POST['commandsOrder']));

				foreach ($checkedCommands AS $commandID=>$commandName){
					$publicADO->Execute('UPDATE DeviceTemplate_Input SET OrderNo=? WHERE FK_DeviceTemplate=? AND FK_Command=?',array($commandOrder[$commandID],$dtID,$commandID));
				}
	
		}
			if($return==0){
				header('Location: index.php?section=addModel&step=6&dtID='.$dtID.'&deviceID='.$deviceID);
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}
		
		exit();
	}
	
function sendInputTwice($dtID,$dbADO){
	$data=getFieldsAsArray('DeviceTemplate_AV','ToggleInput',$dbADO,'WHERE FK_DeviceTemplate='.$dtID);
	
	return ($data['ToggleInput'][0]==2)?true:false;
}	
?>
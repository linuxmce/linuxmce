<?
	$ampReceivers=array(103);
	$deviceID=(int)@$_REQUEST['deviceID'];
	$dtID=$_REQUEST['dtID'];
	$return=(int)@$_REQUEST['return'];
	
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,ToggleDSP',$publicADO,'INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate  WHERE PK_DeviceTemplate='.$dtID);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}

	$dsp=(isset($_REQUEST['dsp']))?(int)$_REQUEST['dsp']:$dtDataArray['ToggleDSP'][0];
	$dsp=($dsp==0)?1:$dsp;
	
	if($action=='form'){
		$inputSelectedTxt='
				<table border="0" class="normaltext" align="center" cellpadding="3" cellspacing="0">
					<tr class="tablehead">
						<td align="center"><B>DSP Modes</b><br> (check all that apply)</td>
					</tr>			
			';
		$queryDSPMode="
				SELECT 
					DeviceTemplate_DSPMode.*,
					Command.Description as DSPMode_Desc, 
					PK_Command
				FROM Command 	
				LEFT JOIN DeviceTemplate_DSPMode ON PK_Command = FK_Command AND FK_DeviceTemplate='$dtID'
				WHERE FK_CommandCategory=21
				ORDER BY DSPMode_Desc ASC";
		$commandsArray=array();
		$checkedCommands=array();

		$resDSPMode= $publicADO->Execute($queryDSPMode);
		if ($resDSPMode) {
			while ($row=$resDSPMode->FetchRow()) {
				$commandsArray[$row['PK_Command']]=$row['DSPMode_Desc'];
				if(!is_null($row['FK_DeviceTemplate'])){
					$checkedCommands[$row['PK_Command']]=$row['DSPMode_Desc'];
				}
				$inputSelectedTxt.='
					<tr>
						<td><input type="checkbox" name="cmd_'.$row['PK_Command'].'" value="'.$row['DSPMode_Desc'].'" '.((!is_null($row['FK_DeviceTemplate']))?'checked':'').' onClick="enableObjects('.$row['PK_Command'].');">'.$row['DSPMode_Desc'].'</td>
					</tr>';
			}
		}
		$resDSPMode->close();


		$inputSelectedTxt.='</table>
			<input type="hidden" name="oldCheckedCommands" value="'.join(',',array_keys($checkedCommands)).'">';

		
		if(!isset($_REQUEST['dsp'])){
			if(count($checkedCommands)!=0){
				$dsp=($dtDataArray['ToggleDSP'][0]==0)?2:3;
			}	
		}
		
		if($return==0){
			$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
			$submitLabel=$TEXT_NEXT_CONST;
		}else{
			$submitLabel=$TEXT_SAVE_CONST;
		}	
		
		$out='
		<script>
		function enableObjects(val)
		{
			eval("flag=!document.addModel.cmd_"+val+".checked");
			if(flag==false){
				eval("newText=document.addModel.cmd_"+val+".value");
				addToPulldown(\'document.addModel.orderItem\',val,newText)

			}else{
				removeFromPulldown(\'document.addModel.orderItem\',val);
			}
		}
			
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
		
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}		
		</script>		
		
		<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q6_TITLE_CONST.'</B><br><br>
		
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandsOrder" value="">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">
			<input type="hidden" name="toggleDSP" value="'.$dtDataArray['ToggleDSP'][0].'">
		
		';
		if(!in_array($dtDataArray['FK_DeviceCategory'][0],$ampReceivers)){
			$out.='<p class="normaltext">'.$dtDataArray['Description'][0].' devices normally don\'t have multiple DSP Modes, like "Church", "Concert hall", "Dolby Digital", etc.  You can probably ignore this step and click next.<br><br>';
		}else{
			$out.='<p class="normaltext">'.$TEXT_Q6_MULTI_DSP_CONST;
		}
		
		$out.='
		<table class="normaltext" cellpadding="5" cellspacing="0">
			<tr>
				<td>
					<input type="radio" name="dsp" value="1" '.(($dsp==1)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=1&deviceID='.$deviceID.'&return='.$return.'\'"> '.$TEXT_Q6_OPT1_CONST.'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="dsp" value="2" '.(($dsp==2)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=2&deviceID='.$deviceID.'&return='.$return.'\'"> '.$TEXT_Q6_OPT2_CONST.'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="dsp" value="3" '.(($dsp==3)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=3&deviceID='.$deviceID.'&return='.$return.'\'"> '.$TEXT_Q6_OPT3_CONST.'		
				</td>
			</tr>';
		if($dsp>1){
			
			$out.='
			<tr>
				<td align="center">'.$inputSelectedTxt.'</td>
			</tr>
			<tr>
				<td align="center"><a href="#" onClick="windowOpen(\'index.php?section=addCommand&from=addModel&commandCateg=21\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">'.$TEXT_ADD_NEW_COMMAND_CONST.'</a></td>
			</tr>			
			<input type="hidden" name="commandsArray" value="'.urlencode(serialize($commandsArray)).'">
			';
		}
		
			$out.='
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="'.$submitLabel.'"></td>
			</tr>
		</table>
		<br>
		</form>
	
		';
	}else{
		$oldDSP=(int)@$_POST['toggleDSP'];
		// process
		if($dtID!=0){
			if($dsp!=1){
				$commandsArray=unserialize(urldecode(@$_POST['commandsArray']));
				$oldCheckedCommands=explode(',',@$_POST['oldCheckedCommands']);
				
				if(count($commandsArray)>0){
					foreach ($commandsArray AS $commandID=>$commandName){
						if(isset($_POST['cmd_'.$commandID])){
							if(!in_array($commandID,$oldCheckedCommands)){
								$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_DSPMode (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,$commandID));
							}
							
						}else{
							if(in_array($commandID,$oldCheckedCommands)){
								$publicADO->Execute('DELETE FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate=? AND FK_Command=?',array($dtID,$commandID));
							}
						}
					}
				}
				
				$ToggleDSP=($dsp==2)?0:1;
				if(($ToggleDSP==0 && $oldDSP!=0) || ($ToggleDSP==1 && $oldDSP!=2)){
					$publicADO->Execute('UPDATE DeviceTemplate_AV SET ToggleDSP=? WHERE FK_DeviceTemplate=?',array($ToggleDSP,$dtID));
				}
				
			}else{
				$publicADO->Execute('DELETE FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate=?',array($dtID));
				$publicADO->Execute('UPDATE DeviceTemplate_AV SET ToggleDSP=? WHERE FK_DeviceTemplate=?',array(0,$dtID));
			}

			$nextStep=($dsp==3)?'6b':7;
			if($return==0){
				header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID.'&deviceID='.$deviceID);
			}elseif($dsp==3){
				header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1');
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}			
			
			exit();
		}
	}
?>

<?php
	$multiInputsCategs=array(103,77,126,98,109);
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	$dtID=$_REQUEST['dtID'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	
	$whereClause='
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
		LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate 
		WHERE PK_DeviceTemplate='.$dtID;
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,FK_MediaType,ToggleInput',$publicADO,$whereClause);
	$resend_input=($dtDataArray['ToggleInput'][0]==2)?1:0;

	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}
		
	if($action=='form'){
		switch((int)@$dtDataArray['FK_DeviceCategory'][0]){
			// Amps/Preamps/Receivers/Tuners
			case 103:
				$categoryInfo=$TEXT_Q5_AMPS_RECEIVERS_INFO_CONST;
			break;
			// Audio Zone
			case 95:
				$categoryInfo=$TEXT_Q5_AUDIO_ZONE_INFO_CONST;
			break;
			// 	Cable Boxes
			case 105:
				$categoryInfo=$TEXT_Q5_CABLE_BOXES_INFO_CONST;
			break;
			// 	Cassette Decks
			case 104:
				$categoryInfo=$TEXT_Q5_CASSETTE_DECKS_INFO_CONST;
			break;
			// CD Players/Recorders
			case 106:
				$categoryInfo=$TEXT_Q5_CDPLAYERS_RECORDERS_INFO_CONST;
			break;
			// Combo Units
			case 129:
				$categoryInfo=$TEXT_Q5_COMBO_UNITS_INFO_CONST;
			break;
			// DVD Players/Recorders
			case 107:
				$categoryInfo=$TEXT_Q5_DVD_PLAYERS_RECORDERS_INFO_CONST;
			break;
			// Laser Disc
			case 135:
				$categoryInfo=$TEXT_Q5_LASER_DISC_INFO_CONST;
			break;
			// Other/Misc
			case 136:
				$categoryInfo=$TEXT_Q5_OTHER_MISC_INFO_CONST;
			break;
			// PVR Capture Cards
			case 75:
				$categoryInfo=$TEXT_Q5_PVR_CAPTURE_CARDS_INFO_CONST;
			break;
			// Radios
			case 134:
				$categoryInfo=$TEXT_Q5_RADIOS_INFO_CONST;
			break;
			// Satellite Boxes
			case 108:
				$categoryInfo=$TEXT_Q5_SATELLITE_BOXES_INFO_CONST;
			break;
			// Switches
			case 126:
				$categoryInfo=$TEXT_Q5_SWITCHES_INFO_CONST;
			break;
			// TVs
			case 77:
				$categoryInfo=$TEXT_Q5_TVS_INFO_CONST_CONST;
			break;
			// TV/VCR Combo
			case 109:
				$categoryInfo=$TEXT_Q5_TV_VCR_COMBO_INFO_CONST;
			break;
			// VCR
			case 98:
				$categoryInfo=$TEXT_Q5_VCR_INFO_CONST;
			break;
			
			default:
				$categoryInfo='&nbsp;';
			break;
		}
		
		$connectorsArray=getAssocArray('ConnectorType','PK_ConnectorType','Description',$publicADO,'','ORDER BY Description ASC');
		$mediaTypesArray=getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'WHERE DCEAware=0','ORDER BY Description ASC');

		$inputSelectedTxt='
			<p class="normaltext"> '.$TEXT_Q5_INPUTS_NOTE_CONST.'
			<table border="0" class="normaltext" align="center" cellpadding="3" cellspacing="0">
				<tr class="tablehead">
					<td align="center"><B>'.$TEXT_INPUTS_CONST.'</b><br> (check all that apply)</td>
					<td align="center"><B>'.$TEXT_SOURCE_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_CONNECTOR_CONST.'</B></td>
				</tr>			
			';

		$queryInput="
			SELECT 
				DeviceTemplate_Input.*,
				Command.Description as Input_Desc, 
				FK_ConnectorType,
				PK_Command
			FROM Command 	
			LEFT JOIN DeviceTemplate_Input ON PK_Command = FK_Command AND DeviceTemplate_Input.FK_DeviceTemplate='$dtID'
			WHERE FK_CommandCategory=22
			ORDER BY Input_Desc ASC";
		$commandsArray=array();
		$checkedCommands=array();
		$checkedCommandsAssoc=array();
		$oldMTArray=array();

		$resInput= $publicADO->Execute($queryInput);
		if ($resInput) {
			while ($row=$resInput->FetchRow()) {
				$commandsArray[$row['PK_Command']]=$row['Input_Desc'];
				if(!is_null($row['FK_DeviceTemplate'])){
					$checkedCommands[$row['PK_Command']]=$row['Input_Desc'];
					$rs=$publicADO->Execute('
					SELECT 
					FK_DeviceTemplate_ControlledVia,
					DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate,
					FK_MediaType,FK_Command
					FROM DeviceTemplate_DeviceTemplate_ControlledVia
					LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate
					INNER JOIN DeviceTemplate_Input ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=DeviceTemplate_Input.FK_DeviceTemplate
					WHERE FK_DeviceTemplate_ControlledVia=? AND RerouteMessagesToParent=1 AND FK_Command=?',array($dtID,$row['PK_Command']));
					
					$rowMT=$rs->FetchRow();
				}
				$inputSelectedTxt.='
					<tr>
						<td><input type="checkbox" name="cmd_'.$row['PK_Command'].'" value="'.$row['Input_Desc'].'" '.((!is_null($row['FK_DeviceTemplate']))?'checked':'').' onClick="enableObjects('.$row['PK_Command'].');">'.$row['Input_Desc'].'</td>
						<td>'.pulldownFromArray($mediaTypesArray,'mediaType_'.$row['PK_Command'],@$rowMT['FK_MediaType'],((is_null($row['FK_DeviceTemplate']))?'disabled':''),'key','External device').'</td>
						<td>'.pulldownFromArray($connectorsArray,'connector_'.$row['PK_Command'],$row['FK_ConnectorType'],((is_null($row['FK_DeviceTemplate']))?'disabled':'')).'</td>
					</tr>';
				if(isset($rowMT['FK_MediaType'])){
					$oldMTArray[$row['PK_Command']]['FK_MediaType']=$rowMT['FK_MediaType'];
					$oldMTArray[$row['PK_Command']]['FK_EmbeddedDevice']=$rowMT['FK_DeviceTemplate'];
					unset($rowMT['FK_MediaType']);
				}
			}
		}
		
		$resInput->close();

		$inputSelectedTxt.='</table>
		<input type="hidden" name="oldMTArray" value="'.urlencode(serialize($oldMTArray)).'">
		<input type="hidden" name="oldCheckedCommands" value="'.join(',',array_keys($checkedCommands)).'">';
		
		if(!isset($_REQUEST['is'])){
			if(is_null($dtDataArray['FK_MediaType'][0])){
				// hard coded: if receivers, default 2
				//$is=($dtDataArray['FK_DeviceCategory'][0]==103)?2:1;
				$is=($dtDataArray['ToggleInput'][0]==0)?2:3;
			}else
				$is=1;	
		}else{
			$is=(int)$_REQUEST['is'];
		}
		$resend_input=($is!=3)?0:$resend_input;
		
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
			eval("flag=document.addModel.mediaType_"+val+".disabled="+flag);
			eval("flag=document.addModel.connector_"+val+".disabled="+flag);
			if(flag==false){
				eval("newText=document.addModel.cmd_"+val+".value");
			}
		}
		
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}

		</script>
		
		<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q5_TITLE_CONST.'</B><br><br>';
		$out.='
		<p class="normaltext">'.$TEXT_Q5_NOTE_CONST.'
		<p class="normaltext">'.$categoryInfo.'
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="commandsOrder" value="">
			<input type="hidden" name="return" value="'.$return.'">
			<input type="hidden" name="toggleInput" value="'.$dtDataArray['ToggleInput'][0].'">
		';
		
		$out.='
		<table class="normaltext" cellpadding="5" cellspacing="0">
			<tr>
				<td>
					<input type="radio" name="is" value="1" '.(($is==1)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=1&deviceID='.$deviceID.'&return='.$return.'\'"> #1 - '.$TEXT_Q5_OPT1_CONST.' '.pulldownFromArray($mediaTypesArray,'dtMediaType',@$dtDataArray['FK_MediaType'][0],($is!=1)?'disabled':'').'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="2" '.(($is==2)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=2&deviceID='.$deviceID.'&return='.$return.'\'"> #2 - '.$TEXT_Q5_OPT2_CONST.'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="3" '.(($is==3)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=3&deviceID='.$deviceID.'&return='.$return.'\'"> #3 - '.$TEXT_Q5_OPT3_CONST.'		
				</td>
			</tr>
			<tr>
				<td>
					<input type="checkbox" name="resend_input" value="1" '.(($resend_input==1)?'checked':'').' '.(($is==3)?'':'disabled').'> <span class="'.(($is==3)?'':'grayout').'">'.$TEXT_RESEND_INPUT_SELECT_WHEN_TOGGLING_CONST.'</span>		
				</td>
				
			</tr>';
		if($is>1){
			$out.='
			<tr>
				<td align="center"><table>
					<tr>
						<td>'.$inputSelectedTxt.'</td>
					</tr>
					<tr>
						<td align="center"><a href="#" onClick="windowOpen(\'index.php?section=addCommand&from=addModel&commandCateg=22\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">'.$TEXT_ADD_NEW_COMMAND_CONST.'</a></td>
					</tr>
			
				</table>
			</tr>
			<input type="hidden" name="commandsArray" value="'.urlencode(serialize($commandsArray)).'">
			<tr>
				<td class="normaltext">'.$TEXT_Q5_BUILT_IN_NOTE_CONST.'</td>
			</tr>
			';
		}else{
			$out.='<input type="hidden" name="oldDT_MT" value="'.@$dtDataArray['FK_MediaType'][0].'">';
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
		// process
		$oldInput=(int)@$_POST['toggleInput'];

		if($dtID!=0){
			$is=(int)$_REQUEST['is'];
			if($is==1){
				$dtMediaType=(int)$_POST['dtMediaType'];
				$oldDT_MT=(int)$_POST['oldDT_MT'];
				if($dtMediaType>0){
					if($oldDT_MT==0){
						$publicADO->Execute('INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($dtID,$dtMediaType));	
					}else{
						$publicADO->Execute('UPDATE DeviceTemplate_MediaType SET FK_MediaType=? WHERE FK_DeviceTemplate=? AND FK_MediaType=?',array($dtMediaType,$dtID,$oldDT_MT));	
					}
				}
			}else{
				$publicADO->Execute('DELETE FROM DeviceTemplate_MediaType WHERE FK_DeviceTemplate=?',$dtID);
				$mediaTypesArray=getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'','ORDER BY Description ASC');
				$commandsArray=unserialize(urldecode($_POST['commandsArray']));
				$oldMTArray=unserialize(urldecode($_POST['oldMTArray']));

				$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);
				if($is==3){
					$commandOrder=array_flip(explode(',',$_POST['commandsOrder']));
				}

				foreach ($commandsArray AS $commandID=>$commandName){
					if(isset($_POST['cmd_'.$commandID])){
	
						$connectorType=((int)$_POST['connector_'.$commandID]>0)?(int)$_POST['connector_'.$commandID]:NULL;
						$mediaType=(int)$_POST['mediaType_'.$commandID];
	
						if(!in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command,FK_ConnectorType) VALUES (?,?,?)',array($dtID,$commandID,$connectorType));
						}else{
							$publicADO->Execute('UPDATE DeviceTemplate_Input SET FK_ConnectorType=? WHERE FK_DeviceTemplate=? AND FK_Command=?',array($connectorType,$dtID,$commandID));
						}

	
						if($mediaType!=0){
							if(@$oldMTArray[$commandID]['FK_MediaType']!=$mediaType){
								if(isset($oldMTArray[$commandID]['FK_MediaType'])){
									deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
								}
								
								// create embedded device template
								createEmbeddedDeviceTemplate($commandName.' - '.$mediaTypesArray[$mediaType],$dtDataArray['FK_Manufacturer'][0],$dtDataArray['FK_DeviceCategory'][0],$userID,$dtID,$commandID,$mediaType,$publicADO);
							}
							
						}else{
							if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=0){
								deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
							}
						}
					}else{
						if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=0){
							deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
						}
						
						if(in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('DELETE FROM DeviceTemplate_Input WHERE FK_DeviceTemplate=? AND FK_Command=?',array($dtID,$commandID));
						}
					}
				}
				$ToggleInput=($is==2)?0:1;
				$resend_input=(int)@$_POST['resend_input'];
				$ToggleInput=($is==3 && $resend_input==1)?2:$ToggleInput;
				
				if(($ToggleInput!=$oldInput)){
					$publicADO->Execute('UPDATE DeviceTemplate_AV SET ToggleInput=? WHERE FK_DeviceTemplate=?',array($ToggleInput,$dtID));
				}
			}
		}
		
		$nextStep=($is==3)?'5b':6;
		if($return==0){
			header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID.'&deviceID='.$deviceID);
		}elseif($is==3){
			header('Location: index.php?section=addModel&step=5b&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1');
		}else{
			header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
		}	
		
		exit();
	}
	
function deleteEmbeddedDT($dtID,$publicADO){
	$devices=array_keys(getAssocArray('Device','PK_Device','Description',$publicADO,'WHERE FK_DeviceTemplate='.$dtID));
	foreach ($devices AS $embeddedID){
		deleteDevice($embeddedID,$publicADO);
	}
	
	$publicADO->Execute('
	DELETE DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
	FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
	INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia ON FK_DeviceTemplate_DeviceTemplate_ControlledVia=PK_DeviceTemplate_DeviceTemplate_ControlledVia
	WHERE FK_DeviceTemplate=?',$dtID);
	
	$publicADO->Execute('DELETE FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate=?',$dtID);
	
	$publicADO->Execute('DELETE FROM DeviceTemplate WHERE PK_DeviceTemplate=?',$dtID);
	
}
?>

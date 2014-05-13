<?php
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','RepeatIR,RepeatVolume',$publicADO,'WHERE FK_DeviceTemplate='.$dtID);
	if($return==0){
		//$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';		
		$submitLabel=$TEXT_NEXT_CONST;
	}else{
		$submitLabel=$TEXT_SAVE_CONST;
	}
	
	if($action=='form'){
		$out='<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q7_TITLE_CONST.'</B><br>
		<p class="normaltext">'.$TEXT_Q7_INFO_CONST.'
		
		<form action="index.php" method="POST" name="addModel" onSubmit="return confirmRepeats();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="isDef" value="'.@$_REQUEST['isDef'].'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">
		
		<table class="normaltext" align="center">
			<tr>
				<td>'.$TEXT_Q7_GENERAL_REPEAT_TEXT_CONST.' <input type="text" name="RepeatIR" value="'.(((int)@$_REQUEST['isDef']==1)?'4':@$dtArray['RepeatIR'][0]).'" size="2"></td>
			</tr>
			<tr>
				<td>'.$TEXT_Q7_VOLUME_REPEAT_TEXT_CONST.' <input type="text" name="RepeatVolume" size="2" value="'.(((int)@$_REQUEST['isDef']==1)?'2':@$dtArray['RepeatVolume'][0]).'"></td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="add" value="'.$submitLabel.'"> </td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>
			<tr>
				<td><B>* </B> <em>'.$TEXT_Q7_NOTE_CONST.'</em></td>
			</tr>		
		</table><br>
		</form>
	
		
		<script>
		function confirmRepeats(){
			generalRepeat=document.addModel.RepeatIR.value;
			volumeRepeat=document.addModel.RepeatVolume.value;
			digitDelay=document.addModel.DigitDelay.value;
			
			if(generalRepeat<0 || volumeRepeat<0){
				alert("Delays cannot be negative.");
				return false;
			}
			
			if(generalRepeat>15 || volumeRepeat>15){
				if(confirm("One or more repeats are greater than 15. Are you sure it is correct?")){
					return true;
				}else{
					return false;
				}
			}
			
			return true;
		}
		</script>
		';
	}else{
		// process
		if($dtID!=0){
			$RepeatIR=$_POST['RepeatIR'];
			$RepeatVolume=$_POST['RepeatVolume'];
						
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET RepeatIR=?, RepeatVolume=? WHERE FK_DeviceTemplate=?',array($RepeatIR,$RepeatVolume,$dtID));
			if($return==0){
				header('Location: index.php?section=addModel&step=8&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}
			exit();
		}
		
		header('Location: index.php?section=addModel&step=7&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
	}
?>

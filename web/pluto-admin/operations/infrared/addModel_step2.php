<?
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','IR_PowerDelay,IR_ModeDelay, DigitDelay',$publicADO,'WHERE FK_DeviceTemplate='.$dtID);
	if($return==0){
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$submitLabel=$TEXT_NEXT_CONST;
	}else{
		$submitLabel=$TEXT_SAVE_CONST;
	}
	
	if($action=='form'){
		$out='<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q2_TITLE_CONST.'</B><br>
		<p class="normaltext">'.$TEXT_Q2_INFO_CONST.'
		
		<form action="index.php" method="POST" name="addModel" onSubmit="return confirmDelays();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="isDef" value="'.@$_REQUEST['isDef'].'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">
		
		<table class="normaltext" align="center">
			<tr>
				<td>'.$TEXT_Q2_POWER_DELAY_TEXT_CONST.' <input type="text" name="IR_PowerDelay" value="'.(((int)@$_REQUEST['isDef']==1)?'7000':@$dtArray['IR_PowerDelay'][0]).'" size="3"></td>
			</tr>
			<tr>
				<td>'.$TEXT_Q2_MODE_DELAY_DELAY_TEXT_CONST.' <input type="text" name="IR_ModeDelay" size="3" value="'.(((int)@$_REQUEST['isDef']==1)?'2000':@$dtArray['IR_ModeDelay'][0]).'"></td>
			</tr>		
			<tr>
				<td>'.$TEXT_Q2_DIGIT_DELAY_DELAY_TEXT_CONST.' <input type="text" name="DigitDelay" value="'.(((int)@$_REQUEST['isDef']==1)?'250':@$dtArray['DigitDelay'][0]).'" size="3"></td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="add" value="'.$submitLabel.'"> </td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>
			<tr>
				<td><B>* </B> <em>'.$TEXT_Q2_NOTE_CONST.'</em></td>
			</tr>		
		</table><br>
		</form>
	
		
		<script>
		function confirmDelays(){
			powerDelay=document.addModel.IR_PowerDelay.value;
			modeDelay=document.addModel.IR_ModeDelay.value;
			digitDelay=document.addModel.DigitDelay.value;
			
			if(powerDelay<0 || modeDelay<0 || digitDelay<0){
				alert("Delays cannot be negative.");
				return false;
			}
			
			if(powerDelay>20000 || modeDelay>20000 || digitDelay>20000){
				if(confirm("One or more delays are greater than 20s. Are you sure it is correct?")){
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
			$IR_PowerDelay=$_POST['IR_PowerDelay'];
			$IR_ModeDelay= $_POST['IR_ModeDelay'];
			$DigitDelay =  $_POST['DigitDelay'];
			
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET IR_PowerDelay=?, IR_ModeDelay=?, DigitDelay=? WHERE FK_DeviceTemplate=?',array($IR_PowerDelay,$IR_ModeDelay,$DigitDelay,$dtID));
			if($return==0){
				header('Location: index.php?section=addModel&step=3&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}
			exit();
		}
		
		header('Location: index.php?section=addModel&step=2&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
	}
?>

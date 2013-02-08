<?
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	
	$cdCategsArray=array(104,106,107,129,137);
	$dtDataArray=getFieldsAsArray('DeviceTemplate','DeviceTemplate.FK_DeviceCategory,DeviceCategory.Description,DeviceTemplate.FK_CommMethod AS FK_CommMethod',$publicADO,'INNER JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup WHERE PK_DeviceTemplate='.$dtID);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','NumericEntry',$publicADO,'WHERE FK_DeviceTemplate='.$dtID);
	
	$isEnter=(strpos(@$dtArray['NumericEntry'][0],'E')!==false)?1:0;
	$isEnter=((int)@$_REQUEST['isDef']==1)?1:$isEnter;
	
	$digits=str_replace('E','',@$dtArray['NumericEntry'][0]);
	$digits=((int)@$_REQUEST['isDef']==1)?'':$digits;
	
	$cdContent=(in_array($dtDataArray['FK_DeviceCategory'][0],$dtDataArray))?'<p class="normaltext">'.$dtDataArray['Description'][0].' devices don’t normally tune to stations or channels, so you can probably ignore this step and just click next.':'';
	

	if($return==0){
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$submitLabel=$TEXT_NEXT_CONST;
	}else{
		$submitLabel=$TEXT_SAVE_CONST;
	}
		
/*	if(@$_SESSION['selectedCommMethod']!=1){
		$comMethod=(isset($_SESSION['selectedCommMethod']))?$_SESSION['selectedCommMethod']:$dtDataArray['FK_CommMethod'][0];
		$commMethodArray=getAssocArray('CommMethod','PK_CommMethod','Description',$publicADO,'','ORDER BY PK_CommMethod ASC');
		$comMethodName=($comMethod!=0)?$commMethodArray[$comMethod]:'This';
		$Content='
			<p class="normaltext">'.$comMethodName.' devices don’t normally require any special rules to tune.<br><br><br>
			<div align="center"><input type="button" class="button" name="next" value="Next" onClick="self.location=\'index.php?section=addModel&step=4&dtID='.$dtID.'&deviceID='.$deviceID.'\'"></div>';
	}else */ { 

		$Content='
		<table class="normaltext">
			<tr>
				<td>'.$TEXT_Q3_COMM_METHOD_1_NOTE_CONST.'</td>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</tr>		
			<tr>
				<td>'.$TEXT_Q3_COMM_METHOD_1_ENTER_BUTTON_CONST.' <input type="radio" name="enterButton" value="E" '.(($isEnter==1)?'checked':'').'> '.$TEXT_YES_CONST.' <input type="radio" name="enterButton" value="" '.(($isEnter==0)?'checked':'').'> '.$TEXT_NO_CONST.'</td>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</tr>		
		
			<tr>
				<td>'.$TEXT_Q3_COMM_METHOD_1_NUMBER_OF_DIGITS_CONST.' <input type="text" name="digits" value="'.$digits.'" size="2"></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="'.$submitLabel.'"> </td>
			</tr>
		
		</table>		
		
		';
	}
	
	
	if($action=='form'){
		$out='<br>
		'.@$navigationButtons.'
		<B>'.$TEXT_Q3_TITLE_CONST.'</B><br><br>
		'.$cdContent.'
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="isDef" value="'.@$_REQUEST['isDef'].'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">
		
		'.$Content.'
		
		<br>
		</form>
	
		';
	}else{
		// process
		
		if($dtID!=0){
			$enterButton=$_POST['enterButton'];
			$digits=$_POST['digits'];
			$NumericEntry=$digits.$enterButton;
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET NumericEntry=? WHERE FK_DeviceTemplate=?',array($NumericEntry,$dtID));
			if($return==0){
				header('Location: index.php?section=addModel&step=4&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}
			exit();
		}
	}
?>

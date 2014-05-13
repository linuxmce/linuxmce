<?php
function customBackground($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/customBackground.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$oID=(int)$_REQUEST['oID'];
	
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'');
	
	$orbiterData=getFieldsAsArray('Device','Description',$dbADO,'WHERE PK_Device='.$oID);	

	if ($action == 'form') {
		$existingPics=grabFiles($GLOBALS['orbiterBackgroundPath'],'-type f',$oID);
		$out.='
	<div class="err" align="center">'.stripslashes(@$_GET['err']).'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="customBackground" enctype="multipart/form-data">
	<input type="hidden" name="section" value="customBackground">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="oID" value="'.$oID.'">	
		
	<h2>'.$TEXT_CUSTOM_BACKGROUND_FOR_ORBITER_CONST.' '.$oID.', '.$orbiterData['Description'][0].'</h2>
	
	'.$TEXT_ADD_CUSTOM_MAIN_MENU_BACKGROUND_CONST.':<br>
	<input type="file" name="pic" value=""> '.pulldownFromArray($roomsArray,'room',0,'','key','- '.$TEXT_ANY_ROOM_CONST.' -').'	<input type="submit" class="button" name="add" value="'.$TEXT_ADD_PICTURE_CONST.'">
	<br><br>';
	if(count($existingPics)>0){
		$out.='	
		'.$TEXT_EXISTING_BACKGROUNDS_CONST.':
		<table>
			<tr class="tablehead">
				<td><B>'.$TEXT_ROOM_CONST.'</B></td>
				<td><B>'.$TEXT_PICTURE_CONST.'</B></td>
				<td><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>';
		$pos=0;
		foreach ($existingPics AS $pic){
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$roomName=(strpos($pic,'_')!==false)?@$roomsArray[(int)str_replace(array($oID.'_','.png'),array('',''),$pic)]:'Any';
			$out.='
			<tr bgcolor="'.$color.'">
				<td>'.$roomName.'</td>
				<td><a href="orbiter_bg/'.str_replace($GLOBALS['orbiterBackgroundPath'],'',$pic).'" target="_blank"><img src="orbiter_bg/'.str_replace($GLOBALS['orbiterBackgroundPath'],'',$pic).'" width="100" height="100" title="Click for larger view" border="0"></a></td>
				<td><a href="javascript:if(confirm(\''.$TEXT_CONFIRM_DELETE_PICTURE_CONST.'\'))self.location=\'index.php?section=customBackground&oID='.$oID.'&action=del&dpic='.$pic.'\'">'.$TEXT_DELETE_CONST.'</a></td>
			</tr>';
		}
		$out.='
		</table>';
	}
	$out.='
		<input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">
	</form>';
	
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=customBackground&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		if(isset($_POST['add']) && $_FILES['pic']['name']!=''){
			$room=(int)$_REQUEST['room'];
			$picName=$oID.(($room>0)?'_'.$room:'').'.png';
			
			if(move_uploaded_file($_FILES['pic']['tmp_name'],$GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name'])){
				// recreate pic as png 
				$resizeFlag=resizeImage($GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name'], $GLOBALS['orbiterBackgroundPath'].$picName,0,0);
				if($resizeFlag!=0){
					$error='Convertion to PNG failed';
				}else{
					@unlink($GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name']);
				}
			}else{
				//upload fail, prompt error message
				$error=$TEXT_ERROR_PICTURE_UPLOAD_FAILED_CONST.' '.$GLOBALS['orbiterBackgroundPath'].'.';
			}
		}
		
		if(isset($_REQUEST['dpic'])){
			if(!@unlink($GLOBALS['orbiterBackgroundPath'].$_REQUEST['dpic']))
				$error=$TEXT_ERROR_DELETE_PICTURE_FAILED_CONST.' '.$GLOBALS['orbiterBackgroundPath'].$_REQUEST['dpic'];
		}
		
		$sufix=(isset($error)?'&err='.$error:'&msg='.$TEXT_ORBITER_BACKGROUND_UPDATED_CONST);
		header("Location: index.php?section=customBackground&oID=$oID".$sufix);	
		exit();	
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CUSTOM_BACKGROUND_FOR_ORBITER_CONST.' '.$oID);
	$output->output();
}
?>

<?php
function userPic($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userPic.lang.php');
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	$filePath=$GLOBALS['usersPicsPath'].$userID.'.png';
	if(file_exists($filePath)){
		$randNumber=rand(0,99999);
		$userImage='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'">';
	}
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		
		<form action="index.php" method="post" name="userPic" enctype="multipart/form-data">
			<input type="hidden" name="section" value="userPic">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="userID" value="'.$userID.'">
		
		<h3>'.$TEXT_UPDATE_USER_PICTURE_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm">'.stripslashes(@$_GET['msg']).'</div>
							
		<table>	
			<tr>
				<td align="center">'.@$userImage.'</td>
			</tr>
			<tr>
				<td align="center">'.$TEXT_CHOOSE_PICTURE_CONST.' * <input type="file" name="pic"> <input type="submit" class="button" name="add" value="'.$TEXT_UPLOAD_PICTURE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
			<tr>
				<td align="left">* '.$TEXT_ALLOWED_FILE_TYPE_CONST.' <br>
		&nbsp;&nbsp;&nbsp;'.$TEXT_RATIO_NOTE_CONST.' <br>
		&nbsp;&nbsp;&nbsp;'.$TEXT_PIC_NOTE_CONST.'</td>
			</tr>		
		</table>
		</form>
			';
		
	} else {
		
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=userPic&from=$from&userID=$userID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
		
		if($_FILES['pic']['name']!=''){
			switch($_FILES['pic']['type']){
				case 'image/x-png':
				case 'image/png';
				case 'image/jpg':
				case 'image/pjpeg':
				case 'image/jpeg': $invalidType=false;
				break;
				default:
					$invalidType=true;
				break;
			}
			
			if($invalidType===true){
				header("Location: index.php?section=userPic&from=$from&userID=$userID&error=$TEXT_ERROR_USER_PIC_INVALID_FILE_TYPE_CONST");
				exit();
			}
			
			if(!move_uploaded_file($_FILES['pic']['tmp_name'],$filePath)){
				header("Location: index.php?section=userPic&from=$from&userID=$userID&error=$TEXT_ERROR_USER_PIC_UPLOAD_FAILED_CONST $filePath.");
				exit();
			}else{
				exec('chmod 777 -R '.$filePath);
				$resizeFlag=resizeImage($filePath, $filePath, 160, 160,1);
			}
			
			$out.='
			<script>
				opener.document.forms[0].action.value=\'form\';
				opener.document.forms[0].submit();
				self.close();
			</script>';

		}else{
			header("Location: index.php?section=userPic&from=$from&userID=$userID&error=$TEXT_PICTURE_NOT_SELECTED_CONST");
			exit();		
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_UPDATE_USER_PICTURE_CONST);			
	$output->output();	
}
?>

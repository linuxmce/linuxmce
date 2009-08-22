<?
function editContact($output,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$picture_path = $GLOBALS['contactPicturesPath'];
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$cid=(int)@$_REQUEST['cid'];

	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.@$_GET['error'].'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" enctype="multipart/form-data" name="editContact">
	<input type="hidden" name="section" value="editContact">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID,$cid).'
	</form>
	';
	} else {
		
		if(isset($_POST['editContact'])){
			$wasError = false;
			$Name=cleanString($_POST['Name']);
			$Company=cleanString($_POST['Company']);
			$Title=cleanString($_POST['Title']);
			$EK_Users=(isset($_POST['EK_Users']) && (int)$_POST['EK_Users']>0)?(int)$_POST['EK_Users']:NULL;

			$telecomADO->Execute('
				UPDATE Contact SET
					`Name`=?,
					`Company`=?,
					`Title`=?,
					`EK_Users`=?
				WHERE PK_Contact=?',
			array($Name,$Company,$Title,$EK_Users,$cid));

			if ($_FILES['Picture']['name'] != ''){

				if (!move_uploaded_file($_FILES['Picture']['tmp_name'],$picture_path.'/'.$cid.'.jpg')) {
					$msg='Unable to move uploaded contact picture into storage.';
					$wasError = true;
				}

				system('convert -resize 400x400 '.$picture_path.'/' . $cid. '.jpg '.$picture_path.'/'.$cid.'.png');

			}

			$msg='The contact was updated.';
		}	
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setMenuTitle($TEXT_PHONE_BOOK_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_CONTACT_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_BOOK_CONST=>'index.php?section=phoneBook',$TEXT_EDIT_CONTACT_CONST=>'index.php?section=editContact&cid='.$cid));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_CONTACT_CONST);
	$output->output();
}

// multipage display for phone numbers
function phoneNumberForm($telecomADO,$userID,$cid){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	if($cid<=0){
		return $TEXT_CONTACT_NOT_FOUND_CONST;
	}	

	$cData=getFieldsAsArray('Contact','Name,Company,Title,EK_Users',$telecomADO,'WHERE PK_Contact='.$cid);
	
	$out='
	<a href="index.php?section=phoneBook">'.$TEXT_PHONE_BOOK_CONST.'</a>
	<input type="hidden" name="cid" value="'.$cid.'">	
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td><B>'.$TEXT_NAME_CONST.' *</B></td>
			<td><input type="text" name="Name" value="'.$cData['Name'][0].'"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_COMPANY_CONST.'</B></td>
			<td><input type="text" name="Company" value="'.$cData['Company'][0].'"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_TITLE_CONST.'</B></td>
			<td><input type="text" name="Title" value="'.$cData['Title'][0].'"></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_CONTACT_PICTURE_CONST.'</B></td>
			<td><input type="file" name="Picture" value=""></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td><img src="'.(file_exists('/var/www/lmce-admin/contacts/' . $cid . '.png') ? '/lmce-admin/contacts/' . $cid . '.png' : '/lmce-admin/UnknownUser.png').'"></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_PRIVATE_CONTACT_CONST.'</B></td>
			<td><input type="checkbox" name="EK_Users" value="'.$userID.'" '.((!is_null($cData['EK_Users'][0]))?'checked':'').'></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="editContact" value="'.$TEXT_SAVE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>
	</table>
	<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editContact");
 			frmvalidator.addValidation("Name","req","'.$TEXT_CONTACT_NAME_REQUIRED_CONST.'");
		</script>			
	';
			
	return $out;
}
?>

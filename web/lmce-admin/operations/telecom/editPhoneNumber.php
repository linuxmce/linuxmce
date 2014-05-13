<?php
function editPhoneNumber($output,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$id=(int)@$_REQUEST['id'];

	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.stripslashes(@$_GET['error']).'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="editPhoneNumber">
	<input type="hidden" name="section" value="editPhoneNumber">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID).'
	</form>
	';
	} else {
		if(isset($_POST['editPhoneNumber'])){
			$contact=((int)@$_POST['contact']>0)?(int)$_POST['contact']:NULL;
			$PhoneType=((int)@$_POST['PhoneType']>0)?(int)$_POST['PhoneType']:NULL;
			$CountryCode=cleanString($_POST['CountryCode']);
			$AreaCode=cleanString($_POST['AreaCode']);
			$PhoneNumber=cleanString($_POST['PhoneNumber']);
			$Extension=cleanString($_POST['Extension']);
			$DialAs=cleanString($_POST['DialAs']);
			
			if($DialAs=='' && $PhoneNumber==''){
				header('Location: index.php?section=editPhoneNumber&error='.$TEXT_ERROR_PHONE_NUMBER_REQUIRED_CONST);
				exit();
			}
			
			$telecomADO->Execute('
				UPDATE PhoneNumber SET
					`FK_Contact`=?,
					`FK_PhoneType`=?,
					`CountryCode`=?,
					`AreaCode`=?,
					`PhoneNumber`=?,
					`Extension`=?,
					`DialAs`=?
				WHERE PK_PhoneNumber=?',
			array($contact,$PhoneType,$CountryCode,$AreaCode,$PhoneNumber,$Extension,$DialAs,$id));
			$msg=$TEXT_PHONE_NUMBER_UPDATED_CONST;
		}
		
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setMenuTitle($TEXT_PHONE_BOOK_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_PHONE_NUMBER_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_BOOK_CONST=>'index.php?section=phoneBook',$TEXT_EDIT_PHONE_NUMBER_CONST=>'index.php?section=editPhoneNumber&id='.$id));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PHONE_NUMBER_CONST);
	$output->output();
}

// multipage display for phone numbers
function phoneNumberForm($telecomADO,$userID){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	$contacts=getAssocArray('Contact','PK_Contact','Name',$telecomADO,'ORDER By Name ASC');
	$PhoneTypes=getAssocArray('PhoneType','PK_PhoneType','Description',$telecomADO);
	$id=(int)@$_REQUEST['id'];
	if($id<=0){
		return $TEXT_PHONE_NUMBER_NOT_FOUND_CONST;
	}
	
	$pnData=getFieldsAsArray('PhoneNumber','FK_Contact,FK_PhoneType,CountryCode,AreaCode,PhoneNumber,Extension,DialAs',$telecomADO,'WHERE PK_PhoneNumber='.$id);

	$out='
	<script>
		function setDialAs(){
			cc=document.editPhoneNumber.CountryCode.value;
			ac=document.editPhoneNumber.AreaCode.value;
			pn=document.editPhoneNumber.PhoneNumber.value;
			dialAs=((cc=="1")?"91":"9"+cc)+ac+pn;
			document.editPhoneNumber.DialAs.value=dialAs;
		}
	</script>	
	<a href="index.php?section=phoneBook">'.$TEXT_PHONE_BOOK_CONST.'</a>
	<input type="hidden" name="id" value="'.$id.'">
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td><B>'.$TEXT_CONTACT_CONST.' *</B></td>
			<td>'.pulldownFromArray($contacts,'contact',$pnData['FK_Contact'][0],'style="width:160px;"').'</td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_PHONE_TYPE_CONST.' *</B></td>
			<td>'.pulldownFromArray($PhoneTypes,'PhoneType',$pnData['FK_PhoneType'][0],'style="width:160px;"').'</td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_COUNTRY_CODE_CONST.'</B></td>
			<td><input type="text" name="CountryCode" value="'.$pnData['CountryCode'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_AREA_CODE_CONST.'</B></td>
			<td><input type="text" name="AreaCode" value="'.$pnData['AreaCode'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_PHONE_NUMBER_CONST.'</B></td>
			<td><input type="text" name="PhoneNumber" value="'.$pnData['PhoneNumber'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_EXTENSION_CONST.'</B></td>
			<td><input type="text" name="Extension" value="'.$pnData['Extension'][0].'"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_DIAL_AS_CONST.'</B></td>
			<td><input type="text" name="DialAs" value="'.$pnData['DialAs'][0].'"></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="editPhoneNumber" value="'.$TEXT_SAVE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>	
	</table>
	<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
	<script>
	 	var frmvalidator = new formValidator("editPhoneNumber");
		frmvalidator.addValidation("contact","dontselect=0","'.$TEXT_CONTACT_REQUIRED_CONST.'");
		frmvalidator.addValidation("PhoneType","dontselect=0","'.$TEXT_PHONE_TYPE_REQUIRED_CONST.'");
	</script>
	';
			
	return $out;
}
?>

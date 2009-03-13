<?
function addPhoneNumber($output,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$addContact=(int)@$_REQUEST['addContact'];
	
	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.stripslashes(@$_GET['error']).'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="addPhoneNumber">
	<input type="hidden" name="section" value="addPhoneNumber">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID,$addContact).'
	</form>
	';
	} else {
		if(isset($_POST['addPhoneNumber'])){
			$contact=((int)@$_POST['contact']>0)?(int)$_POST['contact']:NULL;
			$PhoneType=((int)@$_POST['PhoneType']>0)?(int)$_POST['PhoneType']:NULL;
			$CountryCode=cleanString($_POST['CountryCode']);
			$AreaCode=cleanString($_POST['AreaCode']);
			$PhoneNumber=cleanString($_POST['PhoneNumber']);
			$Extension=cleanString($_POST['Extension']);
			$DialAs=cleanString($_POST['DialAs']);
			if($DialAs=='' && $PhoneNumber==''){
				header('Location: index.php?section=addPhoneNumber&error='.$TEXT_ERROR_PHONE_NUMBER_REQUIRED_CONST);
				exit();
			}
			
			$telecomADO->Execute('
				INSERT INTO PhoneNumber
					(`FK_Contact`, `FK_PhoneType`, `CountryCode`, `AreaCode`, `PhoneNumber`, `Extension`, `DialAs`)
				VALUES
					(?,?,?,?,?,?,?)',
			array($contact,$PhoneType,$CountryCode,$AreaCode,$PhoneNumber,$Extension,$DialAs));
			$msg=$TEXT_PHONE_NUMBER_ADDED_CONST;
			header("Location: index.php?section=phoneBook&msg=".@$msg);
			exit();
		}
		
		if(isset($_POST['addContact'])){
			$Name=cleanString($_POST['Name']);
			$Company=cleanString($_POST['Company']);
			$Title=cleanString($_POST['Title']);
			$EK_Users=(isset($_POST['EK_Users']) && (int)$_POST['EK_Users']>0)?(int)$_POST['EK_Users']:NULL;
			
			$telecomADO->Execute('
				INSERT INTO Contact
					(`Name`, `Company`, `Title`, `EK_Users`)
				VALUES
					(?,?,?,?)',
			array($Name,$Company,$Title,$EK_Users));
			$msg=$TEXT_CONTACT_WAS_ADDED_CONST;
		}	
		header("Location: index.php?section=addPhoneNumber&msg=".@$msg);
	}
	$title=($addContact==0)?$TEXT_ADD_PHONE_NUMBER_CONST:$TEXT_ADD_CONTACT_CONST;
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_BOOK_CONST=>'index.php?section=phoneBook',$title=>'index.php?section=addPhoneNumber&addContact='.$addContact));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$title);
	$output->output();
}

// multipage display for phone numbers
function phoneNumberForm($telecomADO,$userID,$addContact){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	$userID = (int)@$_SESSION['userID'];
	
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	$contacts=getAssocArray('Contact','PK_Contact','Name',$telecomADO,'WHERE EK_Users IS NULL OR EK_Users='.$userID);
	$PhoneTypes=getAssocArray('PhoneType','PK_PhoneType','Description',$telecomADO);
	
	if($addContact==0){
		$out='
	<script>
		function setDialAs(){
			cc=document.addPhoneNumber.CountryCode.value;
			ac=document.addPhoneNumber.AreaCode.value;
			pn=document.addPhoneNumber.PhoneNumber.value;
			dialAs=((cc=="1")?"91":"9011"+cc)+ac+pn;
			document.addPhoneNumber.DialAs.value=dialAs;
		}
	</script>
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td colspan="2"><h3>'.$TEXT_ADD_PHONE_NUMBER_CONST.'</h3></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_CONTACT_CONST.' *</B></td>
			<td>'.pulldownFromArray($contacts,'contact',0).'</td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_PHONE_TYPE_CONST.' *</B></td>
			<td>'.pulldownFromArray($PhoneTypes,'PhoneType',0).'</td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_COUNTRY_CODE_CONST.'</B></td>
			<td><input type="text" name="CountryCode" value="1" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_AREA_CODE_CONST.'</B></td>
			<td><input type="text" name="AreaCode" value="" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_PHONE_NUMBER_CONST.'</B></td>
			<td><input type="text" name="PhoneNumber" value="" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_EXTENSION_CONST.'</B></td>
			<td><input type="text" name="Extension" value=""></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_DIAL_AS_CONST.'</B></td>
			<td><input type="text" name="DialAs" value=""></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="addPhoneNumber" value="'.$TEXT_ADD_PHONE_NUMBER_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>	
		</table>
	<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
	<script>
		 	var frmvalidator = new formValidator("addPhoneNumber");
			frmvalidator.addValidation("contact","dontselect=0","'.$TEXT_CONTACT_REQUIRED_CONST.'");
 			frmvalidator.addValidation("PhoneType","dontselect=0","'.$TEXT_PHONE_TYPE_REQUIRED_CONST.'");
	</script>';
	}else{
		$out='
	<input type="hidden" name="section" value="addPhoneNumber">
	<input type="hidden" name="action" value="add">	
	
	<table celspacing="0" cellpadding="3" align="center">		
		<tr>
			<td colspan="2"><h3>'.$TEXT_ADD_CONTACT_CONST.'</h3></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_NAME_CONST.' *</B></td>
			<td><input type="text" name="Name" value=""></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_COMPANY_CONST.'</B></td>
			<td><input type="text" name="Company" value=""></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_TITLE_CONST.'</B></td>
			<td><input type="text" name="Title" value=""></td>
		</tr>	
		<tr>
			<td><B>'.$TEXT_PRIVATE_CONTACT_CONST.'</B></td>
			<td><input type="checkbox" name="EK_Users" value="'.$userID.'"></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="addContact" value="'.$TEXT_ADD_CONTACT_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>
	</table>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addPhoneNumber");
 			frmvalidator.addValidation("Name","req","'.$TEXT_CONTACT_NAME_REQUIRED_CONST.'");
		</script>			
		';
	}
			
	return $out;
}
?>
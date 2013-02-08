<?
function callersForMe($output,$dbADO,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
	
	global 	$stepsArray;

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	
	if($action=='form'){
		$out=getCallersForMeTable($dbADO,$telecomADO);
	}elseif($action=='addCaller'){
		$out=addCallerForm($dbADO,$telecomADO);
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=callersForMe&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}			
		
		if(isset($_REQUEST['add'])){
			processAddCallerStep($telecomADO);
		}
		
		if(isset($_REQUEST['did']) && (int)$_REQUEST['did']!=0){
			deleteCaller((int)$_REQUEST['did'],$telecomADO);
		}
		
		if(isset($_REQUEST['update'])){
			updateCallers($telecomADO);
		}
		
		header('Location: index.php?section=callersForMe');
	}

	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($TEXT_CALLERS_FOR_ME_CONST);
	$output->setNavigationMenu(array($TEXT_CALLERS_FOR_ME_CONST=>'index.php?section=callersForMe'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getCallersForMeArray($telecomADO){
	$res=$telecomADO->Execute('SELECT * FROM CallersForMe ORDER BY EK_Users ASC');
	$pn=array();
	while($row=$res->FetchRow()){
		$pn[$row['EK_Users']][$row['PK_CallersForMe']]=$row['PhoneNumber'];
	}
	
	return $pn;
}

function getCallersForMeTable($dbADO,$telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
	
	$jsValidation='';	
	$out='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="callersForMe">
			<input type="hidden" name="section" value="callersForMe">
			<input type="hidden" name="action" value="update">
	
	';
	$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);	
	$phoneNumbers=getCallersForMeArray($telecomADO);
	
	$out.='
	<table align="center" cellpadding="2" cellspacing="0">';
	$idArray=array();
	if(count($users)>0){
		foreach ($users AS $userID=>$username){
	
			$jsValidation.='frmvalidator.addValidation("new_phone_'.$userID.'","numeric","'.$TEXT_WARNING_NUMERICAL_ONLY_CONST.'");';
			$out.='
			<tr>
				<td colspan="2" align="center" class="tablehead"><B>'.$username.'</B></td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td align="center"><B>Phone number</B></td>
				<td align="center"><B>Action</B></td>
			</tr>';
			if(count(@$phoneNumbers[$userID])>0){
				foreach ($phoneNumbers[$userID] AS $id=>$number){
					$idArray[]=$id;
					$out.='
					<tr>
						<td><input type="text" name="phone_'.$id.'" value="'.$number.'"></td>
						<td><input type="button" class="button" name="del" value="'.$TEXT_DELETE_CONST.'" onClick="if(confirm(\''.$TEXT_DELETE_CALLER_FOR_ME_NUMBER_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=callersForMe&action=del&did='.$id.'\'"></td>
					</tr>';
					$jsValidation.='frmvalidator.addValidation("phone_'.$id.'","numeric","'.$TEXT_WARNING_NUMERICAL_ONLY_CONST.'");';
				}
			}
			$out.='
			<tr >
				<td><input type="text" name="new_phone_'.$userID.'" value=""></td>
				<td align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
			</tr>		
			<tr>
				<td colspan="2">&nbsp;</td>
			</tr>';
		}
	}
	if(count($idArray)>0){
	$out.='	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>';
	}
	$out.='
	</table>
	<input type="hidden" name="usersArray" value="'.join(',',array_keys($users)).'">
	<input type="hidden" name="idArray" value="'.join(',',$idArray).'">
	</form>
	<script>
	 	var frmvalidator = new formValidator("callersForMe");
		'.$jsValidation.'
	</script>	
	';
	
	return $out;
}

function processAddCallerStep($telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
		
	$usersArray=explode(',',$_POST['usersArray']);
	$added=0;
	foreach ($usersArray AS $userID){
		if($_POST['new_phone_'.$userID]!=''){
			$telecomADO->Execute('INSERT INTO CallersForMe (EK_Users,PhoneNumber) VALUES (?,?)',array($userID,$_POST['new_phone_'.$userID]));
			$added++;
		}
	}
	if($added>0){
		header('Location: index.php?section=callersForMe&msg='.$TEXT_CALLER_FOR_ME_ADDED_CONST);
	}else{
		header('Location: index.php?section=callersForMe&err='.$TEXT_ERROR_NOTHING_TO_ADD_CONST);
	}
	exit();
}

function deleteCaller($id,$telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
		
	$telecomADO->Execute('DELETE FROM CallersForMe WHERE PK_CallersForMe=?',$id);
	
	header('Location: index.php?section=callersForMe&msg='.$TEXT_CALLER_FOR_ME_DELETED_CONST);
	exit();
}

function updateCallers($telecomADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/callersForMe.lang.php');
	
	$idArray=explode(',',$_POST['idArray']);
	foreach ($idArray AS $id){
		if($_POST['phone_'.$id]!=''){
			$telecomADO->Execute('UPDATE CallersForMe SET PhoneNumber=? WHERE PK_CallersForMe=?',array($_POST['phone_'.$id],$id));
		}
	}
	header('Location: index.php?section=callersForMe&msg='.$TEXT_CALLER_FOR_ME_UPDATED_CONST);
	exit();
	
}
?>
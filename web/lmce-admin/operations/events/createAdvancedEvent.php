<?php
function createAdvancedEvent($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/createAdvancedEvent.lang.php');
	
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	

	if ($action=='form') {
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="createAdvancedEvent">
		<input type="hidden" name="section" value="createAdvancedEvent">
		<input type="hidden" name="action" value="add">';
		
		$queryEvents='
			SELECT Event.*, EventCategory.Description AS EventCategory
			FROM Event 
			INNER JOIN EventCategory ON FK_EventCategory=PK_EventCategory
			ORDER By EventCategory.Description ASC, Event.Description ASC';
		$resEvents=$dbADO->Execute($queryEvents);
		$out.='
			<table>
			<tr>
				<td colspan="3">&nbsp;</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B>*</td>
				<td colspan="2"><input type="text" name="Description" value=""></td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_NEW_EVENT_CONST.'</B> </td>
				<td colspan="2"><select name="event">
					<option value="0">- '.$TEXT_PLEASE_SELECT_AN_EVENT_CONST.' -</option>';
		$optgroup='';
		while($row=$resEvents->FetchRow()){
			if($optgroup!=$row['EventCategory']){
				$out.='<optgroup label="'.$row['EventCategory'].'">';
				$optgroup=$row['EventCategory'];
			}
			$out.='<option value="'.$row['PK_Event'].'">'.$row['Description'].'</option>';
		}
		$out.='
			</select></td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="continue" value="'.$TEXT_ADD_CONST.'"></td>
			</tr>';
		$out.='
		</table>
		</form>
		<em>'.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("createAdvancedEvent");			
 			frmvalidator.addValidation("Description","req","'.$TEXT_PLEASE_ENTER_THE_DESCRIPTION_CONST.'");
			frmvalidator.addValidation("event","dontselect=0","'.$TEXT_PLEASE_SELECT_AN_EVENT_CONST.'!");
		</script>		
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventHandler&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
		$description=cleanString($_POST['Description']);
		$eventID=cleanInteger($_POST['event']);
		
		$ehID=createEventHandler($description,$eventID,$installationID,$dbADO,0);
		header("Location: index.php?section=advancedEvents&highligh=$ehID");
		exit();
	}
	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_CREATE_ADVANCED_EVENT_CONST);
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CREATE_ADVANCED_EVENT_CONST);			
	$output->output();
}
?>

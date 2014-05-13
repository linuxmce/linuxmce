<?php
function restrictAccess($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/restrictAccess.lang.php');

	/* @var $dbADO ADOConnection */
	// WARNING: the checkboxes behavior is reversed, checked = Room_User doesn't exist
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);
	$roomsArray=getArrayFromTable('Room','PK_Room','Description',$dbADO,'WHERE HideFromOrbiter=0 AND FK_Installation='.$installationID,'ORDER BY Description ASC');
	
	$usersArray=getFieldsAsArray('Users','PK_Users,Username,RequirePinToSelect',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.$installationID,'ORDER BY Username ASC');
	$usersAssocArray=array();
	for($i=0;$i<count($usersArray['PK_Users']);$i++){
		$usersAssocArray[$usersArray['PK_Users'][$i]]=$usersArray['Username'][$i];
	}

	$orbitersArray=getDevicesArrayFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
	$addToRoom=(int)@$_REQUEST['addToRoom'];
	
	$out.='
	<p>'.$TEXT_RESTRICT_ACCESS_INFO_CONST.'</p>';

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>
		<p class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'
		<div align="left" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		<form action="index.php" method="post" name="restrictAccess">
		<input type="hidden" name="section" value="restrictAccess">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="addToRoom" value="'.@$_REQUEST['addToRoom'].'">
		<input type="hidden" name="delR" value="">
		
		<p><B>'.$TEXT_ROOMS_CONST.'</B>
		<table cellpadding="3" cellspacing="0">
			<tr>
				<td colspan="2" bgcolor="#F0F3F8" align="center"><h3>-- Restrict access to rooms --</h3></td>
			</tr>';
		foreach($roomsArray AS $roomID=>$roomName){
			$rs=$dbADO->Execute('SELECT * FROM Room_Users WHERE FK_Room=?',$roomID);
			$restrictions='';
			if($rs->RecordCount()==0){
				$roomLabel=$TEXT_FULL_ACCESS_TO_EVERYONE_CONST;
				$roomButton='<input type="button" class="button" name="addRestrict" value="'.$TEXT_RESTRICT_ACCESS_TO_CONST.'" onClick="document.restrictAccess.action.value=\'form\';document.restrictAccess.addToRoom.value='.$roomID.';document.restrictAccess.submit();">';
			}else{
				$roomLabel='Only allow access to';
				$roomButton='<input type="button" class="button" name="addRestrict" value="'.$TEXT_ADD_ACCESS_CONST.'" onClick="document.restrictAccess.action.value=\'form\';document.restrictAccess.addToRoom.value='.$roomID.';document.restrictAccess.submit();">';
				while($rowR=$rs->FetchRow()){
					$restrictions.='
				<tr>
					<td>User: '.pulldownFromArray($usersAssocArray,'user',$rowR['FK_Users'],'','key','-- '.$TEXT_ANY_CONST.' --').'</td>
					<td>using orbiter '.pulldownFromArray($orbitersArray,'orbiter',$rowR['FK_Orbiter'],'','key','-- '.$TEXT_ANY_CONST.' --').' <input type="button" class="button" name="del" value="Delete" onClick="if(confirm(\'Are you sure you want to delete this restriction?\')){document.restrictAccess.delR.value='.$rowR['PK_Room_Users'].';document.restrictAccess.submit();}"></td>
				</tr>';
				}
			}
			
			$out.='
			<tr>
				<td class="tablehead" colspan="2"><B>'.$roomName.'</B></td>
			</tr>
			<tr>
				<td>'.$roomLabel.'</td>
				<td align="right">'.$roomButton.'</td>
			</tr>';
			$out.=@$restrictions;
			
			if($addToRoom==$roomID){
				$out.='
				<tr bgcolor="lightyellow">
					<td>User: '.pulldownFromArray($usersAssocArray,'adduser',0,'','key','-- '.$TEXT_ANY_CONST.' --').'</td>
					<td>using orbiter '.pulldownFromArray($orbitersArray,'addorbiter',0,'','key','-- '.$TEXT_ANY_CONST.' --').' <input type="submit" class="button" name="submit" value="Save"></td>
				</tr>';
			}
			$out.='
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>';
		}
		$out.='
			<tr>
				<td colspan="2" bgcolor="#F0F3F8" align="center"><h3>-- '.$TEXT_RESTRICT_ACCESS_TO_USERS_CONST.' --</h3></td>
			</tr>
			<tr bgcolor="#EFEFEF">
				<td align="center"><B>'.$TEXT_USER_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_REQUIRE_PIN_CODE_CONST.'</B></td>
			</tr>';
		
		for($i=0;$i<count($usersArray['PK_Users']);$i++){
			$color=($i%2==0)?'#FFFFFF':'#FCFFEF';
			$out.='
			<tr bgcolor="'.$color.'">
				<td align="center"><B>'.$usersArray['Username'][$i].'</B></td>
				<td align="center"><input type="checkbox" value="1" name="user_'.$usersArray['PK_Users'][$i].'" '.(($usersArray['RequirePinToSelect'][$i]==1)?'checked':'').' onClick="document.restrictAccess.submit();"></td>
			</tr>';
			if($usersArray['RequirePinToSelect'][$i]==1){
				$checkedOrbiters=array();
				$qid=$dbADO->Execute('SELECT * FROM Room_Users WHERE FK_Users=? AND FK_Room IS NULL',$usersArray['PK_Users'][$i]);
				while($rowOrb=$qid->FetchRow()){
					$checkedOrbiters[]=$rowOrb['FK_Orbiter'];
				}
				$orbiters='';
				foreach ($orbitersArray AS $orbiterID=>$orbiterName){
					$orbiters.=(in_array($orbiterID,$checkedOrbiters))?'<input type="hidden" name="old_orbiter_'.$usersArray['PK_Users'][$i].'_'.$orbiterID.'" value="1">':'';
					$orbiters.='<input type="checkbox" value="1" name="orbiter_'.$usersArray['PK_Users'][$i].'_'.$orbiterID.'" '.((in_array($orbiterID,$checkedOrbiters))?'checked':'').' onClick="document.restrictAccess.submit();"> '.$orbiterName.' ';
				}				
				$out.='
				<tr bgcolor="'.$color.'">
					<td colspan="2"><fieldset><legend>'.$TEXT_DONT_REQUIRE_PIN_CODE_WITH_ORBITERS_CONST.'</legend>'.$orbiters.'</fieldset></td>
				</tr>';
				
			}
		}
		
		$out.='
		</table>
		</form>';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=restrictAccess&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$user=((int)@$_POST['adduser']>0)?(int)$_POST['adduser']:NULL;
		$orbiter=((int)@$_POST['addorbiter']>0)?(int)$_POST['addorbiter']:NULL;
		if($addToRoom!=0){
			$dbADO->Execute('INSERT INTO Room_Users (FK_Room,FK_Users,FK_Orbiter) VALUES (?,?,?)',array($addToRoom,$user,$orbiter));
		}

		$delR=(int)@$_POST['delR'];
		if($delR!=0){
			$dbADO->Execute('DELETE FROM Room_Users WHERE PK_Room_Users=?',$delR);
		}

		// restrict access to users
		for($i=0;$i<count($usersArray['PK_Users']);$i++){
			$RequirePinToSelect=(int)@$_POST['user_'.$usersArray['PK_Users'][$i]];
			$dbADO->Execute('UPDATE Users SET RequirePinToSelect=? WHERE PK_Users=?',array($RequirePinToSelect,$usersArray['PK_Users'][$i]));
			
			if($RequirePinToSelect==0 && $usersArray['RequirePinToSelect'][$i]==1){
				// delete records from Room_Users
				$dbADO->Execute('DELETE FROM Room_Users WHERE FK_Room IS NULL AND FK_Users=?',$usersArray['PK_Users'][$i]);
			}
			
			// add/delete records in Room_Users
			foreach ($orbitersArray AS $orbiterID=>$orbiterName){
				$restrOrb=(int)@$_POST['orbiter_'.$usersArray['PK_Users'][$i].'_'.$orbiterID];
				$oldOrb=(int)@$_POST['old_orbiter_'.$usersArray['PK_Users'][$i].'_'.$orbiterID];
				if($restrOrb==0){
					$dbADO->Execute('DELETE FROM Room_Users WHERE FK_Room IS NULL AND FK_Users=? AND FK_Orbiter=?',array($usersArray['PK_Users'][$i],$orbiterID));
				}elseif($oldOrb==0){
					$dbADO->Execute('INSERT INTO Room_Users (FK_Room,FK_Users,FK_Orbiter) VALUES (NULL,?,?)',array($usersArray['PK_Users'][$i],$orbiterID));
				}
			}
		}
		//Orbiters need regenerated to activate changes
		setOrbitersNeedConfigure($installationID,$dbADO);

		header("Location: index.php?section=restrictAccess&msg=$TEXT_RESTRICTIONS_UPDATED_CONST");

	}

	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_RESTRICT_ACCESS_CONST);
	$output->setNavigationMenu(array($TEXT_RESTRICT_ACCESS_CONST=>'index.php?section=restrictAccess'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_RESTRICT_ACCESS_CONST);
	$output->output();  		
}
?>

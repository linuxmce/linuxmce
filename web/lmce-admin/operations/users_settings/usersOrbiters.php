<?php
function usersOrbiters($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/usersOrbiters.lang.php');

	/* @var $dbADO ADOConnection */
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);

	$usersArray=array();
	$checkedArray=array();
	$resUsers=$dbADO->Execute('
		SELECT * FROM Users 
		INNER JOIN Installation_Users ON FK_Users=PK_Users
		WHERE FK_Installation=?
		ORDER BY UserName ASC',$installationID);
	while($rowUsers=$resUsers->FetchRow()){
		$usersArray[$rowUsers['PK_Users']]=$rowUsers['UserName'];
		$checkedArray[$rowUsers['PK_Users']][]=0;
	}
	
	$orbiterDTArray=getDeviceTemplatesFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
	if(count($orbiterDTArray)==0)
		$orbiterDTArray[]=0;
	$resDevices=$dbADO->Execute('
		SELECT * FROM Device 
		WHERE FK_DeviceTemplate IN ('.join(',',$orbiterDTArray).') AND FK_Installation=?',$installationID);
	$orbitersArray=array();
	while($rowDevices=$resDevices->FetchRow()){
		$orbitersArray[$rowDevices['PK_Device']]=$rowDevices['Description'];
	}
	$queryOrbitersArray=$orbitersArray;
	if(count($queryOrbitersArray)==0)
		$queryOrbitersArray[]=0;
	$queryUsersOrbiters='SELECT * FROM Device_Users WHERE FK_Device IN ('.join(',',array_keys($queryOrbitersArray)).') ORDER BY FK_Users ASC';
	$resUsersOrbiters=$dbADO->Execute($queryUsersOrbiters);
	while($rowUsersOrbiters=$resUsersOrbiters->FetchRow()){
		$checkedArray[$rowUsersOrbiters['FK_Users']][]=$rowUsersOrbiters['FK_Device'];
	}
	
	$out.='
	<p>'.$TEXT_PRIVACY_SETTINGS_INFO_CONST.'</p>';

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>
		<p class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'
		<div align="left" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		<form action="index.php" method="post" name="usersOrbiters">
		<input type="hidden" name="section" value="usersOrbiters">
		<input type="hidden" name="action" value="add">
		
		<p><B>'.$TEXT_ORBITERS_CONST.'</B>
		<table cellpadding="3" cellspacing="0">
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_USER_ORBITER_CONST.'</B></td>';
		foreach($orbitersArray AS $orbiterName){
			$out.='<td align="center"><B>'.stripslashes($orbiterName).'</B></td>';
		}
		$out.='
			</tr>
		';
		$userCount=0;
		foreach($usersArray AS $userID=>$username){
			$userCount++;
			$out.='
			<tr bgcolor="'.(($userCount%2==0)?'#F0F3F8':'').'">
				<td><B>'.$username.'</B></td>';
			foreach($orbitersArray AS $orbiterID=>$orbiterName){
				$out.='<td align="center"><input type="checkbox" name="user_'.$userID.'_orbiter_'.$orbiterID.'" value="1" onClick="document.usersOrbiters.submit();" '.((in_array($orbiterID,$checkedArray[$userID]))?'checked':'').'></td>';
			}
		$out.='
			</tr>';
		}
		$out.='
		</table>
		</form>';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=usersOrbiters&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		foreach($usersArray AS $userID=>$username){
			foreach($orbitersArray AS $orbiterID=>$orbiterName){
				if(isset($_POST['user_'.$userID.'_orbiter_'.$orbiterID])){
					if(!in_array($orbiterID,$checkedArray[$userID]))
						$dbADO->Execute('INSERT INTO Device_Users (FK_Users,FK_Device) VALUES (?,?)',array($userID,$orbiterID));
				}else{
					if(in_array($orbiterID,$checkedArray[$userID]))
						$dbADO->Execute('DELETE FROM Device_Users WHERE FK_Users=? AND FK_Device=?',array($userID,$orbiterID));
				}
			}
		}
		//Orbiters need regenerated to activate changes
		setOrbitersNeedConfigure($installationID,$dbADO);

		header("Location: index.php?section=usersOrbiters&msg=$TEXT_USER_ORBITER_UPDATED_CONST");
	}

	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_USER_ORBITER_CONST);
	$output->setNavigationMenu(array($TEXT_USER_ORBITER_CONST=>'index.php?section=usersOrbiters'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PRIVACY_SETTINGS_CONST);			
	$output->output();  		
}
?>

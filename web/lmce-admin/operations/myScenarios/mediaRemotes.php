<?php
function mediaRemotes($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaRemotes.lang.php');

	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	if ($action=='form') {

		$out.='

		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="mediaRemotes">
		<input type="hidden" name="section" value="mediaRemotes">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="drc" value="">
		
<span style="padding-left:10px; padding-right:10px;">
<h3>'.$TEXT_CHOOSE_PREFERRED_REMOTES_CONST.'</h3>		
<p>'.$TEXT_PREFERRED_REMOTES_INFO_CONST.'
</span>
<br><br>';
		$orbitersArray=getDevicesArrayFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
		$avDevicesArray=getDevicesArrayFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
		$remotesArray=array();
		if((int)@$_POST['mediaType']!=0){
			$mtArray=getFieldsAsArray('MediaType_DesignObj','FK_DesignObj_Popup,DesignObj.Description,Screen.Description AS Screen',$dbADO,'LEFT JOIN DesignObj ON FK_DesignObj_Popup=PK_DesignObj LEFT JOIN Screen on FK_Screen_OSD=PK_Screen WHERE FK_MediaType='.(int)@$_POST['mediaType']);
			for($i=0;$i<count($mtArray['FK_DesignObj_Popup']);$i++){
				$remotesArray[$mtArray['FK_DesignObj_Popup'][$i].':mt_do']=$mtArray['Description'][$i].(($mtArray['Screen'][$i]!='')?'/'.$mtArray['Screen'][$i]:'');
			}
		}


		if((int)@$_POST['device']!=0){
			$dt_mtArray=getFieldsAsArray('DeviceTemplate_MediaType','DesignObj.PK_DesignObj,DesignObj.Description',$dbADO,'
		INNER JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate
		INNER JOIN DeviceTemplate_MediaType_DesignObj ON FK_DeviceTemplate_MediaType=PK_DeviceTemplate_MediaType
		INNER JOIN DesignObj ON DeviceTemplate_MediaType_DesignObj.FK_DesignObj_Popup=PK_DesignObj
		WHERE PK_Device='.(int)@$_POST['device']);
			for($i=0;$i<count(@$dt_mtArray['PK_DesignObj']);$i++){
				$remotesArray[$dt_mtArray['PK_DesignObj'][$i].':dt_mt_do']=$dt_mtArray['Description'][$i];
			}
		}

		$remotesTable='<table>';
		$res=$dbADO->Execute('
	SELECT PK_RemoteControl,mt.Description AS mt,dt_mt.Description AS dt_mt, FK_MediaType_DesignObj,FK_DeviceTemplate_MediaType_DesignObj
	FROM RemoteControl
	LEFT JOIN DesignObj mt ON FK_MediaType_DesignObj=mt.PK_DesignObj
	LEFT JOIN DesignObj dt_mt ON FK_DeviceTemplate_MediaType_DesignObj=dt_mt.PK_DesignObj');
		while($row=$res->FetchRow()){
			$remotesTable.='
		<tr>
			<td>'.(!is_null($row['FK_MediaType_DesignObj'])?$row['mt']:$row['dt_mt']).'</td>
			<td><a href="javascript:if(confirm(\''.$TEXT_REMOVE_REMOTE_CONFIRMATION_CONST.'\')){document.mediaRemotes.drc.value='.$row['PK_RemoteControl'].';document.mediaRemotes.submit();}">Delete</a></td>
		</tr>
	';
		}
		$remotesTable.='</table>';


		$out.='
<table>
	<tr>
		<td><B>'.$TEXT_EXISTING_REMOTES_CONST.'</B></td>
		<td></td>
	</tr>
	<tr>
		<td>&nbsp;</td>
		<td>'.$remotesTable.'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_FOR_THIS_TYPE_OF_MEDIA_CONST.'</B></td>
		<td>'.generatePullDown('mediaType','MediaType','PK_MediaType','Description',(int)@$_POST['mediaType'],$dbADO,'','onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"').'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_BEING_PLAYED_ON_THIS_ORBITER_CONST.'</B></td>
		<td>'.pulldownFromArray($orbitersArray,'orbiter',(int)@$_POST['orbiter'],'onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"','key','- Any orbiter -').'</td>
	</tr>
	<tr>
		<td><B>'.$TEXT_WITH_THIS_DEVICE_AS_THE_SOURCE_CONST.'</B></td>
		<td>'.pulldownFromArray($avDevicesArray,'device',(int)@$_POST['device'],'onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"','key','- Any device -').'</td>
	</tr>		
	<tr>
		<td><B>'.$TEXT_USE_THIS_REMOTE_CONST.'</B></td>
		<td>'.pulldownFromArray($remotesArray,'remote',0,'','key','- '.$TEXT_PLEASE_SELECT_CONST.' -').'</td>
	</tr>
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="addPreference" value="'.$TEXT_ADD_PREFERENCE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
	</tr>
</table>		
		</form>
		';

	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=mediaRemotes&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$mediaType=(int)$_POST['mediaType'];
		$orbiter=((int)$_POST['orbiter']>0)?(int)$_POST['orbiter']:NULL;
		$device=(int)$_POST['device'];
		$remote=$_POST['remote'];

		if($remote!='0'){
			$parts=explode(':',$remote);
			$remoteID=@$parts[0];
			if($remoteID!=''){
				if($parts[1]=='dt_mt_do')
				$dbADO->Execute('INSERT INTO RemoteControl (FK_Orbiter,FK_DeviceTemplate_MediaType_DesignObj) VALUES (?,?)',array($orbiter,$remoteID));
				else
				$dbADO->Execute('INSERT INTO RemoteControl (FK_Orbiter,FK_MediaType_DesignObj) VALUES (?,?)',array($orbiter,$remoteID));

				header('Location: index.php?section=mediaRemotes&msg='.$TEXT_REMOTE_ADDED_CONST);
				exit();
			}
		}else{
			if((int)@$_POST['drc']>0){
				$dbADO->Execute('DELETE FROM RemoteControl WHERE PK_RemoteControl=?',(int)$_POST['drc']);

				header('Location: index.php?section=mediaRemotes&msg='.$TEXT_REMOTE_DELETED_CONST);
				exit();
			}

			header('Location: index.php?section=mediaRemotes&error='.$TEXT_NO_REMOTE_SELECTED_CONST);
			exit();
		}
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_CHOOSE_PREFERRED_REMOTES_CONST);
	$output->output();
}
?>

<?php
function resyncCodes($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/resyncCodes.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = isset($_SESSION['installationID'])?cleanInteger($_SESSION['installationID']):0;
 	$dtID=(int)@$_REQUEST['dtID'];
	$from=@$_REQUEST['from'];
 	
	if ($action == 'form') {
		$selectDT='
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				FK_InfraredGroup,
				DeviceTemplate_AV.*,
				DeviceTemplate_AV.FK_DeviceTemplate AS AVTemplate
			FROM DeviceTemplate 
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?';
		$resDTData=$dbADO->Execute($selectDT,array($dtID));		
		$rowDTData=$resDTData->FetchRow();
			
		$out.='
			<div class="err">'.@$_GET['error'].'</div>
			<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
		
		<form action="index.php" method="POST" name="resyncCodes">
		<input type="hidden" name="section" value="resyncCodes">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="dtID" value="'.$dtID.'">	
		<input type="hidden" name="from" value="'.$from.'">	
		
		<table border="0" width="100%" class="normaltext">
			<tr>
				<td valign="top" colspan="2">'.$TEXT_DEVICE_TEMPLATE_CONST.' <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>
			<tr>
				<td valign="top" colspan="2"><input type="checkbox" name="override" value="1">'.$TEXT_OVERRIDE_EXISTING_CODES_CONST.'<td>
			</tr>
			<tr>
				<td valign="top" colspan="2">
					<input type="submit" name="sync" value="'.$TEXT_RESYNC_CODES_CONST.'" class="button">
					<input type="button" name="backbtn" value="'.$TEXT_BACK_CONST.'" onClick="self.location=\'index.php?section='.$from.'\'" class="button">
				<td>
			</tr>
		</table>		
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=resyncCodes&from=$from&dtID=$dtID");
			exit(0);
		}
		
		if(isset($_POST['sync'])){
			$override=(int)@$_POST['override'];
			
			if($override==1){
				// if override checkbox is checked, reset psc_mod for all codes that are in his InfraredGroup and had psc_batch positive
				$codesToReset=getAssocArray('InfraredGroup_Command','PK_InfraredGroup_Command','PK_InfraredGroup_Command',$dbADO,'INNER JOIN DeviceTemplate on DeviceTemplate.FK_InfraredGroup=InfraredGroup_Command.FK_InfraredGroup WHERE PK_DeviceTemplate='.$dtID.' AND InfraredGroup_Command.psc_batch>0 AND InfraredGroup_Command.psc_mod!=0');

				if(count($codesToReset)>0){
					$dbADO->Execute('UPDATE InfraredGroup_Command SET psc_mod=0 WHERE PK_InfraredGroup_Command IN ('.join(',',array_keys($codesToReset)).')');
				}
			}

					
			$isSync=GetIRCodesForDevice(NULL,$dbADO,$dtID);
			if($isSync!=0){
				header("Location: index.php?section=resyncCodes&from=$from&dtID=$dtID&error=".urlencode($TEXT_SYNCRONIZATION_FAILED_CONST));
				exit();
			}
			
			header("Location: index.php?section=resyncCodes&from=$from&dtID=$dtID&msg=".urlencode($TEXT_SYNCRONIZATION_SUCCESS_CONST));
			exit();
		}
		
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_RESYNC_CODES_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_RESYNC_CODES_CONST=>'index.php?section=resyncCodes&from='.$from.'&dtID='.$dtID));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_RESYNC_CODES_CONST);			
	$output->output();  	
}
?>
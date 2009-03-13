<?
function commMethod($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/commMethod.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$dtID=(int)$_REQUEST['dtID'];
	$from=cleanString($_REQUEST['from']);
	
	if($action=='form'){

					
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3>'.$TEXT_COMM_METHOD_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>		
		<form action="index.php" method="POST" name="commMethod">
		<input type="hidden" name="section" value="commMethod">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="dtID" value="'.$dtID.'">
		<input type="hidden" name="from" value="'.htmlentities($from).'">
		
		
		<table cellpadding="3" align="left">';
		if($dtID==0){
			$out.='
			<tr>
				<td colspan="2" class="error">'.$TEXT_DEVICE_TEMPLATE_NOT_SPECIFIED_CONST.'</td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="button" class="button" name="cancel" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
			';
		}
		
		$commMethodsArray=getAssocArray('CommMethod','PK_CommMethod','Description',$dbADO,'','ORDER BY Description ASC');
		
		$selectDT='
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				FK_InfraredGroup,
				DeviceTemplate_AV.*,
				DeviceTemplate_AV.FK_DeviceTemplate AS AVTemplate,
				DeviceTemplate.FK_CommMethod AS FK_CommMethod
			FROM DeviceTemplate 
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?';
		$resDTData=$dbADO->Execute($selectDT,array($dtID));
		if($resDTData->RecordCount()==0){
			$out.='
			<tr>
				<td colspan="2" class="error">'.$TEXT_DEVICE_TEMPLATE_NOT_FOUND_CONST.'</td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="button" class="button" name="cancel" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
			';
		}else{
			$row=$resDTData->FetchRow();
			$out.='
			<tr>
				<td class="tablehead" colspan="2"><b>'.$TEXT_DEVICE_TEMPLATE_CONST.' #'.$dtID.'</b></td>
			</tr>			
			<tr>
				<td><b>'.$TEXT_DESCRIPTION_CONST.'</b></td>
				<td>'.$row['Template'].'</td>
			</tr>			
			<tr class="alternate_back">
				<td><b>'.$TEXT_DEVICE_CATEGORY_CONST.'</b></td>
				<td>'.$row['Category'].'</td>
			</tr>			
			<tr>
				<td><b>'.$TEXT_MANUFACTURER_CONST.'</b></td>
				<td>'.$row['Manufacturer'].'</td>
			</tr>			
			<tr class="alternate_back">
				<td><b>'.$TEXT_COMM_METHOD_CONST.'</b></td>
				<td>'.pulldownFromArray($commMethodsArray,'commMethod',$row['FK_CommMethod']).'</td>
			</tr>			
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="button" class="button" name="cancel" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>			
			';
		}
			
		$out.='	
		</table>
		</form>';
	}else{
	// process area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if (!$canModifyInstallation) {
			header('Location: index.php?section=commMethod&dtID='.$dtID.'&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}
	
		$commMethod=(int)@$_REQUEST['commMethod'];
		$commMethod=($commMethod==0)?null:$commMethod;
		
		$dbADO->Execute("UPDATE DeviceTemplate SET FK_CommMethod=? WHERE PK_Devicetemplate=?",array($commMethod,$dtID));
		
		$out="
			<script>
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();			
			</script>
		";
		
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_COMM_METHOD_CONST);
	$output->output();
}

?>
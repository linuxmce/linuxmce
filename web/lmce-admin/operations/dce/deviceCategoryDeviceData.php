<?
function deviceCategoryDeviceData($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deviceCategoryDeviceData.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMasterDevice.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$categoryID=(int)@$_REQUEST['categoryID'];	
	
	if($action=='form'){
		$deviceCategoriesArray=getHierachicalCategories($dbADO);
		$deviceCategoryFormElement=pulldownFromArray($deviceCategoriesArray,'categoryID',$categoryID,'class="input_big" onchange="document.deviceCategoryDeviceData.action.value=\'form\';document.deviceCategoryDeviceData.submit();"');

		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<form action="index.php" method="POST" name="deviceCategoryDeviceData">
			<input type="hidden" name="section" value="deviceCategoryDeviceData">
			<input type="hidden" name="action" value="update">

		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		
			<table border="0">
				<tr>
					<td width="120"><B>'.$TEXT_DEVICE_CATEGORY_CONST.' '.(($categoryID>0)?'#'.$categoryID:'').'</B></td>
					<td width="450">'.$deviceCategoryFormElement.'</td>
				</tr>
				<tr>
					<td colspan="2">
								<table>';

		$deviceData="
			SELECT 
				DeviceCategory_DeviceData.*,
				DeviceData.Description as DD_desc,
				PK_DeviceData,
				ParameterType.Description as PT_Desc FROM 
				DeviceCategory_DeviceData 
			INNER JOIN DeviceData on FK_DeviceData = PK_DeviceData
			INNER JOIN ParameterType on FK_ParameterType = PK_ParameterType
			WHERE (FK_DeviceCategory='$categoryID')
		";

		$resDeviceData = $dbADO->_Execute($deviceData);
		$usedParams = array();
		$usedParams[] = 0;
		$deviceDataFormValidation='';
		if ($resDeviceData) {
				$out.='<tr>
							<td colspan="2">
								<table border="1" cellpadding="3" cellspacing="0">
									<tr class="tablehead">
										<td><B>'.$TEXT_CURRENT_DATA_CONST.'</B></td>
										<td><B>'.$TEXT_COMMENTS_CONST.'</B></td>
										<td><B>'.$TEXT_DEFAULT_VALUE_CONST.'</B></td>
										<td><B>'.$TEXT_REQUIRED_CONST.'</B></td>
										<td><B>'.$TEXT_ALLOWED_TO_MODIFY_CONST.'</B></td>
										<td><B>'.$TEXT_USER_MASTER_DEVICE_LIST_DEFAULTS_CONST.'</B></td>
										<td><B>'.$TEXT_SET_BY_DEVICE_CONST.'</B></td>
										<td><B>'.$TEXT_ACTION_CONST.'</B></td>
									</tr>
										';
			while ($row=$resDeviceData->FetchRow()) {
				$out.="
									<tr ".((trim($row['Description'])=='')?(' bgColor = "lightgreen"'):('')).">
										<td>
											<table width=\"100%\">
												<tr>
													<td>#{$row['FK_DeviceData']} {$row['DD_desc']}({$row['PT_Desc']})</td>
													<td align=\"right\"><input type=\"button\" class=\"button\" onClick=\"windowOpen('index.php?section=editDeviceData&from=deviceCategoryDeviceData&categoryID=$categoryID&deviceDataID={$row['PK_DeviceData']}','status=0,resizable=1,width=500,height=250,toolbar=1');\" value=\"$TEXT_EDIT_CONST\"></td>
												</tr>
											</table>
										</td>
										<td><textarea name='Data_Description_{$row['FK_DeviceData']}' rows='1'>{$row['Description']}</textarea></td>
										<td><textarea name='Data_DefaultValue_{$row['FK_DeviceData']}' rows='1'>{$row['IK_DeviceData']}</textarea></td>
										<td align=\"center\"><input type='checkbox' value='1' name='Data_Required_{$row['FK_DeviceData']}' ".($row['Required']==1?' checked="checked" ':'')."></td>
										<td align=\"center\"><input type='checkbox' value='1' name='Data_AllowedToModify_{$row['FK_DeviceData']}' ".($row['AllowedToModify']==1?' checked="checked" ':'')."></td>
										<td align=\"center\"><input type='checkbox' value='1' name='Data_UseCategoryTemplateDefault_{$row['FK_DeviceData']}' ".($row['UseDeviceCategoryDefault']==1?' checked="checked" ':'')."></td>
										<td align=\"center\"><input type='checkbox' value='1' name='Data_SetByDevice_{$row['FK_DeviceData']}' ".($row['SetByDevice']==1?' checked="checked" ':'')."></td>
										<td align=\"center\"><input type=\"button\" class=\"button\" onClick=\"if(confirm('.$TEXT_CONFIRM_DELETE_DEVICE_DATA_FROM_DEVICE_CATEGORY_CONST'))self.location='index.php?section=deviceCategoryDeviceData&deviceData={$row['FK_DeviceData']}&categoryID={$categoryID}&action=delete';\" value=\"$TEXT_DELETE_CONST\"></td>
									</tr>							
							";	
				$usedParams[]=$row['FK_DeviceData'];
				$deviceDataFormValidation.= '
					frmvalidator.addValidation("Data_Description_'.$row['FK_DeviceData'].'","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
					';
			}
			if ($resDeviceData->RecordCount()>0) {
				$out.='
								 <input type=\'hidden\' name="usedData" value="'.(join(",",$usedParams)).'">
								</table>
							</td>
					   </tr>	
							';
			} else {
				$out.='<tr><td colspan="8">'.$TEXT_NO_DEVICE_DATA_FOR_THIS_DEVICE_CATEGORY_CONST.'</td></tr>';
			}

		}
		
		$remainingDDArray=getAssocArray('DeviceData','PK_DeviceData','Description',$dbADO,"WHERE PK_DeviceData NOT IN (".join(",",$usedParams).")",'ORDER BY Description ASC');
		$out.='
				</table>		
					</td>
				</tr>';
		if ($resDeviceData->RecordCount()>0){
			$out.='
				<tr>
					<td align="center" colspan="8"><input type="submit" class="button" name="Update" value="'.$TEXT_UPDATE_CONST.'"></td>
				</tr>';
		}

		if($categoryID!=0){
			$out.='
				<tr>
					<td><a name="deviceData_link"></a>'.$TEXT_ADD_A_NEW_PARAMETER_CONST.'</td>
					<td>'.pulldownFromArray($remainingDDArray,'newDeviceData',0,'class="input_big"').' <input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>			
				<tr>
					<td colspan="2">'.$TEXT_IF_PARAMETER_IS_NOT_IN_THE_LIST_CONST.' <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addParameter&from=deviceCategoryDeviceData&categoryID='.$categoryID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">click here to create a new parameter</a></td>
				</tr>';
		}
		$out.='
			</table>
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=deviceCategoryDeviceDatacategoryID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		if($action=='delete'){
			$deviceData=$_REQUEST['deviceData'];
			$dbADO->Execute('DELETE FROM DeviceCategory_DeviceData WHERE FK_DeviceCategory=? AND FK_DeviceData=?',array($categoryID,$deviceData));
			
			header('Location: index.php?section=deviceCategoryDeviceData&categoryID='.$categoryID.'&msg='.$TEXT_DEVICE_DATA_DELETED_FROM_DEVICE_CATEGORY_CONST);
			exit();
		}
		
		$usedData=explode(',',@$_POST['usedData']);
		foreach ($usedData AS $param){
			if($param!=0){
				$descriptionParam = cleanString(@$_POST['Data_Description_'.$param]);
				$defaultValueParam = cleanString(@$_POST['Data_DefaultValue_'.$param]);
				$requiredParam = cleanInteger(@$_POST['Data_Required_'.$param]);
				$allowedToModifyParam = cleanInteger(@$_POST['Data_AllowedToModify_'.$param]);
				$useDeviceCategoryDefaultParam = cleanInteger(@$_POST['Data_UseDeviceCategoryDefault_'.$param]);
				$setByDeviceParam = cleanInteger(@$_POST['Data_SetByDevice_'.$param]);
				
				$dbADO->Execute('UPDATE DeviceCategory_DeviceData SET 
					  IK_DeviceData = ?,
					  Description = ?,
					  Required = ?,
					  AllowedToModify = ?,
					  UseDeviceCategoryDefault = ?,
					  SetByDevice = ?
					WHERE FK_DeviceData = ? AND FK_DeviceCategory = ?',array($defaultValueParam,$descriptionParam,$requiredParam,$allowedToModifyParam,$useDeviceCategoryDefaultParam,$setByDeviceParam,$param,$categoryID));
			}
		}
		
		$newDeviceData = (int)$_POST['newDeviceData'];
		if ($newDeviceData!=0 && $categoryID!=0) {
			$query = "INSERT INTO DeviceCategory_DeviceData( FK_DeviceCategory, FK_DeviceData) values(?,?)";
			$dbADO->Execute($query,array($categoryID,$newDeviceData));

			$locationGoTo = "#Data_Description_{$newDeviceData}";
			
			header("Location: index.php?section=deviceCategoryDeviceData&categoryID=$categoryID&msg=$TEXT_SAVED_CONST".$locationGoTo);
			exit();
		}
			
		header('Location: index.php?section=deviceCategoryDeviceData&categoryID='.$categoryID);
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle('DCE '.$TEXT_DEVICE_CATEGORY_DEVICE_DATA_DATA_CONST);
	$output->setNavigationMenu(array($TEXT_DEVICE_DATA_CONST=>'index.php?section=deviceCategoryDeviceData'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
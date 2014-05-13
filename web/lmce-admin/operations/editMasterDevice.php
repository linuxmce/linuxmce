<?php
function editMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMasterDevice.lang.php');

	//$dbADO->debug=true;
	$out='';
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */


	//get _POST
	$deviceID = (int)$_REQUEST['model'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$fromDeviceID = isset($_REQUEST['fromDeviceID'])?'&deviceID='.cleanString($_REQUEST['fromDeviceID']):'';
	$dtRelatedValues=array('1'=>'Sister device', '2'=>'Install on core', '3'=>'Plugin for the router', '4'=>'Anywhere on same PC', '5'=>'My Child');

	//the form
	if ($action=='form') {
		
		$queryGetMasterDeviceDetails = "
			SELECT DeviceTemplate.*,FK_DeviceTemplate
			FROM  DeviceTemplate 				
			LEFT JOIN DeviceTemplate_AV ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?";
		$rs = $dbADO->Execute($queryGetMasterDeviceDetails,array($deviceID));
		if($rs->RecordCount()==0){
			die('Device template # '.$deviceID.' was not found');
		}

		$row = $rs->FetchRow();
		$description = stripslashes($row['Description']);
		$deviceCategID = $row['FK_DeviceCategory'];
		$manufacturerID = $row['FK_Manufacturer'];
		$manufacturerURL = $row['ManufacturerURL'];
		$internalURLsufix = $row['InternalURLSuffix'];
		$implementsDCE = (int)$row['ImplementsDCE'];
		$commandLine = stripslashes($row['CommandLine']);
		$userID = @$_SESSION['userID'];
		$quarantine = $row['FK_StabilityStatus'];
		$package = $row['FK_Package'];
		$isPlugIn = $row['IsPlugIn'];
		$isEmbedded = $row['IsEmbedded'];
		$inheritsMacFromPC = $row['InheritsMacFromPC'];
		$isIPBased=$row['IsIPBased'];
		$ConfigureScript=$row['ConfigureScript'];
		$comments=$row['Comments'];
		$isAVDevice = is_null($row['FK_DeviceTemplate'])?0:1;
		$rubyCodesButton=($commandLine==$GLOBALS['GenericSerialDeviceCommandLine'])?'<input type="button" class="button" name="rubyCodes"  value="'.$TEXT_RUBY_CODES_CONST.'" onClick="windowOpen(\'index.php?section=rubyCodes&from=editMasterDevice&dtID='.$deviceID.'\',\'status=0,resizable=1,width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');")>':'';
		$commMethod=$row['FK_CommMethod'];
		$commMethods=getAssocArray('CommMethod','PK_CommMethod','Description',$dbADO,'','ORDER BY Description ASC');
		
		$rs->Close();

		$out='
		<script type="text/javascript">
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.@$_GET['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editMasterDevice">
			<input type="hidden" name="section" value="editMasterDevice">
			<input type="hidden" name="action" value="modify">
			<input type="hidden" name="lastAction" value="">
			<input type="hidden" name="model" value="'.$deviceID.'">
			<input type="hidden" name="toDel" value="">
		<h3>'.$TEXT_EDIT_DEVICE_TEMPLATE_CONST.' # '.$deviceID.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
		<fieldset>
			<legend>'.$TEXT_DEVICE_TEMPLATE_CONST.' #'.$deviceID.'</legend>
			<table>
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.' *</td>
					<td><input type="text" name="description" value="'.$description.'" class="input_big"></td>
				</tr>
				<tr>
					<td>
						<input type="checkbox" name="ImplementsDCE" value="1" '.($implementsDCE==1?" checked ":"").'  onClick="javascript:this.form.submit();">'.$TEXT_IMPLEMENTS_DCE_CONST.'
					</td>
					<td>
						'.$TEXT_COMMAND_LINE_CONST.' <input type="text" name="CommandLine" value="'.$commandLine.'" size="23"> '.$rubyCodesButton.'
					</td>
				</tr>
				<tr>
					<td>'.$TEXT_DEVICE_CATEGORY_CONST.' *</td><td>
						<select name="MasterDeviceCategory" class="input_big">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
		';
		$GLOBALS['categoriesArray']=array();
		$queryMasterDeviceCategories_parents = 'select PK_DeviceCategory,Description from DeviceCategory where FK_Devicecategory_Parent is null order by Description asc';
		$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
		while ($row = $rs->FetchRow()) {
			$GLOBALS['categoriesArray'][$row['PK_DeviceCategory']]=$row['Description'];
			$out.='<option '.($row['PK_DeviceCategory']==$deviceCategID?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.stripslashes($row['Description']).' #'.$row['PK_DeviceCategory'].'</option>';
			$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],stripslashes($row['Description']),$deviceCategID,'',$dbADO);
		}
		$out.='			</select>
					</td>
				</tr>
			<tr>
				<td>'.$TEXT_MANUFACTURER_CONST.' *</td><td>
					<select name="Manufacturer" class="input_big">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
		';
		$manufacturersArray=array();
		$queryManufacturers = 'select CONCAT(Description,\' #\',PK_Manufacturer) AS Description, PK_Manufacturer from Manufacturer order by Description asc';
		$rs = $dbADO->_Execute($queryManufacturers);
		while ($row = $rs->FetchRow()) {
			$manufacturersArray[$row['PK_Manufacturer']]=$row['Description'];
			$out.='<option '.($row['PK_Manufacturer']==$manufacturerID?' selected ': ' ').' value="'.$row['PK_Manufacturer'].'">'.stripslashes($row['Description']).'</option>';
		}
		$out.='		</select> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addManufacturer&from=editMasterDevice\',\'width=300,height=200,toolbar=1\');">Create Manufacturer</a>
				</td>
			</tr>
			<tr>
				<td>'.$TEXT_MANUFACTURER_URL_CONST.'</td>
				<td><input type="text" name="manufacturerURL" value="'.$manufacturerURL.'" class="input_big"> Internal URL sufix <input type="text" name="internalURLsufix" value="'.$internalURLsufix.'" class="input_big"></td>
			</tr>
			<tr>
				<td valign="top">'.$TEXT_DESIGN_OBJECTS_TO_USE_AS_REMOTES_CONST.'</td>
				<td bgcolor="#EFEFEF">
		';
		$selectObjects = 'select PK_DesignObj,Description from DesignObj inner join DeviceTemplate_DesignObj on FK_DesignObj = PK_DesignObj where FK_DeviceTemplate = ? order by Description asc';
		$rs = $dbADO->Execute($selectObjects,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$out.=stripslashes($row['Description']) . "&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteObjectFromMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['PK_DesignObj']}','status=0,resizable=1,width=300,height=200,toolbar=1');\">$TEXT_DELETE_CONST</a> <br />";
		}
		if ($rs->RecordCount()===0) {
			$out.=$TEXT_NO_OBJECTS_CONST;
		}
		$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addObjectToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=300,height=200,toolbar=1');\">".$TEXT_ADD_A_NEW_OBJECT_TO_DEVICE_CONST."</a>";

		$out.='		</td>
				</tr>
				<tr>
					<td valign="top">'.$TEXT_THIS_DEVICE_IS_CONTROLLED_VIA_CONST.'</td>
					<td bgcolor="#EFEFEF">
					';
		$selectControlled = 'select MDL1.Description as d1,MDL1.PK_DeviceTemplate as pk1,
												MDL2.Description as d2,MDL2.PK_DeviceTemplate as pk2,
												DC.Description as d3,
												MDL_CV.RerouteMessagesToParent,MDL_CV.AutoCreateChildren,
												MDL_CV.PK_DeviceTemplate_DeviceTemplate_ControlledVia as pk
								FROM DeviceTemplate_DeviceTemplate_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 
								INNER JOIN DeviceTemplate MDL2 on FK_DeviceTemplate_ControlledVia  = MDL2.PK_DeviceTemplate
								LEFT JOIN DeviceCategory DC on MDL2.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ? order by DC.Description asc';
		$rs = $dbADO->Execute($selectControlled,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$out.=$TEXT_DEVICE_CONST.':'.stripslashes($row['d2']) .'&nbsp; Category: '. stripslashes($row['d3']) . "&nbsp; &nbsp;
							<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editControlledViaToMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['pk']}','status=0,resizable=1,width=800,height=600');\">$TEXT_EDIT_CONST</a>
							&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteControlledViaFromMasterDevice&from=editMasterDevice&objID={$row['pk']}','status=0,resizable=1,width=10,height=10,toolbar=1');\">$TEXT_DELETE_CONST</a>
							<br />";
		}
		if ($rs->RecordCount()===0) {
			$out.=$TEXT_NO_RECORDS_CONST;
		}
		$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addControlledViaToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=500,height=250,toolbar=1');\">".$TEXT_ADD_A_NEW_CONTROLLED_VIA_DEVICE_CONST."</a>";

		$out.='
					</td>
				</tr>
				<tr>
					<td valign="top">'.$TEXT_THIS_DEVICE_IS_CONTROLLED_VIA_CATEGORY_CONST.':</td>
					<td bgcolor="#EFEFEF">
					';
		$selectControlled = 'select MDL1.Description as d1,MDL1.PK_DeviceTemplate as pk1,
												DC.Description as d3,
												MDL_CV.RerouteMessagesToParent,MDL_CV.AutoCreateChildren,
												MDL_CV.PK_DeviceTemplate_DeviceCategory_ControlledVia as pk
								FROM DeviceTemplate_DeviceCategory_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 								
								INNER JOIN DeviceCategory DC on MDL_CV.FK_DeviceCategory = DC.PK_DeviceCategory
								WHERE MDL_CV.FK_DeviceTemplate = ? order by DC.Description asc';
		$rs = $dbADO->Execute($selectControlled,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$out.=$TEXT_DEVICE_CATEGORY_CONST.': '. stripslashes($row['d3']) . "&nbsp; &nbsp;
							<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editControlledViaCategoryToMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['pk']}','status=0,resizable=1,width=800,height=500');\">$TEXT_EDIT_CONST</a>
							&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteControlledViaCategoryFromMasterDevice&from=editMasterDevice&objID={$row['pk']}','status=0,resizable=1,width=700,height=800,toolbar=1,scrollbars=1');\">$TEXT_DELETE_CONST</a>
							<br />";
		}
		if ($rs->RecordCount()===0) {
			$out.=$TEXT_NO_RECORDS_CONST;
		}
		$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addControlledViaCategoryToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=800,height=600,toolbar=1');\">".$TEXT_ADD_A_NEW_CONTROLLED_VIA_CATEGORY_DEVICE_CONST."</a>";

		$out.='
					</td>
				</tr>
				<tr>
					<td valign="top">'.$TEXT_PACKAGES_CONST.':</td><input type="hidden" name="packagesCtrl" value="0">
					<td><select name="package" class="input_big">
						<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>
					';
		$querySelectPackages = "SELECT * FROM Package ORDER BY Description ASC";
		$rs = $dbADO->Execute($querySelectPackages);
		if($rs->RecordCount()==0)
		$out.=$TEXT_NO_RECORDS_CONST.'<br>';
		while ($row = $rs->FetchRow()) {
			$out.='<option value="'.$row['PK_Package'].'" '.(($row['PK_Package']==$package)?'selected':'').'>'.$row['Description'].'</option>';
		}
		$rs->Close();
		$out.='</select>&nbsp;<input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'">';
		if($package!=0)
		$out.="  <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=editMasterDevice&deviceID={$deviceID}&PK_Package={$package}','status=0,resizable=1,width=700,height=700,toolbar=1,scrollbars=1,fullscreen=1');\">Edit package</a>";
		$out.="&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">Create new package</a>";
		$out.='
					</td>
				</tr>
				<tr>
					<td valign="top"><a name="isAVDevice_link"></a>'.$TEXT_AUDIO_VIDEO_DEVICE_CONST.'</td>
					<td>
						<input type="hidden" value="'.$isAVDevice.'" name="old_isAVDevice"><input type="checkbox" name="isAVDevice" '.($isAVDevice==1?" checked='checked' ":"").' value="1" onClick="javascript:this.form.submit();"><input type="button" class="button" name="isAV"  '.($isAVDevice!=1?'value="'.$TEXT_IS_NOT_AUDIO_VIDEO_CONST.'" disabled="disabled" ':" value=\"".$TEXT_EDIT_AUDIO_VIDEO_PROPERTIES_CONST."\" onClick=\"windowOpen('index.php?section=irCodes&from=editMasterDevice&dtID={$deviceID}${fromDeviceID}','status=0,resizable=1,width=800,height=600,toolbar=1,scrollbars=1,resizable=1');\"").'>
					</td>
				</tr>
				<tr>
					<td valign="top">'.$TEXT_IS_PLUGIN_CONST.'</td>
					<td><input type="checkbox" name="isPlugIn" value="1" '.(($isPlugIn==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsPlugIn" value="'.(($isPlugIn==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">'.$TEXT_IS_EMBEDDED_CONST.'</td>
					<td><input type="checkbox" name="isEmbedded" value="1" '.(($isEmbedded==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsEmbedded" value="'.(($isEmbedded==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">'.$TEXT_INHERITS_MAC_FROM_PC_CONST.'</td>
					<td><input type="checkbox" name="inheritsMacFromPC" value="1" '.(($inheritsMacFromPC==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldInheritsMacFromPC" value="'.(($inheritsMacFromPC==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">'.$TEXT_IS_IP_BASED_CONST.'</td>
					<td><input type="checkbox" name="isIPBased" value="1" '.(($isIPBased==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsIPBased" value="'.(($isIPBased==1)?'1':'0').'"></td>
				</tr>	
				<tr>
					<td valign="top">Comm Method</td>
					<td>'.pulldownFromArray($commMethods,'commMethod',$commMethod).'
				</tr>				
				<tr>
					<td valign="top">'.$TEXT_CONFIGURATION_SCRIPT_CONST.'</td>
					<td><input type="text" name="ConfigureScript" value="'.$ConfigureScript.'" class="input_big">
				</tr>	
				
				<tr>
					<td valign="top">'.$TEXT_COMMENTS_CONST.'</td>
					<td><textarea name="comments" rows="2" class="input_big">'.$comments.'</textarea></td>
				</tr>	
			</table>
		</fieldset>
		
		<table border="0" width="100%">
				<tr>
					<td valign="top" colspan="2"><a name="deviceData_link"></a>				
						<fieldset>
		            		<legend>'.$TEXT_DEVICE_DATA_CONST.'</legend>
								<table>';

		$deviceData="SELECT DeviceTemplate_DeviceData.*,
							DeviceData.Description as DD_desc,PK_DeviceData,
							ParameterType.Description as PT_Desc FROM 
							DeviceTemplate_DeviceData 
								INNER JOIN DeviceData on FK_DeviceData = PK_DeviceData
								INNER JOIN ParameterType on FK_ParameterType = PK_ParameterType
							WHERE (FK_DeviceTemplate='$deviceID')
							";

		$resDeviceData = $dbADO->_Execute($deviceData);
		$usedParams = array();
		$usedParams[] = 0;
		$deviceDataFormValidation='';
		if ($resDeviceData) {
			if ($resDeviceData->RecordCount()>0) {
				$out.='<tr>
										<td colspan="2">
											<table border="1" class="regular_table" cellpadding="3" cellspacing="0">
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
			}
			while ($row=$resDeviceData->FetchRow()) {
				$out.="
												<tr ".((trim($row['Description'])=='')?(' bgColor = "lightgreen"'):('')).">
													<td><table width=\"100%\">
														<tr>
															<td>#{$row['FK_DeviceData']} {$row['DD_desc']}({$row['PT_Desc']})</td>
															<td width=\"50\"><input type=\"button\" class=\"button\" onClick=\"windowOpen('index.php?section=editDeviceData&from=editMasterDevice&deviceID=$deviceID&deviceDataID={$row['PK_DeviceData']}','status=0,resizable=1,width=500,height=250,toolbar=1');\" value=\"$TEXT_EDIT_CONST\"></td>
														</tr>
													</table>
													</td>
													<td><textarea name='Data_Description_{$row['FK_DeviceData']}' rows='1'>{$row['Description']}</textarea></td>
													<td><textarea name='Data_DefaultValue_{$row['FK_DeviceData']}' rows='1'>{$row['IK_DeviceData']}</textarea></td>
													<td align=\"center\"><input type='checkbox' value='1' name='Data_Required_{$row['FK_DeviceData']}' ".($row['Required']==1?' checked="checked" ':'')."></td>
													<td align=\"center\"><input type='checkbox' value='1' name='Data_AllowedToModify_{$row['FK_DeviceData']}' ".($row['AllowedToModify']==1?' checked="checked" ':'')."></td>
													<td align=\"center\"><input type='checkbox' value='1' name='Data_UseDeviceTemplateDefault_{$row['FK_DeviceData']}' ".($row['UseDeviceTemplateDefault']==1?' checked="checked" ':'')."></td>
													<td align=\"center\"><input type='checkbox' value='1' name='Data_SetByDevice_{$row['FK_DeviceData']}' ".($row['SetByDevice']==1?' checked="checked" ':'')."></td>
													<td align=\"center\"><input type=\"button\" class=\"button\" onClick=\"windowOpen('index.php?section=deleteParameterFromMasterDevice&from=editMasterDevice&deviceData={$row['FK_DeviceData']}&deviceID={$deviceID}','status=1,resizable=1,width=500,height=250,toolbar=1');\" value=\"$TEXT_DELETE_CONST\"></td>
												</tr>							
							";	
				$usedParams[]=$row['FK_DeviceData'];
				$deviceDataFormValidation.= '
								frmvalidator.addValidation("Data_Description_'.$row['FK_DeviceData'].'","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
								//frmvalidator.addValidation("Data_DefaultValue_'.$row['FK_DeviceData'].'","req","'.$TEXT_DEFAULT_VALUE_REQUIRED_CONST.'");
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
				$out.='<tr><td colspan="2">'.$TEXT_NO_DEVICE_DATA_FOR_THIS_DEVICE_CONST.'</td></tr>';
			}

		}

		$resDeviceData->close();

		$remainingData='<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
		$querySelectRemainingParameterForDevice = "
					select  * from DeviceData
					where PK_DeviceData NOT IN (".join(",",$usedParams).")
					order by Description asc";
		$rs = $dbADO->_Execute($querySelectRemainingParameterForDevice);
		while ($row = $rs->FetchRow()) {
			$remainingData.='<option value="'.$row['PK_DeviceData'].'">'.$row['Description'].'</option>';
		}
		$rs->Close();

		$out.='<tr>
					
				</tr>
				
				<tr>
					<td colspan="2"><a name="deviceData_link"></a>'.$TEXT_ADD_A_NEW_PARAMETER_CONST.': <select name="newDeviceData" class="input_big">'.$remainingData.'</select>&nbsp;&nbsp;
					<input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'>
				<br /> '.$TEXT_IF_PARAMETER_IS_NOT_IN_THE_LIST_CONST.' <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addParameter&from=editMasterDevice&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">click here to create a new parameter</a>
					</td>
				</tr>
				</table>
					</fieldset>
				</td>
				</tr>	
			
				<tr>
					<td valign="top" colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td valign="top" colspan="2"><a name="commands_link"></a>				
						<fieldset>
		            		<legend>'.$TEXT_COMMANDS_CONST.'</legend>
								<table border="0">				
				';

		$querySelCheckedCommandGroups = 'select FK_DeviceCommandGroup from DeviceTemplate_DeviceCommandGroup where  FK_DeviceTemplate = ?';
		$resSelCheckedCommandGroups = $dbADO->Execute($querySelCheckedCommandGroups,$deviceID);
		$selCheckedCommandsGroups = array();
		if ($resSelCheckedCommandGroups) {
			while ($rowSelCheckedCommandGroups = $resSelCheckedCommandGroups->FetchRow()) {
				$selCheckedCommandsGroups[]=$rowSelCheckedCommandGroups['FK_DeviceCommandGroup'];
			}
		}

		if (!is_array($selCheckedCommandsGroups)) {
			$selCheckedCommandsGroups=array();
			$selCheckedCommandsGroups[]=0;
		}
		if (count($selCheckedCommandsGroups)==0) {
			$selCheckedCommandsGroups[]=0;
		}

		$query = "select  ".$dbADO->IfNull('FK_DeviceCategory_Parent','0')." as a from DeviceCategory
							WHERE 
								PK_DeviceCategory = ?
				";
		$deviceParent=0;
		$res = $dbADO->Execute($query,array($deviceCategID));
		if ($res) {
			while ($row = $res->FetchRow()) {
				$deviceParent=$row['a'];
			}
		}
		$deviceParent=(int)$deviceParent;
		$deviceCG = array();
		$query = "select DeviceCommandGroup.* FROM
							 DeviceCommandGroup
								LEFT JOIN DeviceCategory on FK_DeviceCategory = PK_DeviceCategory
							WHERE 
								FK_DeviceCategory in ($deviceParent,$deviceCategID) or
								PK_DeviceCommandGroup in (".join(",",$selCheckedCommandsGroups).")
							ORDER BY DeviceCommandGroup.Description Asc
				";
		$resCommands = $dbADO->_Execute($query);

		$commandsDisplayed = array();
		if ($resCommands) {
			while ($row = $resCommands->FetchRow()) {
				$out.='
					<tr>
						<td width="200"><input '.(in_array($row['PK_DeviceCommandGroup'],$selCheckedCommandsGroups)?" checked='checked' ": '').' type="checkbox" name="DeviceCommandGroup_'.$row['PK_DeviceCommandGroup'].'" value="1">'.stripslashes($row['Description']).'</td>
						<td><input type="button" class="button" onClick="windowOpen(\'index.php?section=editCommandGroupFromMasterDevice&from=editMasterDevice&deviceID='.$deviceID.'&commandGroupID='.$row['PK_DeviceCommandGroup'].'\',\'width=800,height=600,toolbar=1,resizable=1,scrollbars=1\');" value="'.$TEXT_EDIT_COMMANDS_CONST.'"></td>
					</tr>';
				$commandsDisplayed[]=$row['PK_DeviceCommandGroup'];
			}
		}
		if (count($commandsDisplayed)==0) {
			$commandsDisplayed[]=0;
		}
		$out.='
						<input type="hidden" name="DeviceCommandGroupDisplayed" value="'.(join(",",$commandsDisplayed)).'">
						
						<tr>
							<td colspan="2"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandGroup&from=editMasterDevice\',\'width=550,height=300,toolbar=1,scrollbars=1,resizable=1\');">'.$TEXT_CREATE_COMMAND_GROUP_CONST.'</a>&nbsp;&nbsp;&nbsp;&nbsp;
						'.$TEXT_MANUALLY_ADD_COMMAND_GROUP_CONST.' 
						<select name="addNewCommandGroupToMasterDevice" class="input_big">
							';
		$query = "select PK_DeviceCommandGroup, Description FROM DeviceCommandGroup where PK_DeviceCommandGroup NOT IN (".join(",",$commandsDisplayed).") Order By Description ASC";
		$res = $dbADO->Execute($query);
		$out.= '<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
		if ($res) {
			while ($row=$res->FetchRow()) {
				$out.='<option value="'.$row['PK_DeviceCommandGroup'].'">'.$row['Description'].'</option>';
			}
		}
		$res->Close();
		$out.='	</select> <input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'></td></tr>
					</table>
					</fieldset>
				</td>
			</tr>
			';



		$out.='
				<tr>
					<td valign="top" colspan="2">&nbsp;</td>
				</tr>
				<tr>
						<td valign="top" colspan="2"><a name="eventsList_link"></a>
				
				<fieldset>
            		<legend>'.$TEXT_EVENTS_CONST.'</legend>
						<table>
					';
		$queryEventsSelected = 'SELECT EL.Description as EL_Desc,MDL_EL.Description as MDL_EL_Desc,PK_Event
						FROM DeviceTemplate_Event MDL_EL
							INNER JOIN DeviceTemplate MDL on FK_DeviceTemplate = PK_DeviceTemplate
							INNER JOIN Event EL on PK_Event = MDL_EL.FK_Event 						
							WHERE 
							FK_DeviceTemplate = ?
						Order By EL_Desc asc
					';
		//INNER JOIN Event_EventParameter EL_EP  on EL_EP.FK_Event = EL.PK_Event
		//INNER JOIN EventParameter EP  on EP.PK_EventParameter = EL_EP.FK_EventParameter
		//INNER JOIN ParameterType PT  on PT.PK_ParameterType = EP.FK_ParameterType

		$resEventsSelected = $dbADO->Execute($queryEventsSelected,array($deviceID));
		$eventsSelectedArray=array();
		$eventsSelectedArray[]=0;
		if ($resEventsSelected) {
			while ($rowEventsSelected = $resEventsSelected->FetchRow()) {
				$out.="<tr><td>".$rowEventsSelected['EL_Desc'].'
							<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editEvent&from=editMasterDevice&EventID='.$rowEventsSelected['PK_Event'].'&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1,scrollbars=1\');">'.$TEXT_EDIT_CONST.'</a>
							<br />#'.$rowEventsSelected['PK_Event'].
				"</td><td><textarea cols='40' rows='5' name='EventDesc_{$rowEventsSelected['PK_Event']}'>".$rowEventsSelected['MDL_EL_Desc']."</textarea></td><td>".'
							 
							&nbsp; &nbsp; <a href="javascript:void(0);" onClick="if (confirm(\'Ar you sure you want to delete this event?\')) windowOpen(\'index.php?section=deleteEventFromMasterDevice&from=editMasterDevice&Event='.$rowEventsSelected['PK_Event'].'&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">'.$TEXT_DELETE_CONST.'</a></td></tr>							
							';
				$eventsSelectedArray[]=$rowEventsSelected['PK_Event'];
			}
		}
		$resEventsSelected->Close();

		$queryEventsList = 'SELECT Description,PK_Event
							FROM Event
							WHERE PK_Event NOT IN ('.join(",",$eventsSelectedArray).')
							ORDER BY Description ASC
					';

		$resEventsList = $dbADO->_Execute($queryEventsList);
		if ($resEventsList) {
			if ($resEventsList->RecordCount()>0) {
				$eventsTxt = '<option value=\'0\'>-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
			}else{
				$eventsTxt = '<option value="0">'.$TEXT_NO_EVENTS_LISTS_AVAILABLE_CONST.'</option>';
			}
			while ($rowEventsList = $resEventsList->FetchRow()) {
				$eventsTxt.= '<option value="'.$rowEventsList['PK_Event'].'">'.$rowEventsList['Description'].'</option>';
			}
		}
		$out.='
							
							<tr>
									<td>'.$TEXT_ADD_NEW_EVENT_CONST.': <select name="newEventToMasterDevice" class="input_big">'.$eventsTxt.'</select></td>
									<td><input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'>
										<input type="hidden" name="eventsListDisplayed" value="'.join(",",$eventsSelectedArray).'">
									</td>
							</tr>
							<tr>
								<td colspan="2">
									<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createEvent&from=editMasterDevice&deviceID='.$deviceID.'\',\'status=1,resizable=1,width=500,height=250,toolbar=1\');">'.$TEXT_CREATE_AN_EVENT_CONST.'</a>
								</td>
							</tr>
						</table>
					</fieldset>
					</td>
				</tr>
				<tr>
						<td valign="top" colspan="2"><a name="plugAndPlay"></a>
				
				<fieldset>
            		<legend>'.$TEXT_PLUG_AND_PLAY_CONST.'</legend>
						<table border="0">';
		$dhcpArray=array();
		$resDHCP=$dbADO->Execute('SELECT * FROM DHCPDevice WHERE FK_DeviceTemplate=?',$deviceID);
		$out.='
							<tr>
								<td colspan="6">'.(($resDHCP->RecordCount()==0)?$TEXT_NO_RECORDS_CONST:$TEXT_RANGE_OF_MAC_CONST).'</td>
							</tr>
						';
		$pnpProtocolArray=getAssocArray('PnpProtocol','PK_PnpProtocol','Description',$dbADO,'','ORDER BY Description ASC');
		while($rowDHCP=$resDHCP->FetchRow()){
			$dhcpArray[]=$rowDHCP['PK_DHCPDevice'];
			$out.='
							<tr>
								<td>'.$TEXT_FROM_CONST.'</td>
								<td><input type="text" name="mac_from_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['Mac_Range_Low'].'" class="input_big"></td>
								<td>'.$TEXT_TO_CONST.'</td>
								<td colspan="2"><input type="text" name="mac_to_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['Mac_Range_High'].'" class="input_big"></td>
							</tr>
							<tr>
								<td>'.$TEXT_VENDOR_MODEL_ID_CONST.'</td>
								<td><input type="text" name="VendorModelID_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['VendorModelID'].'" class="input_big"></td>
								<td>'.$TEXT_PNP_PROTOCOL_CONST.'</td>
								<td colspan="2">'.pulldownFromArray($pnpProtocolArray,'pnpProtocol_'.$rowDHCP['PK_DHCPDevice'],$rowDHCP['FK_PnpProtocol']).'</td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_SERIAL_NUMBER_CONST.'</td>
								<td><input type="text" name="SerialNumber_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['SerialNumber'].'" class="input_big"></td>
								<td>'.$TEXT_PARMS_CONST.'</td>
								<td><input type="text" name="Parms_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['Parms'].'" class="input_big"></td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_PNP_DETECTION_SCRIPT_CONST.'</td>
								<td colspan="4"><input type="text" name="PnpDetectionScript_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['PnpDetectionScript'].'" class="input_big"></td>
							</tr>
							<tr>
								<td align="right">'.$TEXT_COMMENT_CONST.': </td>
								<td colspan="4"><textarea name="commentPnp_'.$rowDHCP['PK_DHCPDevice'].'" rows="2" style="width:100%">'.$rowDHCP['Description'].'</textarea></td>
								<td><input type="button" class="button" name="editDHCP" value="'.$TEXT_EDIT_CONST.'" onClick="windowOpen(\'index.php?section=editDHCP&dhcpID='.$rowDHCP['PK_DHCPDevice'].'\',\'width=500,height=400,toolbar=1,resizable=1\');"> <input type="button" class="button" name="delDHCP" value="'.$TEXT_DELETE_CONST.'" onClick="document.editMasterDevice.toDel.value=\''.$rowDHCP['PK_DHCPDevice'].'\';document.editMasterDevice.submit();"></td>
							</tr>
							<tr>
								<td colspan="6"><hr></td>
							</tr>';
		}
		$out.='
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_FROM_CONST.'</td>
								<td><input type="text" name="mac_from" value="" class="input_big"></td>
								<td>'.$TEXT_TO_CONST.'</td>
								<td colspan="2"><input type="text" name="mac_to" value="" class="input_big"></td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_VENDOR_MODEL_ID_CONST.'</td>
								<td><input type="text" name="VendorModelID" value="" class="input_big"></td>
								<td>'.$TEXT_PNP_PROTOCOL_CONST.'</td>
								<td colspan="2">'.pulldownFromArray($pnpProtocolArray,'pnpProtocol',0).'</td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_SERIAL_NUMBER_CONST.'</td>
								<td><input type="text" name="SerialNumber" value="" class="input_big"></td>
								<td>'.$TEXT_PARMS_CONST.'</td>
								<td><input type="text" name="Parms" value="" class="input_big"></td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>'.$TEXT_PNP_DETECTION_SCRIPT_CONST.'</td>
								<td colspan="4"><input type="text" name="PnpDetectionScript" value="" class="input_big"></td>
							</tr>						
							<tr bgcolor="#EEEEEE">
								<td align="right">'.$TEXT_COMMENT_CONST.': </td>
								<td colspan="4"><textarea name="commentPnp" rows="2" style="width:100%"></textarea></td>
							</tr>
							<tr>
								<td colspan="5"><input type="submit" class="button" name="addDHCP" value="'.$TEXT_ADD_CONST.'"></td>
							</tr>
							<input type="hidden" name="dhcpArray" value="'.((join(',',$dhcpArray))).'">	
						</table>
					</fieldset>
					</td>
				</tr>';

		$resRel=$dbADO->Execute('
				SELECT DeviceTemplate_DeviceTemplate_Related.*, D1.Description
				FROM DeviceTemplate_DeviceTemplate_Related 
				INNER JOIN DeviceTemplate D1 ON FK_DeviceTemplate_Related=PK_DeviceTemplate
				WHERE FK_DeviceTemplate=?',$deviceID);				
		$dtRelatedRows='';
		while($rowRel=$resRel->FetchRow()){
			$pipePos=strpos($rowRel['Value'],'|');
			$relValue=($pipePos!==false)?substr($rowRel['Value'],0,$pipePos):$rowRel['Value'];
			$dtRelatedRows.='
					<tr class="alternate_back">
						<td align="center">#'.$rowRel['FK_DeviceTemplate_Related'].'</td>
						<td>'.$rowRel['Description'].'</td>
						<td>'.$dtRelatedValues[$relValue].'</td>
						<td>'.$TEXT_EXTRA_CONST.'</td>
						<td>'.((strpos($rowRel['Value'],'|')!==false)?substr($rowRel['Value'],strpos($rowRel['Value'],'|')+1):'').'</td>
						<td bgcolor="#EEEEEE"><a href="javascript:if(confirm(\''.$TEXT_DELETE_RELATION_CONFIRMATION_CONST.'\')){document.editMasterDevice.delRelated.value='.$rowRel['PK_DeviceTemplate_DeviceTemplate_Related'].';document.editMasterDevice.submit();}">Delete</a></td>
					</tr>';
		}
		$out.='
				<tr>
						<td valign="top" colspan="2"><a name="dtRelated"></a>
				
				<fieldset>
            		<legend>'.$TEXT_DEVICE_TEMPLATE_RELATED_CONST.'</legend>
						<input type="hidden" name="delRelated" value="">
						<span class="err">'.@$_REQUEST['errRelated'].'</span>
						<table>'.$dtRelatedRows.'
							<tr>
								<td>'.$TEXT_ADD_DEVICE_TEMPLATE_RELATED_CONST.' #ID</td>
								<td><input type="text" name="addRelated" value=""></td>
								<td>'.pulldownFromArray($dtRelatedValues,'valueRelated','','','key','').'</td>
								<td>'.$TEXT_EXTRA_CONST.'</td>
								<td><input type="text" name="extra" value=""></td>
								<td><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
							</tr>
						</table>
					</fieldset>
					</td>
				</tr>';

		$queryDT_DO='
				SELECT DesignObj.Description, PK_DesignObj 
				FROM DeviceTemplate_DesignObj 
				INNER JOIN DesignObj ON FK_DesignObj=PK_DesignObj
				WHERE FK_DeviceTemplate=? ORDER BY Description ASC';
		$resDT_DO=$dbADO->Execute($queryDT_DO,$deviceID);
		$linksArray=array();
		while($rowDT_DO=$resDT_DO->FetchRow()){
			$linksArray[]='<a href="index.php?section=editMasterDevice&model='.$deviceID.'&action=removescreen&doID='.$rowDT_DO['PK_DesignObj'].'" title="Click to remove screen">'.$rowDT_DO['Description'].'</a>';
		}
		$out.='
				<tr>
					<td valign="top" colspan="2"><fieldset>
					<legend>'.$TEXT_SCREENS_CONST.'</legend>
						<table>
							<tr>
								<td valign="top" colspan="2">'.$TEXT_THIS_DEVICE_REQURIES_THE_FOLLOWING_SCREENS_CONST.' '.join(', ',$linksArray).'</td>
							</tr>
							<tr>
								<td valign="top" colspan="2"><input type="text" name="newScreen" value=""> <input type="submit" class="button" name="addScreen" value="'.$TEXT_ADD_SCREEN_CONST.'"></td>
							</tr>
						</table>
					</fieldset></td>
				</tr>
				<tr>
					<td valign="top" colspan="2">&nbsp;</td>
				</tr>
					';

		$out.='
				<tr>
					<td valign="top" colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="javascript:self.close();"></td>
				</tr>
					
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editMasterDevice");
 			frmvalidator.addValidation("description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
			frmvalidator.addValidation("MasterDeviceCategory","dontselect=0","'.$TEXT_DEVICE_CATEGORY_REQUIRED_CONST.'");
			frmvalidator.addValidation("Manufacturer","dontselect=0","'.$TEXT_MANUFACTURER_REQUIRED_CONST.'");			
			'.$deviceDataFormValidation.'
		</script>
		';
	} else {
		if(!isset($_SESSION['userID'])){
			header("Location: index.php?section=editMasterDevice&model=".$deviceID.'&error='.$TEXT_LOGIN_TO_EDIT_REQUIRED_CONST);
			exit();
		}

		if(isset($_REQUEST['doID'])){
			$doID=(int)$_REQUEST['doID'];
			$dbADO->Execute('DELETE FROM DeviceTemplate_DesignObj WHERE FK_DeviceTemplate=? AND FK_DesignObj=?',array($deviceID,$doID));
			header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=".$TEXT_SCREEN_REMOVED_FROM_DEVICE_TEMPLATE_CONST);
			exit();
		}

		$description=cleanString($_POST['description'],30);

		$commandLine = cleanString($_POST['CommandLine'],100);
		$ImplementsDCE = @cleanInteger($_POST['ImplementsDCE']);
		$category = cleanInteger($_POST['MasterDeviceCategory']);
		$manufacturer = cleanInteger($_POST['Manufacturer']);
		$package = (@$_POST['package']!='0')?cleanInteger(@$_POST['package']):NULL;
		$isPlugIn = cleanInteger(@$_POST['isPlugIn']);
		$oldIsPlugIn = cleanInteger(@$_POST['oldIsPlugIn']);
		$isEmbedded = cleanInteger(@$_POST['isEmbedded']);
		$oldIsEmbedded = cleanInteger(@$_POST['oldIsEmbedded']);
		$inheritsMacFromPC = cleanInteger(@$_POST['inheritsMacFromPC']);
		$oldInheritsMacFromPC= cleanInteger(@$_POST['oldInheritsMacFromPC']);
		$isIPBased= cleanInteger(@$_POST['isIPBased']);
		$ConfigureScript=$_POST['ConfigureScript'];
		$oldIsIPBased= cleanInteger(@$_POST['oldIsIPBased']);
		$comments=stripslashes($_POST['comments']);
		$newScreen=(int)@$_POST['newScreen'];
		$newMacFrom=@$_POST['mac_from'];
		$newMacTo=@$_POST['mac_to'];
		$VendorModelID=cleanString($_POST['VendorModelID']);
		$newManufacturer=((int)@$_POST['manufacturerPnp']>0)?(int)$_POST['manufacturerPnp']:NULL;
		$manufacturerURL=($_POST['manufacturerURL']!='')?$_POST['manufacturerURL']:NULL;
		$internalURLsufix=($_POST['internalURLsufix']!='')?$_POST['internalURLsufix']:NULL;
		$newCategory=((int)@$_POST['categoryPnp']>0)?(int)$_POST['categoryPnp']:NULL;
		$newComment=cleanString(@$_POST['commentPnp']);
		$SerialNumber=cleanString($_POST['SerialNumber']);
		$Parms=cleanString($_POST['Parms']);
		$PnpDetectionScript=cleanString($_POST['PnpDetectionScript']);
		$PnpProtocol=((int)$_POST['pnpProtocol']==0)?NULL:$_POST['pnpProtocol'];
		$dhcpArray=explode(',',@$_POST['dhcpArray']);
		$isAVDevice = cleanInteger(@$_POST['isAVDevice']);
		$old_isAVDevice = cleanInteger(@$_POST['old_isAVDevice']);
		$commMethod=((int)@$_POST['commMethod']>0)?(int)@$_POST['commMethod']:NULL;
		

		if(($newMacFrom!='' && $newMacTo!='') || $VendorModelID!=''){
			$dbADO->Execute('INSERT INTO DHCPDevice (FK_DeviceTemplate, Mac_Range_Low, Mac_Range_High,Description,VendorModelID,SerialNumber,Parms,PnpDetectionScript,FK_PnpProtocol) VALUES (?,?,?,?,?,?,?,?,?)',array($deviceID,$newMacFrom,$newMacTo,$newComment,$VendorModelID,$SerialNumber,$Parms,$PnpDetectionScript,$PnpProtocol));
			$locationGoTo='plugAndPlay';
		}

		foreach ($dhcpArray AS $dhcpID){
			$macFrom=@$_POST['mac_from_'.$dhcpID];
			$macTo=@$_POST['mac_to_'.$dhcpID];
			$VendorModelID=cleanString(@$_POST['VendorModelID_'.$dhcpID]);
			$newComment=cleanString(@$_POST['commentPnp_'.$dhcpID]);
			$SerialNumber=cleanString(@$_POST['SerialNumber_'.$dhcpID]);
			$Parms=cleanString(@$_POST['Parms_'.$dhcpID]);
			$PnpDetectionScript=cleanString(@$_POST['PnpDetectionScript_'.$dhcpID]);
			$PnpProtocol=((int)@$_POST['pnpProtocol_'.$dhcpID]==0)?NULL:$_POST['pnpProtocol_'.$dhcpID];
			
			
			$dbADO->Execute('UPDATE DHCPDevice SET Mac_Range_Low=?, Mac_Range_High=?,Description=?,VendorModelID=?,SerialNumber=?,Parms=?,PnpDetectionScript=?,FK_PnpProtocol=? WHERE PK_DHCPDevice=?',array($macFrom, $macTo,$newComment,$VendorModelID,$SerialNumber,$Parms,$PnpDetectionScript,$PnpProtocol,$dhcpID));
		}

		if($_REQUEST['toDel']!=''){
			$dhcpID=(int)$_REQUEST['toDel'];
			if($dhcpID!=0){
				$dbADO->Execute('DELETE FROM DHCPDevice WHERE PK_DHCPDevice=?',$dhcpID);
			}
		}

		if($newScreen!=0){
			$newScreenExist=$dbADO->Execute('SELECT * FROM DesignObj WHERE PK_DesignObj=?',$newScreen);
			if($newScreenExist->RecordCount()>0){
				$screenDT=$dbADO->Execute('SELECT * FROM DeviceTemplate_DesignObj WHERE FK_DeviceTemplate=? AND FK_DesignObj=?',array($deviceID,$newScreen));
				if($screenDT->RecordCount()>0){
					header("Location: index.php?section=editMasterDevice&model=$deviceID&error=Screen already exist!");
					exit();
				}else{
					$dbADO->Execute('INSERT INTO DeviceTemplate_DesignObj (FK_DeviceTemplate,FK_DesignObj) VALUES (?,?)',array($deviceID,$newScreen));
				}
			}else{
				header("Location: index.php?section=editMasterDevice&model=$deviceID&error=".$TEXT_ERROR_SCREEN_DOES_NOT_EXIST_CONST);
				exit();
			}
		}

		$locationGoTo='';


		$usedParams = @cleanString($_POST['usedData']);

		$usedParamsArray = explode(",",$usedParams);
		foreach ($usedParamsArray as $param) {
			$param=(int)$param;
			$descriptionParam = cleanString(@$_POST['Data_Description_'.$param]);
			$defaultValueParam = cleanString(@$_POST['Data_DefaultValue_'.$param]);
			$requiredParam = cleanInteger(@$_POST['Data_Required_'.$param]);
			$allowedToModifyParam = cleanInteger(@$_POST['Data_AllowedToModify_'.$param]);
			$useDeviceTemplateDefaultParam = cleanInteger(@$_POST['Data_UseDeviceTemplateDefault_'.$param]);
			$setByDeviceParam = cleanInteger(@$_POST['Data_SetByDevice_'.$param]);
			if ($descriptionParam!='') {

				$getOldValues = "select IK_DeviceData,Description,Required,
										AllowedToModify,UseDeviceTemplateDefault,SetByDevice
								 FROM DeviceTemplate_DeviceData WHERE FK_DeviceData = $param AND FK_DeviceTemplate = $deviceID";
				$resOldValuesData = $dbADO->Execute($getOldValues);
				if ($resOldValuesData && $resOldValuesData->RecordCount()==1) {
					$rowOldValuesData = $resOldValuesData->FetchRow();
					if ($descriptionParam!=$rowOldValuesData['Description']) {
						$locationGoTo = "Data_Description_{$param}";
					}
					if ($defaultValueParam!=$rowOldValuesData['IK_DeviceData']) {
						$locationGoTo = "Data_DefaultValue_{$param}";
					}
					if ($requiredParam!=$rowOldValuesData['Required']) {
						$locationGoTo = "Data_Required_{$param}";
					}
					if ($allowedToModifyParam!=$rowOldValuesData['AllowedToModify']) {
						$locationGoTo = "Data_AllowedToModify_{$param}";
					}
					if ($useDeviceTemplateDefaultParam!=$rowOldValuesData['UseDeviceTemplateDefault']) {
						$locationGoTo = "Data_UseDeviceTemplateDefault_{$param}";
					}
					if ($setByDeviceParam!=$rowOldValuesData['SetByDevice']) {
						$locationGoTo = "Data_SetByDevice_{$param}";
					}

				}
				$query = "update DeviceTemplate_DeviceData
							SET
						  IK_DeviceData = ?,
						  Description = ?,
						  Required = ?,
						  AllowedToModify = ?,
						  UseDeviceTemplateDefault = ?,
						  SetByDevice = ?
							WHERE FK_DeviceData = ? AND FK_DeviceTemplate = ?
						";			
				$dbADO->Execute($query,array($defaultValueParam,$descriptionParam,$requiredParam,
				$allowedToModifyParam,$useDeviceTemplateDefaultParam,
				$setByDeviceParam,$param,$deviceID));
			}
		}
		//commands process
		$usedCommands = cleanString($_POST['DeviceCommandGroupDisplayed']);
		$usedCommandsArray = explode(",",$usedCommands);


		foreach ($usedCommandsArray as $param) {
			$queryForCommandGroup='';
			$param=(int)$param;
			$deviceCommandGroup = cleanInteger(@$_POST['DeviceCommandGroup_'.$param]);

			$getOldValues = "select FK_DeviceTemplate
								 FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate = $deviceID and FK_DeviceCommandGroup = $param";

			$resOldValuesData = $dbADO->Execute($getOldValues);

			if ($resOldValuesData && $resOldValuesData->RecordCount()==1) {
				if ($deviceCommandGroup==0) {
					$queryForCommandGroup = "delete from DeviceTemplate_DeviceCommandGroup where FK_DeviceTemplate = $deviceID and FK_DeviceCommandGroup = $param";
					$dbADO->Execute($queryForCommandGroup);
					$locationGoTo = "#commands_link";
					//$locationGoTo = "DeviceCommandGroup_".$param;
				} else {
					//nothing: is in database, and it comes selected
				}
			} else {
				//insert record in database
				if ($deviceCommandGroup==1) {
					$queryForCommandGroup = "insert into DeviceTemplate_DeviceCommandGroup(FK_DeviceCommandGroup,FK_DeviceTemplate) values(?,?)";
					$dbADO->Execute($queryForCommandGroup,array($param,$deviceID));
					//$locationGoTo = "DeviceCommandGroup_".$param;
					$locationGoTo = "#commands_link";
				}
			}
		}

		//new command group added: addNewCommandGroupToMasterDevice
		$addNewCommandGroupToMasterDevice = (int)@$_POST['addNewCommandGroupToMasterDevice'];
		if ($addNewCommandGroupToMasterDevice!=0) {
			$query = "insert into DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate , FK_DeviceCommandGroup)
					values ($deviceID,$addNewCommandGroupToMasterDevice)";
			$dbADO->Execute($query);
		}


		//newEventToMasterDevice
		$newEventToMasterDevice = (int)$_POST['newEventToMasterDevice'];
		if ($newEventToMasterDevice!=0) {
			$query = "insert into DeviceTemplate_Event( FK_DeviceTemplate , FK_Event ) values(?,?)";
			$dbADO->Execute($query,array($deviceID,$newEventToMasterDevice));
			$locationGoTo = "EventDesc_{$newEventToMasterDevice}";
		}

		//EventDisplayed
		$usedEvents = cleanString($_POST['eventsListDisplayed']);
		$usedEventsArray = explode(",",$usedEvents);
		foreach ($usedEventsArray as $elem) {
			$desc = cleanString(@$_POST['EventDesc_'.$elem]);
			$selectOldValue = 'select Description from DeviceTemplate_Event where FK_DeviceTemplate = ? and FK_Event = ?';
			$resOldValue = $dbADO->Execute($selectOldValue,array($deviceID,$elem));
			$oldDescForEvent='';
			if ($resOldValue) {
				$row=$resOldValue->FetchRow();
				$oldDescForEvent = $row['Description'];
			}
			if ($desc!='' && $desc!=$oldDescForEvent) {
				$query = 'update DeviceTemplate_Event set Description = ? where FK_DeviceTemplate = ? and FK_Event = ?';
				$dbADO->Execute($query,array($desc,$deviceID,$elem));
				$locationGoTo = "EventDesc_".$elem;
			}
		}

		$newDeviceData = (int)$_POST['newDeviceData'];
		if ($newDeviceData!=0 && $deviceID!=0) {
			$query = "insert into DeviceTemplate_DeviceData( FK_DeviceTemplate , FK_DeviceData) values(?,?)";
			$dbADO->Execute($query,array($deviceID,$newDeviceData));

			$locationGoTo = "Data_Description_{$newDeviceData}";

		}

		if ($isAVDevice!=$old_isAVDevice) {
			if($isAVDevice==1){
				$insertRecord = "INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate) VALUES(?)";
				$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID));
				$controlSQL='INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia (FK_DeviceTemplate,FK_DeviceCategory) VALUES (?,?)';
				$dbADO->Execute($controlSQL,array($deviceID,$GLOBALS['specialized']));
				$dbADO->Execute($controlSQL,array($deviceID,$GLOBALS['InfraredInterface']));
			}else{
				$dbADO->Execute('DELETE FROM DeviceTemplate_AV WHERE FK_DeviceTemplate=?',$deviceID);
				$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE FK_DeviceTemplate=?',$deviceID);
			}
		}


		if ($deviceID!=0 && $description!='' && $category!=0 && $manufacturer!=0) {
			$updateQuery = "UPDATE DeviceTemplate SET
							Description = ?, 
							ImplementsDCE = ?,							
							CommandLine = ?, 
							FK_DeviceCategory = ?,FK_Manufacturer  = ?,
							FK_Package=?,
							IsPlugIn =?,
							IsEmbedded=?,
							InheritsMacFromPC=?,
							IsIPBased=?,
							ConfigureScript=?,
							Comments=?,
							ManufacturerURL=?,
							InternalURLSuffix=?,
							FK_CommMethod=?
							WHERE PK_DeviceTemplate = ?";

			$dbADO->Execute($updateQuery,array($description,$ImplementsDCE,$commandLine,$category,$manufacturer,$package,$isPlugIn,$isEmbedded,$inheritsMacFromPC,$isIPBased,$ConfigureScript,$comments,$manufacturerURL,$internalURLsufix,$commMethod,$deviceID));

			if($isPlugIn==1 && $oldIsPlugIn==0){
				$insertControlledVia = '
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia
						(FK_DeviceTemplate, FK_DeviceTemplate_ControlledVia) 
					VALUES(?,?)';
				$query = $dbADO->Execute($insertControlledVia,array($deviceID,$GLOBALS['rootDCERouter']));

			}
			$addRelated=(int)@$_POST['addRelated'];
			$extra=$_POST['extra'];
			$valueRelated=(int)@$_POST['valueRelated'];
			if($addRelated!=0){
				$dtRelatedArray=getFieldsAsArray('DeviceTemplate','PK_DeviceTemplate',$dbADO,'WHERE PK_DeviceTemplate='.$addRelated);
				if(count($dtRelatedArray)==0){
					header("Location: index.php?section=editMasterDevice&model=$deviceID&errRelated=Device Template no $addRelated does not exist!#dtRelated");
					exit();
				}else{
					//$alreadyExist=getFieldsAsArray('DeviceTemplate_DeviceTemplate_Related','FK_DeviceTemplate_Related',$dbADO,'WHERE FK_DeviceTemplate='.$deviceID.' AND FK_DeviceTemplate_Related='.$addRelated);
					$valueRelated=($extra!='')?$valueRelated.'|'.$extra:$valueRelated;
					$dbADO->Execute('INSERT INTO DeviceTemplate_DeviceTemplate_Related (FK_DeviceTemplate,FK_DeviceTemplate_Related,Value) VALUES (?,?,?)',array($deviceID,$addRelated,$valueRelated));
					$locationGoTo='#dtRelated';
				}
			}

			if((int)@$_POST['delRelated']>0){
				$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceTemplate_Related WHERE PK_DeviceTemplate_DeviceTemplate_Related=?',array((int)$_POST['delRelated']));
				$locationGoTo='#dtRelated';
			}

			if (strstr($locationGoTo,"#")) {
				header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=$TEXT_SAVED_CONST".$locationGoTo);
			} else {
				header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=$TEXT_SAVED_CONST!&lastAction=".$locationGoTo);
			}
		} else {
			header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=".$TEXT_ERROR_MISSING_PARRAMETERS_CONST);
		}


	}
	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.editMasterDevice.{$lastAction}) {document.forms.editMasterDevice.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}

	//$output->setNavigationMenu(array($TEXT_DEVICE_TEMPLATES_CONST=>'index.php?section=deviceTemplates',$description=>'index.php?section=editMasterDevice&model='.$deviceID));

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DEVICE_TEMPLATE_CONST.' :: '.$description);
	$output->output();

}
?>

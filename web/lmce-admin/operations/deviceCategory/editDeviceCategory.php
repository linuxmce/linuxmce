<?php
function editDeviceCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDeviceCategory.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
//$dbADO->debug=false;
//model='.$selectedModel.'
$from = cleanString($_REQUEST['from']);
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$manufacturerID = (int)$_REQUEST['manufacturers'];
$deviceCategSelected = (int)$_REQUEST['deviceCategSelected'];
$deviceSelected = (int)$_REQUEST['deviceSelected'];
$modelSelected = (int)$_REQUEST['model'];

	$query = "select * from DeviceCategory
				WHERE PK_DeviceCategory = ?";
	$res = $dbADO->Execute($query,array($deviceSelected));
	
	if ($res) {
		$row = $res->FetchRow();
		$parentID = $row['FK_DeviceCategory_Parent'];		
		$description = stripslashes($row['Description']);
		$isAV = cleanInteger($row['IsAV']);		
	}
	
		
	if ($action == 'form') {
	$loginRequired='<br>If you are the owner, please <a href="javascript:opener.top.frames[1].location=\'index.php?section=login\';self.close();">login</a> in order to modify this Device Category.';
	$out.='
	<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']).$loginRequired:'').'</div>	
	<form method="post" action="index.php" name="editDeviceCategory">	
	<input type="hidden" name="from" value="'.$from.'">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="section" value="editDeviceCategory">
	<input type="hidden" name="deviceSelected" value="'.$deviceSelected.'">
	<input type="hidden" name="manufacturers" value="'.$manufacturerID.'">
	<input type="hidden" name="deviceCategSelected" value="'.$deviceCategSelected.'">
	<input type="hidden" name="model" value="'.$modelSelected.'">
	
	<table>
	<tr><td>'.$TEXT_DESCRIPTION_CONST.':</td><td><input type="text" name="DeviceCategoryDescription" value="'.$description.'" size="40"></td></tr>
	<tr><td>'.$TEXT_PARENT_CONST.':</td><td>
	<select name="parentID">
		<option value="0">-'.$TEXT_IS_A_TOP_LEVEL_CATEGORY_CONST.'-</option>
	';
	
	$queryMasterDeviceCategories_parents = 'select PK_DeviceCategory,Description from DeviceCategory where FK_Devicecategory_Parent is null and PK_DeviceCategory not in ('.$deviceSelected.') order by Description asc';
		$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row['PK_DeviceCategory']==$deviceCategSelected?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].' #'.$row['PK_DeviceCategory'].'</option>';
								$out.=getCatChilds($row['PK_DeviceCategory'],$row['Description'],$deviceCategSelected,$deviceSelected,$dbADO);								
							}
	
	$out.='		    </select>					
					</td>
				</tr>
				<tr>
					<td>'.$TEXT_IS_AUDIO_VIDEO_CONST.':</td>
					<td>'.$TEXT_YES_CONST.':<input type="radio" value="1" name="IsAV" '.($isAV==1?' checked="checked" ':'').'> &nbsp; 
						'.$TEXT_NO_CONST.':<input type="radio" value="0" name="IsAV" '.($isAV==0?' checked="checked" ':'').'></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
	</table>
	</form>
	<script>
		 	var frmvalidator = new formValidator("editDeviceCategory");
 			frmvalidator.addValidation("DeviceCategoryDescription","req","'.$TEXT_PLEASE_ENTER_A_DESCRIPTION_CONST.'");			
		</script>
	</table>
	';
	
	} else {
		$description = cleanString($_POST['DeviceCategoryDescription']);
		$isAV = cleanInteger($_POST['IsAV']);
		$parentID = cleanInteger($_POST['parentID']);

		if (trim($description)!='') {

			if ($parentID==0) {
				$query = "update DeviceCategory set Description=?,IsAV=?,FK_DeviceCategory_Parent = NULL where PK_DeviceCategory = $deviceSelected";
			} else {
				$query = "update DeviceCategory set Description=?,IsAV=?,FK_DeviceCategory_Parent = $parentID where PK_DeviceCategory = $deviceSelected";
			}
			$dbADO->Execute($query,array($description,$isAV));
			$out.="
				<script>
						alert('$TEXT_DEVICE_CATEGORY_MODIFIED_CONST');					
					    opener.document.forms.{$from}.actionX.value='form';
						opener.document.forms.{$from}.submit();
						self.close();
					</script>
				
				";
		} else {
			header("Location: index.php?section=editDeviceCategory&manufacturers=$manufacturerID&deviceCategSelected=$deviceCategSelected&deviceSelected=$deviceSelected&model=$modelSelected&from=$from");
		}
	}
				
$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DEVICE_CATEGORY_CONST);			
$output->output();  		
}

function getCatChilds($parentID,$parentName,$selectedDeviceCateg,$selectedDevice,$dbADO) {
	$outA='';
	$queryMasterDeviceCategories_childs = 'select * from DeviceCategory where FK_DeviceCategory_Parent = ? and PK_DeviceCategory NOT IN ('.$selectedDevice.') order by Description';
	
		$rs2 = $dbADO->Execute($queryMasterDeviceCategories_childs,array($parentID));
		while ($row2=$rs2->FetchRow()) {
			$outA.='<option '.($row2['PK_DeviceCategory']==$selectedDeviceCateg?' selected ': ' ').' value="'.$row2['PK_DeviceCategory'].'">'.$parentName.' - '.$row2['Description'].' #'.$row2['PK_DeviceCategory'].'</option>';
			$outA.=getCatChilds($row2['PK_DeviceCategory'],$parentName.' - '.$row2['Description'],$selectedDeviceCateg,$selectedDevice,$dbADO);
		}
	return $outA;
}
?>

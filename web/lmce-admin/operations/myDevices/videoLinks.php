<?php
function videoLinks($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/videoLinks.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	$displayedCols=getDevicesArrayFromCategory($GLOBALS['rootCameras'],$dbADO);
	$displayedRows=getDevicesArrayFromCategory($GLOBALS['rootSecurity'],$dbADO);
	
	$cameraIDArray=array_keys($displayedCols);
	$intercomIDArray=array_keys($displayedRows);
	
	$oldValuesArray=array();
	if(count($cameraIDArray)!=0 && count($intercomIDArray)!=0){
		$selectOldValues="
			SELECT * 
				FROM Device_Device_Related 
			WHERE 
				((FK_Device IN (".(join(',',$intercomIDArray)).")) AND (FK_Device_Related IN (".join(',',$cameraIDArray)."))) 
				OR 
				((FK_Device IN (".(join(',',$cameraIDArray)).")) AND (FK_Device_Related IN (".join(',',$intercomIDArray).")))";
		$resOldValues=$dbADO->Execute($selectOldValues);
		$oldValuesArray=array();
		while($rowOldValues=$resOldValues->FetchRow()){
			$oldValuesArray[$rowOldValues['FK_Device']][$rowOldValues['FK_Device_Related']]=1;
		}
	}
		
		
	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="POST" name="videoLinks">
	<input type="hidden" name="section" value="videoLinks">
	<input type="hidden" name="action" value="add">	
		
	<p align="center">'.$TEXT_VIDEO_LINKS_NOTE_CONST.'</p>
		<table align="center">
			<tr class="tablehead">
				<td><B>'.$TEXT_INTERCOM_CAMERA_CONST.'</B></td>';
		$colCount=0;
		foreach($displayedCols AS $key=>$label){
			$colCount++;
			$out.='<td><B>'.$label.'</B></td>';
		}
		$out.='</tr>';
		$rowCount=0;
		foreach($displayedRows AS $row=>$label){
			$rowCount++;
			$out.='
				<tr bgcolor="'.(($rowCount%2==0)?'#E7E7E7':'#FFFFFF').'">
					<td><B>'.$label.'</B></td>';
			foreach($displayedCols AS $col=>$label){
				$out.='<td align="center"><input type="checkbox" name="row_'.$row.'_col_'.$col.'" value="1" '.((isset($oldValuesArray[$row][$col]) && $oldValuesArray[$row][$col]==1)?'checked':'').'></td>';
			}
			$out.='
				<tr>';
		}
		$out.='
			<input type="hidden" name="oldValuesArray" value="'.serialize($oldValuesArray).'">
			<input type="hidden" name="cameraIDArray" value="'.join(',',$cameraIDArray).'">
			<input type="hidden" name="intercomIDArray" value="'.join(',',$intercomIDArray).'">
			<tr>
				<td align="center" colspan="'.($colCount+1).'"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>
		</table>
		</form>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=videoLinks&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		// process and redirect
		$cameraIDArray=explode(',',$_POST['cameraIDArray']);		// columns
		$intercomIDArray=explode(',',$_POST['intercomIDArray']);	// rows
		$oldValuesArray=unserialize($_POST['oldValuesArray']);
		foreach($intercomIDArray AS $row){
			foreach($cameraIDArray AS $col){
				if(isset($oldValuesArray[$row][$col])){
					if(!isset($_POST['row_'.$row.'_col_'.$col])){
						$deleteDevice_DeviceRelated='DELETE FROM Device_Device_Related WHERE FK_Device=? AND FK_Device_Related=?';
						$dbADO->Execute($deleteDevice_DeviceRelated,array($row,$col));
					}
				}else{
					if(isset($_POST['row_'.$row.'_col_'.$col])){
						$insertDevice_DeviceRelated='
							INSERT INTO Device_Device_Related 
								(FK_Device,FK_Device_Related)
							VALUES
								(?,?)';
						$dbADO->Execute($insertDevice_DeviceRelated,array($row,$col));
					}
				}
			}
		}
		
		header("Location: index.php?section=videoLinks&msg=$TEXT_VIDEO_LINKS_UPDATED_CONST");		
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_SECURITY_VIDEO_LINKS_CONST);
	$output->setNavigationMenu(array($TEXT_SECURITY_VIDEO_LINKS_CONST=>'index.php?section=videoLinks'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SECURITY_VIDEO_LINKS_CONST);
	$output->output();
}
?>

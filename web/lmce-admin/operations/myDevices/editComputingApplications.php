<?
function editComputingApplications($output,$dbADO,$mediadbADO) {
	global $wikiHost;
	
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editComputingApplications.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$mdID=(int)$_REQUEST['mdID'];
	$fixedPath='/home/mediapics';
	
	if ($action == 'form') {
		// build pics arrays
		$picsArray=getFieldsAsArray('Picture','PK_Picture,Extension',$mediadbADO);
		$fullPicArray=array();
		for($i=0;$i<count($picsArray['PK_Picture']);$i++)
			$fullPicArray[$picsArray['PK_Picture'][$i].'.'.$picsArray['Extension'][$i]]=$picsArray['PK_Picture'][$i];
		$picsByKey=array_flip($fullPicArray);
		
		
	$mdArray=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
	unset($mdArray[$mdID]);
	$mdCheckboxes='';
	foreach ($mdArray AS $md=>$descr){
		$mdCheckboxes='<input type="checkbox" value="1" name="md_'.$md.'"> '.$descr.'<br>';
	}
	$mdCheckboxes.='<input type="hidden" name="mds" value="'.join(',',array_keys($mdArray)).'">';

		$out.='
	<script>
		function showPreview()
		{
			if(document.editComputingApplications.picture.selectedValue!=0){
				document.editComputingApplications.preview.src=\'include/image.php?imagepath='.$fixedPath.'/\'+document.editComputingApplications.picture.value;
				document.getElementById(\'previewBox\').style.display=\'\';
			}else{
				document.getElementById(\'previewBox\').style.display=\'none\';
			}
		}

		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}

	</script>
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="editComputingApplications" enctype="multipart/form-data">
		<input type="hidden" name="section" value="editComputingApplications">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="mdID" value="'.$mdID.'">	
		<input type="hidden" name="currentPos" value="">
		<h3>'.$TEXT_EDIT_COMPUTING_APPLICATIONS_CONST.'</h3>
		'.$TEXT_COMPUTING_INFO_CONST.'<br>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
				
	
		
		<table cellpadding="3" cellspacing="0">
			<tr class="tablehead">
				<td align="center" colspan="8"><B>'.$TEXT_QUICK_START_TEMPLATES_CONST.'</B></td>
			</tr>
			<tr class="alternate_back">
				<td align="center"><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_TEMPLATE_CATEGORY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_PACKAGE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_BINARY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_ARGUMENTS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_HOMEPAGE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_WINDOW_CLASS_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>';

	$appArray=getFieldsAsArray('QuickStartTemplate','PK_QuickStartTemplate,PK_Device_QuickStart,QuickStartTemplate.Description AS Description,SortOrder,QuickStartTemplate.`Binary` AS `Binary`,QuickStartTemplate.Arguments AS Arguments,EK_Picture,Package.Description AS Package,QuickStartCategory.Description AS Category,QuickStartTemplate.Homepage AS Homepage,WindowClass',$dbADO,'LEFT JOIN Device_QuickStart ON FK_QuickStartTemplate=PK_QuickStartTemplate AND FK_Device='.$mdID.' LEFT JOIN Package ON FK_Package =PK_Package LEFT JOIN QuickStartCategory ON FK_QuickStartCategory=PK_QuickStartCategory','ORDER BY SortOrder ASC');
	$existing=array();
	for($i=0;$i<count(@$appArray['PK_Device_QuickStart']);$i++){
		if(!is_null($appArray['PK_Device_QuickStart'][$i]))
			$existing[]=$appArray['PK_QuickStartTemplate'][$i];
		
		
		$color=($i%2!=0)?'#F0F3F8':'#FFFFFF';
		$pic=(!is_null($appArray['EK_Picture'][$i]))?'<img src="include/image.php?imagepath='.$fixedPath.'/'.$picsByKey[$appArray['EK_Picture'][$i]].'" align="middle">':'&nbsp;';
		$buttons=(!is_null($appArray['PK_Device_QuickStart'][$i]))?'<input type="submit" class="button" value="U" name="u_'.$appArray['PK_Device_QuickStart'][$i].'" onClick="document.editComputingApplications.currentPos.value='.$appArray['SortOrder'][$i].';">&nbsp;<input type="submit" class="button" value="D" name="d_'.$appArray['PK_Device_QuickStart'][$i].'" onClick="document.editComputingApplications.currentPos.value='.$appArray['SortOrder'][$i].';">':'-';
		$out.='
			<input type="hidden" name="device_qs_'.$appArray['PK_QuickStartTemplate'][$i].'" value="'.$appArray['PK_Device_QuickStart'][$i].'">
			<tr bgcolor="'.$color.'">
				<td><input type="checkbox" value="1" name="qst_'.$appArray['PK_QuickStartTemplate'][$i].'" '.((!is_null($appArray['PK_Device_QuickStart'][$i]))?'checked':'').'> '.$appArray['Description'][$i].'</td>
				<td>'.$appArray['Category'][$i].'</td>
				<td>'.$appArray['Package'][$i].'</td>
				<td>'.$appArray['Binary'][$i].'</td>
				<td>'.$appArray['Arguments'][$i].'</td>
				<td>'.$appArray['Homepage'][$i].'</td>
				<td>'.$appArray['WindowClass'][$i].'</td>
				<td align="center">'.$buttons.'</td>
			</tr>
		';
	}
	
	$out.='
		<input type="hidden" name="apps" value="'.@join(',',@array_values($appArray['PK_QuickStartTemplate'])).'">
		<input type="hidden" name="existing" value="'.@join(',',$existing).'">';
		
		$out.='
			<tr class="alternate_back">
				<td colspan="8"><B>'.$TEXT_ADD_NEW_APPLICATION_CONST.'</B> &nbsp; <a href="javascript:windowOpen(\'index.php?section=editQuickStartTemplates\',\'\')">'.$TEXT_CLICK_HERE_CONST.'</a></td>
			</tr>';
	if(count(@$appArray['PK_Device_QuickStart'])>0){
		$out.='
			<tr>
				<td colspan="8">Need help? <a href="'.$wikiHost.'index.php/Adding_new_computing_apps" target="_blank">Click here</a>.</td>
			</tr>
			<tr>
				<td colspan="8" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
			</tr>
		';
	}
	$out.='		
		</table>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editComputingApplications&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		$apps=explode(',',$_POST['apps']);
		$existing=explode(',',$_POST['existing']);
		$currentPos=(int)$_POST['currentPos'];
		foreach ($apps AS $app){
	
			if(isset($_POST['qst_'.$app])){
				if(!in_array($app,$existing)){
					$dbADO->Execute("INSERT INTO Device_QuickStart (FK_Device,FK_QuickStartTemplate,Description) SELECT ?,?,Description FROM QuickStartTemplate WHERE PK_QuickStartTemplate=?", array($mdID,$app,$app));
					$id=$dbADO->Insert_ID();
					$dbADO->Execute("UPDATE Device_QuickStart SET SortOrder=? WHERE PK_Device_QuickStart=?",array($id,$id));
				}
			}else{
				if(in_array($app,$existing)){
					$dbADO->Execute('DELETE FROM Device_QuickStart WHERE FK_QuickStartTemplate=? AND FK_Device=?',array($app,$mdID));				
				}
			}

			$device_qs=$_POST['device_qs_'.$app];
			
			// move up
			if((int)$device_qs!=0 && isset($_POST['u_'.$device_qs])){
				$res=$dbADO->Execute('SELECT * FROM Device_QuickStart WHERE FK_Device=? AND SortOrder<? ORDER BY SortOrder DESC LIMIT 0,1',array($mdID,$currentPos));
				if($res->RecordCount()>0){
					$row=$res->FetchRow();
					$dbADO->Execute('UPDATE Device_QuickStart SET SortOrder=? WHERE PK_Device_QuickStart=?',array($row['SortOrder'],$device_qs));
					$dbADO->Execute('UPDATE Device_QuickStart SET SortOrder=? WHERE PK_Device_QuickStart=?',array($currentPos,$row['PK_Device_QuickStart']));
				}
			}

			// move down

			if((int)$device_qs!=0 && isset($_POST['d_'.$device_qs])){
				$res=$dbADO->Execute('SELECT * FROM Device_QuickStart WHERE FK_Device=? AND SortOrder>? ORDER BY SortOrder ASC LIMIT 0,1',array($mdID,$currentPos));
				if($res->RecordCount()>0){
					$row=$res->FetchRow();
					$dbADO->Execute('UPDATE Device_QuickStart SET SortOrder=? WHERE PK_Device_QuickStart=?',array($row['SortOrder'],$device_qs));
					$dbADO->Execute('UPDATE Device_QuickStart SET SortOrder=? WHERE PK_Device_QuickStart=?',array($currentPos,$row['PK_Device_QuickStart']));
				}
			}
			
			$description=cleanString(@$_POST['description_'.$app]);
			$binary=cleanString(@$_POST['binary_'.$app]);
			$arguments=cleanString(@$_POST['arguments_'.$app]);
			$dbADO->Execute('UPDATE Device_QuickStart SET Description=? WHERE PK_Device_QuickStart=?',array($description,$app));
		}
		
		$sufix=(@$err!='')?'&error='.$err:'&msg='.$TEXT_COMPUTING_APPLICATIONS_UPDATED_CONST;
		
		header("Location: index.php?section=editComputingApplications&mdID=$mdID".$sufix);		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_COMPUTING_APPLICATIONS_CONST);
	$output->output();
}


function getQuickStartTemplates($dbADO){
	$res=$dbADO->Execute('SELECT * FROM QuickStartTemplate');
	$qsTemplates=array();
	while($row=$res->FetchRow()){
		$qsTemplates[$row['FK_QuickStartCategory']][]=$row['PK_QuickStartTemplate'];
		
		$qsTemplates['description'][$row['PK_QuickStartTemplate']]=$row['Description'];
	}
	
	return $qsTemplates;
}

function getQuickStartTree($usedTemplates,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editComputingApplications.lang.php');
	
	$nodes=getNodesArray('QuickStartCategory','PK_QuickStartCategory','FK_QuickStartCategory_Parent',$dbADO);
	$quickStartTemplates=getQuickStartTemplates($dbADO);
	
	$out='
		<input type="hidden" name="qst" value="">
	<table cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td colspan="2"><B>'.$TEXT_QUICK_START_TEMPLATES_CONST.'</B></td>
		</tr>';
	foreach ($nodes['root_node'] AS $rootNode){
		$out.='
		<tr>
			<td bgcolor="#EEEEEE" colspan="2"><B>'.$nodes['description'][$rootNode].'</B></td>
		</tr>';		
		if(count(@$quickStartTemplates[$rootNode])==0){
			$out.='
				<tr>
					<td style="padding-left:20px;" colspan="2">'.$TEXT_NO_QUICK_START_TEMPLATES_CATEGORY_CONST.'</td>
				</tr>
			';
		}
		foreach ($quickStartTemplates[$rootNode] AS $qsTemplate){
			if(!in_array($qsTemplate,$usedTemplates)){
			$out.='
				<tr>
					<td style="padding-left:20px;">'.$quickStartTemplates['description'][$qsTemplate].'</td>
					<td align="right"><a href="javascript:document.editComputingApplications.action.value=\'addFromTemplate\'; document.editComputingApplications.qst.value=\''.$qsTemplate.'\';document.editComputingApplications.submit();">[ '.$TEXT_ADD_CONST.' ]</a></td>
				</tr>
			';
			}
		}
		$out.=getQuickStartChilds($nodes,$quickStartTemplates,$rootNode,$usedTemplates);

	}
	$out.='
		<tr>
			<td colspan="2"><a href="javascript:windowOpen(\'index.php?section=editQuickStartTemplates\',\'\')">'.$TEXT_ADD_OTHER_SOFTWARE_CONST.'</a> '.$TEXT_ADD_OTHER_SOFTWARE_NOTE_CONST.'</td>
		</tr>	
	</table>';
	
	return $out;
}

function getQuickStartChilds($nodes,$quickStartTemplates,$selectedCategory,$usedTemplates){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editComputingApplications.lang.php');
	
	$out='';
	foreach ($nodes[$selectedCategory] AS $childNode){
		$out.='
		<tr>
			<td bgcolor="#EEEEEE" style="padding-left:10px;" colspan="2"><B>'.$nodes['description'][$selectedCategory].' - '.$nodes['description'][$childNode].'</B></td>
		</tr>';		
		if(count(@$quickStartTemplates[$childNode])==0){
			$out.='
				<tr>
					<td style="padding-left:20px;" colspan="2">'.$TEXT_NO_QUICK_START_TEMPLATES_CATEGORY_CONST.'</td>
				</tr>
			';
		}
		foreach ($quickStartTemplates[$childNode] AS $qsTemplate){
			if(!in_array($qsTemplate,$usedTemplates)){
			$out.='
				<tr>
					<td style="padding-left:20px;">'.$quickStartTemplates['description'][$qsTemplate].'</td>
					<td align="right"><a href="javascript:document.editComputingApplications.action.value=\'addFromTemplate\'; document.editComputingApplications.qst.value=\''.$qsTemplate.'\';document.editComputingApplications.submit();">[ '.$TEXT_ADD_CONST.' ]</a></td>
				</tr>
			';
			}
		}
		
		$out.=getQuickStartChilds($nodes,$quickStartTemplates,$childNode,$usedTemplates);		
	}
	
	return $out;
}
?>

<?
function editLongAttribute($output,$mediadbADO) {
	global $mediaTypes;
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$laID=(int)@$_REQUEST['laID'];
	$fileID=(int)@$_REQUEST['fileID'];
	$data=getFieldsAsArray('LongAttribute','Text,FK_AttributeType',$mediadbADO,'WHERE PK_LongAttribute='.$laID);
		
	if($action=='form'){
		$attributeTypes=getAssocArray('AttributeType','PK_AttributeType','Description',$mediadbADO,'INNER JOIN MediaType_AttributeType ON FK_AttributeType=PK_AttributeType INNER JOIN File ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType AND PK_File='.$fileID);

		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="editLongAttribute">
			<input type="hidden" name="section" value="editLongAttribute">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="laID" value="'.$laID.'">
			<input type="hidden" name="fileID" value="'.$fileID.'">
			
			
		<a href="index.php?section=editMediaFile&fileID='.$fileID.'">'.$TEXT_BACK_CONST.'</a><br><br>
		<table>
			<tr>
				<td><B>'.$TEXT_ATTRIBUTE_TYPE_CONST.'</B></td>
				<td>'.pulldownFromArray($attributeTypes,'longAttributeType',$data['FK_AttributeType'][0]).'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_LONG_ATTRIBUTE_CONST.'</B></td>
				<td><textarea name="longAttributeText" style="width:500px;height:400px;">'.$data['Text'][0].'</textarea></td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_UPDATE_CONST.'"></td>
			</tr>
		</table>		
		</form>';
	}else{
		
		$longAttributeType=(int)@$_POST['longAttributeType'];
		$longAttributeText=cleanString($_POST['longAttributeText']);
		if($longAttributeType!=0 && $longAttributeType!=''){
			$mediadbADO->Execute('UPDATE LongAttribute SET FK_AttributeType=?,Text=? WHERE PK_LongAttribute=?',array($longAttributeType,$longAttributeText,$laID));
		}else{
			header('Location: index.php?section=editLongAttribute&laID='.$laID.'&fileID='.$fileID.'&error='.urlencode($TEXT_TYPE_TEXT_REQUIRED_CONST));
			exit();
		}

		
		header('Location: index.php?section=editLongAttribute&laID='.$laID.'&fileID='.$fileID.'&msg='.urlencode($TEXT_ATTRIBUTE_UPDATED_CONST));
		exit();
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_LONGATTRIBUTE_CONST);
	$output->setNavigationMenu(array($TEXT_EDIT_LONGATTRIBUTE_CONST=>'index.php?section=editLongAttribute&laID='.$laID.'&fileID='.$fileID));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_LONGATTRIBUTE_CONST);
	$output->output();
}

?>
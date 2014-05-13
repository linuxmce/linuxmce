<?php
function mainMediaBrowser($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaBrowser.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/utils.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';

	$picsPerLine=6;
	
	if($action=='form'){
		if(isset($_POST['displayedTypes'])){
			$displayedTypes=@$_POST['displayedTypes'];
			$checkedTypes=array();
			$displayedTypesArray=explode(',',$displayedTypes);
			foreach($displayedTypesArray AS $value)
				if(isset($_POST['type_'.$value]))
					$checkedTypes[]=$value;
		}else 
			$checkedTypes=explode(',',@$_SESSION['checkedTypes']);
			
		if(@$_REQUEST['searchType']!=''){
			$searchString=$_REQUEST['searchType'].'%';
			unset($_SESSION['mediaSearchString']);
		}else
			$searchString='%'.@$_SESSION['mediaSearchString'].'%';
		if(count($checkedTypes)>0){
			$_SESSION['searchString']=$searchString;
			$_SESSION['checkedTypes']=join(',',$checkedTypes);
		}
		$checkedTypesArray=explode(',',@$_SESSION['checkedTypes']);
		$out.='
			<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
				<input type="hidden" name="section" value="mainMediaBrowser">
				<input type="hidden" name="action" value="">
				<input type="hidden" name="searchType" value="">
				<input type="hidden" name="attributeID" value="">
		';
		$queryTypes='SELECT * FROM AttributeType ORDER BY Description ASC';
		$resTypes=$mediadbADO->Execute($queryTypes);
		$displayedTypes=array();
		$typeBox='<table border="0">
			<tr>
		';
		$pos=0;
		while($rowTypes=$resTypes->FetchRow()){
			$pos++;
			$displayedTypes[]=$rowTypes['PK_AttributeType'];
			$typeBox.='<td><input type="checkbox" name="type_'.$rowTypes['PK_AttributeType'].'" value="1" '.((isset($_POST['type_'.$rowTypes['PK_AttributeType']])|| in_array($rowTypes['PK_AttributeType'],$checkedTypesArray))?'checked':'').'> '.$rowTypes['Description'].'</td>';
			if($pos%4==0){
				$typeBox.='</tr><tr>';
			}
			

		}
		$typeBox.='</tr></table>';
		
		$out.=$typeBox;
		$out.='
			<input type="hidden" name="displayedTypes" value="'.join(',',$displayedTypes).'">
		</form>';
	
		if(isset($_SESSION['checkedTypes']) && $_SESSION['checkedTypes']!=''){
			$out.='<table>';
			if($_SESSION['searchString']!='none%'){
				$querySearch='
						SELECT Attribute.*, AttributeType.Description AS AttrType 
							FROM Attribute 
						INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
						WHERE Name LIKE ? AND FK_AttributeType IN ('.$_SESSION['checkedTypes'].') 
						ORDER BY Name ASC';
				$resSearch=$mediadbADO->Execute($querySearch,array($_SESSION['searchString']));	
			}
			else{
				$querySearch="
						SELECT Attribute.*, AttributeType.Description AS AttrType 
							FROM Attribute 
						INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
						WHERE (Name REGEXP '^[^a-zA-Z]') AND FK_AttributeType IN (".$_SESSION['checkedTypes'].") 
						ORDER BY Name ASC";
				$resSearch=$mediadbADO->Execute($querySearch);
			}
			if($resSearch->RecordCount()==0){
				$out.='
					<tr>
						<td colspan="3">'.$TEXT_NO_RECORDS_CONST.'</td>
					</tr>
				';
			}
			while($rowSearch=$resSearch->FetchRow()){
				$out.='
						<tr>
							<td>'.$rowSearch['Name'].'</td>
							<td>'.$rowSearch['AttrType'].'</td>
							<td><a href="#" onClick="document.mainMediaBrowser.action.value=\'properties\';document.mainMediaBrowser.attributeID.value='.$rowSearch['PK_Attribute'].';document.mainMediaBrowser.submit();">'.$TEXT_PROPERTIES_CONST.'</a></td>
						</tr>';
			}
			$out.='</table>';
		}else
			$out.='No attribute type selected.<br>Please select an attribute type from the checkboxes above, then type your search string or click on a letter in left menu.';
	}elseif($action=='properties'){
		$attributeID=$_REQUEST['attributeID'];
		$queryAttribute='SELECT * FROM Attribute WHERE PK_Attribute=?';		
		$resAttribute=$mediadbADO->Execute($queryAttribute,$attributeID);
		$rowAttribute=$resAttribute->FetchRow();
		$attrType=$rowAttribute['FK_AttributeType'];
		
		if(isset($_REQUEST['fileID'])){
			$fileID=(int)$_REQUEST['fileID'];
			$resDelFile=$mediadbADO->Execute('SELECT * FROM File WHERE PK_File=?',$fileID);
			if($resDelFile->RecordCount()>0){
				$rowDelFile=$resDelFile->FetchRow();
				$delFilePath=$rowDelFile['Path'].'/'.$rowDelFile['Filename'];
				if(file_exists($delFilePath)){
					unlink($delFilePath);
				}
			}
			$mediadbADO->Execute('DELETE FROM File WHERE PK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File=?',$fileID);
		}
		
		
		if(isset($_REQUEST['picID'])){
			$toDelete=$_REQUEST['picID'];
			
			//Remove the reference to the picture from the attribute
			$deletePicAttribute='DELETE FROM Picture_Attribute WHERE FK_Picture=? AND FK_Attribute=?';
			$mediadbADO->Execute($deletePicAttribute,array($toDelete,$attributeID));

			delete_media_pic_if_unused($toDelete, $mediadbADO);
		}
		$out.='
			<div class="err">'.((isset($_REQUEST['error']))?$_REQUEST['error']:'').'</div>
			<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
				<input type="hidden" name="section" value="mainMediaBrowser">
				<input type="hidden" name="action" value="">
				<input type="hidden" name="searchType" value="">
				<input type="hidden" name="attributeID" value="'.$attributeID.'">
		
			<table border="0">
				<tr>
					<td colspan="4" align="left"><h3>'.$TEXT_ATTRIBUTE_CONST.'</h3></td>
				</tr>		
				<tr>
					<td><B>Name:</B> </td>
					<td><input type="text" name="attrName" value="'.preg_replace('/\"/','',$rowAttribute['Name']).'" size="40"></td>
					<td><B>&nbsp;</B> </td>
					<td><input type="hidden" name="attrType" value="'.$rowAttribute['FK_AttributeType'].'"></td>
				</tr>
				<tr>
					<td colspan="4" align="left"><input type="button" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>
				<tr>
					<td colspan="4" align="center">&nbsp;</td>
				</tr>
				<tr>
					<td valign="top" align="left" colspan="4">';
			$queryFiles='
				SELECT FK_File, FK_Attribute,EK_MediaType, Path, Filename,PK_File, Missing
				FROM File_Attribute
					INNER JOIN File ON File_Attribute.FK_File=PK_File
				WHERE FK_Attribute=?';
			$GLOBALS['attrType']=$attrType;
			$GLOBALS['attributeID']=$attributeID;
		$out.=multi_page($queryFiles, $attributeID,'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties', (isset($_GET['page_no']))?$_GET['page_no']-1:0, 20,$mediadbADO);
		
			$fileTypeSelect='<select name="newType">
				<option value="0">-'.$TEXT_PLEASE_SELECT_CONST.'-</option>';
			$selectFileType='SELECT * FROM MediaType ORDER BY Description ASC';
			$resFileType=$dbADO->Execute($selectFileType);
			while($rowFileType=$resFileType->FetchRow()){
				$fileTypeSelect.='<option value="'.$rowFileType['PK_MediaType'].'">'.$rowFileType['Description'].'</option>';
			}
			$fileTypeSelect.='</select>';
			$out.='<table></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_ADD_FILE_CONST.'</B></td>
					<td colspan="6">'.$TEXT_NAME_CONST.': <input type="text" name="newFile" value=""> Path: <input type="text" name="newPath" value=""> Type: '.$fileTypeSelect.' <input type="button" class="button" name="update" value="Add file" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>
				<tr>
					<td valign="top" align="left"><B>'.$TEXT_PICTURES_CONST.'</B></td>
					<td valign="top" align="left" colspan="6"><table>
				<tr>';
			$queryPictures='
				SELECT * FROM Picture_Attribute
				INNER JOIN Picture ON Picture_Attribute.FK_Picture=PK_Picture
				WHERE FK_Attribute=?';
			$resPictures=$mediadbADO->Execute($queryPictures,$attributeID);
			$picsCount=0;
			while($rowPictures=$resPictures->FetchRow()){
				$picsCount++;
				$out.='
					<td style="background-color:#EEEEEE;" align="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a> <br><a href="#" onClick="if(confirm(\'Are you sure you want to delete this picture?\'))self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties&picID='.$rowPictures['PK_Picture'].'\';">'.$TEXT_DELETE_CONST.'</a></td>
				';
				if($picsCount%$picsPerLine==0)
					$out.='</tr><tr>';
			}
			$out.='
					</tr>
				</table></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_ADD_PICTURE_CONST.'</B></td>
					<td colspan="6"> <input type="file" name="newPic" value=""> <input type="button" class="button" name="update" value="'.$TEXT_ADD_PICTURE_CONST.'" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>

			</table>
		</form>
		';
		
	}else{
		$name=stripslashes($_POST['attrName']);
		$attrType=($_POST['attrType']!='0')?(int)$_POST['attrType']:NULL;
		$attributeID=(int)$_POST['attributeID'];
		$GLOBALS['attrType']=$attrType;
		
		if(!isset($_POST['merge'])){
			$queryExistingAttribute='
				SELECT Attribute.*, AttributeType.Description AS AttributeType,
				FK_File, FK_Attribute,EK_MediaType, Path, Filename,PK_File, Missing
				FROM Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				inner join File_Attribute fa on fa.FK_Attribute=Attribute.PK_Attribute
				inner join `File` f on fa.FK_File=f.PK_File
				WHERE Name=? AND FK_AttributeType=? AND PK_Attribute!=?';
			$resExistingAttribute=$mediadbADO->Execute($queryExistingAttribute,array($name,$attrType,$attributeID));
			$total = $resExistingAttribute->RecordCount();
			if($total>0){
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				$out.='
					<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
						<input type="hidden" name="section" value="mainMediaBrowser">
						<input type="hidden" name="searchType" value="">
						<input type="hidden" name="action" value="update">
						<input type="hidden" name="attrName" value="'.$name.'">
						<input type="hidden" name="attrType" value="'.$attrType.'">
						<input type="hidden" name="attributeID" value="'.$attributeID.'">
						<input type="hidden" name="existingAttributeID" value="'.$rowExistingAttribute['PK_Attribute'].'">
				<table>
					<tr>
						<td height="40">
							'.$TEXT_ATTRIBUTE_EXIST_NOTICE_CONST.'
						</td>
					</tr>
					<tr>
						<td align="center">
							<input type="submit" class="button" name="merge" value="'.$TEXT_YES_MERGE_CONST.'">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="button" name="cancel" value="'.$TEXT_CANCEL_CONST.'" onClick="self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties\';"> 
						</td>
					</tr>
				
				</table>
					</form>
				';
				$AttrIDs = $rowExistingAttribute['PK_Attribute'];
						$out .=$TEXT_FOUND_CONST.': '.$total;
						$out.='<table>
					<tr class="tablehead">
						<td align="center"><B>'.$TEXT_FILES_CONST.'</B></td>
						<td align="center">&nbsp;</td>
					</tr>';
			$out .= multi_page_format($rowExistingAttribute,$index++,$mediadbADO);
			while ($rowExistingAttribute = $resExistingAttribute->FetchRow()) {
				  $out .= multi_page_format($rowExistingAttribute,$index++,$mediadbADO);
			}

			$out.='</td>
				</tr>

			</table>
		';
				
				$dontRedirect=true;
			}else{
				$updateCmd=updateAttribute($name,$attributeID,$dbADO);
			}
				
		}else{
			$existingAttributeID=(int)$_POST['existingAttributeID'];
			$name=stripslashes($_POST['attrName']);
			$attrType=($_POST['attrType']!='0')?(int)$_POST['attrType']:NULL;
			$attributeID=(int)$_POST['attributeID'];
			$queryExistingAttribute='
				SELECT Attribute.*, AttributeType.Description AS AttributeType,
				FK_File, FK_Attribute,EK_MediaType, Path, Filename,PK_File, Missing
				FROM Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				inner join File_Attribute fa on fa.FK_Attribute=Attribute.PK_Attribute
				inner join `File` f on fa.FK_File=f.PK_File
				WHERE Name=? AND FK_AttributeType=? AND PK_Attribute!=?';
			$resExistingAttribute=$mediadbADO->Execute($queryExistingAttribute,array($name,$attrType,$attributeID));
			while ($row=$resExistingAttribute->FetchRow()){
			
			$existingAttributeID = $row[PK_Attribute];
			// delete current attribute
			$deleteCurrentAttribute='DELETE FROM Attribute WHERE PK_Attribute=?';
			$mediadbADO->Execute($deleteCurrentAttribute,$existingAttributeID);
			
			//	asign Files and Pictures from current attribute to existing one
			$updateFileAttribute='UPDATE IGNORE File_Attribute SET FK_Attribute=? WHERE FK_Attribute=?';
			$mediadbADO->Execute($updateFileAttribute,array($attributeID,$existingAttributeID));
			
			$updatePictureAttribute='UPDATE IGNORE Picture_Attribute SET FK_Attribute=? WHERE FK_Attribute=?';
			$mediadbADO->Execute($updatePictureAttribute,array($attributeID,$existingAttributeID));
			}
		#update the merged attribute (if required)
		$queryUpdateAttribute = "UPDATE Attribute set Name=? where PK_Attribute=?";
		$mediadbADO->Execute($queryUpdateAttribute,array($name,$attributeID));
		}		

		$newFile=stripslashes(@$_POST['newFile']);
		$newPath=stripslashes(@$_POST['newPath']);
		$newType=(@$_POST['newPath']!='0')?@$_POST['newPath']:NULL;
		
		if(($newFile!='' && $newPath!='')){
			$insertFile='INSERT INTO File(Path, Filename, EK_MediaType,DateAdded) VALUES (?,?,?,NOW())';
			$mediadbADO->Execute($insertFile,array($newPath,$newFile,$newType));
			
			$insertID=$mediadbADO->Insert_ID();
			$insertFile_Attribute='INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)';
			$mediadbADO->Execute($insertFile_Attribute,array($insertID,$attributeID));
		}
		
		if(isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
			$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
			$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
			$mediadbADO->Execute($insertPicture,$picExtension);
			$insertID=$mediadbADO->Insert_ID();
			$newPicName=$insertID.'.'.$picExtension;
			
			$error='';
			// mihai.t: We need the file types to be png actually since this is the format that the tablet software will understand
			// vali.g: revert to jpgs
			if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
				$error=$TEXT_ERROR_THIS_IS_NOT_JPEG_CONST;
			}
			elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
				// create thumbnail
				$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 256, 256);
				// update database
				$insertPictureAttribute='INSERT INTO Picture_Attribute (FK_Attribute, FK_Picture) VALUES (?,?)';
				$mediadbADO->Execute($insertPictureAttribute,array($attributeID,$insertID));
			}else{
				//upload fail, prompt error message
				$deletePicture='DELETE FROM Picture WHERE PK_Picture=?';
				$mediadbADO->Execute($deletePicture,$insertID);
				$error=$TEXT_ERROR_UPLOAD_FAILED_CONST;
			}
			
		}
		if(!isset($dontRedirect))
			$out.='
				<script>
					self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties'.((isset($error))?'&error='.$error:'').((isset($updateCmd))?'&msg='.urlencode($updateCmd):'').'\';
				</script>
				';
		
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MEDIA_BROWSER_CONST);
	$output->setReloadLeftFrame(false);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function updateAttribute($name,$attributeID,$dbADO){
	$mediaPlugin=getMediaPluginID($_SESSION['installationID'],$dbADO);
	if(is_null($mediaPlugin)){
		header("Location: index.php?section=mainMediaBrowser&attributeID=$attributeID&action=properties&error=Media plugin not found.");
		exit();
	}

	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -r 0 '.$mediaPlugin.' 1 392 123 '.$attributeID.' 5 "'.$name.'"';

	$response=exec_batch_command($cmd,1);

	return $cmd.'<br>Response: '.$response;
	
}
?>

<?php
function editDirectoryAttributes($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$mediaSubTypeEnum = array (1 => "TV Shows",2 => "Movies",3 => "Home Videos", 4 => "Sports Events", 5 => "Music Videos", 6 => "Alternative", 7 => "Popular Music", 8 => "Classical Music");
	$fileFormatEnum = array (1 => "Low Res",2 => "DVD",3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=$_REQUEST['fileID'];
	$dirData=getRows('File','File.*',$mediadbADO,'WHERE PK_File=\''.$fileID.'\'');
	$filesArray=getAssocArray('File','PK_File','Filename',$mediadbADO,'WHERE Path LIKE '.escape(@$dirData[0]['Path'].'/'.@$dirData[0]['Filename']).' AND Missing=0 AND PK_File!=\''.$fileID.'\' AND IsDirectory=0 ORDER BY Filename ASC');
	
	if($action=='form'){
	$attributeTypes=getAssocArray('AttributeType','PK_AttributeType','Description',$mediadbADO,'','ORDER BY Description ASC');
	
	$scriptInHead='
	<script>
function confirmation() 
{
	var answer = confirm("Confirm Cleaning Files?");	
	if (answer){
		alert("Cleaning");
		document.forms[1].clean_value=true;
			document.forms[1].submit();
	}
	else{
		alert("Aborting Process");
	}
}
	function setAll(bylink){
		if(bylink==1){
			editDirectoryAttributes.all.checked=(editDirectoryAttributes.all.checked)?false:true;
		}
		val=editDirectoryAttributes.all.checked;

		for (i = 0; i < editDirectoryAttributes.elements.length; i++)
		   {
			tmpName=editDirectoryAttributes.elements[i].name;
		     if (editDirectoryAttributes.elements[i].type == "checkbox" && tmpName.indexOf("file_")!=-1)
		     {
		         editDirectoryAttributes.elements[i].checked = val;
		     }
		   }
		}	

		function setAttributeName()
		{
			document.editDirectoryAttributes.newAttributeName.value=document.editDirectoryAttributes.existingAttributes[document.editDirectoryAttributes.existingAttributes.selectedIndex].text
		}
		
		function syncPath(path)
		{
			top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
			self.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
		}		
	</script>
	';
		
		$out='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			
		<table width="100%">
			<tr>
				<td><a href="javascript:syncPath(\''.$dirData[0]['Path'].'\')">Back</a></td>
				<td align="right">'.quick_search_box().'</td>
			<tr>
		</table>
					
			<form action="index.php" method="POST" name="editDirectoryAttributes" enctype="multipart/form-data">
				<input type="hidden" name="section" value="editDirectoryAttributes">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="fileID" value="'.$fileID.'">
		';
		if(count($dirData)==0){
			$out.='<span class="err">'.$TEXT_DIRECTORY_NOT_SPECIFIED_CONST.'</span>';
		}else{
			if(count($filesArray)==0){
				$out.='<span class="err">'.$TEXT_NO_FILES_ON_DIRECTORY_CONST.'</span>';
			}else{
				$out.='<table>';
				foreach ($filesArray AS $id=>$filename){
					$file_checked = !isset($_POST['newAttributeType']) || (int)@$_POST['file_'.$id]==1 ? 'checked' : '';
					$out.='
					<tr>
						<td><input type="checkbox" name="file_'.$id.'" value="1" '.$file_checked.'></td>
						<td><a href="index.php?section=editMediaFile&fileID='.$id.'">'.$filename.'</a></td>
					</tr>
					';
				}
				$out.='
					<tr>
						<td colspan="4"><hr width="60%" align="left"></td>
					</tr>				
					<tr>
						<td><input type="checkbox" value="1" name="all" onClick="setAll(0);" checked></td>
						<td colspan="3"><a href="javascript:setAll(1);">Select/unselect all</a> </td>
					</tr>
					<tr>
						<td colspan="4">																							
						<fieldset><legend><B>'.$TEXT_ADD_PICTURE_CONST.'</B><em>'.$TEXT_JPG_ONLY_CONST.'</em></legend>
							<table>
							<tr>
								<td>'.$TEXT_FILE_CONST.'</td>
								<td><input type="file" name="newPic" value=""></td>
							</tr>
							<tr>
								<td>'.$TEXT_URL_CONST.'</td>
								<td><input type="text" name="newUrl" value=""></td>
							</tr>						
							<tr>
								<td><input type="checkbox" value="1" name="replace_pics"></td>
								<td>'.$TEXT_REPLACE_EXISTING_PICS_CONST.'</td>
							</tr>							
							<tr>
								<td></td>
								<td><input type="submit" class="button" name="addPic" value="'.$TEXT_ADD_PICTURE_CONST.'"></td>
							</tr>							
						</table>
						</fieldset>
						</td>
					</tr>				
					<tr>
						<td colspan="4"><hr width="60%" align="left"></td>
					</tr>	
					<tr>	
			<td>
			<input type="button" name="cleanFiles" value="'.$TEXT_CLEAN_FILES_CONST.'" onClick="confirmation()" ><br>
			<input type="hidden" name="clean_value" value="false" id="clean_value"/>							
			</td>
			</tr>				
					<tr>
						<td colspan="4"><fieldset><legend><B>'.$TEXT_ADD_ATTRIBUTE_CONST.'</B></legend>			
						<table>
						<tr>
							<td>&nbsp;</td>
							<td>&nbsp;</td>
						</tr>
						
						<tr>
							<td><B>'.$TEXT_ADD_ATTRIBUTE_CONST.':</B><br>
							'.pulldownFromArray($attributeTypes,'newAttributeType',(int)@$_POST['newAttributeType'],'onChange="document.editDirectoryAttributes.action.value=\'form\';document.editDirectoryAttributes.submit();"').'
							<br><input type="checkbox" name="replace_attributes" value="1"> '.$TEXT_REPLACE_ATTRIBUTES_CONST.'
							
							</td>
							<td><B>'.$TEXT_ATTRIBUTE_NAME_CONST.' *</B><br><input type="text" id="newAttributeName" name="newAttributeName" value="" onKeyPress="document.editDirectoryAttributes.existingAttributes.selectedIndex=-1;"></td>
						</tr>';
						if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
							$newAttributeType=(int)$_POST['newAttributeType'];
							$resAttributesByType=$mediadbADO->Execute('SELECT * FROM Attribute WHERE FK_AttributeType=? ORDER BY Name ASC',$newAttributeType);
							$out.='<script>var newANF = document.getElementById("newAttributeName"); if (newANF) newANF.focus();</script>
							<tr>
								<td>&nbsp;</td>
								<td>';
							if($resAttributesByType->RecordCount()>0){
								$out.='
							<select name="existingAttributes" size="20" onClick="setAttributeName();" onChange="setAttributeName();">';
							while($rowAttributesByType=$resAttributesByType->FetchRow()){
								$out.='<option value="'.$rowAttributesByType['PK_Attribute'].'">'.$rowAttributesByType['Name'].'</option>';
							}
							$out.='
								</select>';
							}
							$out.='
							<input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
							</tr>';
					}
					
				$out.='						
						</table>
						</fieldset></td>
					</tr>
					
				<tr>				
						<td colspan="4">																							
						<fieldset><legend><B>'.$TEXT_FILE_FORMAT_CONST.'</B></legend>
							<table>
							<tr>
								<td>'.$TEXT_FILE_FORMAT_CONST.'</td>
								<td><select name="fileFormat" >';
								foreach ($fileFormatEnum as $key => $value)
								{
								$out.='<option value="'.$key.'">'.$value.'</option>';
								}								
								$out.='</select>
								</td>
							</tr>
							<tr>
								<td>'.$TEXT_SUB_TYPE_CONST.'</td>
								<td><select name="subType" >';
								foreach ($mediaSubTypeEnum as $key => $value)
								{
								$out.='<option value="'.$key.'">'.$value.'</option>';
								}								
								$out.='</select>
								</td>
							</tr>						
													
							<tr>
								<td></td>
								<td><input type="submit" class="button" name="MediaSubTypes" value="Update"></td>
							</tr>	
												
						</table>
						</fieldset>
															
				</table>';
			}			
		}
		$out.='
		</form>';
	}else{
	// process area
	
		if(isset($_REQUEST['addPic'])){
			if(isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
				$newUrl=cleanString($_POST['newUrl']);
				$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
				$picExtension=($picExtension=='jpeg')?'jpg':$picExtension;
				
				$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array($picExtension,$newUrl));
				$insertID=$mediadbADO->Insert_ID();
				$newPicName=$insertID.'.'.$picExtension;
				
				$error='';
				if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
					$error=$TEXT_ERROR_NOT_JPEG_CONST;
				}
				elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
					// create thumbnail
					resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 256, 256);
					
					// update database
					add_pic_to_files($insertID,$filesArray,$mediadbADO);
				}else{
					//upload fail, prompt error message
					$mediadbADO->Execute('DELETE FROM Picture WHERE PK_Picture=?',$insertID);
					$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
				}
			}elseif($_POST['newUrl']!='' && $_FILES['newPic']['name']==''){
				$url=cleanString($_POST['newUrl']);
	
				$picExtension=str_replace('.','',strtolower(strrchr($url,".")));
				$picExtension=($picExtension=='jpeg')?'jpg':$picExtension;
	
				if($picExtension!='jpg'){
					$error=$TEXT_ERROR_NOT_JPEG_CONST;
				}else{
					$insertPicture='INSERT INTO Picture (Extension,URL) VALUES (?,?)';
					$mediadbADO->Execute($insertPicture,array($picExtension,$url));
					$entryID=$mediadbADO->Insert_ID();
					$newPicName=$entryID.'.'.$picExtension;
	
					savePic($_POST['newUrl'],$GLOBALS['mediaPicsPath'].$newPicName);
					resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$entryID.'_tn.'.$picExtension, 256, 256);
	
					add_pic_to_files($entryID,$filesArray,$mediadbADO);
				}
			}
		}


		
		
		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$existingAttributes=(int)@$_POST['existingAttributes'];
			$replace_attributes=(int)@$_POST['replace_attributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					
					// new attribute, insert in Attribute and File_Attribute tables
					
					
					$attributeID=addDirAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);
					
					if((int)$attributeID==0){
						header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST);
						exit();
					}
					addAttributeToFilesInDir($attributeID,$newAttributeType,$fileID,$mediadbADO,$replace_attributes);
				}else{
					addAttributeToFilesInDir($existingAttributes,$newAttributeType,$fileID,$mediadbADO,$replace_attributes);
				}
			}else{
				$attributeID=addDirAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);

				if((int)$attributeID==0){
					header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST);
					exit();
				}
				addAttributeToFilesInDir($attributeID,$newAttributeType,$fileID,$mediadbADO,$replace_attributes);
			}
			
			header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST);
			exit();
		}		
		
		if (isset($_POST['MediaSubTypes'])) 
		{
		$subType = $_POST['subType'];
		$fileFormat = $_POST['fileFormat'];
		updateMediaType($subType, $fileFormat, $filesArray, $mediadbADO);
		header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST);			
		exit();
		}
		
		if (isset($_POST['clean_value']) && $_POST['clean_value'] == true)  // This value is always set, must explicitly check value -tschak
		{
		cleanFiles($filesArray, $mediadbADO);			
		header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&msg='.$TEXT_CLEANED_FILES_CONST);			
		exit();
		}
		
		
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST);
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST=>'index.php?section=editDirectoryAttributes&fileID='.$fileID));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function addDirAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
		
	$mediaPlugin=getMediaPluginID($_SESSION['installationID'],$dbADO);
	if(is_null($mediaPlugin)){
		header("Location: index.php?section=editDirectoryAttributes&fileID=$fileID&error=$TEXT_MEDIA_PLUGIN_NOT_FOUND_CONST");
		exit();
	}

	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -r -o 0 '.$mediaPlugin.' 1 391 122 '.$newAttributeType.' 145 '.$fileID.' 5 "'.$newAttributeName.'"';
	$response=exec_batch_command($cmd,1);
	$suffix=(ereg('RESP: OK',$response))?'RESP: OK':'';
	
	return substr($response,strrpos($response,':')+1);
}

// add attribute to all files in directory
function addAttributeToFilesInDir($attributeID,$attributeTypeID,$fileID,$mediadbADO,$replace=0){
	$dirData=getRows('File','File.*',$mediadbADO,'WHERE PK_File='.$fileID);
	if(count($dirData)==0){
		return false;
	}
	$filesInDir=getAssocArray('File','PK_File','Filename',$mediadbADO,"WHERE Path LIKE '{$dirData[0]['Path']}%' AND IsDirectory=0 AND Missing=0");
	if(count($filesInDir)>0){
		foreach ($filesInDir AS $id=>$name){
			if((int)@$_POST['file_'.$id]==1){
				if($replace==1){
					$mediadbADO->Execute("DELETE File_Attribute FROM File_Attribute INNER JOIN Attribute on FK_Attribute=PK_Attribute AND FK_AttributeType=? WHERE FK_File=?",array($attributeTypeID,$id));
				}				
				$mediadbADO->Execute('INSERT INTO File_Attribute (FK_File,FK_Attribute) VALUES (?,?)',array($id,$attributeID));
			}
		}
	}	
	
	return true;
}

function add_pic_to_files($insertID,$filesArray,$mediadbADO){
	$replace_pics=(int)@$_POST['replace_pics'];
	foreach ($filesArray AS $id=>$filename){
		if((int)@$_POST['file_'.$id]==1){
			if($replace_pics==1){
				// delete all existing pics for selected files
				$picsArray=getFields('Picture_File','FK_Picture',$mediadbADO,'WHERE FK_File='.$id);
				for($i=0;$i<count($picsArray);$i++){
					// Remove link to file
					$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File = ? AND FK_Picture = ?',array($id,$picsArray[$i]['FK_Picture']));
					delete_media_pic_if_unused($picsArray[$i]['FK_Picture'], $mediadbADO);
				}
			}
			$mediadbADO->Execute('INSERT INTO Picture_File (FK_File, FK_Picture) VALUES (?,?)',array($id,$insertID));
		}
	}
}

function updateMediaType($subType, $fileFormat, $filesArray, $mediadbADO)
{

	foreach ($filesArray AS $id=>$filename)
		{
			//$mediadbADO->Execute('UPDATE File SET (FK_MediaSubType, FK_FileFormat) VALUES (?,?) where PK_File = `'.$id.'`',array($subType,$fileFormat));
		//$mediadbADO->Execute('Update File Set Field = FK_MediaSubType Where Search = ?',array($subType,$id));
		$mediadbADO->Execute("UPDATE File SET FK_MediaSubType=? WHERE PK_File = $id $where",array($subType));
		$mediadbADO->Execute("UPDATE File SET FK_FileFormat=? WHERE PK_File = $id $where",array($fileFormat));
		}
}

function cleanFiles($filesArray, $mediadbADO)
{

	foreach ($filesArray AS $id=>$filename)
		{
			//$mediadbADO->Execute('UPDATE File SET (FK_MediaSubType, FK_FileFormat) VALUES (?,?) where PK_File = `'.$id.'`',array($subType,$fileFormat));
		//$mediadbADO->Execute('Update File Set Field = FK_MediaSubType Where Search = ?',array($subType,$id));
		$mediadbADO->Execute("DELETE FROM File_Attribute WHERE FK_File = ".$id."");
        $mediadbADO->Execute("DELETE FROM LongAttribute WHERE FK_File = ".$id." and FK_AttributeType = 37");
        $mediadbADO->Execute("DELETE FROM Picture_File WHERE FK_File = ".$id."");
		}
		return true;
}
?>

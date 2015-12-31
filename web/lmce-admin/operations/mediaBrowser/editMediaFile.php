<?php


function editMediaFile($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=$_REQUEST['fileID'];
	$picsPerLine=6;	
	
	$scriptInHead='	<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 					<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"></script>	
	<script>
	function windowOpen(locationA,attributes) 
	{
		window.open(locationA,\'\',attributes);
	}

	function searchList()
	{
		fragment=document.editMediaFile.newAttributeName.value.toLowerCase();
		for(i=0;i<document.editMediaFile.existingAttributes.length-1;i++){
			listItemName=document.editMediaFile.existingAttributes[i].text.toLowerCase();	
			if(listItemName.substr(0,fragment.length)==fragment){
				document.editMediaFile.existingAttributes[i].selected=true;
				break;
			}else{
				document.editMediaFile.existingAttributes[i].selected=false;
			}
		}
	
	}
	
	
	function setAttributeName()
	{
		document.editMediaFile.newAttributeName.value=document.editMediaFile.existingAttributes[document.editMediaFile.existingAttributes.selectedIndex].text
	}
	
	function syncPath(path)
	{
		top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+encodeURIComponent(path);
		self.location=\'index.php?section=mainMediaFilesSync&path=\'+encodeURIComponent(path);
	}

	function setAll(bylink){
		if(bylink==1){
			editMediaFile.all.checked=(editMediaFile.all.checked)?false:true;
		}
		val=editMediaFile.all.checked;

		for (i = 0; i < editMediaFile.elements.length; i++)
		   {
			tmpName=editMediaFile.elements[i].name;
		     if (editMediaFile.elements[i].type == "checkbox" && (tmpName.indexOf("attribute_")!=-1 || tmpName.indexOf("picture_")!=-1))
		     {
		         editMediaFile.elements[i].checked = val;
		     }
		   }
		}	
	</script>
	';
	if($action=='form'){
		if (!empty($fileID)) {
			$resFile=$mediadbADO->Execute('SELECT * FROM File WHERE PK_File=?',$fileID);
			$rowFile=$resFile->FetchRow();
		}
		$out.='
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>';
		
		if (!empty($fileID) && $rowFile['EK_MediaType'] != 43) { // 43 = streaming audio
			$out.='<table width="100%" >
			<tr>
				<td><a href="javascript:syncPath(\''.$rowFile['Path'].'\')">Back</a></td>
				<td align="right">'.quick_search_box().'</td>
			<tr>
		</table>';
		}
		$out.='	
			<form action="index.php" method="POST" name="editMediaFile" enctype="multipart/form-data">
				<input type="hidden" name="section" value="editMediaFile">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="fileID" value="'.$fileID.'">
		';
		$attributeTypes=array();
		if (!empty($fileID)) {
			$queryAttrTypes='
			SELECT * 
			FROM AttributeType 
			INNER JOIN MediaType_AttributeType ON FK_AttributeType=PK_AttributeType
			WHERE EK_MediaType=?
			ORDER BY Description ASC';
			$resAttrTypes=$mediadbADO->Execute($queryAttrTypes,$rowFile['EK_MediaType']);
			while($rowTypes=$resAttrTypes->FetchRow()){
				$attributeTypes[$rowTypes['PK_AttributeType']]=$rowTypes['Description'];
			}
		}

		$externalAttributesBtn='';
		if(!empty($fileID) && in_array($rowFile['EK_MediaType'],array(3,4,5,28))){			
					
			$externalAttributesBtn='<select name="metadata_section"><option value="checkAmazon">'.$OPTION_CHECK_AMAZON.'</option>';			
			$externalAttributesBtn.='<option value="checkTVDB">'.$OPTION_CHECK_TVDB.'</option>';
			$externalAttributesBtn.='<option value="checkTMDB">'.$OPTION_CHECK_TMDB.'</option>;
			</selection>';
			
			$externalAttributesBtn.='<input type="button" class="button_fixed" value="'.$BUTTON_GET_METADATA.'"
			onClick="self.location=\'index.php?section=\'+document.editMediaFile.metadata_section.options[document.editMediaFile.metadata_section.selectedIndex].value+\'&fileID='.$fileID.'\'">';
		}

		$mediaSubTypes=getAssocArray('MediaSubType','PK_MediaSubType','Description',$mediadbADO,'ORDER BY Description ASC');
		$fileFormat=getAssocArray('FileFormat','PK_FileFormat','Description',$mediadbADO,'ORDER BY Description ASC');
		
		$out.='		
		<table border="0" cellspacing="0" cellpadding="3">		
		';
		if(!empty($fileID) && $rowFile['IsDirectory']==1){
			$out.='
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_THIS_IS_DIRECTORY_TREATED_AS_FILE_CONST.'</B></td>
				<td><a href="javascript:syncPath(\''.$rowFile['Path'].'/'.$rowFile['Filename'].'\');">[ '.$TEXT_SHOW_CONTENT_CONST.' ]</a> <a href="index.php?section=editDirectoryAttributes&fileID='.$fileID.'">[ '.$TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST.' ]</a></td>
			</tr>		
			
			';
		}
		$Filename = '';
		$Path = '';
		$MediaType = 0;
		$FK_MediaSubType = 0;
		$FK_FileFormat = 0;
		if (!empty($fileID)) {
			$Filename = $rowFile['Filename'];
			$Path = $rowFile['Path'];
			$MediaType = $rowFile['EK_MediaType'];
			$FK_MediaSubType = $rowFile['FK_MediaSubType'];
			$FK_FileFormat = $rowFile['FK_FileFormat'];
			$out.='
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_DATE_ADDED_CONST.':</B></td>
				<td>'.$rowFile['DateAdded'].'</td>
			</tr>	        ';
		}
		$out.='
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_FILE_CONST.':</B></td>
				<td><input type="text" name="filename" value="'.htmlentities($Filename,ENT_COMPAT,'UTF-8').'" size="55"></td>
			</tr>';
		if (!empty($fileID)) {
			$out.='
			<tr>
				<td>&nbsp;</td>
				<td align="right">
                                        <input type="button" class="button" name="delete" value="'.$TEXT_DELETE_FILE_CONST.'" onClick="if(confirm(\''.$TEXT_DELETE_FILE_CONFIRMATION_CONST.'\')){document.editMediaFile.action.value=\'del\';document.editMediaFile.submit();}">';
			if ($rowFile['EK_MediaType'] != 43) { // 43 = streaming audio, not possible to move
				$out.=' <input type="button" class="button" name="moveFile" value="Move file to other directory" onClick="windowOpen(\'index.php?section=moveFile&fileID='.$fileID.'&filePath='.urlencode(stripslashes($Path).'/'.$Filename).'\',\'width=500,height=400,toolbar=1,scrollbars=1,resizable=1\');">';
			}
			$out.='</td>
			</tr>';
		}

		if ($rowFile['EK_MediaType'] != 43) { // 43 = streaming audio, uses only filename
			$out.='
			<tr bgcolor="#EBEFF9">
				<td><B>'.$TEXT_LOCATION_CONST.':</B></td>
				<td><input type="text" name="Path" value="'.htmlentities(stripslashes($Path),ENT_COMPAT,'UTF-8').'" size="55">'.(!empty($fileID) ? (file_exists($Path.'/'.$Filename)?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/db.gif align=middle border=0>') : '').'</td>
			</tr>';
		}
		
		$out.='<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_TYPE_CONST.':</B></td>
				<td><select name="type" style="width:200px;">';
			$resType=$dbADO->Execute('SELECT * FROM MediaType ORDER BY Description ASC');
			while($rowType=$resType->FetchRow()){
				$out.='<option value="'.$rowType['PK_MediaType'].'" '.(($rowType['PK_MediaType']==$MediaType)?'selected':'').'>'.$rowType['Description'].'</option>';
			}
			
			$out.='
				</select> '.$externalAttributesBtn.'</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_SUB_TYPE_CONST.':</B></td>
				<td id="subtype" >'.pulldownFromArray($mediaSubTypes,'subtype',$FK_MediaSubType).'</td>
			</tr>			
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_FILE_FORMAT_CONST.':</B></td>
				<td "id="fileformat">'.pulldownFromArray($fileFormat,'fileFormat',$FK_FileFormat).'</td>
			</tr>				
			<tr>';
			if (empty($fileID)) {
				$out.='
				<td colspan="2"><input type="submit" class="button" name="addFile" value="'.$TEXT_ADD_CONST.'">';
			} else {
				$out.='
				<td colspan="2"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'">';
			}
			$out.='
                                <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">
				<input type="hidden" name="oldPath" value="'.$Path.'">
				<input type="hidden" name="oldFilename" value="'.$Filename.'">
				</td>
			</tr>';
			if (!empty($fileID)) {
				$out.='
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>'.$TEXT_ATTRIBUTES_CONST.':</B>
				</td>
				<td>				
				<table width="100%">';
				$queryAttributes='
				SELECT Attribute.*, AttributeType.Description 
				FROM Attribute
				INNER JOIN File_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_File=?
				ORDER BY Description ASC';
				$resAttributes=$mediadbADO->Execute($queryAttributes,$fileID);
				$pos=0;
				while($rowAttributes=$resAttributes->FetchRow()){
					$pos++;
					$out.='
				
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td width="20"><input type="checkbox" value="1" name="attribute_'.$rowAttributes['PK_Attribute'].'"></td>
							<td width="100"><b>'.$rowAttributes['Description'].'</b></td>
							<td>'.$rowAttributes['Name'].'</td>
							<td width="100" align="center"><a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties">'.$TEXT_EDIT_CONST.'</a> <a href="#" onClick="if(confirm(\''.$TEXT_DELETE_ATTRIBUTE_FROM_FILE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=delete&dAtr='.$rowAttributes['PK_Attribute'].'&dpath='.urlencode(stripslashes($rowFile['Path'])).'\'">'.$TEXT_REMOVE_CONST.'</a></td>
						</tr>';
				}
				$out.='
				</table></td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>'.$TEXT_LONG_ATTRIBUTES_CONST.':</B></td>
				<td><table width="100%">';
				$queryAttributes='
				SELECT LongAttribute.*, AttributeType.Description 
				FROM LongAttribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_File=?';
				$resLongAttributes=$mediadbADO->Execute($queryAttributes,$fileID);
				$pos=0;
				while($rowAttributes=$resLongAttributes->FetchRow()){
					$pos++;
					$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td width="20"><input type="checkbox" value="1" name="longattribute_'.$rowAttributes['PK_LongAttribute'].'"></td>
							<td width="100"><b>'.$rowAttributes['Description'].'</b></td>
							<td>'.nl2br($rowAttributes['Text']).'</td>
							<td width="100" align="center"><a href="index.php?section=editLongAttribute&laID='.$rowAttributes['PK_LongAttribute'].'&fileID='.$fileID.'">'.$TEXT_EDIT_CONST.'</a> <a href="#" onClick="if(confirm(\''.$TEXT_DELETE_ATTRIBUTE_FROM_FILE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=delete&dLAtr='.$rowAttributes['PK_LongAttribute'].'&dpath='.urlencode(stripslashes($rowFile['Path'])).'\'">'.$TEXT_REMOVE_CONST.'</a></td>
						</tr>';
				}
				$out.='
				</table></td>
			</tr>

			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td valign="top" align="left"><B>'.$TEXT_PICTURES_CONST.'</B></td>
				<td valign="top" align="left" colspan="6"><table width="100%" border="0">';
				$queryPictures='
				        SELECT * FROM Picture_File
				        INNER JOIN Picture ON Picture_File.FK_Picture=PK_Picture
				        WHERE FK_File=?';
				$resPictures=$mediadbADO->Execute($queryPictures,$fileID);
				$picsCount=0;
				$picsArray=array();
				while($rowPictures=$resPictures->FetchRow()){
					if(!in_array($rowPictures['PK_Picture'],$picsArray)){
						$picsArray[]=$rowPictures['PK_Picture'];
					}
					$out .= '
					<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
					<td width="20"><input type="checkbox" value="1" name="picture_'.$rowPictures['PK_Picture'].'"></td>
					<td valign="center" width="100"><input type="text" name="url_'.$rowPictures['PK_Picture'].'" value="'.$rowPictures['URL'].'"></td>
					<td valign="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a></td>
					<td width="100"><a href="#" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_PICTURE_CONST.'\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=properties&picID='.$rowPictures['PK_Picture'].'&path='.htmlentities($rowFile['Path']).'\';">'.$TEXT_DELETE_CONST.'</a></td>
					</tr>
					';
				}
				$out.='
				</table>
				<input type="hidden" name="picsArray" value="'.join(',',$picsArray).'">
				</td>
			</tr>';
				if($resAttributes->RecordCount() > 0 | $resLongAttributes->RecordCount() > 0 || $resPictures->RecordCount() > 0){
					$out.='
				<tr>
					<td></td>
					<td><table width="100%"><tr>
						<td><input type="checkbox" value="1" name="all" onClick="setAll(0);"></td>
						<td colspan="2">
							<a href="javascript:setAll(1);">Select/unselect all</a>
							<input type="submit" class="button" name="delSelected" value="Delete selected" onClick="if(!confirm(\'Are you sure you want to remove those attributes?\'))return false;">';
					if($rowFile['IsDirectory']==1){
						$out .= '
						<input type="submit" class="button" name="addSelectedRecursively" value="Add recursively" onClick="if(!confirm(\'Are you sure you want to add those attributes to all files under this directory?\'))return false;">
						<input type="submit" class="button" name="replaceSelectedRecursively" value="Add/replace recursively" onClick="if(!confirm(\'Are you sure you want to add or replace those attributes to all files under this director?\nNote, all attributes of the same types will be deleted before!\'))return false;">';
					}
					$out .= '</td></tr></table></td></tr>';
				}
				$out .= '
			<tr>
				<td><B>'.$TEXT_ADD_PICTURE_CONST.'</B><br><em>'.$TEXT_JPG_ONLY_CONST.'</em></td>
				<td colspan="6"> '.$TEXT_URL_CONST.' <input type="text" name="newUrl" value=""> '.$TEXT_FILE_CONST.' <input type="file" name="newPic" value=""> <input type="submit" class="button" name="addPic" value="'.$TEXT_ADD_PICTURE_CONST.'"></td>
			</tr>
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			
			<tr>
				<td><B>'.$TEXT_ADD_ATTRIBUTE_CONST.':</B><br><select name="newAttributeType" onChange="document.editMediaFile.action.value=\'form\';document.editMediaFile.submit();">
					<option value="0">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
				foreach($attributeTypes AS $attributeID=>$attributeName){
					$out.='<option value="'.$attributeID.'" '.(($attributeID==@$_POST['newAttributeType'])?'selected':'').'>'.$attributeName.'</option>';
				}
				$out.='</select></td>
				<td><B>'.$TEXT_ATTRIBUTE_NAME_CONST.' *</B><br><input type="text" name="newAttributeName" value="" onKeyPress="document.editMediaFile.existingAttributes.selectedIndex=-1;"></td>
			</tr>';
				if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
					$newAttributeType=(int)$_POST['newAttributeType'];
					$resAttributesByType=$mediadbADO->Execute('SELECT * FROM Attribute WHERE FK_AttributeType=? ORDER BY Name ASC',$newAttributeType);
					$out.='
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
			<tr>
				<td colspan="2">&nbsp;</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_LONG_ATTRIBUTE_TYPE.':</B></td>
				<td><B>'.$TEXT_ADD_LONG_ATTRIBUTE_CONST.':</B></td>
			</tr>
			<tr>
				<td valign="top">'.pulldownFromArray($attributeTypes,'longAttributeType',0).'</td>
				<td valign="top"><textarea name="longAttributeText"></textarea> <input type="submit" class="button" name="addLA" value="'.$TEXT_ADD_CONST.'"></td>				
			</tr>	
		</table>';
			}
		$out.='
		</form>';
		if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
			$out.='
			<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
			<script>
			 	var frmvalidator = new formValidator("editMediaFile");			
	 			frmvalidator.addValidation("newAttributeName","req","'.$TEXT_ATTRIBUTE_NAME_VALIDATION_CONST.'");
			</script>		
			';
		}	
	}else{
	// process area
		if (isset($_REQUEST['addFile'])) {
			// Insert some basic data
			$type=(int)$_POST['type'];
			$fileName=cleanString(@$_POST['filename']);
			$newFileID = $mediadbADO->GetOne("SELECT MAX(PK_File) AS Max_PK_File FROM File") + 1;
			$mediadbADO->Execute("INSERT INTO File (PK_File,Filename,Path,EK_MediaType,DateAdded) values (?,?,'',?,CURRENT_TIMESTAMP())",array($newFileID,$fileName,$type));
			// and let the update action below set the rest of the values
			$action = 'update';
			$fileID = $newFileID;
		}
		if(isset($_REQUEST['picID'])){
			$toDelete=$_REQUEST['picID'];
			$path=stripslashes(@$_REQUEST['path']);
			
			//remove reference to the picture
			$deletePicFile='DELETE FROM Picture_File WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicFile,$toDelete);
			
			//Is anything else referencing this picture?
			$pictureReference =         'SELECT * FROM Picture_Attribute WHERE FK_Picture = '.$toDelete;
			$pictureReference .= ' UNION SELECT * FROM Picture_Disc WHERE FK_Picture = '.$toDelete;
			$pictureReference .= ' UNION SELECT * FROM Picture_Download WHERE FK_Picture = '.$toDelete;
			$pictureReference .= ' UNION SELECT * FROM Picture_File WHERE FK_Picture = '.$toDelete;
			$numPictureReferences = $mediadbADO->Execute($pictureReference);

			//Delete picture from database and file system only if nothing else is referencing it!
			if($numPictureReferences->RecordCount()==0) {
				$deletePic='DELETE FROM Picture WHERE PK_Picture=?';
				$mediadbADO->Execute($deletePic,$toDelete);
				@unlink($GLOBALS['mediaPicsPath'].$toDelete.'.jpg');
				@unlink($GLOBALS['mediaPicsPath'].$toDelete.'_tn.jpg');
				
			}

			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($path).'"';
			exec_batch_command($cmd);
		}

		if(isset($_POST['addLA'])){
			$longAttributeType=(int)@$_POST['longAttributeType'];
			$longAttributeText=cleanString($_POST['longAttributeText']);
			if($longAttributeType!=0 && $longAttributeType!=''){
				$mediadbADO->Execute('INSERT INTO LongAttribute (FK_File,FK_AttributeType,Text) VALUES (?,?,?)',array($fileID,$longAttributeType,$longAttributeText));
			}
		}

		
		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$existingAttributes=(int)@$_POST['existingAttributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					
					// new attribute, insert in Attribute and File_Attribute tables
					
					
					$AttributeAdded=addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);
					$isAdded=(ereg('RESP: OK',$AttributeAdded))?true:false;
					
					if($isAdded!==true){
						header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST);
						exit();
					}
					
				}else{
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$existingAttributes));
				}
			}else{
				$AttributeAdded=addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);
				$isAdded=(ereg('RESP: OK',$AttributeAdded))?true:false;
				if($isAdded!==true){
					header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST);
					exit();
				}
			}
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_FILE_ATTRIBUTE_ADDED_CONST.':<br>'.urlencode(nl2br(@$AttributeAdded)));
			exit();
		}
		
		if(isset($_REQUEST['dAtr'])){
			$deleteAttribute=$_REQUEST['dAtr'];
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$deleteAttribute));

			$dpath=stripslashes($_REQUEST['dpath']);
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($dpath).'"';
			exec_batch_command($cmd);
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_ATTRIBUTE_DELETED_FROM_FILE_CONST.': '.$cmd);	
			exit();		
		}
	
		if(isset($_REQUEST['dLAtr'])){
			$deleteAttribute=$_REQUEST['dLAtr'];
			$mediadbADO->Execute('DELETE FROM LongAttribute WHERE PK_LongAttribute=?',array($deleteAttribute));

			$dpath=stripslashes($_REQUEST['dpath']);
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($dpath).'"';
			exec_batch_command($cmd);
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_ATTRIBUTE_DELETED_FROM_FILE_CONST.': '.$cmd);	
			exit();		
		}
		
		$path=stripslashes(@$_POST['Path']);
		$fileName=cleanString(@$_POST['filename']);
		$newFilePath=$path.'/'.$fileName;

		$oldPath=stripslashes(@$_POST['oldPath']);
		$oldFilename=stripslashes(@$_POST['oldFilename']);
		$oldFilePath=$oldPath.'/'.$oldFilename;
		
		if($action=='del'){
			// delete physical file
			$delFile='sudo -u root rm -f "'.bash_escape($oldFilePath).'"';
			exec_batch_command($delFile);
			
			$delID3='sudo -u root rm -f "'.bash_escape($oldFilePath).'.id3"';
			exec_batch_command($delID3);

			$delCssKey='sudo -u root rm -f "'.bash_escape($oldFilePath).'.keys.tar.gz"';
			exec_batch_command($delCssKey);
			
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM File WHERE PK_File=?',$fileID);
			
			exec_batch_command('sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($oldPath).'"');
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($oldPath).'&msg='.$TEXT_MEDIA_FILE_DELETED_CONST);			
			exit();

		}
		
		
		if(isset($_REQUEST['addFile']) || isset($_POST['update'])){
			$type=(int)$_POST['type'];
			$subtype=(int)$_POST['subtype'];
			$subtype=($subtype==0)?NULL:$subtype;
			$fileFormat=(int)$_POST['fileFormat'];
			$fileFormat=($fileFormat==0)?NULL:$fileFormat;
					
			if(file_exists($oldFilePath)){
				if($path==$oldPath){
					if($fileName!=$oldFilename){
						$isRenamed=web_rename($oldFilePath,$newFilePath);
						$error=($isRenamed==0)?'Rename failed':'';
					}
				}
				else{
					copy($oldFilePath,$newFilePath);
					exec_batch_command('sudo -u root rm -f "'.bash_escape($oldFilePath).'"');
				}
			}
			$mediadbADO->Execute('UPDATE File SET Filename=?, Path=?, EK_MediaType=?,FK_MediaSubType=?,FK_FileFormat=? WHERE PK_File=?',array($fileName,$path,$type,$subtype,$fileFormat,$fileID));
			
			// update pics urls
			$picsArray=explode(',',$_POST['picsArray']);
			foreach ($picsArray AS $pic){
				$picUrl=cleanString(@$_POST['url_'.$pic]);
				$mediadbADO->Execute('UPDATE Picture SET URL=? WHERE PK_Picture=?',array($picUrl,$pic));
			}
			
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -d "'.bash_escape($path).'"';
			exec_batch_command($cmd);
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST.'&err='.@$error);			
			exit();
		}
		
		if(isset($_REQUEST['addPic'])){
			if(isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
				$newUrl=cleanString($_POST['newUrl']);
				$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
				$picExtension=($picExtension=='jpeg')?'jpg':$picExtension;
				
				$insertPicture='INSERT INTO Picture (Extension,URL) VALUES (?,?)';
				$mediadbADO->Execute($insertPicture,array($picExtension,$newUrl));
				$insertID=$mediadbADO->Insert_ID();
				$newPicName=$insertID.'.'.$picExtension;
				
				$error='';
				if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
					$error=$TEXT_ERROR_NOT_JPEG_CONST;
				}
				elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
					// create thumbnail
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 256, 256);
					// update database
					$insertPictureAttribute='INSERT INTO Picture_File (FK_File, FK_Picture) VALUES (?,?)';
					$mediadbADO->Execute($insertPictureAttribute,array($fileID,$insertID));
				}else{
					//upload fail, prompt error message
					$deletePicture='DELETE FROM Picture WHERE PK_Picture=?';
					$mediadbADO->Execute($deletePicture,$insertID);
					$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
				}
			}

			if($_POST['newUrl']!='' && $_FILES['newPic']['name']==''){
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
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$entryID.'_tn.'.$picExtension, 256, 256);

					if($resizeFlag==0){
						$mediadbADO->Execute('INSERT INTO Picture_File (FK_File, FK_Picture) VALUES (?,?)',array($fileID,$entryID));
					}else{
						$mediadbADO->Execute('DELETE FROM Picture WHERE PK_Picture=?',$entryID);
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
					}
				}
			}
			
	
			if(@$error!=''){
				header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$error);			
				exit();
			}else{
				exec_batch_command('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.bash_escape($path).'"');
				header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_PICTURE_UPLOADED_CONST);			
				exit();
			}
		}
		
		if(isset($_POST['delSelected'])){
			$fileAttributes=getAssocArray('File_Attribute','FK_Attribute','FK_Attribute',$mediadbADO,'WHERE FK_File='.$fileID);
			$fileLongAttributes = getAssocArray('LongAttribute','PK_LongAttribute','PK_LongAttribute',$mediadbADO,'WHERE FK_File='.$fileID);
			$filePictures = getAssocArray('Picture_File', 'FK_Picture', 'FK_Picture', $mediadbADO, 'WHERE FK_FILE='.$fileID);
			foreach ($fileAttributes AS $aID){
				if(isset($_POST['attribute_'.$aID])){
					$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$aID));
				}
			}
			foreach ($fileLongAttributes AS $aID){
				if(isset($_POST['longattribute_'.$aID])){
					$mediadbADO->Execute('DELETE FROM LongAttribute WHERE FK_File=? AND PK_LongAttribute=?',array($fileID,$aID));
				}
			}
			foreach ($fileAttributes AS $aID){
				if(isset($_POST['attribute_'.$aID])){
					$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File=? AND FK_Picture=?',array($fileID,$aID));
				}
			}
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($path).'"';
			exec_batch_command($cmd);
		}
		
		
		// if attributes should be replaced recursivley, first delete all attributes of same type and then add the attributes
		if(isset($_POST['replaceSelectedRecursively'])) {
			// get list of all files below our directory
			$myFiles = getFilesRecursively($fileID, $mediadbADO);

			// get all attributes and extract those that have been selected
			$attributes = getSelectedAttributesForFile($fileID, $mediadbADO);
			$attributeTypes = getTypesOfAttributes($attributes, $mediadbADO);
			// add attributes to all files
			foreach($myFiles as $myFileId) {
				removeAttributeTypes($myFileId, $attributeTypes, $mediadbADO);
				setAttributes($myFileId, $attributes, $mediadbADO);
			}		
			// call UpdateMedia
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($path.'/'.$fileID).'"';
			exec_batch_command($cmd);
		}
		
		// add selected attributes to all files/directories below the current directory
		if(isset($_POST['addSelectedRecursively'])) {
			// get list of all files below our directory
			$myFiles = getFilesRecursively($fileID, $mediadbADO);

			// get all attributes and extract those that have been selected
			$attributes = getSelectedAttributesForFile($fileID, $mediadbADO);
			
			// add attributes to all files
			foreach($myFiles as $myFileId) {
				setAttributes($myFileId, $attributes, $mediadbADO);
			}		
			// call UpdateMedia
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -w -d "'.bash_escape($path.'/'.$fileID).'"';
			exec_batch_command($cmd);
		}
		
		header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST);			
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_MEDIA_FILE_CONST);
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
		
	$mediaPlugin=getMediaPluginID($_SESSION['installationID'],$dbADO);
	if(is_null($mediaPlugin)){
		header("Location: index.php?section=editMediaFile&fileID=$fileID&error=$TEXT_MEDIA_PLUGIN_NOT_FOUND_CONST");
		exit();
	}

	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -r 0 '.$mediaPlugin.' 1 391 122 '.$newAttributeType.' 145 '.$fileID.' 5 "'.$newAttributeName.'"';
	$response=exec_batch_command($cmd,1);
	$suffix=(ereg('RESP: OK',$response))?'RESP: OK':'';
	
	return $cmd.'<br>'.$TEXT_RESPONSE_CONST.': '.$suffix;
}

function web_rename($source, $destination){
	$rename_id3='sudo -u root /usr/pluto/bin/Web_Rename.sh "'.bash_escape($source).'.id3" "'.bash_escape($destination).'.id3"';
	exec_batch_command($rename_id3);
	
	$rename_CssKey='sudo -u root /usr/pluto/bin/Web_Rename.sh "'.bash_escape($source).'.keys.tar.gz" "'.bash_escape($destination).'.keys.tar.gz"';
	exec_batch_command($rename_CssKey);

	$cmd='sudo -u root /usr/pluto/bin/Web_Rename.sh "'.bash_escape($source).'" "'.bash_escape($destination).'"';
	return exec_batch_command($cmd);
}

function getFilesRecursively($fileID, $mediadbADO) {
	$myFiles = array();
	
	//  get top file
	$res = $mediadbADO->Execute("SELECT `Filename`, `Path`, `isDirectory` FROM `File` WHERE `PK_File` = ".$fileID);
	$row = $res->FetchRow();
	// only proceed if this is a directory
	if($row['isDirectory'] == 1) {
		$topFileName = $row['Filename'];
		$topFilePath = $row['Path'];
		
		// get all files below the selected one and put their ID into an array
		$res = $mediadbADO->Execute("SELECT `PK_File` FROM `File` WHERE `Path` LIKE ".escape($topFilePath."/".$topFileName."%"));
		while($row = $res->FetchRow()) {
				$myFiles[] = $row['PK_File'];
		}
	}
	return $myFiles;
}

function getSelectedAttributesForFile($PK_File, $mediadbADO) {
		$fileAttributes = getAssocArray('File_Attribute','FK_Attribute','FK_Attribute',$mediadbADO,'WHERE FK_File='.$PK_File);
		$fileLongAttributes = getAssocArray('LongAttribute','PK_LongAttribute','PK_LongAttribute',$mediadbADO,'WHERE FK_File='.$PK_File);
		$filePictures = getAssocArray('Picture_File', 'FK_Picture', 'FK_Picture', $mediadbADO, 'WHERE FK_FILE='.$PK_File);
		$attributes = array();
		foreach($fileAttributes as $attribute) {
			if(isset($_POST['attribute_'.$attribute])) {
				$attributes['Attribute'][] = $attribute;
			}
		}
		foreach($fileLongAttributes as $attribute) {
			if(isset($_POST['longattribute_'.$attribute])) {
				$attributes['LongAttribute'][] = $attribute;
			}
		}
		foreach($filePictures as $attribute) {
			if(isset($_POST['picture_'.$attribute])) {
				$attributes['Picture'][] = $attribute;
			}
		}
		return $attributes;
}

function setAttributes($PK_File, $attributes, $mediadbADO) {
	if(isset($attributes['Attribute'])) foreach($attributes['Attribute'] as $attribute) {
		$mediadbADO->Execute("DELETE FROM `File_Attribute` WHERE `FK_File` = '".$PK_File."' AND `FK_Attribute` = '".$attribute."'");
		$mediadbADO->Execute("INSERT INTO `File_Attribute` (`FK_File`, `FK_Attribute`) VALUES ('".$PK_File."', '".$attribute."')");
	}
	if(isset($attributes['LongAttribute'])) foreach($attributes['LongAttribute'] as $attribute) {
		$mediadbADO->Execute("INSERT INTO `LongAttribute` (`FK_File`, `FK_AttributeType`, `Text`) SELECT '".$PK_File."', `FK_AttributeType`, `Text` FROM `LongAttribute` WHERE `PK_LongAttribute` = '".$attribute."'");
	}
	if(isset($attributes['Picture']))  foreach($attributes['Picture'] as $attribute) {
		$mediadbADO->Execute("DELETE FROM `Picture_File` WHERE `FK_File` = '".$PK_File."' AND `FK_Picture` = '".$attribute."'");
		$mediadbADO->Execute("INSERT INTO `Picture_File` (`FK_File`, `FK_Picture`) VALUES ('".$PK_File."', '".$attribute."')");
	}
}

function removeAttributeTypes($myFileID, $attributeTypes, $mediadbADO) {
	if(isset($attributeTypes['Attribute'])) foreach($attributeTypes['Attribute'] as $attributeType) {
		$mediadbADO->Execute("DELETE FROM `File_Attribute` WHERE `FK_File` = '".$myFileID."' AND `FK_Attribute` IN (SELECT `PK_Attribute` FROM `Attribute` WHERE `FK_AttributeType` = '".$attributeType."')");
	}
	if(isset($attributeTypes['LongAttribute'])) foreach($attributeTypes['LongAttribute'] as $attributeType) {
		$mediadbADO->Execute("DELETE FROM `LongAttribute` WHERE `FK_File` = '".$myFileID."' AND `FK_AttributeType` = '".$attributeType."'");
	}
	if(isset($attributeTypes['Picture']))  {
		$mediadbADO->Execute("DELETE FROM `Picture_File` WHERE `FK_File` = '".$myFileID."'");
	}
}

function getTypesOfAttributes($attributes, $mediadbADO) {
	$types = array();
	if(isset($attributes['Attribute'])) foreach($attributes['Attribute'] as $attribute) {
		$res = $mediadbADO->Execute("SELECT `FK_AttributeType` FROM `Attribute` WHERE `PK_Attribute` = '".$attribute."'");
		$row = $res->FetchRow();
		$types['Attribute'][] = $row['FK_AttributeType'];
	}
	if(isset($attributes['LongAttribute'])) foreach($attributes['LongAttribute'] as $attribute) {
		$res = $mediadbADO->Execute("SELECT `FK_AttributeType` FROM `LongAttribute` WHERE `PK_LongAttribute` = '".$attribute."'");
		$row = $res->FetchRow();
		$types['LongAttribute'][] = $row['FK_AttributeType'];
	}
	if(isset($attributes['Picture']))  {
		$types['Picture'][] = '1';
	}
	return $types;
}

?>

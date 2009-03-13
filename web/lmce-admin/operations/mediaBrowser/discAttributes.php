<?
function discAttributes($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/discAttributes.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$discID=$_REQUEST['discID'];
	
	$discData=getFieldsAsArray('Disc','ID,EK_MediaType,FK_FileFormat,FK_MediaSubType',$mediadbADO,'WHERE PK_Disc='.$discID);
	$media_type=$discData['EK_MediaType'][0];
	$picsPerLine=4;	
	
	
	if($action=='form'){
	$scriptInHead='
	<script>
	function setAttributeName()
	{
		document.discAttributes.newAttributeName.value=document.discAttributes.existingAttributes[document.discAttributes.existingAttributes.selectedIndex].text
	}
	</script>
	';		
		$attributeTypes=getAssocArray('AttributeType','PK_AttributeType','Description',$mediadbADO,'INNER JOIN MediaType_AttributeType ON FK_AttributeType=PK_AttributeType WHERE EK_MediaType='.$media_type,'ORDER BY Description ASC');
		$mediaSubTypes=getAssocArray('MediaSubType','PK_MediaSubType','Description',$mediadbADO,'ORDER BY Description ASC');
		$fileFormat=getAssocArray('FileFormat','PK_FileFormat','Description',$mediadbADO,'ORDER BY Description ASC');
		
		$out.='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			<form action="index.php" method="POST" name="discAttributes" enctype="multipart/form-data">
				<input type="hidden" name="section" value="discAttributes">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="discID" value="'.$discID.'">
		
		';
		$mediaTypes=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'WHERE PK_MediaType IN (2,3)','ORDER BY Description ASC');

		$out.='
		<a href="index.php?section=rippingStatus">Back</a>
		<table border="0" cellspacing="0" cellpadding="3">
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_DISC_ID_CONST.':</B></td>
				<td><input type="text" name="disc_id" value="'.$discData['ID'][0].'" size="55"></td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_TYPE_CONST.':</B></td>
				<td>'.pulldownFromArray($mediaTypes,'type',$media_type).'</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_SUB_TYPE_CONST.':</B></td>
				<td>'.pulldownFromArray($mediaSubTypes,'subtype',$discData['FK_MediaSubType'][0]).'</td>
			</tr>			
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_FILE_FORMAT_CONST.':</B></td>
				<td>'.pulldownFromArray($fileFormat,'fileFormat',$discData['FK_FileFormat'][0]).'</td>
			</tr>			
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>'.$TEXT_DISC_ATTRIBUTES_CONST.':</B></td>
				<td><table width="100%">';
			$queryAttributes='
				SELECT Attribute.*, AttributeType.Description,Disc_Attribute.Track 
				FROM Attribute
				INNER JOIN Disc_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_Disc=?
				ORDER BY Track ASC,Description ASC';
			$resAttributes=$mediadbADO->Execute($queryAttributes,$discID);
			$pos=0;
			while($rowAttributes=$resAttributes->FetchRow()){
				$pos++;
				$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td><b>'.$rowAttributes['Track'].'</b></td>
							<td><b>'.$rowAttributes['Description'].'</b></td>
							<td><a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties">'.$rowAttributes['Name'].'</a></td>
							<td align="center"><a href="#" onClick="if(confirm(\''.$TEXT_DELETE_ATTRIBUTE_FROM_DISC_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=discAttributes&discID='.$discID.'&action=delete&dAtr='.$rowAttributes['PK_Attribute'].'\'">'.$TEXT_REMOVE_CONST.'</a></td>
						</tr>';
			}
			$out.='
				</table></td>
			</tr>			
			<tr>
				<td colspan="2" align="left"><B>NOTE:</B> for complete identification, the required attributes are:<br>
				- for Pluto CD, <B>Album</B> and <B>Performer</B>;<br>
				- for Pluto DVD, <B>Title</B>.
			</td>
			</tr>			
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'">
				</td>
			</tr>			
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			
			<tr>
				<td colspan="2"><table>
					<tr>
						<td><B>'.$TEXT_ADD_DISC_ATTRIBUTE_CONST.':</B><br>'.pulldownFromArray($attributeTypes,'newAttributeType',@$_POST['newAttributeType'],'onChange="document.discAttributes.action.value=\'form\';document.discAttributes.submit();"').'</td>
						<td><B>'.$TEXT_DISC_ATTRIBUTE_NAME_CONST.'</B><br><input type="text" name="newAttributeName" value=""></td>
						<td><B>Track</B><br><input type="text" name="newAttributeTrack" value=""></td>
					</tr>
				</table></td>
			</tr>';
			if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
				$newAttributeType=(int)$_POST['newAttributeType'];
				$resAttributesByType=$mediadbADO->Execute('SELECT * FROM Attribute WHERE FK_AttributeType=? ORDER BY Name ASC',$newAttributeType);
				$out.='
				<tr>
					<td>&nbsp;</td>
					<td><select name="existingAttributes" size="20" onClick="setAttributeName();" onChange="setAttributeName();">';
				while($rowAttributesByType=$resAttributesByType->FetchRow()){
					$out.='<option value="'.$rowAttributesByType['PK_Attribute'].'">'.$rowAttributesByType['Name'].'</option>';
				}
				$out.='
					</select> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>';
			}
			$out.='
			<tr bgcolor="#EBEFF9">
				<td valign="top" align="left"><B>'.$TEXT_DISC_PICTURES_CONST.'</B></td>
				<td valign="top" align="left" colspan="6">
			<tr>';
			$queryPictures='
				SELECT * FROM Picture_Disc
				INNER JOIN Picture ON Picture_Disc.FK_Picture=PK_Picture
				WHERE FK_Disc=?';
			$resPictures=$mediadbADO->Execute($queryPictures,$discID);
			if($resPictures->RecordCount()==0){
				$out.='
				<tr>
					<td valign="top" align="left" colspan="2">'.$TEXT_NO_DISC_PICTURES_CONST.'</td>
				<tr>				';
			}else{
				$out.='
				<tr>
					<td colspan="2"><table>';

				$picsCount=0;
				while($rowPictures=$resPictures->FetchRow()){
					$picsCount++;
					$out.='
						<td style="background-color:#EEEEEE;" align="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a> <br><a href="#" onClick="if(confirm(\'Are you sure you want to delete this picture?\'))self.location=\'index.php?section=discAttributes&discID='.$discID.'&action=properties&picID='.$rowPictures['PK_Picture'].'\';">'.$TEXT_DELETE_CONST.'</a></td>
					';
					if($picsCount%$picsPerLine==0)
						$out.='</tr><tr>';
				}
				$out.='
					</tr>
				</table>';
			}
				$out.='
				</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_ADD_DISC_PICTURE_CONST.'</B></td>
				<td colspan="6"> <input type="file" name="newPic" value=""> <input type="submit" class="button" name="addPic" value="'.$TEXT_ADD_DISC_PICTURE_CONST.'"></td>
			</tr>
			
		</table>';
		$out.='
		</form>';
		if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
			$out.='
			<script>
			 	var frmvalidator = new formValidator("discAttributes");			
	 			frmvalidator.addValidation("newAttributeName","req","'.$TEXT_NEW_ATTRIBUTE_VALIDATION_CONST.'");
			</script>		
			';
		}	
	}else{
	// process area
		if(isset($_REQUEST['picID'])){
			$toDelete=$_REQUEST['picID'];
			$deletePic='DELETE FROM Picture WHERE PK_Picture=?';
			$mediadbADO->Execute($deletePic,$toDelete);
			
			$deletePicAttribute='DELETE FROM Picture_Attribute WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicAttribute,$toDelete);
			
			$deletePicFile='DELETE FROM Picture_Disc WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicFile,$toDelete);
			
			@unlink($GLOBALS['mediaPicsPath'].$toDelete.'.jpg');
			@unlink($GLOBALS['mediaPicsPath'].$toDelete.'_tn.jpg');
		}
	
		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$newAttributeTrack=(int)$_POST['newAttributeTrack'];
			
			$existingAttributes=(int)@$_POST['existingAttributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					
					// new attribute, insert in Attribute and File_Attribute tables
					$isAdded=addDiscAttribute($newAttributeType,$newAttributeName,$discID,$mediadbADO,$newAttributeTrack);
					
					if($isAdded!==true){
						header('Location: index.php?section=discAttributes&discID='.$discID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST.': '.urlencode(nl2br($AttributeAdded)));
						exit();
					}
					
				}else{
					$mediadbADO->Execute('INSERT IGNORE INTO Disc_Attribute (FK_Disc, FK_Attribute) VALUES (?,?)',array($discID,$existingAttributes));
				}
			}else{
				$isAdded=addDiscAttribute($newAttributeType,$newAttributeName,$discID,$mediadbADO,$newAttributeTrack);
				if($isAdded!==true){
					header('Location: index.php?section=discAttributes&discID='.$discID.'&error='.$TEXT_ERROR_DISC_ATTRIBUTE_NOT_ADDED_CONST);
					exit();
				}
			}
			
			header('Location: index.php?section=discAttributes&discID='.$discID.'&msg='.$TEXT_DISC_ATTRIBUTE_ADDED_CONST.'<br>'.urlencode(nl2br(@$AttributeAdded)));
			exit();
		}
		
		if(isset($_REQUEST['dAtr'])){
			$deleteAttribute=$_REQUEST['dAtr'];
			$mediadbADO->Execute('DELETE FROM Disc_Attribute WHERE FK_Disc=? AND FK_Attribute=?',array($discID,$deleteAttribute));
			
			header('Location: index.php?section=discAttributes&discID='.$discID.'&msg='.$TEXT_ATTRIBUTE_DELETED_FROM_DISC_CONST);	
			exit();		
		}
	
	
		if(isset($_POST['update'])){
			$type=(int)$_POST['type'];
			$subtype=(int)$_POST['subtype'];
			$subtype=($subtype==0)?NULL:$subtype;
			$fileFormat=(int)$_POST['fileFormat'];
			$fileFormat=($fileFormat==0)?NULL:$fileFormat;
						
			$mediadbADO->Execute('UPDATE Disc SET EK_MediaType=?,FK_MediaSubType=?,FK_FileFormat=? WHERE PK_Disc=?',array($type,$subtype,$fileFormat,$discID));
			header('Location: index.php?section=discAttributes&discID='.$discID.'&msg='.$TEXT_DISC_UPDATED_CONST);			
			exit();
		}

		if(isset($_REQUEST['addPic']) && isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
			$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
			$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
			$mediadbADO->Execute($insertPicture,$picExtension);
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
				$insertPictureAttribute='INSERT INTO Picture_Disc (FK_Disc, FK_Picture) VALUES (?,?)';
				$mediadbADO->Execute($insertPictureAttribute,array($discID,$insertID));
			}else{
				//upload fail, prompt error message
				$deletePicture='DELETE FROM Picture WHERE PK_Picture=?';
				$mediadbADO->Execute($deletePicture,$insertID);
				$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
			}
			if($error!=''){
				header('Location: index.php?section=discAttributes&discID='.$discID.'&error='.$error);			
				exit();
			}else{
				header('Location: index.php?section=discAttributes&discID='.$discID.'&msg='.$TEXT_DISC_PICTURE_UPLOADED_CONST);			
				exit();
			}
		}		
		
		header('Location: index.php?section=discAttributes&discID='.$discID);			
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_DISC_ATTRIBUTES_CONST);	
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead(@$scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function addDiscAttribute($newAttributeType,$newAttributeName,$discID,$mediadbADO,$newAttributeTrack=0){
	/* @var $mediadbADO ADOConnection */
	$res=$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($newAttributeType,$newAttributeName));
	$attrID=$mediadbADO->Insert_ID();
	$res=$mediadbADO->Execute('INSERT INTO Disc_Attribute (FK_Disc,FK_Attribute,Track) VALUES (?,?,?)',array($discID,$attrID,$newAttributeTrack));
	
	if($mediadbADO->Affected_Rows()>0){
		return true;
	}
	
	return false;
}

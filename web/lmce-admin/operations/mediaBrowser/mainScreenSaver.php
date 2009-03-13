<?
function mainScreenSaver($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$records_per_page=50;
	$screenSaverAttribute=getScreenSaverAttribute($mediadbADO);
	$screenSaverDisabledAttribute=getScreenSaverAttribute($mediadbADO,46);
	
	if($action=='form'){
		$flickerEnabled=flickrStatus();
		$flicker_enable_disable_text=($flickerEnabled==1)?$TEXT_DISABLE_FLICKR_CONST:$TEXT_ENABLE_FLICKR_CONST;
		$flicker_status=($flickerEnabled==1)?$TEXT_ENABLED_CONST:$TEXT_DISABLED_CONST;
		
		if($path!=''){
			$physicalFiles=grabFiles($path,'');
			
			$out.='
			<script>
			function windowOpen(locationA,attributes) 
			{
				window.open(locationA,\'\',attributes);
			}
			

			function syncPath(path)
			{
				top.treeframe.location=\'index.php?section=leftScreenSaver&startPath=\'+escape(path);
				self.location=\'index.php?section=mainScreenSaver&path=\'+escape(path);
			}
		function set_checkboxes()
		{
		   val=document.mainScreenSaver.sel_all.checked;
		   for (i = 0; i < mainScreenSaver.elements.length; i++)
		   {
			tmpName=mainScreenSaver.elements[i].name;
		     if (mainScreenSaver.elements[i].type == "checkbox")
		     {
		         mainScreenSaver.elements[i].checked = val;
		     }
		   }
		}			
			</script>
			
			<a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a>
			
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			<form action="index.php" method="POST" name="mainScreenSaver">
			<input type="hidden" name="section" value="mainScreenSaver">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			<input type="hidden" name="page" value="'.$page.'">
			';
			
			$out.='
				<input type="hidden" name="flikr" value="'.(($flickerEnabled==0)?1:0).'">
				<fieldset style="width:400px;">
				<legend>Flikr script ('.$flicker_status.')</legend>
				Enable/disable the script who retrieve images from flikr.com<br>
				<input type="submit" class="button" name="flikrBtn" value="'.$flicker_enable_disable_text.'"><hr>
				
				Remove flikr pictures from screensaver and disable the script<br>
				<b>Flikr pictures path:</b> <input type="text" name="flikr_path" value="/home/public/data/pictures/flickr/" style="width:200px;"> <input type="submit" class="button" name="remove_flikr_pics" value="Remove">			
				<hr>
				<input type="submit" name="reload" class="button" value="'.$TEXT_RELOAD_SCREENSAVER_CONST.'">
				</fieldset>
				
				
				<table cellpading="0" cellspacing="0">
					<tr>
						<td>'.getScreensaverFiles($path,$screenSaverAttribute,$screenSaverDisabledAttribute,$mediadbADO,$page,$records_per_page).'</td>
					</tr>';
				$out.='		
				</table>';

		}
	$out.='
			
		</form>
	';

	}else{
		if(isset($_POST['reload'])){
			$cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 '.$GLOBALS['PhotoScreenSaver'].' 1 606';
			exec_batch_command($cmd);
			
			header('Location: index.php?section=mainScreenSaver&path='.urlencode($path).'&msg='.cleanString($TEXT_SCREENSAVER_RELOADED_CONST).'&page='.$page);
			exit();			
		}
				
		if(isset($_POST['flikrBtn'])){
			$flickr=(int)@$_REQUEST['flikr'];
			$parm=($flickr==1)?'-enable':'-disable';
			flickrStatus($parm);
		}

		$displayedFiles=cleanString(@$_POST['displayedFiles']);
		if($displayedFiles!=''){
			
			$displayedFilesArray=explode(',',$displayedFiles);
			foreach ($displayedFilesArray AS $fileID){
				if((int)@$_POST['file_'.$fileID]==0){
					if(file_attribute_exist($fileID,$screenSaverAttribute,$mediadbADO)){
						$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverDisabledAttribute,$fileID));
					}
				}else{
					$mediadbADO->Execute('DELETE File_Attribute.* FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=46 AND FK_File=?',array($fileID));					
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverAttribute,$fileID));
				}
			}
		}
		
		if(isset($_POST['remove_flikr_pics'])){
			flickrStatus('-disable');
			
			$flikr_path=cleanString($_POST['flikr_path']);
			$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) SELECT ?, PK_File FROM File INNER JOIN File_Attribute ON PK_File=File_Attribute.FK_File AND FK_Attribute=? WHERE Path LIKE \''.$flikr_path.'%\'',array($screenSaverDisabledAttribute,$screenSaverAttribute));
		}				
		
		$cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 '.$GLOBALS['PhotoScreenSaver'].' 1 606';
		exec_batch_command($cmd);
		
		header('Location: index.php?section=mainScreenSaver&path='.urlencode($path).'&msg='.cleanString($TEXT_SCREENSAVER_UPDATED_CONST).'&page='.$page);
		exit();
		
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_SCREEN_SAVER_CONST);
	$output->setReloadLeftFrame(false);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getUpperLevel($path)
{
	if($path=='/')
		return $path;
	$upLevel=substr($path,0,-1);
	return substr($upLevel,0,strrpos($upLevel,'/'));
}


function getScreensaverFiles($path,$screenSaverAttribute,$screenSaverDisabledAttribute,$mediadbADO,$page,$records_per_page){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	$query='
		SELECT PK_File,Filename,Path,fa1.FK_Attribute,fa2.FK_Attribute AS AttributeDisabled
		FROM File 
		LEFT JOIN File_Attribute fa1 ON fa1.FK_File=PK_File AND fa1.FK_Attribute=? 
		LEFT JOIN File_Attribute fa2 ON fa2.FK_File=PK_File AND fa2.FK_Attribute=? 
		WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0 AND Path=?';
	$res=$mediadbADO->Execute($query,array($screenSaverAttribute,$screenSaverDisabledAttribute,$path));
	$records=$res->RecordCount();
	
	$noPages=ceil($records/$records_per_page);
	$start=($page-1)*$records_per_page;
	$end=$page*$records_per_page-1;
	$end=($end>$records)?$records:$end;	
	
	$pageLinks='Pages: ';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
	}	
	
	$res=$mediadbADO->Execute($query.' LIMIT '.$start.','.$records_per_page,array($screenSaverAttribute,$screenSaverDisabledAttribute,$path));
	if($res->RecordCount()==0){
		return $TEXT_NO_IMAGES_IN_DIRECTORY_CONST;
	}
	$out='
		<table celspacing="0" cellpadding="3" align="center" width="650">
			<tr class="tablehead">
				<td width="20">&nbsp;</td>
				<td width="20">&nbsp;</td>
				<td><B>'.$TEXT_IMAGE_CONST.'</B></td>
			</tr>';
	$pos=0;
	$displayedFiles=array();
	while($row=$res->FetchRow()){
		$displayedFiles[]=$row['PK_File'];
		$pos++;
		$class=($pos%2==0)?'alternate_back':'';
		$picUrl='include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename']));
		$filePic=(!file_exists($row['Path'].'/'.$row['Filename'].'.tnj'))?'&nbsp;':'<a href="'.$picUrl.'" target="_blank"><img src="include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename'])).'.tnj" border="0"></a>';
		$out.='
			<tr class="'.$class.'">
				<td><input type="checkbox" name="file_'.$row['PK_File'].'" value="1" '.((is_null($row['FK_Attribute']) || (!is_null($row['FK_Attribute']) && !is_null($row['AttributeDisabled'])))?'':'checked').'></td>
				<td>'.$filePic.'</td>
				<td><a href="'.$picUrl.'" target="_blank">'.$row['Filename'].'</a></td>
			</tr>		
		';
	}
	$out.='
			<tr>
				<td colspan="3"><input type="checkbox" name="sel_all" value="1" onClick="set_checkboxes();"> Select/unselect all<br>
				<input type="submit" class="button" name="save" value="Save">	
			</td>
			</tr>
			<tr>
				<td colspan="3" align="right">'.$pageLinks.'</td>
			</tr>		
	
	</table>
	<input type="hidden" name="displayedFiles" value="'.join(',',$displayedFiles).'">';
	
	return $out;
}

/**
 * @param dbADO database connection $mediadbADO
 * @param int FK_AttributeType $type
 * @return int PK_Attribute
 */
function getScreenSaverAttribute($mediadbADO,$type=30){
	$res=$mediadbADO->Execute('SELECT PK_Attribute,Name FROM Attribute WHERE FK_AttributeType='.$type);
	if($res->RecordCount()==0){
		$label=($type==30?'*':'Screen saver disabled');
		$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) SELECT PK_AttributeType,? FROM AttributeType WHERE PK_AttributeType='.$type,array($label));
		return $mediadbADO->Insert_id();
	}
	$attribute=0;
	while($row=$res->FetchRow()){
		if($attribute==0 || $row['Name']=='*'){
			$attribute=$row['PK_Attribute'];
		}
	}
	
	return $attribute;
}

function flickrStatus($param='-status'){
	$cmd='sudo -u root /usr/pluto/bin/flickr_status.sh '.$param;
	
	return trim(exec_batch_command($cmd,1));
}

function file_attribute_exist($fileID,$attributeID,$mediadbADO){
	$res=$mediadbADO->Execute('SELECT * FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$attributeID));
	if($res->RecordCount()>0){
		return true;
	}
	
	return false;
}
?>
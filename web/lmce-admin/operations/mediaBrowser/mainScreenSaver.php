<?php
function mainScreenSaver($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	include('photoUtils.php');
	
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
			
			$out.=getJS();
			$out.=getCSS();
$out.='
			<a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a>  <a href="index.php?section=photoBrowser">Photo Browser</a><div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			<form action="index.php" method="POST" name="mainScreenSaver">
			<input type="hidden" name="section" value="mainScreenSaver">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			<input type="hidden" name="page" value="'.$page.'">
			<select id="newkw_lookup" size="8" style="display: none;"><option value="0"></option></select>
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

		// Ajax
		if (isset($_POST['setScreenSaver'])) {
		       $pkFile = $_POST['pkFile'];
		       $setScreenSaver = $_POST['setScreenSaver'];
		       setScreenSaverState($pkFile, $setScreenSaver == 1, $mediadbADO, $screenSaverAttribute, $screenSaverDisabledAttribute);
		       header('Content-type: application/json');
                       print("{result: 1}\n");
		       exit();
		}
		if (isset($_POST['removeAttribute'])) {
		       $pkFile = $_POST['pkFile'];
		       $pkAttr = $_POST['removeAttribute'];
		       $result = removeAttributeFromFile($pkFile, $pkAttr, $mediadbADO) ? 1 : 0;
		       header('Content-type: application/json');
                       print("{result: ".$result."}\n");
		       exit();
		}
		if (isset($_POST['addAttribute'])) {
		       $pkFile = $_POST['pkFile'];
		       $pkAttr = $_POST['pkAttribute'];
		       $Name = $_POST['Name'];
		       $result = addAttributeToFile($pkFile, $pkAttr, 29, $Name, $mediadbADO) ? 1 : 0;
		       header('Content-type: application/json');
                       print("{result: ".$result."}\n");
		       exit();
		}
		if (isset($_REQUEST['loadKeywords'])) {
		       $pkFile = $_REQUEST['loadKeywords'];
                       print(loadKeywords($pkFile, $mediadbADO));
		       exit();
		}
		if (isset($_REQUEST['loadAttributes'])) {
		       $Name = $_REQUEST['Name'];
                       print(loadAttributes($Name, $mediadbADO));
		       exit();
		}
		if (isset($_REQUEST['changeOrientation'])) {
		       $pkFile = $_REQUEST['pkFile'];
                       print(changeOrientation($pkFile, $_REQUEST['changeOrientation'], $mediadbADO));
		       exit();
		}
		if (isset($_POST['copyAttribute'])) {
		       $toFile = $_REQUEST['pkFile'];
		       $fromFile = $_REQUEST['fromPkFile'];
		       print(copyAttributes($toFile, $fromFile, $mediadbADO));
		       exit();
		}
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

function setScreenSaverState($fileID, $enable, $mediadbADO, $screenSaverAttribute, $screenSaverDisabledAttribute) {
	if(!$enable){
		if(file_attribute_exist($fileID,$screenSaverAttribute,$mediadbADO)){
			$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverDisabledAttribute,$fileID));
		}
	}else{
		$mediadbADO->Execute('DELETE File_Attribute.* FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=46 AND FK_File=?',array($fileID));		
		$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverAttribute,$fileID));
	}
}

function loadKeywords($pkFile, $mediadbADO) {
	$h = "";
	$query='SELECT PK_Attribute, Name
		FROM File_Attribute INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute 
		WHERE FK_File = ? AND FK_AttributeType = ?';
	$res=$mediadbADO->Execute($query,array($pkFile, 29));
	while($row=$res->FetchRow()){
	        $h.='<span class="kwSpan" id="keyword_'.$pkFile.'_'.$row['PK_Attribute'].'" title="Shift+Click to remove" onclick="removeAttributeSpan(event, this);">'.$row['Name'].'</span>';
	}
	return $h;
}
function loadAttributes($Name, $mediadbADO) {
	$h = "{ result: 1, data: [";
	$query='SELECT DISTINCT PK_Attribute, Name
		FROM File_Attribute INNER JOIN File ON File.PK_File=File_Attribute.FK_Attribute 
		INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute
		WHERE EK_MediaType = ? AND FK_AttributeType = ? AND Name LIKE ? ORDER BY Name';
	$res=$mediadbADO->Execute($query,array(7, 29, $Name."%"));
	$first = true;
	while($row=$res->FetchRow()){
		if (!$first)
		   $h.=",";
	        $h.="{".$row['PK_Attribute'].":'".$row['Name']."'}";
		$first = false;
	}
	$h.="]}";
	return $h;
}
function removeAttributeFromFile($pkFile, $pkAttr, $mediadbADO) {
	return $mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_Attribute=? AND FK_File=?',array($pkAttr, $pkFile));
}

function addAttributeToFile($pkFile, $pkAttr, $pkAttrType, $Name, $mediadbADO) {
	// If new attribute, add it
	$newPKAttr = $pkAttr;
	if (!$newPKAttr) {
		$result = $mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) SELECT PK_AttributeType,? FROM AttributeType WHERE PK_AttributeType=?',array($Name,$pkAttrType));
		if (!result)
		   return false;
		$newPKAttr = $mediadbADO->Insert_id();
	}
	// Connect attribute to file
	return $mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($newPKAttr,$pkFile));
}

function copyAttributes($toFile, $fromFile, $mediadbADO) {
	$query='SELECT PK_Attribute, Name
		FROM File_Attribute INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute 
		WHERE FK_File = ? AND FK_AttributeType = ?';
	$res=$mediadbADO->Execute($query,array($fromFile, 29));
	while($row=$res->FetchRow()){
		$result = $mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($row['PK_Attribute'],$toFile));
	}
	return "{result:1}";
}

function changeOrientation($pkFile, $orientation, $mediadbADO) {
	$query='
		SELECT PK_File,Filename,Path
		FROM File 
		WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0 AND PK_File = ?';
	$res=$mediadbADO->Execute($query,array($pkFile));
	while($row=$res->FetchRow()){
		$file = $row['Path'].'/'.$row['Filename'];
	}
	if ($orientation == "1" || $orientation == "3" || $orientation == "6" || $orientation == "8") {
		$cmd = 'sudo -u root /usr/pluto/bin/exivOrientation.sh "'.$file.'" '.$orientation.' && echo "1"';
		$result = exec_batch_command($cmd,1);
		if ($result)
		    return "{ result: 1 }";
	}
	return "{result : 0}";
}
?>
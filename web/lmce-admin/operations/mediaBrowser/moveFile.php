<?php
function moveFile($output,$mediadbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/moveFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=(int)$_REQUEST['fileID'];
	$filePath=stripslashes($_REQUEST['filePath']);
	$fileName=substr($filePath,strrpos($filePath,'/')+1);
	$oldDir=substr($filePath,0,strrpos($filePath,'/')+1);
	$dirToShow=(isset($_REQUEST['dirToShow']))?stripslashes($_REQUEST['dirToShow']):getUpperLevel($oldDir);
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="moveFile" enctype="multipart/form-data">
				<input type="hidden" name="section" value="moveFile">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="filePath" value="'.$filePath.'">
				<input type="hidden" name="fileID" value="'.$fileID.'">
				<input type="hidden" name="newPath" value="">
		';
		
		$directoriesArray=getDirectories ($dirToShow);
	
		$dirLinks='
			<table width="100%" cellpadding="2" cellspacing="0">';
		foreach ($directoriesArray AS $dirPath=>$dirName){
			$dirLinks.='
				<tr>
					<td><a href="index.php?section=moveFile&fileID='.$fileID.'&filePath='.$filePath.'&dirToShow='.$dirPath.'">'.$dirName.'</a></td>
					<td align="right"><input type="submit" class="button" name="choose" value="'.$TEXT_CHOSE_DESTINATION_CONST.'" onclick="document.moveFile.newPath.value=\''.urlencode($dirPath).'\';document.moveFile.submit();"></td>
				</tr>';
		}
		$dirLinks.='</table>';
		
		$out.='
		<h3>'.$TEXT_MOVE_MEDIA_FILE_CONST.'</h3>
		<table border="0" cellspacing="0" cellpadding="3">
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_FILE_CONST.':</B></td>
				<td colspan="2">'.$fileName.'</td>
			</tr>
			<tr bgColor="#DDDDDD">
				<td><B>'.$TEXT_FROM_DIRECTORY_CONST.':</B></td>
				<td colspan="2">'.$oldDir.'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_TO_DIRECTORY_CONST.':</B></td>
				<td><B>'.$dirToShow.'</B></td>
				<td align="right"><input type="submit" class="button" name="choose" value="'.$TEXT_CHOSE_DESTINATION_CONST.'" onclick="document.moveFile.newPath.value=\''.urlencode($dirToShow).'\';document.moveFile.submit();"></td>
			</tr>		
			<tr>
				<td>'.$TEXT_SUBDIRECTORIES_CONST.'</td>
				<td align="right" colspan="2" style="padding:0;">'.$dirLinks.' <a href="index.php?section=moveFile&fileID='.$fileID.'&filePath='.$filePath.'&dirToShow='.getUpperLevel($dirToShow).'">[ '.$TEXT_UP_ONE_LEVEL_CONST.' ]</a></td>
			</tr>
		</table>';
		$out.='
		</form>';
	}else{
	// process area

		if(isset($_POST['newPath']) && $_POST['newPath']!=''){
			$newFilePath=urldecode($_POST['newPath']);

			$filePath = bash_escape($filePath);
			$newFilePath = bash_escape($newFilePath);

			$mvID3='sudo -u root mv \''.$filePath.'.id3\' \''.$newFilePath.'.id3\'';
			exec_batch_command($mvID3);
	
			$mvCssKey='sudo -u root mv \''.$filePath.'.keys.tar.gz\' \''.$newFilePath.'.keys.tar.gz\'';
			exec_batch_command($mvCssKey);

			$cmd='sudo -u root mv \''.$filePath.'\' \''.$newFilePath.'\'';
			$moved_cmd=exec($cmd,$ret,$moved);	
			

			// succes: update database
			$newDir=(substr(urldecode($_POST['newPath']),-1)=='/')?substr(urldecode($_POST['newPath']),0,-1):urldecode($_POST['newPath']);
			$mediadbADO->Execute('UPDATE File SET Path=? WHERE PK_File=?',array($newDir,$fileID));
			
			exec_batch_command('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$oldDir.'"');
			exec_batch_command('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$newDir.'"');
		}
		
		$out.='
			<script>
				opener.document.forms[0].action.value=\'form\';
				opener.document.forms[0].submit();
				alert("'.htmlentities($TEXT_MEDIA_FILE_MOVED_CONST).'");
				self.close();
				//self.location=\'index.php?section=moveFile&fileID='.$fileID.'&filePath='.urlencode($newFilePath).'&msg='.$TEXT_MEDIA_FILE_MOVED_CONST.'\';
			</script>
		';
		//header('Location: index.php?section=moveFile&fileID='.$fileID.'&filePath='.$newFilePath.'&msg=Media file was moved.');			
	}
	$output->setReloadLeftFrame(false);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MOVE_MEDIA_FILE_CONST);
	$output->output();
}

function getDirectories ($path) {
	$dirs=array();
	if (($d = @opendir ($path)) === false) 
		return $dirs;
	else {
		while ($f = readdir ($d)) {
			if ($f != "." && $f != "..") {
				if (@is_dir ($path . $f)) {
					$dirs[$path.$f.'/']=$f;
				} 
			}
		}
		closedir ($d);
	}
	return $dirs;
}

function getUpperLevel($path)
{
	if($path=='/')
		return $path;
	$upLevel=substr($path,0,-1);
	return substr($upLevel,0,strrpos($upLevel,'/')+1);
}
?>

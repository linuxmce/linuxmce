<?php
function mainMediaFilesSync($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaFilesSync.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';

	$_SESSION['media_files_per_page']=(isset($_SESSION['media_files_per_page']))?$_SESSION['media_files_per_page']:20;
	$_SESSION['media_files_per_page']=(isset($_REQUEST['media_files_per_page']))?$_REQUEST['media_files_per_page']:$_SESSION['media_files_per_page'];

	$GLOBALS['typeArray']=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');
	$notInDBArray=array();
	$oldDir=substr($path,strrpos($path,'/')+1);

	$_SESSION['missing']=isset($_SESSION['missing'])?$_SESSION['missing']:0;
	$_SESSION['missing']=isset($_REQUEST['missing'])?$_REQUEST['missing']:$_SESSION['missing'];

	$_SESSION['show_attributes']=isset($_SESSION['show_attributes'])?$_SESSION['show_attributes']:0;
 	$_SESSION['show_attributes']=isset($_REQUEST['show_attributes'])?$_REQUEST['show_attributes']:$_SESSION['show_attributes'];

	$_SESSION['show_woattribute']=isset($_SESSION['show_woattribute'])?$_SESSION['show_woattribute']:0;
 	$_SESSION['show_woattribute']=isset($_REQUEST['show_woattribute'])?$_REQUEST['show_woattribute']:$_SESSION['show_woattribute'];

	$_SESSION['filter']=(isset($_SESSION['filter']))?$_SESSION['filter']:'filter_coverart';
	$_SESSION['filter']=(isset($_REQUEST['filter']))?$_REQUEST['filter']:$_SESSION['filter'];
	$_SESSION['selectedFilterMode']=(isset($_SESSION['selectedFilterMode']))?$_SESSION['selectedFilterMode']:'without';
	$_SESSION['selectedFilterMode']=(isset($_REQUEST['selectedFilterMode']))?$_REQUEST['selectedFilterMode']:$_SESSION['selectedFilterMode'];

	//EDIT BY PAUL MUMBY:
	//Added doRecursive function to js below
	if($action=='form'){
		if($path!=''){
			$physicalFiles=grabFiles($path,'');

			$res = $mediadbADO->Execute("(SELECT DISTINCT CONCAT('filter_attribute_', `PK_AttributeType`) AS 'key', `Description` FROM AttributeType) UNION (SELECT 'filter_coverart' AS 'key', 'Coverart' AS 'Description') ORDER BY `Description` ASC");
			$filterTypes = array();
			while($row = $res->FetchRow()){
				$filterTypes[$row['key']] = $row['Description'];
			}
			$filterModes = array('with' => 'with', 'without' => 'without');

			$out.='
			<script>
			function windowOpen(locationA,attributes)
			{
				window.open(locationA,\'\',attributes);
			}

			function requestName(oldDir)
			{
				var newDir=prompt("'.$TEXT_REQUEST_DIRECTORY_NAME_CONST.' ",oldDir);
				if(newDir!=oldDir && newDir!="" && newDir!=null){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=renDir&newDir="+newDir;
				}
			}

			function confirmDel()
			{
				if(confirm("'.$TEXT_CONFIRM_DELETE_DIRECTORY_CONST.'")){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=delDir";
				}
			}

			function doRecursive()
			{
				self.location="index.php?section=editMediaTagsRecursive&path='.urlencode($path).'";
			}

			function createSubdir()
			{
				var subDir=prompt("'.$TEXT_REQUEST_SUBDIRECTORY_NAME_CONST.' ","");
				if(subDir!=""){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=newDir&subDir="+subDir;
				}
			}

			function syncPath(path)
			{
				top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+encodeURIComponent(path);
				self.location=\'index.php?section=mainMediaFilesSync&path=\'+encodeURIComponent(path);
			}

			function doTag()
			{
			self.location="index.php?section=mainMediaFilesSync&path=\'+encodeURIComponent(path)&action=autoTag&path='.urlencode($path).'";
			windowOpen("operations/logs/taillog.php?log=/var/log/pluto/linuxmcetag.log" ,  "width=800,height=768,scrollbars=1,resizable=1,fullscreen=1");
		}
			</script>


		<table width="100%">
			<tr>
				<td><a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a></td>
				<td align="right">'.quick_search_box().'</td>
			<tr>
		</table>
			<!--
			EDIT BY PAUL MUMBY:
			ADDED RECURSIVE BUTTON BELOW
			-->
			<form action="index.php" method="POST" name="mainMediaFilesSync">
			<input type="hidden" name="section" value="mainMediaFilesSync">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.htmlentities($path).'">

			<table cellpadding="3" cellspacing="0">
				<tr bgcolor="#F0F3F8">
					<td><B>'.$TEXT_DIRECTORY_CONST.': '.$path.'</B></td>
					<td><input type="button" class="button" name="resyncDir" value="'.$TEXT_RESYNCHRONIZE_CONST.'" onClick="windowOpen(\'operations/logs/executeLog.php?script=3&path='.urlencode($path).'\',\'width=1024,height=768,toolbar=1,scrollbars=1,resizable=1\');self.location=\'index.php?section=mainMediaFilesSync&action=resync&path='.urlencode($path).'\';"></td>
					<td><input type="button" class="button" name="renameDir" value="'.$TEXT_RENAME_CONST.'" onClick="requestName(\''.addslashes($oldDir).'\')"></td>
					<td><input type="button" class="button" name="subDir" value="'.$TEXT_CREATE_SUBDIRECTORY_CONST.'" onClick="createSubdir()"></td>
					<td><input type="button" class="button" name="delDir" value="'.$TEXT_DELETE_CONST.'" onClick="confirmDel();"></td>
					<td><input type="button" class="button" name="Recursive" value="'.$TEXT_RECURSIVE.'" onClick="doRecursive();"></td>
					<td><input type="button" class="button" name="autotag" value="'.$TEXT_AUTOTAG.'" onClick="doTag()"></td>
				</tr>
			</table>
 			<table>
 				<tr>
					<td><input type="checkbox" name="show_attributes" value="1" onclick="self.location=\'index.php?section=mainMediaFilesSync&path='.$path.'&show_attributes='.((@$_SESSION['show_attributes']==1)?0:1).'\'" '.(($_SESSION['show_attributes']==1)?'checked':'').'> '.$TEXT_SHOW_ATTRIBUTES_CONST.' </td>
					<td><input type="checkbox" name="show_woattribute" value="1" onclick="self.location=\'index.php?section=mainMediaFilesSync&path='.$path.'&show_woattribute='.((@$_SESSION['show_woattribute']==1)?0:1).'\'" '.(($_SESSION['show_woattribute']==1)?'checked':'').'>Show only items without </td>
					<td>'.pulldownFromArray($filterModes,'selectedFilterMode',$_SESSION['selectedFilterMode'],'onChange="document.mainMediaFilesSync.action.value=\'form\';document.mainMediaFilesSync.submit();"','key','').'</td>
					<td>'.pulldownFromArray($filterTypes,'filter',$_SESSION['filter'],'onChange="document.mainMediaFilesSync.action.value=\'form\';document.mainMediaFilesSync.submit();"','key','').'<td>
 				</tr>
 			</table>

			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			';

			if(isset($_REQUEST['filename']) && $_REQUEST['filename']!='')
				$out.='<a href="index.php?section=leftMediaFilesSync" target="treeframe"><img src="scripts/treeview/diffDoc.gif" border="0" align="middle">'.$TEXT_SHOW_DIRECTORY_STRUCTURE_CONST.'</a>';
			$out.='
			<table cellpading="0" cellspacing="0">';
			$fileID=get_file_dir($path,$mediadbADO);
			if($fileID!==false){
				$out.='
				<tr bgColor="#EEEEEE">
					<td><B>'.$TEXT_THIS_IS_DIRECTORY_TREATED_AS_FILE_CONST.'</B>: <a href="index.php?section=editMediaFile&fileID='.$fileID.'">[ '.$TEXT_SHOW_ATTRIBUTES_CONST.' ]</a> <a href="index.php?section=editDirectoryAttributes&fileID='.$fileID.'">[ '.$TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST.' ]</a></td>
				</tr>';
			}

			$out.='
					<tr>
						<td class="tablehead"><B>'.$TEXT_FILES_ON_DISK_CONST.'</B></td>
					</tr>
					<tr>
						<td>'.physicalFilesList($path,$physicalFiles,$mediadbADO).'</td>
					</tr>';
				if($_SESSION['missing']==1){
				$out.='
					<tr>
						<td class="tablehead"><B>'.$TEXT_FILES_IN_DATABASE_ONLY_CONST.'</B></td>
					</tr>
					<tr>
						<td>'.dbonlyFilesList($path,$physicalFiles,$mediadbADO).'</td>
					</tr>';
				}
				$out.='
				</table>';
			$out.='
				<table>
				<tr>
					<td colspan="2"><B>'.strtoupper($TEXT_LEGEND_FOR_FILES_CONST).'</B><br>
					<img src=include/images/disk.gif align=middle border=0>	'.$TEXT_EXIST_ONLY_ON_DISK_CONST.'<br>
					'.(($_SESSION['missing']==1)?'<img src=include/images/db.gif align=middle border=0> '.$TEXT_EXIST_ONLY_IN_DATABASE_CONST.'<br>':'').'
					<img src=include/images/sync.gif align=middle border=0>	'.$TEXT_EXIST_BOTH_ON_DISK_AND_IN_DATABASE_CONST.'<br>
					<input type="checkbox" name="show_missing" value="1" onclick="self.location=\'index.php?section=mainMediaFilesSync&path='.$path.'&missing='.((@$_SESSION['missing']==1)?0:1).'\'" '.(($_SESSION['missing']==1)?'checked':'').'> '.$TEXT_SHOW_FILES_WHO_ARE_MISSING_FROM_DISK_CONST.'
					<td align="right"><input type="button" class="button" name="del" value="'.$TEXT_DELETE_ALL_FILES_FROM_DATABASE_CONST.'" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_ALL_FILES_FROM_DATABASE_CONST.'\'))self.location=\'index.php?section=mainMediaFilesSync&action=delallmissingfiles&dfile='.@$dbPKFiles[$dbkey].'&path='.urlencode($path).'\';"></td>
				</tr>
			</table>';
		}
	$out.='

		</form>
	';

	}else{

	// process area
		if($action=='renDir'){
			$newDir=stripslashes($_REQUEST['newDir']);
			if(ereg('/',$newDir)){
				header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&error='.$TEXT_ERROR_INVALID_DIRECTORY_NAME_CONST);
				exit();
			}else{
				$newPath=substr($path,0,strrpos($path,'/')+1).$newDir;
				exec_batch_command('sudo -u root mv "'.$path.'" "'.$newPath.'"');
				$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($newPath).'&msg='.$TEXT_DIRECTORY_RENAMED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';
			}
		}

		// delete file from database
		if($action=='delfile'){
			$dfile=(int)@$_REQUEST['dfile'];
			$fileInfo=getFieldsAsArray('File','Path,Filename,IsDirectory',$mediadbADO,'WHERE PK_File='.$dfile);
			if($fileInfo['IsDirectory'][0]==0){
				// is file, delete all references
				delete_file_from_db($dfile,$mediadbADO);
			}else{
				// is directory, get all subdirectories and delete them recursively
				delete_directory_from_db($fileInfo['Path'][0],$mediadbADO);
			}

			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_FILE_WAS_DELETED_FROM_DATABASE_CONST);
			exit();
		}

		// delete from database all files missing from the filesystem
		if($action=='delallmissingfiles'){
			$dfile=(int)@$_REQUEST['dfile'];
			$fileInfo=getFieldsAsArray('File','Path,Filename,IsDirectory',$mediadbADO,'WHERE PK_File='.$dfile);

			$newline = "<br />";

			echo "Here",$newline;
			echo $Path,  $newline;
			echo $path,  $newline;
			echo $dfile,  $newline;

			delete_missingfiles_from_db($path, $mediadbADO);
			exit();
			if($fileInfo['IsDirectory'][0]==0){
				// is file, delete all references
				delete_file_from_db($dfile,$mediadbADO);
			}else{
				// is directory, get all subdirectories and delete them recursively
				delete_directory_from_db($fileInfo['Path'][0],$mediadbADO);
			}

			exit();
		}

		if($action=='delDir'){
			$newPath=getUpperLevel($path);
			exec('sudo -u root rm -rf "'.$path.'"');
			$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&msg='.$TEXT_DIRECTORY_DELETED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';
		}

		if($action=='newDir'){
			$subDir=stripslashes($_REQUEST['subDir']);
			if(ereg('/',$subDir)){
				header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&error='.$TEXT_ERROR_INVALID_SUBDIRECTORY_NAME_CONST);
			}else{
				$newPath=$path.'/'.$subDir;
				exec('sudo -u root mkdir "'.$newPath.'"');
				$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($newPath).'&msg='.$TEXT_SUBDIRECTORY_WAS_CREATED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';
			}

		}
//=============resync
		if($action=='resync'){
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_SYNCHRONIZE_COMMAND_SENT_CONST);
			exit();
		}
//===autotag
		if($action=='autoTag'){
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_AUTOTAG_STARTED);
			exec('/usr/pluto/bin/linuxmceTag "'.$path.'"');			
			exit();
		}

		if($action=='update'){
			$notInDBArray=explode(',',@$_POST['notInDBArray']);
			$ppage=(int)@$_POST['ppage'];
			$dpage=(int)@$_POST['dpage'];

			foreach($notInDBArray AS $physicalkey){
				$filename=stripslashes(@$_POST['filename_'.$physicalkey]);
				$type=(int)@$_POST['type_'.$physicalkey];
				if($type!=0){
					$isDir=(is_dir(stripslashes($path).'/'.$filename))?1:0;
					$mediadbADO->Execute('INSERT INTO File (EK_MediaType, Path, Filename,IsDirectory,DateAdded) VALUES (?,?,?,?,NOW())',array($type,stripslashes($path),$filename,$isDir));
				}
			}
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_FILE_ADDED_TO_DATABASE_CONST.'&ppage='.$ppage.'&dpage='.$dpage);
			exit();
		}
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MEDIA_FILES_SYNC_CONST);
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

function delete_file_from_db($dfile,$mediadbADO){
	// delete records where file is foreign key
	$foreignTables=array('File_Attribute','Picture_File','PlaylistEntry');
	foreach($foreignTables AS $tablename){
		$mediadbADO->Execute('DELETE FROM '.$tablename.' WHERE FK_File='.$dfile);
	}
	$mediadbADO->Execute('DELETE FROM File WHERE PK_File='.$dfile);
}

function delete_directory_from_db($directoryPath,$mediadbADO){
	$res=$mediadbADO->_Execute("SELECT * FROM File WHERE Path LIKE '".addslashes($directoryPath)."/%' OR Path='".addslashes($directoryPath)."'");

	while($row=$res->FetchRow()){
		if($row['IsDirectory']==1 && $directoryPath!=$row['Path']){
			delete_directory_from_db($row['Path'],$mediadbADO);
		}
		delete_file_from_db($row['PK_File'],$mediadbADO);
	}
}

function delete_missingfiles_from_db($directoryPath,$mediadbADO){
	$res=$mediadbADO->_Execute("SELECT * FROM File WHERE (Path LIKE '".addslashes($directoryPath)."/%' OR Path='".addslashes($directoryPath)."') AND Missing=1");
	$newline = "<br />";

	while($row=$res->FetchRow()){
		if($row['IsDirectory']==1 && $directoryPath!=$row['Path']){
			//delete_directory_from_db($row['Path'],$mediadbADO);
			echo "DeleteDir: ", $row['Path'], $newline;
		}
		echo "RemoveFile: ", $row['PK_File'], ": ", $row['Path'], "/", $row['Filename'], $newline;
		delete_file_from_db($row['PK_File'],$mediadbADO);
	}
}

function physicalFilesList($path,$allPhysicalFiles,$mediadbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaFilesSync.lang.php');
	$noPagesArray=array(
		10=>'10',
		20=>'20',
		50=>'50',
		100=>'100',
		500=>'500',
		1000=>'1000',
		2000=>'2000',
		10000=>'10000',
		1000000=>'All *'
	);

	if(count($allPhysicalFiles)==0){
		return $TEXT_NO_FILES_IN_PHYSICAL_DIRECTORY_CONST;
	}

	$ppage=((int)@$_REQUEST['ppage']>0)?(int)$_REQUEST['ppage']:1;
	$noPages=ceil(count($allPhysicalFiles)/$_SESSION['media_files_per_page']);
	$physicalFiles=array_slice($allPhysicalFiles,$_SESSION['media_files_per_page']*($ppage-1),$_SESSION['media_files_per_page']);

	/* evaluate filter conditions */
	$filter_where = "";
	$filter_having = "";
	if($_SESSION['show_woattribute'] == 1) {
			$filter_where_negator = '';
			$filter_having_negator = '!';
		if($_SESSION['selectedFilterMode'] == 'without') {
			$filter_where_negator = ' NOT ';
			$filter_having_negator = '';
		}
		$attributeTypes = getAssocArray('AttributeType','PK_AttributeType','PK_AttributeType',$mediadbADO,'');
		foreach($attributeTypes AS $type){
			if($_SESSION['filter'] == 'filter_attribute_'.$type){
				$filter_where = " AND".$filter_where_negator."`File`.`PK_File` IN
						(SELECT `PK_File`
							FROM `File`
							LEFT JOIN `File_Attribute` ON `File_Attribute`.`FK_File` = `File`.`PK_File`
							LEFT JOIN `Attribute` ON `File_Attribute`.`FK_Attribute` = `Attribute`.`PK_Attribute`
							WHERE `Attribute`.`FK_AttributeType` = '".$type."') ";
			}
		}
		if($_SESSION['filter'] == 'filter_coverart') {
			$filter_having = " HAVING `picsNo` ".$filter_having_negator."= '0' ";
		}
	}

	$queryDBFiles='
		SELECT DISTINCT `File`.*, COUNT(`Picture_File`.`FK_Picture`) AS "picsNo"
		FROM `File`
		LEFT JOIN `Picture_File` ON `Picture_File`.`FK_File` = `File`.`PK_File`
		WHERE (Path="'.addslashes($path).'" OR Path="'.addslashes($path).'/'.'") AND Filename IN ("'.join('","',array_map('addslashes',$physicalFiles)).'")'.$filter_where.'
		GROUP BY PK_File '.$filter_having;

	$rs=$mediadbADO->_Execute($queryDBFiles);
	$dbFiles=array();
	$dbPKFiles=array();
	while($row=$rs->FetchRow()){
		$dbFiles[]=$row['Filename'];
		$dbPKFiles[]=$row['PK_File'];
		$dbPicsNoFiles[]=$row['picsNo'];
	}
	// set navigation bar
	$navBarArray=array();
	for($i=1;$i<$noPages+1;$i++){
		$navBarArray[]=($i==$ppage)?$i:'<a href="index.php?'.str_replace('ppage='.$ppage,'',$_SERVER['QUERY_STRING']).'&ppage='.$i.'" class="white_link">'.$i.'</a>';
	}
	$navBar=join(' | ',$navBarArray);

	$out='<table width="100%" cellpadding="2" cellspacing="0">';
	$itemCounter = 0;
	foreach($physicalFiles as $physicalkey => $filename){
		if((in_array($filename,$dbFiles))){
			$key=array_search($filename,$dbFiles);
			$inDB=1;
		}else{
			$notInDBArray[]=$physicalkey;
			$inDB=0;
			if(!is_dir64($path.'/'.$filename)){
				$addToDB='<td align="right"> '.$TEXT_TYPE_CONST.': <select name="type_'.$physicalkey.'">
								<option value="">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
				foreach ($GLOBALS['typeArray'] AS $typeID=>$typeDescription){
					$addToDB.='<option value="'.$typeID.'">'.str_replace('np_','',$typeDescription).'</option>';
				}
				$addToDB.='</select> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_FILE_TO_DATABASE_CONST.'">
							<input type="hidden" name="filename_'.$physicalkey.'" value="'.$filename.'">
							</td>';
			}else{
				$addToDB='<td>&nbsp;</td>';
			}
		}
		$coverArtIcon=(@$dbPicsNoFiles[$key]!=0)?'&nbsp;<img src="include/images/coverart.gif" border="0" style="vertical-align:middle;">':'';
		/* only display items according to filter rules */
		if($_SESSION['show_woattribute'] == 0
			|| ($_SESSION['show_woattribute'] == 1 &&  @$inDB==1)) {
					$itemCounter++;
    		    $out.='
			    <tr class="'.(($itemCounter%2==0)?'':'alternate_back').'">
				    <td '.(((@$inDB==1)?'colspan="2"':'')).'>'.((@$inDB==1)?'<img src=include/images/sync.gif border=0 style="vertical-align:middle;">':'<img src=include/images/disk.gif border=0 style="vertical-align:middle;">').$coverArtIcon.' '.((@$inDB==1)?'<a href="index.php?section=editMediaFile&fileID='.$dbPKFiles[$key].'"><B>'.$filename.'</B></a> ':'<B>'.$filename.'</B> '.$addToDB).'</td>
			    </tr>';
		    if(@$inDB==1){
 			    if($_SESSION['show_attributes']==1) {
 				    $queryAttributes='
 					    SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name
 					    FROM File_Attribute
 					    INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
 					    INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
 					    WHERE FK_File=? ORDER BY AttributeType.Description ASC
 					    ';
 			    	    $resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$key]);
 				    $attributes='';
 				    while($rowAttributes=$resAttributes->FetchRow()){
 					    $attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).'</a> ';
 				    }
 				    $out.='
 					    <tr class="'.(($physicalkey%2==0)?'':'alternate_back').'">
 						    <td colspan="2">'.@$attributes.'</td>
 					    </tr>';
 			    }

		    }
		}
	}
	$out.='
		<tr class="tablehead">
			<td><B>Files per page:</B> '.pulldownFromArray($noPagesArray,'media_files_per_page',$_SESSION['media_files_per_page'],'onChange="document.mainMediaFilesSync.action.value=\'form\';document.mainMediaFilesSync.submit();"','key','').', Displaying '.$itemCounter.' Items</td>
			<td align="right">'.$navBar.'</td>
		</tr>
		<tr>
			<td colspan="2"><em>WARNING: the page will load very slowly for large numbers of files and may even stop working completely</em></td>
		</tr>
	</table>
	<input type="hidden" name="notInDBArray" value="'.@join(',',@$notInDBArray).'">
	<input type="hidden" name="ppage" value="'.$ppage.'">';

	return $out;
}

function dbonlyFilesList($path,$physicalFiles,$mediadbADO)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaFilesSync.lang.php');
	$dpage=((int)@$_REQUEST['dpage']>0)?(int)$_REQUEST['dpage']:1;

	$queryDBFiles='
		SELECT DISTINCT File.*,count(FK_Picture) AS picsNo
		FROM File
		LEFT JOIN Picture_File ON FK_File=PK_File
		WHERE (Path="'.addslashes($path).'" OR Path="'.addslashes($path).'/'.'") AND Filename NOT IN ("'.join('","',array_map('addslashes',$physicalFiles)).'") AND Missing=1
		GROUP BY PK_File';
	$rs=$mediadbADO->_Execute($queryDBFiles);
	$dbFiles=array();
	$dbPKFiles=array();
	while($row=$rs->FetchRow()){
		$dbFiles[]=$row['Filename'];
		$dbPKFiles[]=$row['PK_File'];
		$dbPicsNoFiles[]=$row['picsNo'];
	}
	$noPages=ceil(count($dbFiles)/$_SESSION['media_files_per_page']);

	if(count($dbFiles)==0)
	{
		return $TEXT_NO_OTHER_FILES_IN_DATABASE_CONST;
	}

	$dbFiles=array_slice($dbFiles,$_SESSION['media_files_per_page']*($dpage-1),$_SESSION['media_files_per_page']);
	$navBarArray=array();
	for($i=1;$i<$noPages+1;$i++)
	{
		$navBarArray[]=($i==$dpage)?$i:'<a href="index.php?'.str_replace('dpage='.$dpage,'',$_SERVER['QUERY_STRING']).'&dpage='.$i.'">'.$i.'</a>';
	}
	$navBar=join(' | ',$navBarArray);

	$out='<table width="100%" cellpadding="2" cellspacing="0">';
	foreach($dbFiles as $dbkey => $filename){
		$queryAttributes='
			SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name
			FROM File_Attribute
			INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
			INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
			WHERE FK_File=? ORDER BY AttributeType.Description ASC
		';
		$resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$dbkey]);
		$attributes='';
		while($rowAttributes=$resAttributes->FetchRow()){
			$attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).'</a> ';
		}

		$out.='
			<tr class="'.(($dbkey%2==0)?'alternate_back':'').'">
				<td><img src=include/images/db.gif align=middle border=0> <a href="index.php?section=editMediaFile&fileID='.@$dbPKFiles[$dbkey].'"><B>'.$filename.'</B></a></td>
				<td align="right"><input type="button" class="button" name="del" value="'.$TEXT_DELETE_FILE_FROM_DATABASE_CONST.'" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_FILE_FROM_DATABASE_CONST.'\'))self.location=\'index.php?section=mainMediaFilesSync&action=delfile&dfile='.@$dbPKFiles[$dbkey].'&path='.urlencode($path).'\';"></td>
			</tr>
			<tr class="'.(($dbkey%2==0)?'alternate_back':'').'">
				<td colspan="2">'.@$attributes.'</td>
			</tr>';
	}
	$out.='
		<tr>
			<td align="right" colspan="2">'.$navBar.'</td>
		</tr>
	</table>
	<input type="hidden" name="dpage" value="'.$dpage.'">';

	return $out;
}

function safe_string(&$ret,$str){
	$ret=addslashes($str);
}

function get_file_dir($path,$mediadbADO){
	$fileInfo=getFieldsAsArray('File','PK_File,IsDirectory',$mediadbADO,'WHERE CONCAT(Path,\'/\',Filename)=\''.addslashes($path).'\' LIMIT 0,1');
	if(count($fileInfo)>0){
		return (($fileInfo['IsDirectory'][0]==1)?$fileInfo['PK_File'][0]:false);
	}

	return false;
}
?>

<?
function filePicker($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/filePicker.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$cgcID=(int)@$_REQUEST['cgcID'];
	$cp=(int)@$_REQUEST['cp'];

	if($action=='form'){
		$directory=cleanString(@$_REQUEST['directory']);
		
		$out.='
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<script>
		function setFile(value){
			opener.document.forms[0].CommandParameterValue_'.$cgcID.'_'.$cp.'.value=value;
			opener.focus();
			self.close();
		}
		</script>	
		<form action="index.php" method="POST" name="filePicker">
			<input type="hidden" name="section" value="filePicker">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="cgcID" value="'.$cgcID.'">
			<input type="hidden" name="cp" value="'.$cp.'">
			
			<h3>'.$TEXT_FILE_PICKER_CONST.'</h3>
			<table width="100%">
				<tr>
					<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
				</tr>
			</table>
					
		<table>
			<tr>
				<td><b>'.$TEXT_DIRECTORY_CONST.'</b></td>
				<td>'.directoriesPulldown('directory',$directory,$mediadbADO,'onChange="document.filePicker.action.value=\'form\';document.filePicker.submit();"').'</td>
			</tr>
			<tr>
				<td><b>'.$TEXT_FILES_IN_DIRECTORY_CONST.'</b></td>
				<td>'.listFiles($directory,$mediadbADO).'</td>
			</tr>			
		</table>
		
		</form>';
	}else{
			
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=filePicker&type='.$type.$suffix);
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_FILE_PICKER_CONST);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_FILE_PICKER_CONST);
	$output->output();
}

function directoriesPulldown($name,$selected,$mediadbADO,$extra){
	$directories=getAssocArray('File','PK_File',"CONCAT(Path,'/',Filename) AS Dirname",$mediadbADO,'WHERE IsDirectory=1 AND Missing=0','ORDER BY Path ASC,Filename ASC');
	
	$formatted=array();
	foreach ($directories AS $id=>$directory){
		$parts=explode('/',$directory);
		$path='';
		foreach ($parts AS $pos=>$part){	
			if($part!=''){
				$path.='/'.$part;
				$formatted[$path]=indent($pos).$part;
			}
		}
	}
	
	return pulldownFromArray($formatted,$name,$selected,$extra);
	
}

function indent($pos){
	$out='';
	for($i=0;$i<$pos;$i++){
		$out.='&nbsp;&nbsp;&nbsp;';
	}
	
	return $out;
}

function listFiles($directory,$mediadbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/filePicker.lang.php');
	
	$res=$mediadbADO->Execute("
	SELECT PK_File,Filename,Name,FK_Picture,FK_Attribute,Extension
	FROM File
	LEFT JOIN File_Attribute ON File_Attribute.FK_File=PK_File 
	LEFT JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=13 
	LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File
	LEFT JOIN Picture ON FK_Picture=PK_Picture
	WHERE IsDirectory=0 AND Missing=0 AND (File.Path=? OR File.Path=?)
	GROUP BY FK_AttributeType,FK_Picture",
	array($directory,$directory.'/'));
	if($res->RecordCount()==0){
		return $TEXT_NO_FILES_IN_SELECTED_DIRECTORY_CONST;
	}
	
	$out='<table>';
	$pos=0;
	while($row=$res->FetchRow()){
		$class=($pos%2==0)?"":"alternate_back";
		$out.='
		<tr class="'.$class.'">
			<td>'.$row['Filename'].'</td>
			<td>'.(((int)$row['FK_Attribute']!=0)?$TEXT_FILE_TITLE_CONST.': '.$row['Filename']:'').'</td>
			<td>'.(((int)$row['FK_Picture']!=0)?'<a href="mediapics/'.$row['FK_Picture'].'.'.$row['Extension'].'" target="_blank"><img src="include/images/coverart.gif" border="0"></a>':'').'</td>
			<td><a href="javascript:setFile('.$row['PK_File'].');">'.$TEXT_PICK_CONST.'</a></td>
		</tr>
		';
		$pos++;
	}
	$out.='</table>';
	
	return $out;
}
?>
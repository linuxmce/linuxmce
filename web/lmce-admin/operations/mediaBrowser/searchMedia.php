<?
function searchMedia($output,$mediadbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/searchMedia.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$keyword=cleanString(@$_REQUEST['keyword']);
	$path=cleanString(@$_REQUEST['path']);
	$partial=(int)@$_REQUEST['partial'];
	$attributes=(int)@$_REQUEST['attributes'];
	
	$scriptInHead='
	<script>

	function syncPath(path)
	{
		top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
		self.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
	}
	function syncFile(path,fileID)
	{
		top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
		self.location=\'index.php?section=editMediaFile&fileID=\'+fileID;
	}
	</script>
	';

	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			<form action="index.php" method="GET" name="searchMedia" enctype="multipart/form-data">
				<input type="hidden" name="section" value="searchMedia">
				<input type="hidden" name="action" value="form">
			<table>
				<tr>
					<td>'.$TEXT_SEARCH_KEYWORD_CONST.'</td>
					<td><input type="text" name="keyword" value="'.$keyword.'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_STARTING_PATH_CONST.'</td>
					<td><input type="text" name="path" value="'.$path.'"></td>
				</tr>				
				<tr>
					<td>'.$TEXT_PARTIAL_SEARCH_CONST.'</td>
					<td><input type="checkbox" name="partial" value="1" checked></td>
				</tr>				
				<tr>
					<td>'.$TEXT_SEARCH_IN_ATTRIBUTES_CONST.'</td>
					<td><input type="checkbox" name="attributes" value="1" checked></td>
				</tr>				
				<tr>
					<td>&nbsp;</td>
					<td><input type="submit" class="button" name="doSearch" value="Search"></td>
				</tr>				
			</table>
			<hr>
		';
		if(isset($_REQUEST['doSearch'])){
			$out.=search_results($keyword,$path,$partial,$attributes,$mediadbADO);
		}
	}else{
	// process area
		header('Location: index.php?section=searchMedia');			
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_SEARCH_MEDIA_CONST);
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function search_results($keyword,$path,$partial,$attributes,$mediadbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/searchMedia.lang.php');
	
	if($keyword=='' && $path==''){
		return $TEXT_PARAMETERS_NOT_SPECIFIED_CONST;
	}
	
	if(strlen($keyword)<3 && strlen($keyword)<3){
		return $TEXT_SEARCH_STRING_TOO_SHORT_CONST;
	}
	
	$whereArray=array();
	$joinArray=array();
	
	if($keyword!=''){
		$whereArray[]=($partial==0)?'Filename=\''.$keyword.'\'':'Filename LIKE \'%'.$keyword.'%\'';
	}

	if($attributes==1){
		$joinArray[]='JOIN File_Attribute ON FK_File=PK_File JOIN Attribute ON FK_Attribute=PK_Attribute';
		$whereArray[]=($partial==0)?'Name=\''.$keyword.'\'':'Name LIKE \'%'.$keyword.'%\'';
	}

	
	if($path!=''){
		$where=' WHERE Missing=0 AND ('.(join(' OR ',$whereArray)).') AND ('.(($partial==0)?'Path=\''.$path.'\'':'Path LIKE \'%'.$path.'%\'').')';
	}else{
		$where=' WHERE Missing=0 AND ('.(join(' OR ',$whereArray)).')';
	}



	$dataFiles=getFieldsAsArray('File','PK_File,Filename,Path',$mediadbADO,join(' ',$joinArray).$where,'GROUP BY PK_File ORDER BY Path ASC,Filename ASC');

	if(count(@$dataFiles)==0){
		return $TEXT_NO_RECORDS_CONST;
	}
	
	$out='<table>
		<tr class="tablehead">
			<td align="center">#</td>
			<td align="center"><b>Path</b></td>
			<td align="center"><b>Filename</b></td>
		</tr>	
	';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$records_per_page=20;	
	$totalRecords=count(@$dataFiles['PK_File']);
	$noPages=ceil($totalRecords/$records_per_page);
	$start=($page-1)*$records_per_page+1;
	$start=($start==1)?0:$start;
	$end=$page*$records_per_page;
	$end=($end>$totalRecords)?$totalRecords:$end;

	$pageLinks='';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="index.php?'.str_replace('&page='.$i,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
	}

	for($i=0;$i<count($dataFiles['PK_File']);$i++){
		if($i>=$start && $i<=$end){
			$class=($i%2==0)?'alternate_back':'';
			$out.='
			<tr class="'.$class.'">
				<td>'.$dataFiles['PK_File'][$i].'</td>
				<td><a href="javascript:syncPath(\''.$dataFiles['Path'][$i].'\');">'.$dataFiles['Path'][$i].'</a></td>
				<td><a href="javascript:syncFile(\''.$dataFiles['Path'][$i].'\','.$dataFiles['PK_File'][$i].');">'.$dataFiles['Filename'][$i].'</a></td>
			</tr>';
		}
	}
	$out.='
		<tr>
			<td align="right"colspan="3">'.$pageLinks.'</td>
		</tr>	
	</table>';
	
	return $out;
}
?>
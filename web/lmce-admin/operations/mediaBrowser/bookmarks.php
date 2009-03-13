<?
function bookmarks($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/bookmarks.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$records_per_page=30;
	
	//$mediaTypes=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');
	$type=(int)$_REQUEST['type'];
	switch($type){
		case 1:
			$mediaTypes=array(1,6);
		break;
		case 2:
			$mediaTypes=array(4,5,7);
		break;
		case 3:
			$mediaTypes=array(2,3);
		break;
	}
	
	if($action=='form'){
		
		
		$out.='
		
		<script>
		function set_checkboxes()
		{
		   val=document.bookmarks.sel_all.checked;
		   for (i = 0; i < bookmarks.elements.length; i++)
		   {
			tmpName=bookmarks.elements[i].name;
		     if (bookmarks.elements[i].type == "checkbox")
		     {
		         bookmarks.elements[i].checked = val;
		     }
		   }
		} 
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="bookmarks">
			<input type="hidden" name="section" value="bookmarks">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="type" value="'.$type.'">
		
		<table celspacing="0" cellpadding="3" align="center">
			<tr class="tablehead">
				<td>&nbsp;</td>
				<td><B>'.$TEXT_ICON_CONST.'</B></td>
				<td><B>'.$TEXT_MEDIA_PROVIDER_CONST.'</B></td>
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td><B>'.$TEXT_CHANNEL_CONST.'</B></td>
				<td><B>'.$TEXT_FILENAME_CONST.'</B></td>
				<td><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>';
		$bookmarksArray=getFieldsAsArray('Bookmark','PK_Bookmark,FK_MediaProvider,Bookmark.EK_MediaType,FK_Picture,Description,Position,Filename',$mediadbADO,'LEFT JOIN File on FK_File=PK_File WHERE Bookmark.EK_MediaType IN ('.join(',',$mediaTypes).')');
		if(count(@$bookmarksArray['PK_Bookmark'])==0){
			$out.='
			<tr>
				<td colspan="6" align="center" bgcolor="#F0F3F8"> '.$TEXT_NO_BOOKMARKS_CONST.'</td>
			</tr>';
		}
		
		$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
		$noItems=count($bookmarksArray['PK_Bookmark']);
		$noPages=ceil($noItems/$records_per_page);
		$start=($page-1)*$records_per_page;
		$end=$page*$records_per_page-1;
		$end=($end>$noItems)?$noItems:$end;
	
		$pageLinks='Pages: ';
		for($i=1;$i<=$noPages;$i++){
			$pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
		} 
		
		
		for($i=0;$i<count(@$bookmarksArray['PK_Bookmark']);$i++){
			if($i>=$start && $i<=$end){
				$color=($i%2==0)?'#F0F3F8':'#FFFFFF';
				$out.='
				<tr bgcolor="'.$color.'">
					<td><input type="checkbox" name="bookmark_'.$bookmarksArray['PK_Bookmark'][$i].'" value="1"></td>
					<td>'.$bookmarksArray['FK_Picture'][$i].'</td>
					<td>'.$bookmarksArray['FK_MediaProvider'][$i].'</td>
					<td>'.$bookmarksArray['Description'][$i].'</td>
					<td>'.$bookmarksArray['Position'][$i].'</td>
					<td>'.$bookmarksArray['Filename'][$i].'</td>
					<td align="center"><a href="javascript:if(confirm(\''.$TEXT_DELETE_BOOKMARK_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=bookmarks&type='.$type.'&action=del&did='.$bookmarksArray['PK_Bookmark'][$i].'\'">'.$TEXT_DELETE_CONST.'</a></td>
				</tr>';
			}
		}
		$out.='
			<tr>
				<td colspan="3"><input type="checkbox" name="sel_all" value="1" onClick="set_checkboxes();"> Select/unselect all<br>
				<a href="javascript:if(confirm(\'Are you sure you want to delete the records selected?\'))document.bookmarks.submit();">Delete selected</a></td>
				<td align="right" colspan="4">'.$pageLinks.'</td>
			</tr>		
		</table>

		
		</form>';
	}else{
		if(isset($did)){
			$did=(int)@$_REQUEST['did'];
			if($did>0){
				$mediadbADO->Execute('DELETE FROM Bookmark WHERE PK_Bookmark=?',array($did));
				$msg='The bookmark was deleted.';
			}
		}

		$bookmarksArray=getFieldsAsArray('Bookmark','PK_Bookmark,FK_MediaProvider,Bookmark.EK_MediaType,FK_Picture,Description,Position,Filename',$mediadbADO,'LEFT JOIN File on FK_File=PK_File WHERE Bookmark.EK_MediaType IN ('.join(',',$mediaTypes).')');
		for($i=0;$i<count(@$bookmarksArray['PK_Bookmark']);$i++){
			if((int)@$_REQUEST['bookmark_'.$bookmarksArray['PK_Bookmark'][$i]]==1){
				$mediadbADO->Execute('DELETE FROM Bookmark WHERE PK_Bookmark=?',array($bookmarksArray['PK_Bookmark'][$i]));
			}
		}
			
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=bookmarks&type='.$type.$suffix);
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_BOOKMARKS_CONST);
	$output->setNavigationMenu(array($TEXT_BOOKMARKS_CONST=>'index.php?section=bookmarks&type='.$type));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_BOOKMARKS_CONST);
	$output->output();
}
?>
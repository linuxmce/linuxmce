<?
function editPlaylist($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];
	
	$playlistInfo=getAssocArray('Playlist','FK_Picture','Name',$mediadbADO,'WHERE PK_Playlist=',$playlistID);
	$nameArr=array_values($playlistInfo);
	$picArr=array_keys($playlistInfo);
	$pname=$nameArr[0];
	$picID=$picArr[0];
	
	
	
	if($action=='form'){
		$pic=(!is_null($picID))?'<a href="mediapics/'.$picID.'.jpg" target="_blank"><img src="mediapics/'.$picID.'_tn.jpg" border="0"></a>':'';
		
		// media types - hardcoded to music, videos amd movies
		$mediaTypes=array(
			0=>'All',
			4=>'Music',
			5=>'Video',
			3=>'Movies'
		);
		$selMediaType=(isset($_REQUEST['mediaType']))?(int)$_REQUEST['mediaType']:0;
		
		// subtypes
		$subTypesArray=getAssocArray('MediaSubType','PK_MediaSubType','Description',$mediadbADO,'ORDER BY Description ASC');
		$selSubType=(isset($_REQUEST['subtype']))?(int)$_REQUEST['subtype']:0;
		
		// genres
		$genresArray=getAssocArray('Attribute','PK_Attribute','Name',$mediadbADO,'WHERE FK_AttributeType=8','ORDER BY Name ASC');
		$selGenre=(isset($_REQUEST['genre']))?(int)$_REQUEST['genre']:0;
		
		// order by array
		$orderByArray=array(
			0=>array(
				'filename'=>'Filename',
				'title'=>'Title'
				),
			4=>array(
				'artist'=>'Artist',
				'album'=>'Album',
				'filename'=>'Filename',
				'title'=>'Title'
				),
			5=>array(
				'filename'=>'Filename',
				'title'=>'Title'
				),
			3=>array(
				'filename'=>'Filename',
				'title'=>'Title'
				),
		);
		$selOrderBy=(isset($_REQUEST['selOrderBy']))?(int)$_REQUEST['selOrderBy']:'filename';
		
		$whereArray=array();
		$whereArray[]='IsDirectory=0 AND Missing=0';
		
		// filter my media type
		if((int)@$_REQUEST['mediaType']!=0){
			$whereArray[]='EK_MEdiaType='.(int)@$_REQUEST['mediaType'];
		}
	
		// filter my media sub-type
		if((int)@$_REQUEST['subtype']!=0){
			$whereArray[]='FK_MediaSubType='.(int)@$_REQUEST['subtype'];
		}
		
		
		$where='WHERE '.join(' AND ',$whereArray);
		// files to be added to playlist
		$toAdd=getAssocArray('File','CONCAT("file_",PK_File) AS PK_File','Filename',$mediadbADO,$where);
				
		
		$queryPlaylistEntry="
			SELECT PlaylistEntry.*,File.Filename AS FileFilename, File.Path AS FilePath
			FROM PlaylistEntry
			LEFT JOIN File ON PlaylistEntry.FK_File=PK_File
			WHERE FK_Playlist=?
			ORDER BY 'Order' ASC";
		$added=getAssocArray('PlaylistEntry','PK_PlaylistEntry','File.Filename AS FileFilename',$mediadbADO,'LEFT JOIN File ON PlaylistEntry.FK_File=PK_File WHERE FK_Playlist='.$playlistID.' ORDER BY \'Order\' ASC');

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="editPlaylist" enctype="multipart/form-data" onSubmit="return updateList();">
			<input type="hidden" name="section" value="editPlaylist">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="jumpFromTo" value="">
			<input type="hidden" name="oldPic" value="'.$picID.'">

			<table>
				<tr>
					<td align="center" colspan="2">'.$pic.'</td>
				</tr>
				<tr>
					<td><B>'.$TEXT_NAME_CONST.'</B></td>
					<td><input type="text" name="pname" value="'.$pname.'"></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_NEW_FILE_CONST.'</B></td>
					<td><input type="file" name="ppic" value=""></td>
				</tr>
				
				<tr>
					<td align="center" colspan="2"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
				</tr>				
			</table>
			<table cellpadding="3" cellspacing="2">
				<tr class="alternate_back">
					<td align="center"><B>Show</B></td>
					<td align="center">'.pulldownFromArray($mediaTypes,'mediaType',$selMediaType,'onChange="reloadForm();"').'</td>
					<td align="center"><B>of type</B></td>
					<td align="center">'.pulldownFromArray($subTypesArray,'subtype',$selSubType,'onChange="reloadForm();"').'</td>
					<td align="center"><B>genre</B></td>
					<td align="center">'.pulldownFromArray($genresArray,'genre',$selGenre,'onChange="reloadForm();"').'</td>
					<td align="center"><B>order by</B></td>
					<td align="center">'.pulldownFromArray($orderByArray[$selGenre],'orderBy',$selOrderBy,'onChange="reloadForm();"','key','').'</td>
					<td align="center"><input type="button" class="button" name="go" value="'.$TEXT_GO_CONST.'" onClick="reloadForm();"></td>
				</tr>
				<tr>
					<td align="center" colspan="9">
						<table>
							<tr>
								<td>'.pulldownFromArray($toAdd,'toAdd[]','',' id="toAddList"multiple style="height:300px;" class="input_big"','key','').'</td>
								<td align="center">
								<input type="button" class="button" name="moveL" value="&lt;" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),false);">
								<input type="button" class="button" name="moveR" value="&gt;" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),false);"><br>
								<input type="button" class="button" name="moveAllL" value="&lt;&lt;" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),true);">
								<input type="button" class="button" name="moveAllR" value="&gt;&gt;" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),true);">
								</td>
								<td>'.pulldownFromArray($added,'added[]',0,' id="addedList" multiple style="height:300px;" class="input_big"','key','').'</td>
								<td align="center"><input type="button" class="button" name="moveD" value="U" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,-1);"><br>
								<input type="button" class="button" name="moveD" value="D" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,1);">
								 </td>
							</tr>
						</table>
					</td>
				</tr>
				<tr>
					<td align="center" colspan="9"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"></td>
				</tr>				
			<table>
			<input type="hidden" name="oldEntries" value="'.join(',',array_keys($added)).'">
			<input type="hidden" name="newEntries" value="">
		</form>
		
		<script>
			function reloadForm(){
				document.editPlaylist.action.value="form";
				document.editPlaylist.submit();
			}
			
		// Dual list move function
		
		function moveDualList( srcList, destList, moveAll) {
		
			var len = destList.length;
		
			// get source elements
			for( var i = 0; i < srcList.length; i++ ) { 
		    	if ((srcList[i].selected || moveAll)) {    
		       		destList[ len ] = new Option( srcList[i].text, srcList[i].value);
		      		len++;
		    	}
		 	}
			
		 	for( var i = srcList.options.length - 1; i >= 0; i-- ) { 
			   	if ( srcList.options[i] != null && ( srcList.options[i].selected == true || moveAll ) ) {
					srcList.options[i]       = null;
			   	}
			}
		} 
		
		function eraseElement(value,destList)
		{
			// Erase source list selected elements
			for( var i = destList.options.length - 1; i >= 0; i-- ) { 
				if ( destList.options[i].value==value) {
					destList.options[i] = null;
		    	}
			}
		}
		
		function move(formObj,index,to) {
		
			var list = formObj;
			var total = list.options.length-1;
		
			if (index == -1) return false;
			if (to == +1 && index == total) return false;
			if (to == -1 && index == 0) return false;
		
				var items = new Array;
				var values = new Array;
					for (i = total; i >= 0; i--) {
						items[i] = list.options[i].text;
						values[i] = list.options[i].value;
					}
					for (i = total; i >= 0; i--) {
						if (index == i) {
							list.options[i + to] = new Option(items[i],values[i + to], 0, 1);
							list.options[i] = new Option(items[i + to], values[i]);
							i--;
						} else {
							list.options[i] = new Option(items[i], values[i]);
		   				}
					}
			list.focus();
		}
		
		
		function updateList(){
			myList=document.getElementById("addedList");
			var arr=new Array(myList.options.length);
			
			for( var i=0; i< myList.options.length; i++) { 
				arr[i]=myList.options[i].value;
			}
			document.editPlaylist.newEntries.value=arr.join();
			
			return true;
		}
		</script>';
	}else{
	// process area
	
		$oldEntries=explode(',',$_POST['oldEntries']);
		$newEntries=explode(',',$_POST['newEntries']);

		$toAdd=array_diff($newEntries,$oldEntries);
		$toRemove=array_diff($oldEntries,$newEntries);
		
		if(count($toRemove)>0){
			$mediadbADO->Execute("DELETE FROM PlaylistEntry WHERE PK_PlaylistEntry IN (".join(',',array_values($toRemove)).")");
		}
		
		if(count($toAdd)>0){
			foreach ($toAdd AS $pos=>$file){
				$fileID=substr($file,5);
				$mediadbADO->Execute("INSERT INTO PlaylistEntry (FK_File, FK_Playlist,Path,Filename,`Order`) SELECT $fileID,$playlistID,Path,Filename,$pos FROM File WHERE PK_File=?",$fileID);
			}
		}	
			
		
		header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.urlencode($TEXT_PLAYLIST_ENTRIES_UPDATED_CONST));
		exit();
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID);
	$output->setNavigationMenu(array("Playlists"=>'index.php?section=playlists',$TEXT_EDIT_PLAYLIST_CONST=>'index.php?section=editPlaylist&plID='.$playlistID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID);
	$output->output();
}
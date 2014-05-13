<?php
function editPlaylist($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/playlists.lang.php');
	

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];

	$playlistInfo=getFieldsAsArray('Playlist','EK_User, Name,FK_Picture',$mediadbADO,'WHERE PK_Playlist='.$playlistID);
	$pname=$playlistInfo['Name'][0];
	$picID=$playlistInfo['FK_Picture'][0];
	$playlistUserID=$playlistInfo['EK_User'][0];
	$userID=$_SESSION['userID'];	

	$selRecordsPerPage=(isset($_REQUEST['recordsPerPage']))?(int)$_REQUEST['recordsPerPage']:1000;
	$selLayout=(isset($_REQUEST['layout']))?$_REQUEST['layout']:'horizontal';
	$selMediaType=(isset($_REQUEST['mediaType']))?$_REQUEST['mediaType']:'all';	
	$selGenre=(isset($_REQUEST['genre']))?(int)$_REQUEST['genre']:0;
	$selOrderBy=(isset($_REQUEST['orderBy']))?$_REQUEST['orderBy']:'filename';

	if($action=='form'){
		$pic=($picID!=0)?'<a href="mediapics/'.$picID.'.jpg" target="_blank"><img src="mediapics/'.$picID.'_tn.jpg" border="0"></a>':'';

		// media types - hardcoded to music, videos and movies
		// games added 2009/05/26 tschak
		$mediaTypes=array(
			0=>'All',
			4=>'Music',
			5=>'Video',
			3=>'Movies',
			29=>'Games'
		);

		//Layouts
		$layoutArray=array(
			'horizontal'=>'Horizontal',
			'vertical'=>'Vertical'
		);
		
		// genres
		$genresArray=getAssocArray('Attribute','PK_Attribute','Name',$mediadbADO,'WHERE FK_AttributeType=8','ORDER BY Name ASC');
				
		// order by array
		if($selMediaType==4){
			//TODO: If this is a Music filetype, give some extra options
			$orderByArray=array(
				'filename'=>'Filename',
				'title'=>'Title',
			);
			
		} else {
			//All other options just have display as Filename or Title
			$orderByArray=array(
				'filename'=>'Filename',
				'title'=>'Title'
			);
		}
		
		
		$titlesArray=array_keys(getAssocArray('Attribute','PK_Attribute','Name',$mediadbADO,'WHERE FK_AttributeType=13','ORDER BY Name ASC'));
		
		$whereArray=array();
		$joinArray=array();
		
		$whereArray[]='IsDirectory=0 AND Missing=0';
				
		// filter my media type
		if((int)@$_REQUEST['mediaType']!=0){
			$whereArray[]='EK_MediaType='.(int)@$_REQUEST['mediaType'];
		} else {include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
			//we don't want ALL media types, but we do want the supported ones!
			foreach($mediaTypes as $Key=>$thisElement) {
				if($Key!=0) {
				$values[] = 'EK_MediaType='.$Key;

				}
			}
			$whereArray []='('.join(' OR ',$values).')';
		}
	
				
		// filter my genre
		if((int)@$_REQUEST['genre']!=0){
			$joinArray[]='LEFT JOIN File_Attribute FA1 ON FA1.FK_File=PK_File';
			$whereArray[]='FA1.FK_Attribute='.(int)@$_REQUEST['genre'];
		}

		// join clause for title attributes
		if(count($titlesArray)==0){
			$titlesArray[]="";
		}
		$joinArray[]='
			LEFT JOIN File_Attribute FA2 ON FA2.FK_File=PK_File AND FA2.FK_Attribute IN ('.join(',',$titlesArray).')
			LEFT JOIN Attribute A2 ON FA2.FK_Attribute=A2.PK_Attribute
		';

		//Protect user security
		//Only present public files, and this user's private files.
		$whereArray[]='(File.EK_Users_Private IS NULL OR File.EK_Users_Private='.$userID.')';
		$join=join(' ',$joinArray);
		$where=' WHERE '.join(' AND ',$whereArray);
		
		switch ($selOrderBy){
			case 'title':
				$order=' ORDER BY A2.Name ASC';
			break;
			default:
				$order=' ORDER BY File.Filename ASC';
			break;
		}
	
		$limit="";
		$records_per_page=$selRecordsPerPage;
		$lpage=(isset($_REQUEST['lpage']))?(int)$_REQUEST['lpage']:1;
		$rpage=(isset($_REQUEST['rpage']))?(int)$_REQUEST['rpage']:1;
		if($lpage==0) {
			$lpage=1;
		}

		$res=$mediadbADO->Execute('SELECT COUNT(*) AS total FROM File '.$join.$where.$order);
		$row=$res->FetchRow();
		$totalToAdd=$row['total'];

		if($totalToAdd>$records_per_page){
			$noPages=ceil($totalToAdd/$records_per_page);
			$start=($lpage-1)*$records_per_page;
			$end=$lpage*$records_per_page;
			$end=($end>$totalToAdd)?$totalToAdd:$end;
	
			$pageLinks='';
			for($i=1;$i<=$noPages;$i++){
				//$pageLinks.=($i==$lpage)?' '.$i:' <a onClick="return updateList()" href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&lpage='.$lpage,'',$_SERVER['QUERY_STRING']).'&lpage='.$i.'&action=autoSave">'.$i.'</a>';
				$pageLinks.=($i==$lpage)?' '.$i:' <a href="javascript:autoSave('.$i.')">'.$i.'</a>';
			}
			$limit=" LIMIT $start, $records_per_page";
		}

		//$mediadbADO->debug=true;
		// files to be added to playlist
		if($selOrderBy=='filename'){
			$toAdd=getAssocArray('File','CONCAT("file_",PK_File) AS PK_File','File.Filename AS Filename',$mediadbADO,$join.$where,$order.$limit);
			$added=getAssocArray('PlaylistEntry','PK_PlaylistEntry','File.Filename',$mediadbADO,'LEFT JOIN File ON PlaylistEntry.FK_File=PK_File WHERE FK_Playlist='.$playlistID.' ORDER BY PlaylistEntry.`Order` ASC'.$rlimit);
		} else if ($selOrderBy=='title') {
			$toAdd=getAssocArray('File','CONCAT("file_",PK_File) AS PK_File','IF(A2.Name IS NULL,CONCAT("--",File.Filename,"-- (No Title Assigned)"),A2.Name) AS Filename',$mediadbADO,$join.$where,$order.$limit);
			$added=getAssocArray('(File','PlaylistEntry.PK_PlaylistEntry','IF(A2.Name IS NULL,CONCAT("--",File.Filename,"-- (No Title Assigned)"),A2.Name) AS Filename',$mediadbADO,'INNER JOIN PlaylistEntry ON File.PK_File = PlaylistEntry.FK_File) LEFT JOIN (Attribute A2 INNER JOIN File_Attribute FA2 ON A2.PK_Attribute=FA2.FK_Attribute) ON File.PK_File=FA2.FK_File WHERE FK_Playlist='.$playlistID.' ORDER BY PlaylistEntry.`Order` ASC');
		}
				
		$totalAdded=count($added);	
		$privateChecked=($playlistUserID!=0)?'checked':'';
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		
			<div align="left" class="err">'.@$_REQUEST['error'].'</div>
			<div align="left" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			


			<form action="index.php" method="POST" name="editPlaylist" enctype="multipart/form-data" onSubmit="return updateList();">
			<input type="hidden" name="section" value="editPlaylist">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="jumpFromTo" value="">
			<input type="hidden" name="oldPic" value="'.$picID.'">

			
			<table border =0 cellpadding="3" cellspacing="2">
				<tr>
					<td><B>Layout: </B></td>
					<td align="left">'.pulldownFromArray($layoutArray,'layout',$selLayout, 'onChange="reloadForm();"','key','').'</td>
					<td align="left"><B>Media Files Per Page:</B></td>
					<td align="left"><input type="text" onkeypress="return isNumberKey(event)" name="recordsPerPage" value="'.$selRecordsPerPage.'" size ="20"></td>
				<td align="left"><input type="button" class="button" name="goRecordsPerPage" value="'.$TEXT_GO_CONST.'" onClick="reloadForm();"></td>
				</tr>
				<tr colspan=9>
					<td colspan="9"><HR></td>
				</tr>
				<tr class="alternate_back">
					<td align="center"><B>Show:</B></td>
					<td align="center">'.pulldownFromArray($mediaTypes,'mediaType',$selMediaType,'onChange="reloadForm();"','key','').'</td>
					<td align="center"><B>Genre:</B></td>
					<td align="center">'.pulldownFromArray($genresArray,'genre',$selGenre,'onChange="reloadForm();"').'</td>
					<td align="center"><B>Display As:</B></td>
					<td align="center">'.pulldownFromArray($orderByArray,'orderBy',$selOrderBy,'onChange="reloadForm();"','key','').'</td>
					<td align="center"><input type="button" class="button" name="go" value="'.$TEXT_GO_CONST.'" onClick="reloadForm();"></td>
				</tr>
				<tr>
					<td align="center" colspan="9">';
						if($selLayout=='horizontal') {
							$out=$out.
						'<table>
							<tr>
								<td width="350" halign="left" valign="top">'.$pageLinks.'</td>
								<td align="center">&nbsp;</td>'.
								'<td align="center">&nbsp;</td>
								<td align="center">&nbsp;</td>
							</tr>						
							<tr>
								<td>'.pulldownFromArray($toAdd,'toAdd[]','',' id="toAddList"multiple style="height:400px;width:350px;" class="input_big"','key','').'</td>
								<td align="center">
									<input type="button" class="button" name="moveL" value="&lt;" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),false);">
									<input type="button" class="button" name="moveR" value="&gt;" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),false);"><br>
									<input type="button" class="button" name="moveAllL" value="&lt;&lt;" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),true);">
									<input type="button" class="button" name="moveAllR" value="&gt;&gt;" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),true);">
								</td>
								<td>'.pulldownFromArray($added,'added[]',0,' id="addedList" multiple style="height:400px;width:350px;" class="input_big"','key','').'</td>
								<td align="center"><input type="button" class="button" name="moveD" value="U" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,-1);"><br>
									<input type="button" class="button" name="moveD" value="D" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,1);">
								</td>
							</tr>
							<tr>
								<td class="alternate_back">'.$TEXT_FILES_TO_ADD_CONST.': <b>'.$totalToAdd.'</b></td>
								<td align="center">&nbsp;</td>
								<td class="alternate_back" align="right">'.$TEXT_FILES_ADDED_CONST.': <b>'.$totalAdded.'</b></td>
								<td align="center">&nbsp;</td>
							</tr>							
						</table>';

						} else {
							$out=$out.
						'<Table border=0 cellpadding=0 cellspacing=0>
							<tr>
								<td width=25>&nbsp;</td>
								<td width=200>&nbsp;</td>
								<td width=200>&nbsp;</td>
								<td width=200>&nbsp;</td>
								<td width=25>&nbsp;</td>
							</tr>
							<tr>
		
								<td colspan = 5>'.@$pageLinks.'</td>
							</tr>
							<tr>
								<td colspan = 5>'.$TEXT_FILES_TO_ADD_CONST.': <b>'.$totalToAdd.'</td>						
							</tr>
							<tr>
								<td colspan = 5><img src="./operations/mediaBrowser/images/addTopBorder.png"></td>
							</tr>
							<tr height=200>
								<td><img src="./operations/mediaBrowser/images/addTab.png"></td>
								<td colspan=3>' .pulldownFromArray($toAdd,'toAdd[]','',' id="toAddList"multiple style="height:200px;width:600px" class="input_big"','key','').'</td>
								<td><img src="./operations/mediaBrowser/images/addRight.png"></td>
							</tr>

							<tr height=15>
								<td colspan=5><img src="./operations/mediaBrowser/images/addBottomBorder.png"></td>
							</tr>
							<tr rowspan = 3 height=50>
								<td>&nbsp;</td>
								<td rowspan=3 valign="Top" height=115><img src="./operations/mediaBrowser/images/addArrow.png" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),false);"></td>
								<td><img src="./operations/mediaBrowser/images/removeAll.png" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),true);"></td>
								<td rowspan = 3 valign="Bottom"><img src="./operations/mediaBrowser/images/removeArrow.png" onClick="moveDualList(document.getElementById(\'addedList\'),document.getElementById(\'toAddList\'),false);"></td>
								<td>&nbsp;</td>
							</tr>
							<tr height=15>
								<td&nbsp;</td>
								<td>&nbsp;</td>
								<td>&nbsp;</td>
		
							</tr>
							<tr height=50>
								<td>&nbsp;</td>
								<td><img src="./operations/mediaBrowser/images/addAll.png" onClick="moveDualList(document.getElementById(\'toAddList\'),document.getElementById(\'addedList\'),true);"></td>
								<td>&nbsp;</td>
		
		
							</tr>
							<tr height=15>
								<td colspan=5><img src="./operations/mediaBrowser/images/removeTopBorder.png"></td>
							</tr>
							<tr>
								<td rowspan=3><img src="./operations/mediaBrowser/images/playlistTab.png"></td>
								<td colspan=3 rowspan=3 height=200>'.pulldownFromArray($added,'added[]',0,' id="addedList" multiple style="height:200px;width:600px" class="input_big"','key','').'</td>
								<td valign="Top" height=25><img src="./operations/mediaBrowser/images/reorderUp.png" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,-1);"></td>
							</tr>
							<tr>
		
								<td><img src="./operations/mediaBrowser/images/reorderLabel.png"></td>
							</tr>
							<tr>
								<td height=25><img src="./operations/mediaBrowser/images/reorderDown.png" onClick="move(document.getElementById(\'addedList\'),document.getElementById(\'addedList\').selectedIndex,1);"></td>
							</tr>
							<tr>
								<td colspan=5>
									<img src="./operations/mediaBrowser/images/playlistBottomBorder.png">
								</td>
							</tr>
							<tr>
								<td colspan=5>'.$TEXT_FILES_ADDED_CONST.': <b>'.$totalAdded.'</td>
							</tr>
							<tr>
								<td colspan=5>
									&nbsp;
								</td>						
							</tr>
						</Table>';
						}
					$out=$out.
					'</td>

				<tr>
					<td align="center" colspan="9"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="advanced" value="'.$TEXT_ADVANCED_CONST.'" onClick="self.location=\'index.php?section=editPlaylistAdvanced&plID='.$playlistID.'\'"></td>
				</tr>	
				<tr>
					<td align="center" colspan="9">
									<table border=0 width=650>
										<tr>
											<td align="center" colspan="2" class="alternate_back">
												<HR>
												<B>Playlist Properties</B>
											</td>
										</tr>
										<tr>
											<td align="center" colspan="2">'.$pic.'</td>
										</tr>
										<tr>
											<td><B>'.$TEXT_NAME_CONST.'</B></td>
											<td><input type="text" name="pname" value="'.$pname.'" size ="50"></td>
										</tr>
										<tr>
											<td><B>'.$TEXT_NEW_FILE_CONST.'</B></td>
											<td><input type="file" name="ppic" value="" size="46"></td>
										</tr>
										<tr>
											<td><b>Private</b></td>
											<td><input type="checkbox" name="private" value="1" '.$privateChecked.'></td>
										</tr>
										<tr>
											<td align="center" colspan="2"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"><input type="submit" class="button" name="export" value="Export Playlist" onClick="windowOpen(\'index.php?section=exportPlaylist&playlistID='.$playlistID.'\',\'width=400,height=450,toolbar=0,scrollbars=0,resizable=1\');" '.'</td>
										</tr>				
									</table>
					</TD>
				</tr>			
			<table>
			
			<input type="hidden" name="oldEntries" value="'.join(',',array_keys($added)).'">
			<input type="hidden" name="newEntries" value="">
			<input type="hidden" name="readyToUpdate" value="0">
			<input type="hidden" name="lpage" value="'.$lpage.'">
			
		</form>
		
		<script>
			function reloadForm(){
				updateList();
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
							list.options[i + to] = new Option(items[i],values[i], 0, 1);
							list.options[i] = new Option(items[i + to], values[i+to]);
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
			
			//lets set a flag for the process area to know its ok to update/delete entries. The only time this flag will be set is if
			//this updateList function is called by a submit in the html form. This prevents unwanted additions/deletions if we get to
			//the process area without getting there by a submit from the form. This is what caused the bug where sometimes entire playlists were wiped clean.		
			document.editPlaylist.readyToUpdate.value=1;
			
			return true;
		}
 		function isNumberKey(evt) {
        		var charCode = (evt.which) ? evt.which : event.keyCode
         		if (charCode > 31 && (charCode < 48 || charCode > 57))
            			return false;
			return true;
     		}
		function autoSave(nextPage) {
			//this is only called while changing pages
			//so lets push in the new page number
			document.editPlaylist.lpage.value=nextPage;
			//prepare lists for update and set readyToUpdate flag
			updateList();
			//and fake a form submit
			document.editPlaylist.submit();
		}
		</script>';
	}else{
	// process area
	
		$lpage=(int)$_POST['lpage'];
		$pname=cleanString($_POST['pname']);
		$private=(int)@$_POST['private'];
		$userID=($private==1)?$userID:0;
		$mediadbADO->Execute('UPDATE Playlist SET Name=?, EK_User=? WHERE PK_Playlist=?',array($pname,$userID,$playlistID));

		if($_FILES['ppic']['name']!==''){
			$newPic=uploadPicture($_FILES['ppic'],$mediadbADO);

			if($newPic!==false){
				$mediadbADO->Execute('UPDATE Playlist SET FK_Picture=? WHERE PK_Playlist=?',array($newPic,$playlistID));
			}
		}
		
		
		if($_POST['readyToUpdate']) {
			$oldEntries=explode(',',$_POST['oldEntries']);
			$newEntries=explode(',',$_POST['newEntries']);
	
			$toAdd=array_diff($newEntries,$oldEntries);
			$toRemove=cleanArray(array_diff($oldEntries,$newEntries));
	
			if(count($toRemove)>0){
				$mediadbADO->Execute("DELETE FROM PlaylistEntry WHERE PK_PlaylistEntry IN (".join(',',array_values($toRemove)).")");
			}
			
			if(count($toAdd)>0){
	
				foreach ($toAdd AS $pos=>$file){
					$fileID=substr($file,5);
					if((int)$fileID>0){
						$mediadbADO->Execute("INSERT INTO PlaylistEntry (FK_File, FK_Playlist,Path,Filename,`Order`) SELECT $fileID,$playlistID,Path,Filename,$pos FROM File WHERE PK_File=?",$fileID);
					}
				}
			}	
			
			if(count($newEntries)>0){
				for($i=0;$i<count($newEntries);$i++){
					$mediadbADO->Execute('UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?',array($i,$newEntries[$i]));
				}
			}
	
		}//end if readyToUpdte	

		
		header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.urlencode($TEXT_PLAYLIST_ENTRIES_UPDATED_CONST).'&mediaType='.$selMediaType.'&genre='.$selGenre.'&orderBy='.$selOrderBy.'&layout='.$selLayout.'&recordsPerPage='.$selRecordsPerPage.'&lpage='.$lpage);
		exit();
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID);
	$output->setNavigationMenu(array($TEXT_PLAYLISTS_CONST=>'index.php?section=playlists',$TEXT_EDIT_PLAYLIST_CONST=>'index.php?section=editPlaylist&plID='.$playlistID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID);
	$output->output();
}

function uploadPicture($fileArray,$mediadbADO){
	$picExtension='jpg';
	if(($fileArray['type']!="image/jpg") && ($fileArray['type']!="image/pjpeg") && ($fileArray['type']!="image/jpeg")){
		$error=$TEXT_ERROR_NOT_JPEG_CONST;
	}else{
		$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
		$mediadbADO->Execute($insertPicture,array($picExtension));
		$pictureID=$mediadbADO->Insert_ID();
		$newPlaylistPictureName=$pictureID.'.'.$picExtension;

		if(move_uploaded_file($fileArray['tmp_name'],$GLOBALS['mediaPicsPath'].$newPlaylistPictureName)){
			// create thumbnail
			resizeImage($GLOBALS['mediaPicsPath'].$newPlaylistPictureName, $GLOBALS['mediaPicsPath'].$pictureID.'_tn.'.$picExtension, 256, 256);
		}
	}
	return (isset($error))?false:$pictureID;
}
?>

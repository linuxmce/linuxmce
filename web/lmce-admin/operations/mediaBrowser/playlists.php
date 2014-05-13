<?php
function playlists($output,$mediadbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/playlists.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/exportPlaylist.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$userID=$_SESSION['userID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			
			<form action="index.php" method="POST" name="playlists" enctype="multipart/form-data">
			<input type="hidden" name="section" value="playlists">
			<input type="hidden" name="action" value="update">
		
			<table cellpadding="3" cellspacing="0" border="0">
				<tr class="tablehead">
					<td align="center"><B>#</B></td>
					<td align="center"><B>'.$TEXT_PICTURE_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_PLAYLIST_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>
		';
		$resPlaylist=$mediadbADO->Execute('SELECT * FROM Playlist WHERE EK_User=? || EK_User=0',$userID);
		if($resPlaylist->RecordCount()==0){
			$out.='				
				<tr>
					<td align="center" colspan="4">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>
			';
		}
		$pos=0;
		while($rowPlaylist=$resPlaylist->FetchRow()){
			$pos++;
			$pic=(!is_null($rowPlaylist['FK_Picture']))?'<a href="mediapics/'.$rowPlaylist['FK_Picture'].'.jpg" target="_blank"><img src="mediapics/'.$rowPlaylist['FK_Picture'].'_tn.jpg" border="0"></a>':'';
			$out.='				
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#EBEFF9').'">
					<td align="center">'.$pos.'</td>
					<td align="center">'.$pic.'</td>
					<td align="center">'.$rowPlaylist['Name'].'</td>
					<td align="center"><a href="index.php?section=editPlaylist&plID='.$rowPlaylist['PK_Playlist'].'">'.$TEXT_EDIT_CONST.'</a> <a href="#" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_PLAYLIST_CONST.'\'))self.location=\'index.php?section=playlists&action=delete&plID='.$rowPlaylist['PK_Playlist'].'\'">'.$TEXT_DELETE_CONST.'</a><BR><a href="#" onClick="windowOpen(\'index.php?section=exportPlaylist&playlistID='.$rowPlaylist['PK_Playlist'].'\',\'width=400,height=450,toolbar=0,scrollbars=0,resizable=1\');">'.$TEXT_EXPORT_PLAYLIST_CONST.'</a></td>
				</tr>
			';
		}
		$out.='
				<tr>
					<td align="center" colspan="4">&nbsp;</td>
				</tr>
				<tr>
					<td align="center" colspan="4"><B>'.$TEXT_ADD_PLAYLIST_CONST.'</B> <br>
					<table>
						<tr>
							<td><b>'.$TEXT_NAME_CONST.'</b></td>
							<td><input type="text" name="newPlaylist" value=""></td>
						</tr>
						<tr>
							<td><b>Private</b></td>
							<td><input type="checkbox" name="private" value="1" checked></td>
						</tr>
						<tr>
							<td><b>'.$TEXT_PICTURE_CONST.'</b> (jpg)</td>
							<td><input type="file" name="newPlaylistPicture" value=""></td> 
						</tr>
						<tr>
							<td colspan="2" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td> 
						</tr>
					</table>
					</td>
				</tr>
			</table>
		</form>';
	}else{
	// process area
		if(isset($_POST['add'])){
			$picExtension='jpg';
			
			$error='';
			$pictureID=NULL;
			if($_FILES['newPlaylistPicture']['name']!=''){
				if(($_FILES['newPlaylistPicture']['type']!="image/jpg") && ($_FILES['newPlaylistPicture']['type']!="image/pjpeg") && ($_FILES['newPlaylistPicture']['type']!="image/jpeg")){
					$error=$TEXT_ERROR_NOT_JPEG_CONST;
				}
				else{
					$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
					$mediadbADO->Execute($insertPicture,array($picExtension));
					$pictureID=$mediadbADO->Insert_ID();
					$newPlaylistPictureName=$pictureID.'.'.$picExtension;
					
					if(move_uploaded_file($_FILES['newPlaylistPicture']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPlaylistPictureName)){
						// create thumbnail
						resizeImage($GLOBALS['mediaPicsPath'].$newPlaylistPictureName, $GLOBALS['mediaPicsPath'].$pictureID.'_tn.'.$picExtension, 256, 256);
					}
				}
			}			
			
			$newPlaylist=cleanString($_POST['newPlaylist']);
			$private=(int)@$_POST['private'];
			$userID=($private==1)?$userID:0;
			$mediadbADO->Execute('INSERT INTO Playlist (EK_User, Name,FK_Picture) VALUES (?,?,?)',array($userID,$newPlaylist,$pictureID));
			
			
			
			header('Location: index.php?section=playlists&msg='.$TEXT_NEW_PLAYLIST_ADDED_CONST.'&error='.$error);
		}
	
		if(isset($_REQUEST['plID'])){
			$plID=$_REQUEST['plID'];
			$mediadbADO->Execute('DELETE FROM Playlist WHERE PK_Playlist=?',$plID);
			
			header('Location: index.php?section=playlists&msg='.$TEXT_PLAYLIST_DELETED_CONST);
		}
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_PLAYLISTS_CONST);
	$output->setNavigationMenu(array($TEXT_PLAYLISTS_CONST=>'index.php?section=playlists'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PLAYLISTS_CONST);
	$output->output();
}

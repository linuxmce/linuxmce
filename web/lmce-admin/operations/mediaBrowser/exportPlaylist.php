<?php
function exportPlaylist($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/exportPlaylist.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/playlists.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$action = (isset($_REQUEST['action']))?$_REQUEST['action']:'form';
	$userID=$_SESSION['userID'];
	$playlistID=$_REQUEST['playlistID'];
	$exportFileTypes = (isset($_REQUEST['exportFileTypes']))?$_REQUEST['exportFileTypes']:(int)1;
	$exportFileTypes = ($exportFileTypes>3)?3:$exportFileTypes;


	if($action=='form'){
		$exportTypes=array(
			'extm3u'=>'Extended M3U (*.m3u)          ',
			'm3u'   =>'M3U (*.m3u)                   ',
			'pls'   =>'PLS (*.pls)                   ',

		);

		$out = ' 
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		
		<div align="left" class="err">'.@$_REQUEST['error'].'</div>
		<div align="left" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			

		<form action="index.php" method="POST" name="exportPlaylist" enctype="multipart/form-data">
			<input type="hidden" name="section" value="exportPlaylist">
			<input type="hidden" name="action" value="export">
			<input type="hidden" name="playlistID" value="'.$playlistID.'"?
			<table class="normaltext" cellpadding="5" cellspacing="0" width="400">
				<tr>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td bgcolor="#000000">&nbsp;</td>
				</tr>
				<tr>
					<td>'.$TEXT_EXPORT_PLAYLIST_HINT_CONST.'</td>
				</tr>
				<tr>
					<td>'.$TEXT_EXPORT_PLAYLIST_SELECT_FILES_CONST.'</td>
				</tr>
				<tr>
					<td>
						<input type="radio" name="exportFileTypes" value="music" '.(($exportFileTypes==1)?'checked':'').'>'.$TEXT_EXPORT_PLAYLIST_MUSIC_CONST.'
					</td>
				</tr>
				<tr>
					<td>
						<input type="radio" name="exportFileTypes" value="media" '.(($exportFileTypes==2)?'checked':'').'>'.$TEXT_EXPORT_PLAYLIST_MEDIA_CONST.'
					</td>
				</tr>
				<tr>
					<td>
						<input type="radio" name="exportFileTypes" value="all" '.(($exportFileTypes==3)?'checked':'').'>'.$TEXT_EXPORT_PLAYLIST_ALL_CONST.'		
					</td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td>'.$TEXT_EXPORT_PLAYLIST_SELECT_TYPE_CONST.'</td>
				</tr>
				<tr><input type="hidden" name="action" value="update">
					<td>'.pulldownFromArray($exportTypes,'exportType',1,'','key','').'<BR><HR></td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td align="center"><input type="submit" class="button" name="export" value="'.$TEXT_EXPORT_PLAYLIST_CONST.'"></td>
				</tr>
			</table>

		</form>
		';
	} else {
		//Process Area
		$exportType = $_POST['exportType'];
		$exportFileTypes = $_POST['exportFileTypes'];
		$playlistID = $_POST['playlistID'];

		//Where clause filter for file type
		if($exportFileTypes=='music') {
			$filterFileType = " AND File.EK_MediaType = 4";
		} else if($exportFileTypes=='media') {
			$filterFileType = " AND (File.EK_MediaType = 3 OR File.EK_MediaType = 4 OR File.EK_MediaType = 5)";
		} else if($exportFileTypes=='all') {
			$filterFileType = "";
		} else {
			//Default to Music
			$filterFileType = " AND File.EK_MediaType = 3";
		}

		$order = " ORDER BY PlaylistEntry.Order";
		
		//get playlist name
		$res=$mediadbADO->Execute('SELECT Name FROM Playlist WHERE PK_Playlist ='.$playlistID);
		$row = $res->FetchRow();		
		$playlistName = $row['Name'];
		//Get playlist infor
		$playlistInfo=getFieldsAsArray('Playlist','EK_User, Name,FK_Picture',$mediadbADO,'WHERE PK_Playlist='.$playlistID);
		$pname=stripslashes($playlistInfo['Name'][0]);
		$strSQL ='
			SELECT 
			     PlaylistEntry.Path, 
			     PlaylistEntry.Filename, 
			     IF(File_Attribute.FK_File Is Null, PlaylistEntry.Filename, Attribute.Name) AS Title
			 
			FROM 
			     PlaylistEntry LEFT JOIN 
				  (File LEFT JOIN 
				       (File_Attribute LEFT JOIN 
					    (Attribute LEFT JOIN AttributeType 
					    ON Attribute.FK_AttributeType = AttributeType.PK_AttributeType) 
				       ON File_Attribute.FK_Attribute = Attribute.PK_Attribute) 
				  ON File.PK_File = File_Attribute.FK_File) 
			     ON PlaylistEntry.FK_File = File.PK_File
			 
			WHERE
			     (AttributeType.Description="title" Or AttributeType.Description Is Null) 
			     AND (PlaylistEntry.FK_Playlist='.$playlistID.')'.$filterFileType.$order.';
		';

		$res=$mediadbADO->Execute($strSQL);
		if(!$res->RecordCount()) {
			header('Location: index.php?section=exportPlaylist&msg='.urlencode($TEST_EXPORT_PLAYLIST_EMPTY_CONST).'&exportType='.$exportType.'&exportFileTypes='.$exportFileTypes.'&playlistID='.$playlistID);
		}

		if($exportType=="extm3u") {
			//build extm3u playlist
			header('Content-type: audio/xmpegurl');
			header('Content-Disposition: attachment; filename="'.$playlistName.'.m3u"');
			print "#EXTM3U\r\n";
			print "#Created by LinuxMCE\r\n";
			for($i=0;$i<$res->RecordCount();$i++){
				$row=$res->FetchRow();
				$path=createSmbPath($row['Path']);
				$filename=$row['Filename'];
				print "#EXTINF:-1,".$row['Title']."\r\n";
				print $path.$filename."\r\n";
			}
			exit();
		} else if($exportType=="m3u") {
			//build m3u playlist
			header('Content-type: audio/xmpegurl');
			header('Content-Disposition: attachment; filename="'.$playlistName.'.m3u"');

			print "#Created by LinuxMCE on "."\r\n";
			for($i=0;$i<$res->RecordCount();$i++){
				$row=$res->FetchRow();
				$path=createSmbPath($row['Path']);
				$filename=$row['Filename'];
				print $path.$filename."\r\n"; 
			}
			exit();
		} else if($exportType=="pls") {
			//build pls playlist
			header('Content-type: audio/x-scpls');
			header('Content-Disposition: attachment; filename="'.$playlistName.'.pls"');
			print "[playlist]\r\n";
			for($i=0;$i<$res->RecordCount();$i++){
				$row=$res->FetchRow();
				$path=createSmbPath($row['Path']);
				$filename=$row['Filename'];
				$index = $i+1;
				print "File".$index."=".$path.$filename."\r\n";
				print "Title".$index."=".$row['Title']."\r\n";
				print "Length".$index."=-1\r\n"; 
			}
			print "NumberOfEntries=".$index."\r\n";
			print "Version=2\r\n";
			exit();
		} else {
			//Unsupported playlist type
			header('Location: index.php?section=exportPlaylist&msg='.urlencode($TEXT_EXPORT_PLAYLIST_UNSUPPORTED_CONST).'&exportType='.$exportType.'&exportFileTypes='.$exportFileTypes.'&playlistID='.$playlistID);
		}
	}

	$output->setMenuTitle($TEXT_EXPORT_PLAYLIST_CONST." |");
	$output->setPageTitle($TEXT_EXPORT_PLAYLIST_CONST."# ".$playlistID);
	$output->setNavigationMenu(array($TEXT_PLAYLISTS_CONST=>'index.php?section=playlists',$TEXT_EXPORT_PLAYLIST_CONST=>'index.php?section=exportPlaylist&playlistID='.$playlistID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.'Export Playlist');
	$output->output();
	


}//end function

function createSmbPath($path) {
	$path=str_replace("/home","smb://dcerouter",$path);
	$path=str_replace("/data","",$path);
	$path .= "/";
	return $path;
}


?>

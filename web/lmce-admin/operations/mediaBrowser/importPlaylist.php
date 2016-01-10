<?php
function importPlaylist($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/playlists.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/importPlaylist.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$action = (isset($_REQUEST['action']))?$_REQUEST['action']:'form';
	$userID=$_SESSION['userID'];
	$playlistID=$_REQUEST['playlistID'];
	$startIndex=$_REQUEST['startIndex'];

	if($action=='form'){
	$out = '
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
			function checkFiletype() {
  				var ext = document.f.uploadedFile.value;
  				ext = ext.substring(ext.length-3,ext.length);
  				ext = ext.toLowerCase();
				alert("You selected an invalid playlist file. Please select a .m3u or .pls file instead!");
				return false;
 				if(ext != "m3u" && ext != "pls") {
    					alert("You selected an invalid playlist file. Please select a .m3u or .pls file instead!");
   					 return false;
				 } else {
					 return true; 
				 }
			}

		</script>		
		
		<div align="left" class="err">'.@$_REQUEST['error'].'</div>
		<div align="left" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form enctype="multipart/form-data"  onsubmit="checkFiletype()" action="index.php" method="POST">
			Choose a file to upload: <input name="uploadedFile" type="file"><br>
			<input type="submit" class = "button" value="Upload File">
		</form>

	';
	} else {
		//Process Area


	}

	$output->setMenuTitle($TEXT_EXPORT_PLAYLIST_CONST." |");
	$output->setPageTitle($TEXT_EXPORT_PLAYLIST_CONST."# ".$playlistID);
	$output->setNavigationMenu(array($TEXT_PLAYLISTS_CONST=>'index.php?section=playlists',$TEXT_EXPORT_PLAYLIST_CONST=>'index.php?section=exportPlaylist&playlistID='.$playlistID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.'Export Playlist');
	$output->output();
}
?>

<?
function editPlaylistAdvanced($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];
	$entry=(int)@$_REQUEST['entry'];
	
	$playlistInfo=getAssocArray('Playlist','FK_Picture','Name',$mediadbADO,'WHERE PK_Playlist=',$playlistID);
	$nameArr=array_values($playlistInfo);
	$picArr=array_keys($playlistInfo);
	$pname=$nameArr[0];
	$picID=$picArr[0];
	if($action=='form'){
		$pic=(!is_null($picID))?'<a href="mediapics/'.$picID.'.jpg" target="_blank"><img src="mediapics/'.$picID.'_tn.jpg" border="0"></a>':'';
		
		$queryPlaylistEntry="
			SELECT PlaylistEntry.*,FileFilename, File.Path AS FilePath
			FROM PlaylistEntry
			LEFT JOIN File ON PlaylistEntry.FK_File=PK_File
			WHERE FK_Playlist=?
			ORDER BY 'Order' ASC";
		$entries=getAssocArray('PlaylistEntry','PK_PlaylistEntry','File.Filename',$mediadbADO,'LEFT JOIN File ON PlaylistEntry.FK_File=PK_File WHERE FK_Playlist='.$playlistID.' ORDER BY PlaylistEntry.`Order` ASC');

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="editPlaylistAdvanced" enctype="multipart/form-data" onSubmit="return updateList();">
			<input type="hidden" name="section" value="editPlaylistAdvanced">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="jumpFromTo" value="">
			<input type="hidden" name="oldPic" value="'.$picID.'">

			<table>
				<tr>
					<td align="center" colspan="2">'.$pic.'</td>
				</tr>
				<tr>
					<td><B>'.$TEXT_PLAYLIST_CONST.'</B></td>
					<td>'.$pname.'</td>
				</tr>
			</table>
			<a href="index.php?section=editPlaylist&plID='.$playlistID.'">'.$TEXT_BACK_TO_SIMPLE_MODE_CONST.'</a>
			<table border="0">
				<tr>
					<td>'.pulldownFromArray($entries,'entry',$entry,' id="entry" style="height:300px;" class="input_big" size="20" onChange="reloadForm();"','key','').'</td>
					<td align="center"></td>
				';
		if($entry!=0){
			$commandGroups=getAssocArray('CommandGroup','PK_CommandGroup','Description',$dbADO);
			$entryData=getFieldsAsArray('PlaylistEntry','EK_CommandGroup_Start,EK_CommandGroup_Stop,Duration',$mediadbADO,'WHERE PK_PlaylistEntry='.$entry);
			$out.='
					<td valign="top">
					<table border="0">
						<tr class="tablehead">
							<td align="center"><B>'.$TEXT_START_COMMAND_GROUP_CONST.'</B></td>
							<td align="center"><B>'.$TEXT_STOP_COMMAND_GROUP_CONST.'</B></td>
							<td align="center"><B>'.$TEXT_DURATION_CONST.'</B></td>		
							<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>			
						</tr>
						<tr class="alternate_back">
							<td>
								<table width="100%">
								 	<tr>
										<td align="center">'.((!is_null($entryData['EK_CommandGroup_Start'][0]))?$commandGroups[$entryData['EK_CommandGroup_Start'][0]]:'N/A').'</td>
										<td width="90"><a href="javascript:windowOpen(\'index.php?section=pickScenario&plID='.$playlistID.'&mode=start&entry='.$entry.'\',\'width=800,height=600,toolbar=1\');">'.$TEXT_PICK_COMMAND_GROUP_CONST.' / '.$TEXT_REMOVE_CONST.'</a></td>
									</tr>
								</table>
							</td>
							<td><table width="100%" border="0">
								 	<tr>
										<td align="center">'.((!is_null($entryData['EK_CommandGroup_Stop'][0]))?$commandGroups[$entryData['EK_CommandGroup_Stop'][0]]:'N/A').'</td>
										<td width="90"><a href="javascript:windowOpen(\'index.php?section=pickScenario&plID='.$playlistID.'&mode=stop&entry='.$entry.'\',\'width=800,height=600,toolbar=1\');">'.$TEXT_PICK_COMMAND_GROUP_CONST.' / '.$TEXT_REMOVE_CONST.'</a></td>
									</tr>
								</table></td>
							<td align="center"><input type="text" name="duration" value="'.$entryData['Duration'][0].'" style="width:50px;"></td>
							<td align="center"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"></td>
						</tr>					
					</table>
					</td>';
		}
		$out.='
				</tr>
			</table>
			<em>'.$TEXT_PLAYLIST_ADVANCED_NOTE_CONST.'</em>
		</form>
		
		<script>
			function reloadForm(){
				document.editPlaylistAdvanced.submit();
			}
			
		</script>';
	}else{
	// process area
		if(isset($_POST['duration'])){
			$duration=$_POST['duration'];
			$mediadbADO->Execute('UPDATE PlaylistEntry SET Duration=? WHERE PK_PlaylistEntry=?',array($duration,$entry));
			$updated=1;
		}
	
	
		if(isset($_REQUEST['cgID'])){
			$cgID=((int)$_REQUEST['cgID']==0)?NULL:(int)$_REQUEST['cgID'];
			$mode=$_REQUEST['mode'];
			$field=($mode=='start')?'EK_CommandGroup_Start':'EK_CommandGroup_Stop';
			$mediadbADO->Execute('UPDATE PlaylistEntry SET '.$field.'=? WHERE PK_PlaylistEntry=?',array($cgID,$entry));
			$updated=1;
		}		
		
		$msg=(@$updated==1)?urlencode($TEXT_PLAYLIST_ENTRIES_UPDATED_CONST):'';
		
		header('Location: index.php?section=editPlaylistAdvanced&plID='.$playlistID.'&msg='.$msg.'&entry='.$entry);
		exit();
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_PLAYLIST_ADVANCED_CONST.'# '.$playlistID);
	$output->setNavigationMenu(array("Playlists"=>'index.php?section=playlists',$TEXT_EDIT_PLAYLIST_ADVANCED_CONST=>'index.php?section=editPlaylistAdvanced&plID='.$playlistID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PLAYLIST_ADVANCED_CONST.'# '.$playlistID);
	$output->output();
}


?>
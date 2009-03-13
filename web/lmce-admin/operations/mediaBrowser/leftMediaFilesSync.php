<?
function leftMediaFilesSync($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$startPath=(isset($_REQUEST['startPath']))?urldecode($_REQUEST['startPath']):'/home/public/data';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';	
	$switchDirURL=(substr($startPath,0,5)!='/home')?'<a href="javascript:syncPath(\'/home/public/data\')">Switch to /home/public/data</a>':'<a href="javascript:syncPath(\'/mnt/upnp\')">Switch to /mnt/upnp</a>';
	
	if($action=='form'){
		$out='
		<script>
		function syncPath(path)
		{
			top.basefrm.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
			self.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
		}
		</script>
		<form action="index.php" method="POST" name="leftMediaFilesSync">
		<input type="hidden" name="section" value="leftMediaFilesSync">
		<input type="hidden" name="action" value="search">
		<input type="hidden" name="startPath" value="'.htmlentities($startPath).'">
		
	<table border="0" cellpading="2" cellspacing="0" width="100%">
			<tr>
				<td colspan="3" align="center" height="133" class="left_frame_logo">
					<a href="index.php?section=index" target="_top"><img src="include/images/spacer.gif" border="0" width="220" height="90"></a>
				</td>
			</tr>
			<tr>
				<td colspan="3" align="center" height="9" bgcolor="black"><img src="include/images/spacer.gif" border="0"></td>
			</tr>
	';
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
		$out.='
				<tr>
					<td valign="center" colspan="3" class="left_menu" height="30">'.((@$_SESSION['userLoggedIn']==true)?'<a href="index.php?section=login&action=logout" target="basefrm" >'.$TEXT_LOGOUT_CONST.'</a>':'').'</td>					
				</tr>
		';
	}
		
		$linksTree='';
		$pathsArray=explode('/',$startPath);
		$pathTo='';
		$indent='';
		$childsArray=getDirectories (stripslashes($startPath));
		foreach ($pathsArray AS $directory){
			if($directory!=''){
				$pathTo.='/'.$directory;
				$linksTree.=$indent.(($pathTo==$startPath)?'<B>[ <a href="javascript:syncPath(\''.htmlentities($pathTo).'\')"><B> '.stripslashes($directory).'</B></a> ]</B><br>':'<a href="javascript:syncPath(\''.htmlentities($pathTo).'\')"><B>&gt; '.stripslashes($directory).'</B></a><br>');
				$indent.='&nbsp;&nbsp;&nbsp;&nbsp;';
			}
		}
		
		foreach ($childsArray AS $childPath=>$childName){
			$linksTree.=$indent.'<a href="javascript:syncPath(\''.htmlentities(addslashes($childPath)).'\')">&gt; '.stripslashes($childName).'</a><br>';
		}

		$out.='
			<tr>
				<td colspan="2" class="left_menu">'.$linksTree.'</td>
			</tr>
			<tr>
				<td colspan="2" class="left_menu">'.$switchDirURL.'</td>
			</tr>			
			<tr>
				<td colspan="2" class="left_menu"><hr></td>
			</tr>			
			<tr>
				<td colspan="2" class="left_menu"><input type="checkbox" name="UpdateMediaDaemon" value="1" '.((get_update_media_daemon_status($dbADO)==1)?'checked':'').' onClick="document.leftMediaFilesSync.submit();"> '.$TEXT_UPDATE_MEDIA_DAEMON_CONST.'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center" class="left_menu"><input type="submit" class="button_fixed" name="updateThumbnails" value="Update thumbnails"></td>
			</tr>			
			<tr>
				<td colspan="2" align="center" class="left_menu"><input type="submit" class="button_fixed" name="searchTokens" value="Search tokens"></td>
			</tr>			
			</table>
		</form>';
	}else{
		// processing area
		set_update_media_daemon_status((int)@$_REQUEST['UpdateMediaDaemon'],$dbADO);

		if(isset($_POST['updateThumbnails'])){
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -h localhost -d /home/public/data/ -t';
			exec_batch_command($cmd);
		}

		if(isset($_POST['searchTokens'])){
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -h localhost -d /home/public/data/ -s';
			exec_batch_command($cmd);
		}
		
		
		header('Location: index.php?section=leftMediaFilesSync&startPath='.htmlentities($_REQUEST['startPath']));
		exit();
	}	

	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getDirectories ($path) {
	$dirs=array();
	if (($d = @opendir ($path)) === false) {
		return $dirs;
	}
	else {
		while (false !== ($f = readdir($d))) {
			if ($f != "." && $f != "..") {
				//echo $path.'/'.$f.'<br>';
				if (@is_dir64($path .'/'.$f)) {
					$dirs[$path.'/'.$f]=$f;
				} 
			}
		}
		closedir ($d);
	}
	asort($dirs,SORT_STRING);
	reset($dirs);

	return $dirs;
}

function get_update_media_daemon_status($dbADO){
	$coreDevices=array_keys(getDevicesArrayFromCategory($GLOBALS['CategoryCore'],$dbADO));
	$scriptInfo=getAssocArray('Device_StartupScript','FK_StartupScript','Enabled',$dbADO,'WHERE FK_Device='.$coreDevices[0].' AND FK_StartupScript='.$GLOBALS['UpdateMediaDaemon']);
	
	return @$scriptInfo[$GLOBALS['UpdateMediaDaemon']];
}

function set_update_media_daemon_status($value,$dbADO){
	$coreDevices=array_keys(getDevicesArrayFromCategory($GLOBALS['CategoryCore'],$dbADO));
	$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=? WHERE FK_Device=? AND FK_StartupScript=?',array($value,$coreDevices[0],$GLOBALS['UpdateMediaDaemon']));

	$param=($value==1)?'-enable':'-disable';
	$cmd='sudo -u root /usr/pluto/bin/UpdateMediaDaemonControl.sh '.$param;
	exec($cmd);
	
	$cmd='sudo -u root /usr/pluto/bin/generateRcScripts.sh';
	exec($cmd);
}
?>
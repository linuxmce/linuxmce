<?
function backup($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/backup.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	
		
	if($action=='form'){
		$out.='
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<form action="index.php" method="POST" name="backup" enctype="multipart/form-data">
		<input type="hidden" name="section" value="backup">
		<input type="hidden" name="action" value="add">
		
		<table width="600">
			<tr>
				<td><B>'.$TEXT_BACKUP_CONST.'</B></td>
			</tr>		
			<tr>
				<td>'.$TEXT_BACKUP_INFO_CONST.'</td>
			</tr>		
			<tr>
				<td align="center"><textarea name="comment" style="width:300px;height:100px;"></textarea></td>
			</tr>			
			<tr>
				<td align="center"><input type="submit" name="backup" class="button" value="'.$TEXT_BACKUP_CONST.'"></td>
			</tr>
			<tr>
				<td><hr></td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_EXISTING_BACKUP_FILES_CONST.'</B></td>
			</tr>		
			<tr>
				<td>'.backups_table().'</td>
			</tr>		
			<tr>
				<td><hr></td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_RESTORE_CONST.'</B></td>
			</tr>		
			<tr>
				<td>'.$TEXT_RESTORE_INFO_CONST.'</td>
			</tr>		
			<tr>
				<td align="center"><input type="file" name="restore_file" value=""></td>
			</tr>
			<tr>
				<td align="center"><input type="submit" name="restore" class="button" value="'.$TEXT_RESTORE_CONST.'"></td>
			</tr>		
		</table>
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=backup&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		if(isset($_POST['backup'])){
			$comment=cleanString($_POST['comment']);
			$cmd='sudo -u root /usr/pluto/bin/PlutoRestorePoint.sh --backup "'.addslashes($comment).'"';
			$result=exec_batch_command($cmd,1);
			
			
			if(ereg('Restore point created.',$result)){
				$backupFile='/home/backup/download/backup-'.date('Y-m-d').'.tar.bz2';
				if(file_exists($backupFile)){
					header("Location: index.php?section=backup&msg=".urlencode($result));
					exit();
				}else{
					header("Location: index.php?section=backup&error=".urlencode($TEXT_BACKUP_FILE_NOT_FOUND_CONST));
					exit();
				}
			}else{
				header("Location: index.php?section=backup&error=".urlencode($TEXT_BACKUP_ERROR_CONST." :: ".$result));
				exit();
			}
		}
		
		if(isset($_POST['restore'])){

			if($_FILES['restore_file']['name']==''){
				header("Location: index.php?section=backup&error=".urlencode($TEXT_NOTHING_TO_RESTORE_CONST." :: ".$result));
				exit();
			}
			/*
			$isMoved=move_uploaded_file($_FILES['restore_file']['tmp_name'],'/home/backup/upload/'.$_FILES['restore_file']['name']);
			if(!$isMoved){
				header("Location: index.php?section=backup&error=".urlencode($TEXT_RESTORE_PERMISSION_DENIED_CONST." :: ".$result));
				exit();
			}
			*/
			$mkdir_cmd='sudo -u root mkdir -p /home/backup';
			exec_batch_command($mkdir_cmd);
			
			$mkdir_cmd='sudo -u root mkdir -p /home/backup/upload';
			exec_batch_command($mkdir_cmd);
			
			$move_cmd='sudo -u root mv '.$_FILES['restore_file']['tmp_name'].' '.'/home/backup/upload/'.$_FILES['restore_file']['name'];
			exec_batch_command($move_cmd);
			
			// removed: it will be executed on reboot
			// $cmd='sudo -u root /usr/pluto/bin/PlutoRestorePoint.sh --restore --skip-md5';
			// $result=exec_batch_command($cmd,1);

			header("Location: index.php?section=backup&msg=".urlencode($TEXT_RESTORE_SUCCESS_CONST));
			exit();
		}
		
		if(isset($_REQUEST['local'])){
			$backupFile=cleanString($_REQUEST['filepath']);
			$pos=strpos($backupFile,'/home/backup/download/');
			if($pos!==false){
				$mkdir_cmd='sudo -u root mkdir -p /home/backup/upload';
				exec_batch_command($mkdir_cmd);				
				
				$move_cmd='sudo -u root mv '.$backupFile.' '.'/home/backup/upload/'.str_replace('/home/backup/download/','',$backupFile);
				exec_batch_command($move_cmd);

				header("Location: index.php?section=backup&msg=".urlencode($TEXT_RESTORE_SUCCESS_CONST));
				exit();
			}else{
				// error: wrong file specified
				header("Location: index.php?section=backup&error=".urlencode('Invalid file'));
				exit();
			}
		}
		
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_BACKUP_RESTORE_CONST);
	
	$output->setNavigationMenu(array($TEXT_BACKUP_RESTORE_CONST=>'index.php?section=backup'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_BACKUP_RESTORE_CONST);
	$output->output();
}

function backups_table(){
	$files=grabFiles('/home/backup/download/');
	if(count($files)>0){
		$out='
		<table>';
		foreach ($files AS $file){
			$ext= get_extension($file);
			if($ext!='md5'){
				$md5=@join('',file('/home/backup/download/'.str_replace('tar.bz2','md5',$file)));
				$md5=trim(str_replace($file,'',$md5));
				$out.='
				<tr>
					<td><B>'.$file.'</B></td>
					<td>(md5: '.$md5.')</td>
					<td><a href="index.php?section=backup&action=restore&local=1&filepath=/home/backup/download/'.$file.'">Restore</a> <a href="fdownload.php?full=1&filepath=/home/backup/download/'.$file.'" target="_blank">Download</a></td>
				</tr>';
			}
		}
		$out.='	
		</table>';
		
		return $out;
	}else{
		return 'No backup files available.';
	}
}

function get_extension($filename){
	$filename = substr($filename,strrpos ($filename,'.')+1);
	
	return $filename;
}
?>
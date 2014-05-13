<?php

ini_set('display_errors', 1); 
error_reporting(E_ALL);

// include language files
includeLangFile('common.lang.php');
includeLangFile('voicemail.lang.php');

# Parameters
$vmcontext='default';
$fsbase='/var/spool/asterisk';
$vmbase=$fsbase.'/voicemail/'.$vmcontext;
$vminbox='INBOX';
$vmoldinbox='Old';
$generalvm='100';

function scanforvm($vmpath,&$messages, $new) {
	global $fsbase;
	$vmfiles = glob($vmpath.'/*.txt');
	foreach($vmfiles as $msg) {
		$id=basename($msg,'.txt');
		$id=(strlen($id) > 3) ? substr($id,3,strlen($id)-3) : 'err';
		$messages[$id]['new']=$new;
		$messages[$id]['filename']=$msg;
		$url=str_replace($fsbase, "", $msg);
		$url=str_replace(".txt", "", $url);
		$messages[$id]['msgurl']=$url;
	}
	if($vmfiles) return TRUE;
	else return FALSE;
}

function listvm($messages) {
	global $generalvm;
	$out='<table align="center" cellpadding="7" cellspacing="0">';
	$out.='<tr class="tablehead">
		<td align="center"></td>
		<td align="center">'.translate('TEXT_MESSAGE_ID_CONST').'</td>
		<td>'.translate('TEXT_MESSAGE_DATETIME_CONST').'</td>
		<td>'.translate('TEXT_MESSAGE_FROM_CONST').'</td>
		<td>'.translate('TEXT_MESSAGE_DURATION_CONST').'</td>
		<td>'.translate('TEXT_MESSAGE_PLAYBACK_CONST').'</td>
		<td>'.translate('TEXT_MESSAGE_COMMANDS_CONST').'</td>
	</tr>';
	$count=0;
	foreach($messages as $key => &$msg) {
		// get message details from it's .txt file
		$f = fopen($msg['filename'], "r");
		while ( $line = fgets($f, 1000) ) {
			if(strstr($line,'=')) {
				$parts=explode('=',$line);
				$msg[trim($parts[0])]=trim($parts[1]);
			}
		}
		fclose($f);
		
		//setlocale(LC_TIME, "de_DE");
		$color=(++$count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='<tr bgcolor="'.$color.'">
			<td>'.(($msg['new'])?'<img src="/'.APPDIRECTORY.'/img/new.png"  
					height="20" 
					alt="'.translate('TEXT_NEW_CONST').'" 
					title="'.translate('TEXT_NEW_CONST').'"
				>':'').'</td>
			<td>'.$key.'</td>
			<td>'.strftime("%d/%m/%Y %H:%M:%S (%A)",$msg['origtime']).'</td>
			<td>'.$msg['callerid'].'</td>
			<td align=right>'.floor($msg['duration'] / 60).':'.($msg['duration'] % 60).'</td>
			<td align=center>
				<audio if="test" controls preload="auto" autobuffer onended="MoveToOld(\''.$key.'\',\''.(($_SESSION['Extension']==$generalvm)?'&general=1':'').'\');">
					<source src="'.$msg['msgurl'].'.wav" type="audio/wav" />
					<source src="'.$msg['msgurl'].'.mp3" type="audio/mp3" />
					Unsupported browser
				</audio>
			</td>
			<td>
				<img src="/'.APPDIRECTORY.'/img/forward.png"  
					height="20" 
					alt="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'" 
					title="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'"
					onclick="javascript:self.location=\'index.php?section=voicemail&action=fwd&id='.$key.(($_SESSION['Extension']==$generalvm)?'&general=1':'').'\'"
				>
				&nbsp;&nbsp;
				<img src="/'.APPDIRECTORY.'/img/email.png" 
					height="17" 
					alt="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'" 
					title="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'"
					onclick="javascript:self.location=\'index.php?section=voicemail&action=mail&id='.$key.(($_SESSION['Extension']==$generalvm)?'&general=1':'').'\'"
				>
				&nbsp;&nbsp;
				<img src="/'.APPDIRECTORY.'/img/delete.png" 
					height="20" 
					alt="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'" 
					title="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'"
					onclick="javascript:if(confirm(\''.translate('TEXT_CONFIRM_DELETE_CONST').' '.$key.'\'))
						self.location=\'index.php?section=voicemail&action=del&id='.$key.'&new='.$msg['new'].(($_SESSION['Extension']==$generalvm)?'&general=1':'').'\'"
				>
			</td>
		</tr>';
		}
	$out.='</table>';
	return $out;
}

function getUser($dbADO,$ext) {
	$ret = $dbADO->Execute('SELECT FirstName,LastName FROM Users WHERE Extension = '.$ext);		
	$row=$ret->FetchRow();
	$user = $row['FirstName'].' '.$row['LastName'];	
	return $user;
}

function voicemail($output,$dbADO) {
	global $vmbase,$vminbox,$vmoldinbox, $generalvm;
	$out='	<script type="text/javascript">
				function MoveToOld($key, $suffix)
				{
					self.location=\'index.php?section=voicemail&action=read&id=\'+$key+$suffix;
				}
			</script>
	';
	
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

	if(isset($_REQUEST['general'])){
		if($_SESSION['AccessGeneralMailbox']==0){
			$error=translate('TEXT_NO_ACCESS_TO_GENERAL_MAILBOX_CONST');
			$_SESSION['Extension']=$_SESSION['MyExtension'];		
		}else{
			$_SESSION['Extension']=$generalvm;
		}
	}else{
		$_SESSION['Extension']=$_SESSION['MyExtension'];
	}

	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	if(isset($error))$out.='<span class="err">'.$error.'</span>';

	$out.='<div align="center" class="err">'.stripslashes(@$_REQUEST['err']).'</div>';
	$out.='<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>';
	switch ($action) { 
		case 'form':
			$out.=setLeftMenu($dbADO);
			$user=getUser($dbADO, $_SESSION['Extension']);
			$out.='<h2 align="center">';
			if($_SESSION['Extension']==$generalvm) $out.=translate('TEXT_MAILBOX_GENERAL_CONST');
			else $out.=translate('TEXT_MAILBOX_FOR_USER_CONST').' '.$user.' ('.$_SESSION['Extension'].')';
			$out.='</h2>';
		
			# find all new messages in extension directory
			$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox;
			scanforvm($vmpath, $messages, true);
					
			# find all old messages in extension directory
			$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/'.$vmoldinbox;
			scanforvm($vmpath, $messages, false);
			
			if(isset($messages)) {
				$out.=listvm($messages);
			} else {
				$out.='<div align="center" class="err">'.translate('TEXT_NO_MESSAGES_CONST').'.</div>';
			}

			break;
			
		case 'del':
			if($_REQUEST['new']) $vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/msg'.$_REQUEST['id'].'.*';
			else $vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/'.$vmoldinbox.'/msg'.$_REQUEST['id'].'.*';
			$cmd='sudo -u root rm '.$vmpath;
			$response=exec_batch_command($cmd,1);
			$suffix='&msg='.translate('TEXT_MSG_MESSAGE_DELETED_CONST');
			if($_SESSION['Extension']==$generalvm) $suffix.='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;

		case 'read':
			$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/msg'.$_REQUEST['id'].'.*';
			$vmold=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/Old';
			exec_batch_command('sudo -u root mkdir -p '.$vmold,1);
			exec_batch_command('sudo -u root chown -R asterisk:www-data '.$vmold,1);
			exec_batch_command('sudo -u root chmod 770 '.$vmold,1);
			exec_batch_command('sudo -u root chmod 770 '.$vmpath,1);
			exec_batch_command('sudo -u root mv '.$vmpath.' '.$vmold,1);
			if($_SESSION['Extension']==$generalvm) $suffix='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;

		case 'fwd':
			$suffix='&err=Not yet implemented';
			if($_SESSION['Extension']==$generalvm) $suffix.='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;

		case 'mail':
			$suffix='&err=Not yet implemented';
			if($_SESSION['Extension']==$generalvm) $suffix.='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;
	}

	$output->setMenuTitle(translate('TEXT_TELECOM_CONST').' |');
	$output->setPageTitle(translate('TEXT_VOICEMAIL_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_VOICEMAIL_CONST')=>'index.php?section=voicemail'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_VOICEMAIL_CONST'));
	$output->output();
}
?>

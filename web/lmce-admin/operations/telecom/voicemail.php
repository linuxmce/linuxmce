<?

ini_set('display_errors', 1); 
error_reporting(E_ALL);

// include language files
includeLangFile('common.lang.php');
includeLangFile('voicemail.lang.php');

function voicemail($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$vmcontext='default';
	$vmbase='/var/spool/asterisk/voicemail/'.$vmcontext;
	$vminbox='INBOX';

	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

	if(isset($_REQUEST['general'])){
		if($_SESSION['AccessGeneralMailbox']==0){
			$error=translate('TEXT_NO_ACCESS_TO_GENERAL_MAILBOX_CONST');
			$_SESSION['Extension']=$_SESSION['MyExtension'];		
		}else{
			$_SESSION['Extension']=100;
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
			$out.='<h3 align="center">'.translate('TEXT_MAILBOX_FOR_EXTENSION_CONST').' '.$_SESSION['Extension'].'</h3>';
	
			# find all messages in extension directory
			$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox;
			$vmfiles = glob($vmpath.'/*.txt');
			foreach($vmfiles as $msg) {
				$id=basename($msg,'.txt');
				$id=(strlen($id) > 3) ? substr($id,3,strlen($id)-3) : 'err';
				$messages[$id]['filename']=$msg;
				$messages[$id]['wavurl']='/voicemail/'.$vmcontext.'/'.$_SESSION['Extension'].'/'.$vminbox.'/'.basename($msg,'.txt').'.WAV';
			}
			
			$out.='<table align="center" cellpadding="7" cellspacing="0">';
			$out.='<tr class="tablehead">
				<td>'.translate('TEXT_MESSAGE_ID_CONST').'</td>
				<td>'.translate('TEXT_MESSAGE_DATETIME_CONST').'</td>
				<td>'.translate('TEXT_MESSAGE_FROM_CONST').'</td>
				<td>'.translate('TEXT_MESSAGE_DURATION_CONST').'</td>
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
					<td>'.$key.'</td>
					<td>'.strftime("%d/%m/%Y %H:%M:%S (%A)",$msg['origtime']).'</td>
					<td>'.$msg['callerid'].'</td>
					<td align=right>'.floor($msg['duration'] / 60).':'.($msg['duration'] % 60).'</td>
					<td align=center>
						<audio src="'.$msg['wavurl'].'" preload="auto" autobuffer controls>
							Unsupported browser
						</audio>
						&nbsp;&nbsp;
						<img src="/'.APPDIRECTORY.'/img/forward.png"  
							height="20" 
							alt="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'" 
							title="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'"
							onmouseover="this.style.cursor=\'pointer\';"
							onclick="javascript:self.location=\'index.php?section=voicemail&action=fwd&id='.$key.(($_SESSION['Extension']=='100')?'&general=1':'').'\'"
						>
						&nbsp;&nbsp;
						<img src="/'.APPDIRECTORY.'/img/email.png" 
							height="17" 
							alt="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'" 
							title="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'"
							onmouseover="this.style.cursor=\'pointer\';"
 							onclick="javascript:self.location=\'index.php?section=voicemail&action=mail&id='.$key.(($_SESSION['Extension']=='100')?'&general=1':'').'\'"
						>
						&nbsp;&nbsp;
						<img src="/'.APPDIRECTORY.'/img/delete.png" 
							height="20" 
							alt="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'" 
							title="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'"
							onmouseover="this.style.cursor=\'pointer\';"
							onclick="javascript:if(confirm(\''.translate('TEXT_CONFIRM_DELETE_CONST').' '.$key.'\'))self.location=\'index.php?section=voicemail&action=del&id='.$key.(($_SESSION['Extension']=='100')?'&general=1':'').'\'"
						>
					</td>
				</tr>';
				}
			$out.='</table>';
			break;
			
		case 'del':
			$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox.'/msg'.$_REQUEST['id'].'.*';
			$cmd='sudo -u root rm '.$vmpath;
			$response=exec_batch_command($cmd,1);
			$suffix='&msg='.translate('TEXT_MSG_MESSAGE_DELETED_CONST');
			if($_SESSION['Extension']=='100') $suffix.='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;

		case 'fwd':
			$suffix='&err=Not yet implemented';
			if($_SESSION['Extension']=='100') $suffix.='&general=1';
			header('Location: index.php?section=voicemail'.@$suffix);
			break;

		case 'mail':
			$suffix='&err=Not yet implemented';
			if($_SESSION['Extension']=='100') $suffix.='&general=1';
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

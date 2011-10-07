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
		}else{
			$_SESSION['Extension']=100;
		}
	}else{
		$_SESSION['Extension']=$_SESSION['MyExtension'];
	}

	//exec_batch_command('sudo -u root /usr/pluto/bin/Asterisk_FixVoicemailRights.sh');
	
	if(isset($error)){
		$out.='<span class="err">'.$error.'</span>';
	} else {
		$out.=setLeftMenu($dbADO);
		$out.='<h3 align="center">'.translate('TEXT_MAILBOX_FOR_EXTENSION_CONST').' '.$_SESSION['Extension'].'</h3>';

		$vmpath=$vmbase.'/'.$_SESSION['Extension'].'/'.$vminbox;
		$vmfiles = glob($vmpath.'/*.txt');
		
		foreach($vmfiles as $msg) {
			$id=basename($msg,'.txt');
			$id=(strlen($id) > 3) ? substr($id,3,strlen($id)-3) : 'err';
			$messages[$id]['filename']=$msg;
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
					<audio src="/lmce-admin/operations/telecom/default/301/INBOX/'.basename($msg['filename'],'txt').'WAV" preload="auto" autobuffer controls>Unsupported browser</audio>
					&nbsp;&nbsp;&nbsp;
					<img src="/'.APPDIRECTORY.'/img/forward.png"  height="20" alt="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'" title="'.translate('TEXT_MESSAGE_COMMAND_FORWARD_CONST').'">
					&nbsp;&nbsp;&nbsp;
					<img src="/'.APPDIRECTORY.'/img/email.png" height="17" alt="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'" title="'.translate('TEXT_MESSAGE_COMMAND_EMAIL_CONST').'">
					&nbsp;&nbsp;&nbsp;
					<img src="/'.APPDIRECTORY.'/img/delete.png" height="20" alt="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'" title="'.translate('TEXT_MESSAGE_COMMAND_DELETE_CONST').'">
				</td>
			</tr>';
			}
		$out.='</table>';
		//$sql='SELECT * from ';
		//$resUsers = $dbADO->Execute($queryUsers,array($installationID));
		//echo "<EMBED SRC=\"$wave_file.wav\" HIDDEN=\"TRUE\" AUTOSTART=\"TRUE\"></EMBED>";

	}

	$output->setMenuTitle(translate('TEXT_TELECOM_CONST').' |');
	$output->setPageTitle(translate('TEXT_VOICEMAIL_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_VOICEMAIL_CONST')=>'index.php?section=voicemail'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_VOICEMAIL_CONST'));
	$output->output();
}
?>

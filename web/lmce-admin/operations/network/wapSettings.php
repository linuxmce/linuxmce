<?
function wapSettings($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/wapSettings.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */

	$settingsFile=$GLOBALS['wap.conf'];
	if(!file_exists($settingsFile)){
		$_REQUEST['error']='ERROR: The file '.$settingsFile.' does not exist.';
	}else{
		$fileArr=file($settingsFile);
		if($fileArr===false){
			$_REQUEST['error']='ERROR: The file '.$settingsFile.' could not be opened.';
		}else{
			$wapURL=trim(implode('',$fileArr));
		}
	}
	
	if(!file_exists($GLOBALS['pluto_callerid.conf'])){
		$_REQUEST['error']='ERROR: The file '.$GLOBALS['pluto_callerid.conf'].' does not exist.';
	}else{
		$fileArr=file($GLOBALS['pluto_callerid.conf']);
		if($fileArr===false){
			$_REQUEST['error']='ERROR: The file '.$GLOBALS['pluto_callerid.conf'].' could not be opened.';
		}else{
			$caller_id_number=trim(implode('',$fileArr));
		}
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = cleanInteger($_SESSION['installationID']);
	
	
	if ($action=='form') {
		if(!isset($wapURL) || $wapURL==''){
			$coreFields=getFieldsAsArray('Device','IPAddress',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Installation='.$installationID.' AND FK_DeviceCategory='.$GLOBALS['CategoryCore']);
			$wapURL='http://'.$coreFields['IPAddress'][0].'/lmce-admin/check.wml';
		}

		$out.=setLeftMenu($dbADO).'
		
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div class="err"><B>'.@$_REQUEST['error'].'</B></div>
	<form action="index.php" method="post" name="wapSettings" onSubmit="return validateInput();">
	<input type="hidden" name="section" value="wapSettings">
	<input type="hidden" name="action" value="add">
		
	<p><B>'.$TEXT_INSTRUCTIONS_CONST.':</B>	
	'.$TEXT_INSTRUCTIONS_DETAILS_CONST;
		
$code='
<?php
Header( "Content-type: text/vnd.wap.wml");
$Message=queryServer($_SERVER[\'QUERY_STRING\'],\'http://<your_core_ip_address>/lmce-admin/check.wml\');

print $Message;

function queryServer($params,$url)
{
$defined_vars = get_defined_vars();
$user_agent = @$defined_vars[\'HTTP_USER_AGENT\'];

$ch = curl_init();
$serverAddress=($_SERVER[\'QUERY_STRING\']!=\'\')?$url.\'?\'.$_SERVER[\'QUERY_STRING\']:$url;

curl_setopt($ch, CURLOPT_URL, $serverAddress);
curl_setopt($ch, CURLOPT_HEADER, 0);
curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);

$result=curl_exec ($ch);
curl_close ($ch);

return $result;
}
?>';
$out.='<br>'.highlight_string($code,true).'<br>';
$out.=
$TEXT_WAP_NOTES_CONST.'
	<table>
		<tr>
			<td>'.$TEXT_ENTER_WAP_GPRS_URL_CONST.'</td>
			<td><input type="text" name="url" value="'.@$wapURL.'" size="50"></td>
		</tr>
		<tr>
			<td>'.$TEXT_CALLER_ID_NUMBER_CONST.'</td>
			<td><input type="text" name="caller_id_number" value="'.@$caller_id_number.'" size="50"></td>
		</tr>
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
		</tr>
	</table>
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=wapSettings&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}
		
		if(isset($_POST['save'])){
			$url=$_POST['url'];
			$caller_id_number=$_POST['caller_id_number'];
			
			// write url to wap settings file
			$handle = @fopen($settingsFile, 'w');
			if (!@fwrite($handle, $url)) {
				header("Location: index.php?section=wapSettings&error=$TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST ".$settingsFile);
				exit();
			}
			fclose($handle);

			// write callerID number to pluto_callerID.conf file
			$handle = @fopen($GLOBALS['pluto_callerid.conf'], 'w');
			if (!@fwrite($handle, $caller_id_number)) {
				header("Location: index.php?section=wapSettings&error=$TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST ".$GLOBALS['pluto_callerid.conf']);
				exit();
			}
			fclose($handle);
						
			header("Location: index.php?section=wapSettings&msg=$TEXT_WAP_GPRS_SETTINGS_UPDATED_CONST");
			exit();
		}

		header("Location: index.php?section=wapSettings");
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_WAP_GPRS_SETTINGS_CONST);
	$output->setNavigationMenu(array($TEXT_WAP_GPRS_SETTINGS_CONST=>'index.php?section=wapSettings'));		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_WAP_GPRS_SETTINGS_CONST);
	$output->output();  		
}
?>

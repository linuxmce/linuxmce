<?php
function transmissiondaemon($output,$dbADO,$transmissionADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;

        //Obtain the Transmission download Directory
        exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getSettings download-dir', $Transmission_Root);
        $Transmission_Root=$Transmission_Root[0];

	$out='';

	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):$Transmission_Root;
	$CLpage=((int)@$_REQUEST['CLpage']>0)?(int)$_REQUEST['CLpage']:1;
	$VLpage=((int)@$_REQUEST['VLpage']>0)?(int)$_REQUEST['VLpage']:1;
	$transmission_files_per_page = (isset($_REQUEST['files_per_page']) && $_REQUEST['files_per_page']!='')?cleanString($_REQUEST['files_per_page']):'20';

	$out='
		<form action="index.php" method="POST" name="transmission" id="transmission">
		<input type="hidden" name="section" value="transmission-daemon">
		<input type="hidden" name="action" id="action" value="'.$action.'">
		<input type="hidden" name="Transmission_Root" id="Transmission_Root" value="'.$Transmission_Root.'">
		<input type="hidden" name="dirSep" id="dirSep" value="' . DIRECTORY_SEPARATOR . '">
		<input type="hidden" name="parentdir" id="parentdir" value="'.$Transmission_Root.'">
		<input type="hidden" name="path" id="path" value="'.$path.'">
		<input type="hidden" name="CLpage" id="CLpage" value="'.$CLpage.'">
		<input type="hidden" name="VLpage" id="VLpage" value="'.$VLpage.'">
		<input type="hidden" name="files_per_page" id="files_per_page" value="'.$transmission_files_per_page.'">

		<table cellpadding="3" align="center">
		<tr bgcolor="#EEEEEE">
		<td><input type="submit" class="button" name="transmission-daemon" id="transmission-daemon" value="'.$TEXT_TRANSMISSIONDAEMON.'" onclick="document.getElementById(\'action\').value=\'transmission\';"></td>
		<td><input type="submit" class="button" name="create_links" id="create_links" value="'.$TEXT_CREATE_LINKS.'" onclick="document.getElementById(\'action\').value=\'create_links\';"><td>
		<td><input type="submit" class="button" name="validate_links" id="validate_links" value="'.$TEXT_VALIDATE_LINKS.'" onclick="document.getElementById(\'action\').value=\'validate_links\';"></td>
		<td><input type="submit" class="button" name="manage" id="manage" value="'.$TEXT_MANAGETRANSMISSION.'" onclick="document.getElementById(\'action\').value=\'manage\';"></td>
		</tr>
		</table>
		';

	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);

	if (!$canModifyInstallation){
		$out='';
		header("Location: index.php?section=transmission-daemon&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
		exit();
	}

	switch($action){
		case 'transmission-daemon' :
		default :
			if ( $_SERVER['HTTPS']=="on" ){
				$out.='<iframe src="https://'.$_SERVER['HTTP_HOST'].'/transmission/web" style="width:98%;height:600px;" scrolling="yes"></iframe>';
			}else{
				$out.='<iframe src="http://'.$_SERVER['HTTP_HOST'].':9091" style="width:98%;height:600px;" scrolling="yes"></iframe>';
			};

			break;

		case 'create_links' :
			include('transmissionCreatelinks.php');
			$out.=transmissionCreatelinks($path, $Transmission_Root, $transmissionADO, $CLpage, $transmission_files_per_page);
			break;

		case 'validate_links' :
			include('transmissionValidatelinks.php');
			$out.=transmissionValidatelinks($transmissionADO, $VLpage, $transmission_files_per_page);
			break;

		case 'manage' :
			include('transmissionManage.php');
			$out.=transmissionManage();
			break;
	};

	$out.='</form>';

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_TRANSMISSIONSERVER_CONST);
	$output->setNavigationMenu(array($TEXT_TRANSMISSIONSERVER_CONST=>'index.php?section=transmission-daemon'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_TRANSMISSIONSERVER_CONST);
	$output->output();
}
?>

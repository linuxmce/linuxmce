<?
function setupDisklessMD($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/setupDisklessMD.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='';

	if ($action == 'form') {
		
		$out= '
		<input type="hidden" name="action" value="add">	
		<h3>'.$TEXT_SETUP_DISKLESS_MEDIA_DIRECTOR_CONST.'</h3>
		
		<iframe src="operations/logs/executeLog.php?script=1" width="950" height="600"></iframe>
		
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setupDisklessMD&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		header("Location: index.php?section=setupDisklessMD");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SETUP_DISKLESS_MEDIA_DIRECTOR_CONST);
	$output->output();
}
?>
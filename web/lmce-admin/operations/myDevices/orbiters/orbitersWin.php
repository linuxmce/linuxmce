<?php
function orbitersWin($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/orbitersWin.lang.php');
	
	global $dbPlutoMainDatabase,$wikiHost;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<form action="index.php" method="POST" name="devices">
	<input type="hidden" name="section" value="orbitersWin">
	<input type="hidden" name="action" value="add">	
	<br>
	<a href="index.php?section=login" target="_top">'.$TEXT_BACK_CONST.'</a>
	<table>
		<tr>
			<td><B>'.$TEXT_PROCESSOR_CONST.'</B></td>
			<td><B>'.$TEXT_OPERATING_SYSTEM_CONST.'</B></td>
			<td>&nbsp;</td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Intel/AMD x86 (normal)</td>
			<td>Windows 98/ME/XP</td>
			<td><a href="fdownload.php?filepath=installers/OrbiterInstaller.msi" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr>
			<td>XScale</td>
			<td>Windows CE.NET</td>
			<td><a href="fdownload.php?filepath=installers/Orbiter_CeNet4_XScale.CAB" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Intel x86</td>
			<td> Windows CE.NET</td>
			<td><a href="fdownload.php?filepath=installers/Orbiter_CeNet4_x86.CAB" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr>
			<td>Android</td>
			<td>Orbiter For Android Qt5</td>
			<td><a href="fdownload.php?filepath=installers/linuxmce-qorbiter-qt5-debug.apk" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr>
			<td>Touch Orbiter for Android</td>
			<td>Android</td>
			<td><a href="http://deb.linuxmce.org/LinuxMCE-Orbiter.apk"></a></td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Nokia n900</td>
			<td>Maemo Fremantle</td>
			<td><a href="fdownload.php?filepath=installers/fremantle.install" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr>
			<td>Import Outlook Contacts</td>
			<td>Windows 98/ME/XP</td>
			<td><a href="fdownload.php?filepath=installers/ImportContacts.zip" target="_blank">('.$TEXT_DOWNLOAD_CONST.')</a></td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Outlook sync utility</td>
			<td>Windows XP</td>
			<td><a href="fdownload.php?filepath=installers/OutlookSync.msi" target="_blank">(download)</a></td>
		</tr>				
		<tr>
			<td>Pluto Backup & Share (see <a href="'.$wikiHost.'index.php/PlutoBaS">Wiki help</a>)</td>
			<td>Windows XP</td>
			<td><a href="fdownload.php?filepath=installers/PlutoBaSInstaller.msi" target="_blank">(download)</a></td>
		</tr>		
		<tr bgcolor="#F0F3F8">
			<td>Pluto Switcher</td>
			<td>Windows XP</td>
			<td><a href="fdownload.php?filepath=installers/PlutoRebootSetup.msi" target="_blank">(download)</a></td>
		</tr>		
	</table>
		
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		header("Location: index.php?section=orbitersWin");		
	}
	
	$output->setMenuTitle($TEXT_DOWNLOAD_WINDOWS_SOFTWARE_FOR_ORBITERS_CONST);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DOWNLOAD_WINDOWS_SOFTWARE_FOR_ORBITERS_CONST);
	$output->output();
}

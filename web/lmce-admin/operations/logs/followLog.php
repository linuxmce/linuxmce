<?
function followLog($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/logs.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID=(int)@$_REQUEST['deviceID'];
	$installationID=(int)@$_SESSION['installationID'];
	if($deviceID==0){
		die($TEXT_INVALID_DEVICE_ID_SPECIFIED_CONST);
	}

	if ($action == 'form') {
		$selectDevice='
			SELECT Device.Description,PK_Device,DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory,FK_DeviceTemplate,CommandLine
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			WHERE PK_Device=?';
		$resDevice=$dbADO->Execute($selectDevice,$deviceID);
		$rowDevice=$resDevice->FetchRow();
		$isGSD=($rowDevice['CommandLine']=='Generic_Serial_Device')?1:0;
		
		$logName=(isset($_REQUEST['system_log']))?'/var/log/syslog':getLogName($deviceID,$rowDevice['FK_DeviceTemplate'],$rowDevice['Template'],(int)@$_REQUEST['parentID'],(int)@$_REQUEST['orbiter'],$isGSD,$rowDevice['CommandLine']);;
		
		if(file_exists($logName)){
			$logBody='<iframe src="operations/logs/taillog.php?log='.$logName.'" width="1005" height="600"></iframe>';
		}else{
			$logBody='<div style="background:black;color:white;">'.$TEXT_ERROR_LOG_NOT_FOUND_CONST.': '.$logName.'</div>';
		}		
		
		$out.='
		<div class="err">'.strip_tags(@$_GET['error']).'</div>
		<div class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>
		<form action="index.php" method="POST" name="followLog" enctype="multipart/form-data">
		<input type="hidden" name="section" value="followLog">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="action" value="add">	
		
		<h3>'.$TEXT_FOLLOW_LOG_CONST.'</h3>
			<table width="100%">
				<tr>
					<td valign="top" colspan="3">Device <B>'.$rowDevice['Description'].'</B>, # <B>'.$rowDevice['PK_Device'].'</B> <td>
				</tr>
				<tr>
					<td valign="top" colspan="3">Device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B> and manufacturer <B>'.$rowDevice['Manufacturer'].'</B>.<td>
				</tr>
				<tr>
					<td valign="top" colspan="3">&nbsp;<td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" style="background:black;color:white;">'.$logBody.'<td>
				</tr>		
			</table>
			</form>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=followLog&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		header("Location: index.php?section=followLog&deviceID=$deviceID");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_FOLLOW_LOG_CONST);
	$output->output();
}
?>
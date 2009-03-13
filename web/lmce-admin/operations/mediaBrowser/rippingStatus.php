<?
function rippingStatus($output,$dbADO,$mediadbADO) {
	global $mediaTypes;
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/rippingStatus.lang.php');
	global $dbPlutoMainDatabase,$dbPlutoMediaDatabase;
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$statusMapping=array(
		'p' => "in progress",
		'e' => "error",
		's' => "success",
		'a' => "aborted",
		'b' => "completed with errors",
		'' => 'unknown status'
	);

	if($action=='form'){
		
		$out.='
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="rippingStatus">
			<input type="hidden" name="section" value="rippingStatus">
			<input type="hidden" name="action" value="update">
	';

	$res=$mediadbADO->Execute('
		SELECT RipStatus.*,Device.Description,File.Filename, File.Path,File.PK_File,RipStatus.File AS RipFile 
		FROM '.$dbPlutoMediaDatabase.'.RipStatus 
		LEFT JOIN '.$dbPlutoMainDatabase.'.Device ON RipStatus.EK_Device=PK_Device 
		LEFT JOIN File ON RipStatus.FK_File=PK_File
		ORDER BY PK_RipStatus DESC');
	$out.='
	<a href="index.php?section=rippingStatus&action=purge">'.$TEXT_PURGE_ALL_CONST.'</a>
	<table>
		<tr class="tablehead">
			<td>'.$TEXT_DEVICE_CONST.'</td>
			<td>'.$TEXT_SLOT_CONST.'</td>
			<td>'.$TEXT_DISC_CONST.'</td>
			<td>'.$TEXT_FILE_CONST.'</td>
			<td>'.$TEXT_TYPE_CONST.'</td>
			<td>'.$TEXT_RIP_JOB_CONST.'</td>
			<td>'.$TEXT_DATETIME_CONST.'</td>
			<td>'.$TEXT_STATUS_CONST.'</td>
			<td>'.$TEXT_MESSAGE_CONST.'</td>
		</tr>';
	$pos=0;
	while($row=$res->FetchRow()){
		$pos++;
		$class=($pos%2==0)?'alternate_back':'';
		$color='';
		$color=($row['Status']=='e')?'#FFBFBF':$color;
		$color=($row['Status']=='s')?'#D8FF9F':$color;

		$out.='
		<tr bgcolor="'.$color.'">
			<td align="center"><a href="index.php?section=editDeviceParams&deviceID='.$row['EK_Device'].'">'.$row['Description'].'</a></td>
			<td align="center">'.$row['Slot'].'</td>
			<td align="center"><a href="index.php?section=discAttributes&discID='.$row['FK_Disc'].'">'.$row['FK_Disc'].'</a></td>
			<td align="center">'.((is_null($row['FK_File']))?$row['RipFile']:'<a href="index.php?section=editMediaFile&fileID='.$row['PK_File'].'">'.$row['Path'].'/'.$row['Filename'].'</a>').'</td>
			<td align="center">'.$row['Type'].'</td>
			<td align="center">'.$row['RipJob'].'</td>
			<td align="center">'.$row['DateTime'].'</td>
			<td align="center">'.$statusMapping[$row['Status']].'</td>
			<td align="center">'.$row['Message'].'</td>
		</tr>';
	}
	$out.='</table>';

	
	$out.='		
		</form>';
	}else{
		if($action=='purge'){
			$mediadbADO->Execute('DELETE FROM RipStatus');
			
			header('Location: index.php?section=rippingStatus&msg='.$TEXT_RIP_STATUS_PURGED_CONST);
			exit();			
		}
		
		header('Location: index.php?section=rippingStatus&msg='.$TEXT_UPDATED_CONST);
		exit();
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_RIPPING_STATUS_CONST);
	$output->setNavigationMenu(array($TEXT_RIPPING_STATUS_CONST=>'index.php?section=rippingStatus'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_RIPPING_STATUS_CONST);
	$output->output();
}
?>

<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');

// get mythconverg  database settings
exec('cat /etc/mythtv/mysql.txt | grep -v -E "^#|^$" ',$retArray);
foreach ($retArray as $dbSetting){
	parse_str($dbSetting);
}

$mdDSN = 'mysql://'.urlencode($DBUserName).':'.urlencode($DBPassword).'@'.$DBHostName.'/'.urlencode($DBName); 
$mdADO = &ADONewConnection($mdDSN);

if(isset($_POST['save'])){
	$mdID=(int)$_POST['mdID'];
	$inputsArray=explode(',',$_POST['inputsArray']);
	foreach ($inputsArray AS $inputID){
		$device=(int)@$_POST['device_'.$inputID];
		if(isset($_POST['external_'.$inputID]) && $device!=0){
			// TODO: the external command
			$extCommand='/usr/pluto/bin/TuneToChannel.sh '.$device;
			$mdADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array($extCommand,$inputID));
		}else{
			$mdADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array('',$inputID));
		}
	}
	$installationID=(int)$_REQUEST['installationID'];
	header('Location: mediaDirector.php?mdID='.$mdID.'&installationID='.$installationID);
	exit();
}

$cssFile='<link rel="stylesheet" type="text/css" media="all" href="include/styles/main.css" />';
$out='
<html>
<head>
<title>Media Directors</title>
'.$cssFile.'
<script>
function enableDevice(val)
{
	eval("if(document.mdForm.external_"+val+".checked)document.mdForm.device_"+val+".disabled=false;else document.mdForm.device_"+val+".disabled=true;");
}
</script>
</head>
<body>
';
if(!isset($_REQUEST['orbID']) && !isset($_REQUEST['mdID'])){
	print $cssFile.'<div class="err">No orbiter ID or media director ID specified. Call this page with <B>?orbID=X</B> or <B>?mdID=X</B></div>';
	exit();
}

$orbiterID=(int)$_REQUEST['orbID'];
// TODO: remove test value
//$orbiterID=594;
if(!isset($_REQUEST['mdID'])){
	$resOrbiter=$dbADO->Execute('SELECT FK_Installation,FK_Device_ControlledVia FROM Device WHERE PK_Device=?',$orbiterID);
	if($resOrbiter->RecordCount()==0){
		print $cssFile.'<div class="err">Cannot retrieve installation for selected orbiter.</div>';
		exit();
	}else{
		$rowOrbiter=$resOrbiter->FetchRow();
		$installationID=$rowOrbiter['FK_Installation'];
		$selectedMD=$rowOrbiter['FK_Device_ControlledVia'];
	}
}else{
	$selectedMD=(int)$_REQUEST['mdID'];
	$installationID=(int)$_REQUEST['installationID'];
}

$DTArray=getDeviceTemplatesFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
if(count($DTArray)==0){
	print $cssFile.'<div class="err">No media directors in pluto_main database.</div>';
	exit();
}else{
	$out.='
	<form action="mediaDirector.php" method="POST" name="mdForm">
		<input type="hidden" name="orbID" value="'.$orbiterID.'">
		<input type="hidden" name="installationID" value="'.$installationID.'">
	Select media director: <select name="mdID" onChange="document.mdForm.submit();">';
	$queryDevice='
		SELECT 
			Device.* FROM Device 
		WHERE
			FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=?';	
	$resDevice=$dbADO->Execute($queryDevice,$installationID);
	while($rowD=$resDevice->FetchRow()){
		$out.='<option value="'.$rowD['PK_Device'].'" '.(($rowD['PK_Device']==$selectedMD)?'selected':'').'>'.$rowD['Description'].'</option>';
	}
	$out.='</select> <input type="submit" class="button" name="submit" value="GO"><br>';
}

$resMD=$dbADO->Execute('SELECT * FROM Device WHERE PK_Device=?',$selectedMD);
$rowMD=$resMD->FetchRow();
$roomID=$rowMD['FK_Room'];

$devicesInRoom=array();
$AV_DTArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
if(count($AV_DTArray)==0)
	$AV_DTArray[]=0;
$resDevices=$dbADO->Execute('SELECT Device.* FROM Device WHERE FK_Room=? AND FK_DeviceTemplate IN ('.join(',',$AV_DTArray).') ORDER BY Description ASC',$roomID);
while($rowDevices=$resDevices->FetchRow()){
	$devicesInRoom[$rowDevices['PK_Device']]=$rowDevices['Description'];
}

$resHost=$mdADO->Execute('SELECT hostname FROM settings WHERE value=? AND data=?',array('BackendServerIP',$rowMD['IPaddress']));
if($resHost->RecordCount()==0){
	print $cssFile.'<div class="err">Cannot retrieve host for curent Media Director.</div>';
	exit();
}else{
	$rowHost=$resHost->FetchRow();
	$host=$rowHost['hostname'];
}

$out.='Selected Media director IP: <B>'.$rowMD['IPaddress'].'</B>, host: <B>'.$host.'</B>';

$resCaptureCard=$mdADO->Execute('SELECT * FROM capturecard WHERE hostname=?',$host);
$out.='<table>
	<tr>
		<td><B>Capture Card</B></td>
		<td align="center"><B>Input</B></td>
	</tr>';
$inputsArray=array();
$pos=0;
while($rowCaptureCard=$resCaptureCard->FetchRow()){
	$pos++;
	$resInputs=$mdADO->Execute('
		SELECT cardinput.* 
		FROM cardinput
		INNER JOIN capturecard ON cardinput.cardid=capturecard.cardid
		WHERE cardinput.cardid=?',$rowCaptureCard['cardid']);
	$out.='
	<tr bgcolor="'.(($pos%2==1)?'#EEEEEE':'').'">
		<td>'.$rowCaptureCard['defaultinput'].'</td>
		<td><table>';
		while($rowInputs=$resInputs->FetchRow()){
			$inputsArray[]=$rowInputs['cardinputid'];
			$pulldownEnabled=((ereg('/usr/pluto/bin',$rowInputs['externalcommand']))?'':'disabled');
			$out.='
				<tr>
					<td>'.$rowInputs['inputname'].'</td>
					<td><input type="checkbox" name="external_'.$rowInputs['cardinputid'].'" value="1" '.((ereg('/usr/pluto/bin',$rowInputs['externalcommand']))?'checked':'').' onClick="enableDevice(\''.$rowInputs['cardinputid'].'\')"> External</td>
					<td>'.pulldown('device_'.$rowInputs['cardinputid'],$devicesInRoom,$rowInputs['externalcommand'],$pulldownEnabled).'</td>
				</tr>';
		}
		$out.='</table>
		</td>
	</tr>';
}
$out.='
	<input type="hidden" name="inputsArray" value="'.join(',',$inputsArray).'">
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Update"></td>
	</tr>
</table>';
$out.='
</form>
</body>
</html>';


print $out;

function pulldown($name,$devicesInRoom,$externalCommand,$pulldownEnabled)
{
	$commandParts=explode(' ',$externalCommand);
	$selectedVal=(int)$commandParts[count($commandParts)-1];

	$retVal='<SELECT name="'.$name.'" '.$pulldownEnabled.'>
		<option value="0">- Please select -</option>';
	foreach ($devicesInRoom AS $deviceID=>$description){
		$retVal.='<option value="'.$deviceID.'" '.((isset($selectedVal) && $selectedVal==$deviceID)?'selected':'').'>'.$description.'</option>';
	}
	$retVal.='
	</SELECT>';
	return $retVal;
}
?>
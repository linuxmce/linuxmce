<?
function eibDevices($output,$dbADO,$eibADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('eibDevices.lang.php');
	
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $eibADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$type=$_REQUEST['type'];
	
	switch ($type){
		case 'lights':
			$allowedTemplates=array($GLOBALS['LightSwitchOnOff']=>'Light Switch On/Off',$GLOBALS['LightSwitchDimmable']=>'Light Switch Dimmable');
			$targetDeviceData=$GLOBALS['Channel'];
			$labels=array(translate('TEXT_KNX_ONOFF'),translate('TEXT_KNX_STATUS'),translate('TEXT_KNX_DIM'));
		break;
		case 'security':
			$resDT=$dbADO->Execute('SELECT DT.PK_DeviceTemplate,DT.Description FROM DeviceTemplate AS DT,DeviceTemplate_DeviceCategory_ControlledVia as CV WHERE DT.PK_DeviceTemplate= CV.FK_DeviceTemplate AND CV.FK_DeviceCategory=? AND DT.FK_DeviceCategory=? ORDER BY DT.Description ASC',array($GLOBALS['specialized'],$GLOBALS['rootSecurity']));
			$allowedTemplates=array();
			while($rowDT=$resDT->FetchRow()){
				$allowedTemplates[$rowDT['PK_DeviceTemplate']]=$rowDT['Description'];
			}
			$targetDeviceData=$GLOBALS['Channel'];
			$labels=array(translate('TEXT_KNX_ONOFF'));
		break;
		case 'climate':
			$resDT=$dbADO->Execute('SELECT DT.PK_DeviceTemplate,DT.Description FROM DeviceTemplate AS DT,DeviceTemplate_DeviceCategory_ControlledVia as CV WHERE DT.PK_DeviceTemplate= CV.FK_DeviceTemplate AND CV.FK_DeviceCategory=? AND DT.FK_DeviceCategory=? ORDER BY DT.Description ASC',array($GLOBALS['specialized'],$GLOBALS['rootClimate']));
			$allowedTemplates=array();
			while($rowDT=$resDT->FetchRow()){
				$allowedTemplates[$rowDT['PK_DeviceTemplate']]=$rowDT['Description'];
			}
			$targetDeviceData=$GLOBALS['Channel'];
			$labels=array(translate('TEXT_KNX_ONOFF'));
		break;
		case 'drapes':
			$allowedTemplates=array($GLOBALS['DrapesSwitch']=>'Drapes Switch');
			$targetDeviceData=$GLOBALS['Channel'];
			$labels=array('MOVE','STEP');
		break;
		case 'thermostats':
			$allowedTemplates=array($GLOBALS['StandardThermostat']=>'Standard Thermostat');
			$targetDeviceData=$GLOBALS['Channel'];
			$labels=array('SetPoint Temperature','Actual Temperature');
		break;
		default:
			$out='<div class="err">'.translate('TEXT_EIB_INVALID_PARAMETER_CONST').'</div>';
			$output->setBody($out);
			$output->output();
		break;
	}
	$multiGroupAddress=array($GLOBALS['LightSwitchOnOff'],$GLOBALS['LightSwitchDimmable'],$GLOBALS['DrapesSwitch'],$GLOBALS['StandardThermostat']);
	
	$channelArray=array();
	$resGroupAddresses=$eibADO->Execute('SELECT * FROM groupaddresses ORDER BY name ASC');
	while($rowGA=$resGroupAddresses->FetchRow()){
		$channelArray[$rowGA['address']]=$rowGA['name'];
	}
	
	if ($action == 'form') {
		$out.='
	<script>
		var groupNameToSet="";
		var groupValueToSet="";
		
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}
		
		function enableSecond()
		{
			if(document.eibDevices.deviceTemplate.value=='.$GLOBALS['LightSwitchDimmable'].' || document.eibDevices.deviceTemplate.value=='.$GLOBALS['DrapesSwitch'].' || document.eibDevices.deviceTemplate.value=='.$GLOBALS['StandardThermostat'].'){
				document.eibDevices.setNewDim.disabled=false;
			}else
				document.eibDevices.setNewDim.disabled=true;
		}
		
		function pickGroupAddress(groupName,groupAddress)
		{
			document.getElementById("addressPicker").style.display="";
			document.getElementById("addressPicker").style.top=yMousePos;
			document.getElementById("addressPicker").style.left=xMousePos-200;
			eval("selectedValue=document.eibDevices."+groupAddress+".value");
			for(i=0;i<document.eibDevices.generic.length;i++){
				if(document.eibDevices.generic[i].value==selectedValue){
					document.eibDevices.generic[i].selected=true;
					break;
				}
			}
			groupNameToSet=groupName;
			groupValueToSet=groupAddress;
		}

		function setGroupValue()
		{
			if(groupNameToSet!="" && groupValueToSet!=""){
				eval("document.eibDevices."+groupValueToSet+".value=\'"+document.eibDevices.generic[document.eibDevices.generic.selectedIndex].value+"\'");
				eval("document.eibDevices."+groupNameToSet+".value=\'"+document.eibDevices.generic[document.eibDevices.generic.selectedIndex].text+"\'");
			}
			document.getElementById("addressPicker").style.display="none";
		}
		
		if (document.layers) { // Netscape
		    document.captureEvents(Event.MOUSEMOVE);
		    document.onmousemove = captureMousePosition;
		} else if (document.all) { // Internet Explorer
		    document.onmousemove = captureMousePosition;
		} else if (document.getElementById) { // Netcsape 6
		    document.onmousemove = captureMousePosition;
		}
		
		xMousePos = 0; // Horizontal position of the mouse on the screen
		yMousePos = 0; // Vertical position of the mouse on the screen

		function captureMousePosition(e) {
		    if (document.layers) {
		        xMousePos = e.pageX;
		        yMousePos = e.pageY;
		    } else if (document.all) {
		        xMousePos = window.event.x+document.body.scrollLeft;
		        yMousePos = window.event.y+document.body.scrollTop;
		    } else if (document.getElementById) {
		        xMousePos = e.pageX;
		        yMousePos = e.pageY;
		    }
		}


	</script>
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="eibDevices" enctype="multipart/form-data">
	<input type="hidden" name="section" value="eibDevices">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="type" value="'.$type	.'">	
		
	<table align="center" cellpadding="3" cellspacing="0" border="0">
		<tr>
			<td colspan="3" align="center"><table align="center" border="0">
				<tr>
					<td><B>'.translate('TEXT_UPDATE_GROUP_ADDRESSES_CONST').'</B></td>
					<td><input type="file" name="newFile"> <input type="submit" class="button" name="add" value="'.translate('TEXT_UPLOAD_CONST').'"></td>
				</tr>
				</table>
			</td>
		</tr>
		<tr>
			<td colspan="3">&nbsp;</td>
		</tr>';
	
	$resEIB=$dbADO->Execute('SELECT * FROM Device WHERE (FK_DeviceTemplate=? OR FK_DeviceTemplate=?) AND FK_Installation=? ORDER BY Description ASC',array($GLOBALS['EIB'],$GLOBALS['KNX'],$installationID));
	$eibDevices=array();
	while($row=$resEIB->FetchRow()){
		$eibDevices[$row['PK_Device']]=$row['Description'];
	}
	
	$queryDevices='
		SELECT Device.*,Device_DeviceData.IK_DeviceData,DeviceTemplate.Description AS Template
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device
		WHERE FK_DeviceTemplate IN ('.join(',',array_keys($allowedTemplates)).') AND FK_Device_ControlledVia IN ('.join(',',array_keys($eibDevices)).') AND FK_DeviceData=? AND Device.FK_Installation=?
		ORDER BY Description ASC
	';
	$resDevices=$dbADO->Execute($queryDevices,array($targetDeviceData,$installationID));
	$devicesArray=array();
	while($rowDevices=$resDevices->FetchRow()){
		$devicesArray[$rowDevices['PK_Device']]=$rowDevices['FK_DeviceTemplate'];
		$channelParts=explode('|',$rowDevices['IK_DeviceData']);
		$out.='
		<input type="hidden" name="oldDD_'.$rowDevices['PK_Device'].'" value="'.$rowDevices['IK_DeviceData'].'">
		<tr>
			<td align="center">'.translate('TEXT_DEVICE_CONST').': <B>'.$rowDevices['Description'].' # '.$rowDevices['PK_Device'].'</B><br> '.translate('TEXT_DEVICE_TEMPLATE_CONST').': <B>'.$rowDevices['Template'].'</B></td>
			<td align="right">
				<fieldset><legend><B>'.translate('TEXT_GROUP_ADDRESSES_CONST').'</B></legend>
				<table>
					<tr>
						<td>'.$labels[0].'</td>
						<td><input type="text" name="onOffName_'.@$rowDevices['PK_Device'].'" size="50" value="'.@$channelArray[$channelParts[0]].' ('.@$channelParts[0].')'.'" disabled> <input type="hidden" name="onOff_'.$rowDevices['PK_Device'].'" value="'.@$channelParts[0].'"></td>
						<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'onOffName_'.$rowDevices['PK_Device'].'\',\'onOff_'.$rowDevices['PK_Device'].'\');"></td>
					</tr>';
		if(count($labels) >= 2){
			$out.='	<tr>
						<td>'.@$labels[1].'</td>
						<td><input type="text" name="statusName_'.$rowDevices['PK_Device'].'" size="50" value="'.@$channelArray[$channelParts[1]].' ('.@$channelParts[1].')'.'" disabled> <input type="hidden" name="status_'.$rowDevices['PK_Device'].'" value="'.@$channelParts[1].'"></td>
						<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'statusName_'.$rowDevices['PK_Device'].'\',\'status_'.$rowDevices['PK_Device'].'\');"></td>
					</tr>
			';
		}
		if((count($labels) >= 3) && $rowDevices['FK_DeviceTemplate'] != $GLOBALS['LightSwitchOnOff']){
			$out.='	<tr>
						<td>'.@$labels[2].'</td>
						<td><input type="text" name="dimName_'.$rowDevices['PK_Device'].'" size="50" value="'.@$channelArray[$channelParts[2]].' ('.@$channelParts[2].')'.'" disabled> <input type="hidden" name="dim_'.$rowDevices['PK_Device'].'" value="'.@$channelParts[2].'"></td>
						<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'dimName_'.$rowDevices['PK_Device'].'\',\'dim_'.$rowDevices['PK_Device'].'\');"></td>
					</tr>
			';

		}
		$out.='</table>
			</fieldset></td>
			<td><input type="button" class="button" name="del" value="'.translate('TEXT_DELETE_CONST').'" onClick="if(confirm(\''.translate('TEXT_EIB_CONFIRM_DELETE_CONST').'\'))self.location=\'index.php?section=eibDevices&action=delDevice&type='.$type.'&delID='.$rowDevices['PK_Device'].'\';"></td>
		</tr>
';
	}
	$firstTemplate=array_shift(array_keys($allowedTemplates));
	$out.='
		<tr>
			<td colspan="3">&nbsp;</td>
		</tr>				
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="update" value="'.translate('TEXT_EIB_UPDATE_DEVICES_CONST').'"></td>
		</tr>
	</table>
	<table align="center">
		<tr>
			<td align="center">'.translate('TEXT_ADD_DEVICE_CONST').': </td>
			<td><input type="text" name="newDevice"></td>
			<td align="right" rowspan="3">
				<fieldset><legend><B>'.translate('TEXT_GROUP_ADDRESSES_CONST').'</B></legend>
					<table>
						<tr>
							<td>'.$labels[0].'</td>
							<td><input type="text" name="newOnOffName" size="50" value="" disabled> <input type="hidden" name="newOnOff" value=""></td>
							<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'newOnOffName\',\'newOnOff\');"></td>
						</tr>';
	if(isset($labels[1])){
		$out.='
						<tr>
							<td>'.@$labels[1].'</td>
							<td><input type="text" name="newStatusName" size="50" value="" disabled></td>
							<td><input type="hidden" name="newStatus" value=""><input type="button" class="button" name="setNewStatus" '.((in_array($firstTemplate,$multiGroupAddress))?'':'disabled').' value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'newStatusName\',\'newStatus\');"></td>
						</tr>';
	}
	if(isset($labels[2])){
		$out.='
						<tr>
							<td>'.@$labels[2].'</td>
							<td><input type="text" name="newDimName" size="50" value="" disabled></td>
							<td><input type="hidden" name="newDim" value=""><input type="button" class="button" name="setNewDim" '.((in_array($firstTemplate,$multiGroupAddress))?'':'disabled').' value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'newDimName\',\'newDim\');"></td>
						</tr>';
	}	$out.='
					</table>
				</fieldset>
			</td>
			<td rowspan="3"><input type="submit" class="button" name="addDevice" value="'.translate('TEXT_ADD_DEVICE_CONST').'"></td>
		</tr>					
		<tr>
			<td>Device template:</td>
			<td><select name="deviceTemplate" onChange="enableSecond();">';
			foreach($allowedTemplates AS $key=>$description){
				$out.='<option value="'.$key.'">'.$description.'</option>';
			}
			$out.='
			</select></td>	
		</tr>
		<tr>
			<td>'.translate('TEXT_CONTROLLED_BY_CONST').':</td>
			<td><select name="controlledBy">';
		foreach ($eibDevices AS $eibID=>$description){
			$out.='<option value="'.$eibID.'">'.$description.'</option>';
		}
		$out.='	
			</select></td>	
		</tr>
	</table>
		<input type="hidden" name="devicesArray" value="'.urlencode(serialize($devicesArray)).'">
		<div id="addressPicker" style="position:absolute;display:none;top:0;left:0;">'.channelPullDown($channelArray,'generic','','onChange="setGroupValue();" onBlur="setGroupValue();"').'</div>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=eibDevices&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		if(isset($_POST['add'])){
			$pos=0;
			if($_FILES['newFile']['name']!=''){
				$fileArray=file($_FILES['newFile']['tmp_name']);
				$eibADO->Execute('TRUNCATE TABLE eib.groupaddresses');
				$linesAdded=0;
				foreach($fileArray AS $line){
					$fields=explode("\t",$line);
					if(count($fields) > 1){
						$description=utf8_encode($fields[1]);
						$fields=explode(".",$fields[0]);
						$GA=$fields[count($fields)-1];
						$eibADO->Execute('INSERT INTO groupaddresses (name,address) VALUES (?,?)',array($description,$GA));
						$linesAdded++;
					}
				}
				header("Location: index.php?section=eibDevices&type=$type&msg=".translate('TEXT_GA_UPDATED_CONST')." $linesAdded.");
				exit();
			}else{
				header("Location: index.php?section=eibDevices&type=$type&error=".translate('TEXT_ERROR_NO_FILE_SELECTED_CONST'));
				exit();
			}
		}

		if(isset($_POST['update'])){
			$devicesArray=unserialize(urldecode($_POST['devicesArray']));
			foreach ($devicesArray as $deviceID=>$templateID){
				switch($templateID) {
				
					case $GLOBALS['LightSwitchOnOff']:
						$newDeviceData=$_POST['onOff_'.$deviceID].'|'.$_POST['status_'.$deviceID];
						break;
						
					case $GLOBALS['LightSwitchDimmable']:
						$newDeviceData=$_POST['onOff_'.$deviceID].'|'.$_POST['status_'.$deviceID].'|'.$_POST['dim_'.$deviceID];
						break;
				
					default:
						if(in_array($templateID,$multiGroupAddress)){
							$newDeviceData=$_POST['onOff_'.$deviceID].'|'.$_POST['status_'.$deviceID].'|'.$_POST['dim_'.$deviceID];
						}else{
							$newDeviceData=$_POST['onOff_'.$deviceID];
						}
						break;
				}
				
				$oldDeviceData=$_POST['oldDD_'.$deviceID];
				if($oldDeviceData!=$newDeviceData){
					if(is_null($oldDeviceData)){
						$dbADO->Execute('INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData, IK_DeviceData) VALUES (?,?,?)',array($deviceID,$targetDeviceData,$newDeviceData));
					}else{
						$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newDeviceData,$deviceID,$targetDeviceData));
					}
				}
			}
			
			header("Location: index.php?section=eibDevices&type=$type&msg=".translate('TEXT_EIB_DEVICES_UPDATED_CONST'));
			exit();
		}

		if(isset($_REQUEST['delID'])){
			$delID=(int)$_REQUEST['delID'];
			deleteDevice($delID,$dbADO);
			
			header("Location: index.php?section=eibDevices&type=$type&msg=The device was deleted.");
			exit();
		}
		
		
		if(isset($_POST['addDevice'])){
			$newDevice=$_POST['newDevice'];
			$deviceTemplate=(int)$_POST['deviceTemplate'];
			$controlledBy=(int)$_POST['controlledBy'];
			
			switch($deviceTemplate) {
			
				case $GLOBALS['LightSwitchOnOff']:
					$deviceData=$_POST['newOnOff'].'|'.$_POST['newStatus'];
					break;
					
				case $GLOBALS['LightSwitchDimmable']:
					$deviceData=$_POST['newOnOff'].'|'.$_POST['newStatus'].'|'.$_POST['newDim'];
					break;
			
				default:
					$deviceData=(in_array($deviceTemplate,$multiGroupAddress))?$_POST['newOnOff'].'|'.$_POST['newDim']:$_POST['newOnOff'];
					break;
			}

			if($newDevice!=''){
				$insertID=createDevice($deviceTemplate,$installationID,$controlledBy,NULL,$dbADO);				
				
				$dbADO->Execute('UPDATE Device SET Description=? WHERE PK_Device=?',array($newDevice,$insertID));
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($deviceData,$insertID,$targetDeviceData));
				
				if($type=='sensors'){
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array(2,$insertID,$GLOBALS['InputOrOutput']));
				}
				header("Location: index.php?section=eibDevices&type=$type&msg=".translate('TEXT_EIB_DEVICE_ADDED_CONST'));
				exit();
			}else{
				header("Location: index.php?section=eibDevices&type=$type&error=".translate('TEXT_DEVICE_NAME_REQUIRED_CONST'));
				exit();
			}
		}
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle('KNX/EIB '.strtoupper($type));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array('KNX/EIB '.strtoupper($type)=>'index.php?section=eibDevices&type='.$type));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function channelPullDown($channelArray,$pulldownName,$selectedValue,$extra='')
{
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('eibDevices.lang.php');
	
	$out='<select name="'.$pulldownName.'" '.$extra.' '.(($pulldownName=='generic')?'style="background-color:lightyellow;"':'').'>';
	if(count($channelArray)==0)
		$out.='<option value="0">- '.translate('TEXT_NO_GROUP_ADDRESS_AVAILABLE_CONST').' -</option>';
	else{
		$out.='<option value="">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>';
		foreach ($channelArray AS $address=>$name){
			$out.='<option value="'.$address.'" '.(($address==$selectedValue)?'selected':'').'>'.$name.' ('.$address.')</option>';
		}
	}
	$out.='</select>';
	return $out;
}
?>

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
	
	// target GA adresses into the Channel field of devices
	$targetDeviceData=$GLOBALS['Channel'];
	$allowedTemplates=array();
	
	switch ($type){
		case 'lights':
			$allowedTemplates[$GLOBALS['LightSwitchOnOff']]['label']=translate('TEXT_KNX_SWITCHONOFF');
			$allowedTemplates[$GLOBALS['LightSwitchOnOff']]['GA'][]=translate('TEXT_KNX_ONOFF');
			$allowedTemplates[$GLOBALS['LightSwitchOnOff']]['GA'][]=translate('TEXT_KNX_STATUS');
			
			$allowedTemplates[$GLOBALS['LightSwitchDimmable']]['label']=translate('TEXT_KNX_SWITCHDIMMABLE');
			$allowedTemplates[$GLOBALS['LightSwitchDimmable']]['GA'][]=translate('TEXT_KNX_ONOFF');
			$allowedTemplates[$GLOBALS['LightSwitchDimmable']]['GA'][]=translate('TEXT_KNX_DIM');
			$allowedTemplates[$GLOBALS['LightSwitchDimmable']]['GA'][]=translate('TEXT_KNX_STATUS');
			$allowedTemplates[$GLOBALS['LightSwitchDimmable']]['GA'][]=translate('TEXT_KNX_DIM_STATUS');
			
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['label']=translate('TEXT_KNX_SWITCHRGB');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_ONOFF');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_R_LEVEL');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_G_LEVEL');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_B_LEVEL');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_STATUS');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_R_STATUS');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_G_STATUS');
			$allowedTemplates[$GLOBALS['LightSwitchRGB']]['GA'][]=translate('TEXT_KNX_B_STATUS');

			$allowedTemplates[$GLOBALS['BrightnessSensor']]['label']=translate('TEXT_KNX_BRIGHTNESS_SENSOR');
			$allowedTemplates[$GLOBALS['BrightnessSensor']]['GA'][]=translate('TEXT_KNX_BRIGHTNESS');
		break;
		case 'security':
			$resDT=$dbADO->Execute('SELECT DT.PK_DeviceTemplate,DT.Description FROM DeviceTemplate AS DT,DeviceTemplate_DeviceCategory_ControlledVia as CV WHERE DT.PK_DeviceTemplate= CV.FK_DeviceTemplate AND CV.FK_DeviceCategory=? AND DT.FK_DeviceCategory=? ORDER BY DT.Description ASC',array($GLOBALS['specialized'],$GLOBALS['rootSecurity']));
			while($rowDT=$resDT->FetchRow()){
				$allowedTemplates[$rowDT['PK_DeviceTemplate']]['label']=$rowDT['Description'];
				$allowedTemplates[$rowDT['PK_DeviceTemplate']]['GA'][]=translate('TEXT_KNX_ACTIVATED');
			}
		break;
		case 'climate':
			$allowedTemplates[$GLOBALS['StandardThermostat']]['label']=translate('TEXT_KNX_THERMOSTAT');
			$allowedTemplates[$GLOBALS['StandardThermostat']]['GA'][]=translate('TEXT_KNX_SETPOINT_TEMP');
			$allowedTemplates[$GLOBALS['StandardThermostat']]['GA'][]=translate('TEXT_KNX_SET_MODE');
			$allowedTemplates[$GLOBALS['StandardThermostat']]['GA'][]=translate('TEXT_KNX_ACTUAL_SETPOINT');
			$allowedTemplates[$GLOBALS['StandardThermostat']]['GA'][]=translate('TEXT_KNX_ACTUAL_MODE');
			$allowedTemplates[$GLOBALS['StandardThermostat']]['GA'][]=translate('TEXT_KNX_ACTUAL_TEMP');

			$allowedTemplates[$GLOBALS['StandardThermometer']]['label']=translate('TEXT_KNX_TEMPERATURE_SENSOR');
			$allowedTemplates[$GLOBALS['StandardThermometer']]['GA'][]=translate('TEXT_KNX_ACTUAL_TEMP');
		break;
		case 'drapes':
			$allowedTemplates[$GLOBALS['DrapesSwitch']]['label']='Drapes switch';
			$allowedTemplates[$GLOBALS['DrapesSwitch']]['GA'][]=translate('TEXT_KNX_LEVEL');
			$allowedTemplates[$GLOBALS['DrapesSwitch']]['GA'][]=translate('TEXT_KNX_LEVEL_STATUS');
			$allowedTemplates[$GLOBALS['DrapesSwitch']]['GA'][]=translate('TEXT_KNX_OPENCLOSE');
			$allowedTemplates[$GLOBALS['DrapesSwitch']]['GA'][]=translate('TEXT_KNX_STOP');

			/*$allowedTemplates[$GLOBALS['BlindsSwitch']]['label']='Blinds Switch';
			$allowedTemplates[$GLOBALS['BlindsSwitch']]['GA'][]=translate('TEXT_KNX_LEVEL');
			$allowedTemplates[$GLOBALS['BlindsSwitch']]['GA'][]=translate('TEXT_KNX_ANGLE');
			$allowedTemplates[$GLOBALS['BlindsSwitch']]['GA'][]=translate('TEXT_KNX_LEVEL_STATUS');
			$allowedTemplates[$GLOBALS['BlindsSwitch']]['GA'][]=translate('TEXT_KNX_ANGLE_STATUS');*/
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
		
		function changeTemplate()
		{
			var allowedTemplates = {};
			var templateID=document.eibDevices.deviceTemplate.value;';

			foreach($allowedTemplates as $templateID=>$template) {
				$out.='
				allowedTemplates["'.$templateID.'"] = {};
				allowedTemplates["'.$templateID.'"].label="'.$template['label'].'";
				allowedTemplates["'.$templateID.'"].GA= {};';
				$num_ga=count($template['GA']);
				for($i=0; $i < $num_ga; $i++ ) {
					$out.='allowedTemplates["'.$templateID.'"].GA['.$i.']="'.$template["GA"][$i].'";';
				}
			}
			$out.='
			var out = "";
			out = "<table>";
			for (var i in allowedTemplates[templateID].GA) {
				out += "<tr>";
				out += "<td>"+allowedTemplates[templateID].GA[i]+"</td>";
				out += "<td><input type=\"text\" name=\"newGA_"+i+"\" size=\"5\" value=\"\"> <input type=\"text\" name=\"newNAME_"+i+"\" size=\"50\" value=\"\" disabled></td>";
				out += "<td><input type=\"button\" class=\"button\" name=\"setGroup\" value=\"'.translate('TEXT_EIB_PICK_CONST').'\" onClick=\"pickGroupAddress(\'newNAME_"+i+"\',\'newGA_"+i+"\');\"></td>";
				out += "</tr>";
			}
			out += "</table>";
			document.getElementById("ga_list").innerHTML  = out;
		}
	
			//<td><input type="text" name="newNAME_'.$i.'" size="50" value="" disabled> <input type="hidden" name="newGA_'.$i.'" value=""></td>
			//<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'newNAME_'.$i.'\',\'newGA_'.$i.'\');"></td>
			
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
			<td align="center">'.translate('TEXT_DEVICE_CONST').': <B>'.$rowDevices['Description'].' # '.$rowDevices['PK_Device'].'</B><br> '.translate('TEXT_DEVICE_TEMPLATE_CONST').': <B>'.$rowDevices['Template'].' # '.$rowDevices['FK_DeviceTemplate'].'</B></td>
			<td align="right">
				<fieldset><legend><B>'.translate('TEXT_GROUP_ADDRESSES_CONST').'</B></legend>
				<table>';
					$template=$allowedTemplates[$rowDevices['FK_DeviceTemplate']];
					$num_ga=count($template['GA']);
					for($i=0; $i < $num_ga; $i++ ) {
						$out.='
						<tr>
							<td>'.$template['GA'][$i].'</td>
							<td>
								<input type="test" size="5" name="GA_'.$rowDevices['PK_Device'].'_'.$i.'" value="'.@$channelParts[$i].'">
								<input type="text" name="NAME_'.@$rowDevices['PK_Device'].'_'.$i.'" size="50" value="'.@$channelArray[$channelParts[$i]].'" disabled>
							</td>
							<td>
								<input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'NAME_'.$rowDevices['PK_Device'].'_'.$i.'\',\'GA_'.$rowDevices['PK_Device'].'_'.$i.'\');">
							</td>
						</tr>';
					}
		$out.='</table>
			</fieldset></td>
			<td><input type="button" class="button" name="del" value="'.translate('TEXT_DELETE_CONST').'" onClick="if(confirm(\''.translate('TEXT_EIB_CONFIRM_DELETE_CONST').'\'))self.location=\'index.php?section=eibDevices&action=delDevice&type='.$type.'&delID='.$rowDevices['PK_Device'].'\';"></td>
		</tr>';
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
					<span id="ga_list"><table>';
					$template=$allowedTemplates[$firstTemplate];
					$num_ga=count($template['GA']);
					for($i=0; $i < $num_ga; $i++ ) {
						$out.='
						<tr>
							<td>'.$template['GA'][$i].'</td>
							<td>
								<input type="text" name="newGA_'.$i.'" size="5" value="">
								<input type="text" name="newNAME_'.$i.'" size="50" value="" disabled>
							</td>
							<td><input type="button" class="button" name="setGroup" value="'.translate('TEXT_EIB_PICK_CONST').'" onClick="pickGroupAddress(\'newNAME_'.$i.'\',\'newGA_'.$i.'\');"></td>
						</tr>';
					}
						$out.='
					</table></span>
				</fieldset>
			</td>
			<td rowspan="3"><input type="submit" class="button" name="addDevice" value="'.translate('TEXT_ADD_DEVICE_CONST').'"></td>
		</tr>					
		<tr>
			<td>Device template:</td>
			<td><select name="deviceTemplate" onChange="changeTemplate();">';
			foreach($allowedTemplates AS $templateID=>$template){
				$out.='<option value="'.$templateID.'">'.$template['label'].'</option>';
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
				$newDeviceData='';
				$template=$allowedTemplates[$templateID];
				$num_ga=count($template['GA']);
				for($i=0; $i < $num_ga; $i++ ) {
					if($i > 0) $newDeviceData.='|';
					$newDeviceData.=$_POST['GA_'.$deviceID.'_'.$i];
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
			
			$template=$allowedTemplates[$deviceTemplate];
			$num_ga=count($template['GA']);
			for($i=0; $i < $num_ga; $i++ ) {
				if($i > 0) $deviceData.='|';
				$deviceData.=$_POST['newGA_'.$i];
			}

			if($newDevice == '') $newDevice=$allowedTemplates[$deviceTemplate]['label'];

			$insertID=createDevice($deviceTemplate,$installationID,$controlledBy,NULL,$dbADO);				
			
			$dbADO->Execute('UPDATE Device SET Description=? WHERE PK_Device=?',array($newDevice,$insertID));
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($deviceData,$insertID,$targetDeviceData));
			
			if($type=='sensors'){
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array(2,$insertID,$GLOBALS['InputOrOutput']));
			}
			header("Location: index.php?section=eibDevices&type=$type&msg=".translate('TEXT_EIB_DEVICE_ADDED_CONST'));
			exit();
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
			$out.='<option value="'.$address.'" '.(($address==$selectedValue)?'selected':'').'>'.$name.'</option>';
		}
	}
	$out.='</select>';
	return $out;
}
?>

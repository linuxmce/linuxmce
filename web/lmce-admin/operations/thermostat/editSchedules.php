<?php
function saveSchedule($deviceid, $version, $MAX_TIMES_PER_DAY){
	//increment version number so thermostat will request update
	$version++;
	// build array out of form fields
	$schedule[]="version,".$version;
	for($day = 1; $day <= 7; $day++) {
		for($time = 1; $time <= $MAX_TIMES_PER_DAY; $time++) {
			$timeFieldName='time_'.$day.'_'.$time;
			$timeValue = $_REQUEST[$timeFieldName];
			if($timeValue != null) {
				$stateFieldName='state_'.$day.'_'.$time;
				$stateValue = isset($_REQUEST[$stateFieldName]);
				if($stateValue) $stateValue='1'; else $stateValue='0';
			  	$schedule[]=$day.','.$timeValue.','.$stateValue;
			}
		}
	}
	//tokenize and insert into deviceData
	$scheduleParameter=implode(";",$schedule);
	mysql_query("UPDATE Device_DeviceData SET IK_DeviceData='".$scheduleParameter."' WHERE FK_Device=".$deviceid." AND FK_DeviceData=291") or die('ERROR: Invalid query: ' . mysql_error());
}

function editSchedules($output,$dbADO){
	// config
	$MAX_TIMES_PER_DAY = 9;

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');

	// DB connection
	$mysqlhost="localhost";
	$mysqluser="root";
	$mysqlpwd="";
	$mysqllmcedb="pluto_main";

	// init var's
	$deviceid= isset($_REQUEST['device']) ? $_REQUEST['device'] : 0;
	$action=$_REQUEST['action'];

	// connect to database and switch to main db
	$connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to database server!");
	mysql_select_db($mysqllmcedb, $connection) or die("ERROR: could not select LinuxMCE main database!");

	if($action == 'saveSchedule') {
		$version=$_REQUEST['version'];
		saveSchedule($deviceid, $version, $MAX_TIMES_PER_DAY);
	}

	// Query for thermostat device list with schedule capabilities
	$device_query = mysql_query("SELECT Device.PK_Device, Device.Description AS DeviceName,Room.Description AS RoomName
				FROM Device, Room, Device_DeviceData
				WHERE ((Device.FK_DeviceTemplate = 4)
				AND (Device.FK_Room = Room.PK_Room) AND (Device_DeviceData.FK_Device = Device.PK_Device)
				AND (Device_DeviceData.FK_DeviceData = 207) AND (Device_DeviceData.IK_DeviceData LIKE '%71%') )
				ORDER BY Device.FK_Room, Device.Description") or die('ERROR: Invalid query: ' . mysql_error());

	// Create device selection HTML form
	$out.='<form action="index.php" method="POST" name="deviceSelect">
				<input type="hidden" name="section" value="thermostatSchedules">
				<input type="hidden" name="action" value="changeDevice">';

	// create devices select box
	$out.=$TEXT_DEVICE_CONST.': <select name="device" style="width: 300px" onchange="document.deviceSelect.submit();">';
	while ($device=mysql_fetch_array($device_query)){
		if($deviceid == 0) $deviceid = $device['PK_Device'];
		$out.='<option value = "'.$device['PK_Device'].'"';
		if($device['PK_Device'] == $deviceid) $out.=' selected="selected"';
		$out.='>'.$device['DeviceName'].' / '.$device['RoomName'].' (ID:'.$device['PK_Device'].')</option>'; 
	}
	$out.='</select></form><br />';

	// Query schedule data for selected thermostat
	$schedule_query = mysql_query("SELECT IK_DeviceData FROM Device_DeviceData
  		WHERE FK_Device = ".$deviceid." AND FK_DeviceData=291") or die('ERROR: Invalid query: ' . mysql_error());
	$scheduleData=mysql_fetch_row($schedule_query);
	$scheduleData=explode(";", $scheduleData[0]);

	// deserialize and create cache of schedule data
	for($i = 0; $i < count($scheduleData); $i++) {
		if($scheduleData[$i] != "") {
			$schedule=explode(",",$scheduleData[$i]);
			if($schedule[0]=='version') $version=$schedule[1];
			else $scheduleCache[$schedule[0]][$schedule[1]]=$schedule[2];
		}
	}

	// Create schedule entry HTML form
	$out.='<form action="index.php" method="POST" name="editSchedules">
					<input type="hidden" name="section" value="thermostatSchedules">
					<input type="hidden" name="action" value="saveSchedule">
					<input type="hidden" name="device" value="'.$deviceid.'">
					<input type="hidden" name="version" value="'.$version.'">';

 	$out.='<table cellpadding="4" cellspacing="1" border="0">';
 	$out.='<tr class="tablehead"><td align="center"><B>'.$TEXT_DAY_CONST.'</B></td>';
	for($i=1; $i <= $MAX_TIMES_PER_DAY; $i++) {
		$out.='<td align="center"><B>'.$TEXT_TIME_CONST.'</B></td>';
		$out.='<td align="center"><B>'.$TEXT_ON_CONST.'</B></td>';
	}
	$out.='</tr>';

	$pos =0;
	for($i=1; $i <= 7; $i++) {
  		++$pos;
   		$out.='<tr class="'.(($pos%2==0)?'alternate_back':'').'">';
    	$out.='<td align="center">';
	    switch ($i) {
	   		case 1: $out.=$TEXT_MONDAY_CONST; break;
	   		case 2: $out.=$TEXT_TUESDAY_CONST; break;
	   		case 3: $out.=$TEXT_WEDNESDAY_CONST; break;
	   		case 4: $out.=$TEXT_THURSDAY_CONST; break;
	   		case 5: $out.=$TEXT_FRIDAY_CONST; break;
	   		case 6: $out.=$TEXT_SATURDAY_CONST; break;
	   		case 7: $out.=$TEXT_SUNDAY_CONST; break;
	    }
    	$out.='</B></td>';

		// fill in existing schedules
		$j=1;
		if(isset($scheduleCache[$i])) {
			foreach($scheduleCache[$i] as $time => $state) {
	     		$out.='<td align="center"><B><input type="text" name="time_'.$i.'_'.$j.'" value="'.$time.'" size="2" /></B></td>';
	     		$out.='<td align="center"><B><input type="checkbox" ';
	     		if($state == 1) $out.='checked="yes" ';
	     		$out.='name="state_'.$i.'_'.$j.'" value="" /></B></td>';
	     		$j++;
			}
		}
		// fill up with empty input boxes
		for($j=$j; $j <= $MAX_TIMES_PER_DAY; $j++) {
     		$out.='<td align="center"><B><input type="text" name="time_'.$i.'_'.$j.'" value="" size="2" /></B></td>';
     		$out.='<td align="center"><B><input type="checkbox" name="state_'.$i.'_'.$j.'" value="" /></B></td>';
		}
		$out.='</tr>';
	}

	$out.='</table><br /><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.' schedule" />';
	$out.='</form>';

 	$output->setBody($out);
	$output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
	$output->setPageTitle("Thermostat schedules");
	$output->setNavigationMenu(array('Thermostat schedules'=>'index.php?section=thermostatSchedules'));
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>

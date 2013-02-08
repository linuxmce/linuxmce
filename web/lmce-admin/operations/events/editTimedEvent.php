<?php
function editTimedEvent($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('timedEvents.lang.php');
	include('eventUtils.inc.php');
		
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];
	$dayNamesArray= array(1=>translate('TEXT_MONDAY_CONST'), 2=>translate('TEXT_TUESDAY_CONST'), 3=>translate('TEXT_WENSDAY_CONST'), 4=>translate('TEXT_THURSDAY_CONST'), 5=>translate('TEXT_FRIDAY_CONST'), 6=>translate('TEXT_SATURDAY_CONST'), 7=>translate('TEXT_SUNDAY_CONST'));
	$monthNamesArray = array(1=>translate('TEXT_JANUARY_CONST'), 2=>translate('TEXT_FEBRUARY_CONST'), 3=>translate('TEXT_MARCH_CONST'), 4=>translate('TEXT_APRIL_CONST'), 5=>translate('TEXT_MAY_CONST'), 6=>translate('TEXT_JUNE_CONST'), 7=>translate('TEXT_JULY_CONST'), 8=>translate('TEXT_AUGUST_CONST'), 9=>translate('TEXT_SEPTEMBER_CONST'), 10=>translate('TEXT_OCTOBER_CONST'), 11=>translate('TEXT_NOVEMBER_CONST'), 12=>translate('TEXT_DECEMBER_CONST')); 	
	$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:3;
	$section=$_REQUEST['section'];
	
	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting 
		FROM EventHandler
			INNER JOIN Criteria ON FK_Criteria=PK_Criteria
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	$rowEH=$resEH->FetchRow();
	$commandGroupID = $rowEH['FK_CommandGroup'];
	$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];
	$timedEventType=$rowEH['TimedEvent'];

	if(isset($_REQUEST['cgcID']) && (int)$_REQUEST['cgcID']!=0){
		$objID=$_REQUEST['cgcID'];
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editTimedEvent&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteObjFromDevice,$objID);

		$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteParamValues,$objID);
		
		reloadEventPlugin($installationID, $dbADO);

		$_REQUEST['msg']=translate('TEXT_COMMAND_DELETED_NOTIFICATION_CONST');
	}

	if ($action=='form') {
		
		$out.='
		<script>
		function showOptions(val)
		{
			for (i=1;i<5;i++){
				eval("document.getElementById(\'type_"+i+"\').style.display=\'none\';");
			}
			eval("document.getElementById(\'type_"+val+"\').style.display=\'\';");
		}
		
		function validateForm(){
			eventChecked=-1;
			for(i=0;i<document.editTimedEvent.timedEventType.length;i++){
				if(document.editTimedEvent.timedEventType[i].checked){
					eventChecked=i;
				}
			}

			switch(eventChecked){
				case 0:
					intervalNr=parseInt(document.editTimedEvent.intervalNumber.value);
					if(isNaN(intervalNr) || intervalNr<0){
						alert("'.translate('TEXT_ERROR_INVALID_INTERVAL_NUMBER_CONST').'");
						document.editTimedEvent.intervalNumber.focus();
						return false;
					}
				break;
				case 1:
					var timeValues=new Array();
					timeValues=document.editTimedEvent.dayOfWeekTime.value.split(",");
		
					for(i=0;i<timeValues.length;i++){
						if(!timeValues[i].checkTimeFormat()){
							alert("'.translate('TEXT_ERROR_INVALID_TIME_FORMAT_CONST').'");
							document.editTimedEvent.dayOfWeekTime.focus();
							return false;
						}
					}
				break;
				case 2:
					var daysValues=new Array();
					daysValues=document.editTimedEvent.dayOfMonths.value.split(",");

					for(i=0;i<daysValues.length;i++){
						if(isNaN(parseInt(daysValues[i])) || parseInt(daysValues[i])<0 || parseInt(daysValues[i]>30)){
							alert("'.translate('TEXT_ERROR_INVALID_DAY_NUMBER_CONST').'");
		
							document.editTimedEvent.dayOfMonths.focus();
							return false;
						}
					}
		
					var timeValues=new Array();
					timeValues=document.editTimedEvent.dayOfMonthTime.value.split(",");

					for(i=0;i<timeValues.length;i++){
						if(!timeValues[i].checkTimeFormat()){
							alert("'.translate('TEXT_ERROR_INVALID_TIME_FORMAT_CONST').'");
							document.editTimedEvent.dayOfMonthTime.focus();
							return false;
						}
					}
				break;
				case 3:
					if(document.editTimedEvent.absoluteDate.value.checkDateFormat()!=0){
						alert("'.translate('TEXT_ERROR_INVALID_DATE_FORMAT_CONST').'");
						document.editTimedEvent.absoluteDate.focus();
						return false;
					}
					var timeValues=new Array();
					timeValues=document.editTimedEvent.absoluteTime.value.split(",");

					for(i=0;i<timeValues.length;i++){
						if(!timeValues[i].checkTimeFormat()){
							alert("'.translate('TEXT_ERROR_INVALID_TIME_FORMAT_CONST').'");
							document.editTimedEvent.absoluteTime.focus();
							return false;
						}
					}
				break;
				default:
					return false;
				break;
			}
		
			return true;
		}
		</script>
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<form action="index.php" method="post" name="editTimedEvent" onSubmit="return validateForm();">
		<input type="hidden" name="section" value="editTimedEvent">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		<input type="hidden" name="oldWizard" value="'.$wizard.'">
		
		<a href="index.php?section=timedEvents">'.translate('TEXT_BACK_CONST').'</a>
		<table border="0" align="center" width="700">
		';
		$out.='
			<tr>
				<td width="200"><input type="radio" name="timedEventType" value="1" '.(($timedEventType==1)?'checked':'').' onClick="showOptions(1);"> <B>'.translate('TEXT_INTERVAL_BASED_CONST').':</B></td>
				<td>'.translate('TEXT_INTERVAL_BASED_TEXT_CONST').'</td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="2" '.(($timedEventType==2)?'checked':'').' onClick="showOptions(2);"> <B>Day of week based:</B></td>
				<td>Do something at 7:00 and 9:00 on Monday, Wednesday and Friday</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="3" '.(($timedEventType==3)?'checked':'').' onClick="showOptions(3);"> <B>Day of month based:</B></td>
				<td>Do something at 8:00 on the 1st and 15th of each month</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="4" '.(($timedEventType==4)?'checked':'').' onClick="showOptions(4);"> <B>Absolute:</B></td>
				<td>Do something on 5 Mar 2005 at 11:15</td>
			</tr>
		
			<tr bgcolor="#E7E7E7">
				<td><B>Description:</B></td>
				<td><input type="text" name="description" value="'.$rowEH['Description'].'"></td>
			</tr>
			<tr>
				<td colspan="2">';
		switch($timedEventType){
			case 1:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$oldIntervalNumber=(ereg('h',$rowOldValues['Value']))?str_replace('h','',$rowOldValues['Value']):$rowOldValues['Value'];
					$oldIntervalNumber=(ereg('m',$oldIntervalNumber))?str_replace('m','',$oldIntervalNumber):$oldIntervalNumber;
					$oldIntervalType=(ereg('h',$rowOldValues['Value']))?1:((ereg('m',$rowOldValues['Value']))?2:3);	// 3 is error case
				}

				$out.='<input type="hidden" name="oldInterval" value="'.@$rowOldValues['Value'].'">';
			break;
			case 2:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['DayOfWeekParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$checkedArray=explode(',',$rowOldValues['Value']);
				}
				$resOldTime=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldTime){
					$rowOldTime=$resOldTime->FetchRow();
				}
				$oldDayOfWeek=@$rowOldValues['Value'].@$rowOldTime['Value'];
				$out.='<input type="hidden" name="oldDayOfWeek" value="'.@$oldDayOfWeek.'">';
			break;
			case 3:
				$resOldMonthValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['DayOfMonthParmList']));
				if($resOldMonthValue->RecordCount()!=0){
					$rowOldMonthValues=$resOldMonthValue->FetchRow();
				}
				$resOldMonthTime=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldMonthTime){
					$rowOldMonthTime=$resOldMonthTime->FetchRow();
				}
				$oldDayOfMonth=@$rowOldMonthValues['Value'].@$rowOldMonthTime['Value'];

				$out.='<input type="hidden" name="oldDayOfMonth" value="'.@$oldDayOfMonth.'">';
			break;
			case 4:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['SpecificDateParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$oldAbsolute=$rowOldValues['Value'];
					$dataParts=explode(' ',$rowOldValues['Value']);
				}
				$out.='<input type="hidden" name="oldAbsolute" value="'.@$oldAbsolute.'">';
			break;
		}

		$out.='<span id="type_1" style="display:'.(($timedEventType!=1)?'none':'').';">Every <input type="text" name="intervalNumber" value="'.@$oldIntervalNumber.'" size="2"> <select name="intervalType"><option value="1" '.((@$oldIntervalType==1)?'selected':'').'>Hours</option><option value="2" '.((@$oldIntervalType==2)?'selected':'').'>Minutes</option></select></span>';
		$out.='<span id="type_2" style="display:'.(($timedEventType!=2)?'none':'').';">';
		$out.='What days of the week: ';
			foreach ($dayNamesArray AS $key=>$dayName){
				$out.='<input type="checkbox" name="dayOfWeek_'.$key.'" '.((is_array(@$checkedArray) && in_array($key,$checkedArray))?'checked':'').'> '.$dayName;
			}
		$out.='<br>Time: <input type="text" name="dayOfWeekTime" value="'.((isset($rowOldTime['Value']))?$rowOldTime['Value']:date('h:i')).'"> (comma separated list for multiple values)</span>';
		$out.='<span id="type_3" style="display:'.(($timedEventType!=3)?'none':'').';">What day of the month (e.g. 2,7)? <input type="text" name="dayOfMonths" value="'.((isset($rowOldMonthValues['Value']))?$rowOldMonthValues['Value']:'').'"> Time: (e.g. 10:00,21:30:35) <input type="text" name="dayOfMonthTime" value="'.((isset($rowOldMonthTime['Value']))?$rowOldMonthTime['Value']:date('h:i')).'"></span>';
		$out.='<span id="type_4" style="display:'.(($timedEventType!=4)?'none':'').';">What date day/month/year? <input type="text" name="absoluteDate" size="7" value="'.((isset($dataParts[0]))?formatMySQLDate($dataParts[0],'d/m/Y'):date('d/m/Y')).'">  What time? (e.g. 10:00,21:30:35) <input type="text" name="absoluteTime" value="'.((isset($dataParts[1]))?$dataParts[1]:date('h:i')).'"> (comma separated list for multiple values)</span>';
		$out.='</td>
			</tr>		
			<tr>
				<td>
			';
			$queryCG_R='
				SELECT Room.Description AS RoomName,PK_Room, FK_CommandGroup 
				FROM Room
				LEFT JOIN CommandGroup_Room ON FK_Room=PK_Room AND FK_CommandGroup=?
				WHERE Room.FK_Installation=? 
				ORDER BY RoomName ASC';

			$resCG_R=$dbADO->Execute($queryCG_R,array($commandGroupID,$installationID));
			$displayedRoomsArray=array();
			$oldCheckedRooms=array();
			while($rowCG_R=$resCG_R->FetchRow()){
				$displayedRoomsArray[]=$rowCG_R['PK_Room'];
				if($rowCG_R['FK_CommandGroup']!='')
					$oldCheckedRooms[]=$rowCG_R['PK_Room'];
				$out.='<input type="checkbox" name="cg_r_'.$rowCG_R['PK_Room'].'" value="1" '.(($rowCG_R['FK_CommandGroup']!='')?'checked':'').'> '.$rowCG_R['RoomName'].'<br>';
			}

			$out.='
					<input type="hidden" name="oldCheckedRooms" value="'.join(',',$oldCheckedRooms).'">
					<input type="hidden" name="displayedRoomsArray" value="'.join(',',$displayedRoomsArray).'">
				</td>
				<td>If you would like to use this event as an alarm clock, choose what rooms this event should act as an alarm clock in. It will appear on the Orbiters\' sleeping menu and you can change the time on the Orbiter.</td>
			</tr>
			<tr bgcolor="#E7E7E7">
				<td colspan="2" align="center"><input type="submit" class="button" name="continue" value="Update"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
			</tr>
			<tr>
				<td colspan="2" align="center">Specify above when you want Pluto to do something, and below what you want Pluto to do by choosing a device, and then a command to send to that device.</td>
			</tr>';
		$out.='
		<tr>
			<td colspan="2">';
		$out.=editCommandsByWizard($wizard,$section,$commandGroupID,$dbADO).'
			</td>
		</tr>
		</table>
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editTimedEvent&ehID=".$eventHandlerID."&error=You are not authorised to modify installation.");
			exit();
		}
		$description=cleanString($_POST['description']);
		$timedEventType=(int)$_POST['timedEventType'];

		// update EventHandler
		if($description==''){
			header("Location: index.php?section=editTimedEvent&ehID=$eventHandlerID&wizard=$wizard&error=Description cannot be empty.");
			exit();
		}
		
		if(isset($_POST['continue'])){
			$updateEventHandler='UPDATE EventHandler SET Description=?, TimedEvent=? WHERE PK_EventHandler=?';
			$dbADO->Execute($updateEventHandler,array($description,$timedEventType,$eventHandlerID));
			
			switch($timedEventType){
				case 1:
					$oldInterval=@$_POST['oldInterval'];
					$intervalNumber=isset($_POST['intervalNumber'])?(int)$_POST['intervalNumber']:0;
					$intervalType=(@$_POST['intervalType']==1)?'h':'m';
					$newInterval=$intervalNumber.$intervalType;
					if($oldInterval!=$newInterval){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$newInterval));
					}
	
				break;
				case 2:
					$newDayOfWeekArray=array();
					foreach ($dayNamesArray AS $key=>$dayName){
						if(isset($_POST['dayOfWeek_'.$key]))
							$newDayOfWeekArray[]=$key;
					}
					$dayOfWeekTime=isset($_POST['dayOfWeekTime'])?$_POST['dayOfWeekTime']:date('h:i');
					$oldDayOfWeek=@$_POST['oldDayOfWeek'];
					$newDayOfWeek=join(',',$newDayOfWeekArray).$dayOfWeekTime;
					if($newDayOfWeek!=$oldDayOfWeek){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['DayOfWeekParmList'],1,join(',',$newDayOfWeekArray)));
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$dayOfWeekTime));
					}

				break;
				case 3:
					$dayOfMonths=@$_POST['dayOfMonths'];
					$dayOfMonthTime=isset($_POST['dayOfMonthTime'])?$_POST['dayOfMonthTime']:date('h:i');
					$oldDayOfMonth=@$_POST['oldDayOfMonth'];
					$newDayOfMonth=$dayOfMonths.$dayOfMonthTime;
					if($newDayOfMonth!=$oldDayOfMonth){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
							if($dayOfMonths!=''){
								$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['DayOfMonthParmList'],1,$dayOfMonths));
								$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$dayOfMonthTime));
							}
					}
				break;
				case 4:
					$oldAbsolute=@$_POST['oldAbsolute'];
					$dateArray=explode('/',isset($_POST['absoluteDate'])?$_POST['absoluteDate']:date('d/m/Y'));
					$absoluteDate=@$dateArray[2].'-'.@$dateArray[1].'-'.@$dateArray[0];
					$absoluteTime=isset($_POST['absoluteTime'])?$_POST['absoluteTime']:date('h:i');
					$newAbsolute=$absoluteDate.' '.$absoluteTime;
					if($newAbsolute!=$oldAbsolute){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['SpecificDateParmList'],1,$newAbsolute));
					}
					
				break;
			}
		}
		
		$displayedRoomsArray=explode(',',$_POST['displayedRoomsArray']);
		$oldCheckedRooms=explode(',',$_POST['oldCheckedRooms']);
		foreach($displayedRoomsArray AS $roomID){
			if(isset($_POST['cg_r_'.$roomID])){
				if(!in_array($roomID,$oldCheckedRooms))
					$dbADO->Execute('INSERT INTO CommandGroup_Room (FK_CommandGroup, FK_Room, Sort) VALUES (?,?,?)',array($commandGroupID,$roomID,$commandGroupID));
			}elseif(in_array($roomID,$oldCheckedRooms)){
				$dbADO->Execute('DELETE FROM CommandGroup_Room WHERE FK_CommandGroup=? AND FK_Room=?',array($commandGroupID,$roomID));
			}
		}
	
		$oldWizard=(int)$_POST['oldWizard'];
		$wizard=(int)$_POST['wizard'];
		
		switch($oldWizard){
			case 0:
				processLightingScenario($commandGroupID,$dbADO);
				$msg='The event handler was updated';
			break;
			case 1:
				processClimateScenario($commandGroupID,$dbADO);
				$msg='The event handler was updated';
			break;
			case 2:
				processIrrigationScenario($commandGroupID,$section,$dbADO);
				$msg='The event handler was updated';
			break;	
			
			case 3:
				list ($isModified,$parametersUpdatedAlert,$returnHook)=processAdvancedScenarios($commandGroupID,$section,$dbADO);
				$msg="Command Group ".($isModified?"":"not")." updated! $parametersUpdatedAlert";
			break;	
		}
	
		reloadEventPlugin($installationID, $dbADO);

		setOrbitersNeedConfigure($installationID,$dbADO);
		if($wizard!=$oldWizard){
			if((int)@$_REQUEST['addNewDevice']!=0)
				$sufix='&addNewDevice='.(int)@$_REQUEST['addNewDevice'];
			header("Location: index.php?section=editTimedEvent&ehID=$eventHandlerID&wizard=$wizard&msg=Timed event updated.".@$sufix);
			exit();
		}else{
			//header("Location: index.php?section=timedEvents&msg=$msg");
			header("Location: index.php?section=editTimedEvent&ehID=$eventHandlerID&msg=Timed event updated.&wizard=$wizard".@$sufix);
			exit();
		}
		
		header('Location: index.php?section=editTimedEvent&ehID='.$eventHandlerID.'&msg=The event handler was updated');
	}

	$output->setMenuTitle(translate('TEXT_TIMED_EVENTS_CONST').' |');
	$output->setPageTitle(translate('TEXT_EDIT_TIMED_EVENT_CONST').' #'.$eventHandlerID);
	$output->setNavigationMenu(array("Timed Events"=>'index.php?section=timedEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

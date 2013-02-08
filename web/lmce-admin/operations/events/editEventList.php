<?php
function editEvent($output,$dbADO) {
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$selectedEvent = (int)$_REQUEST['EventID'];
	
	if ($action=='form') {
		
		$querySelectEvent = "select * from Event where  PK_Event = $selectedEvent";
		$resSelectEvent = $dbADO->_Execute($querySelectEvent);
		if ($resSelectEvent && $resSelectEvent->RecordCount()==1) {
			$row=$resSelectEvent->FetchRow();
			$EventDescription = $row['Description'];
			$EventEventCategory = $row['FK_EventCategory'];
		}
		
		$querySelectEventCategory = "select Description,PK_EventCategory from EventCategory where  FK_EventCategory_Parent IS NULL order by Description asc";
		$resSelectEventCategory = $dbADO->_Execute($querySelectEventCategory);
		
		$EventCategoryTxt = '<option value="0">-please select-</option>';
		if ($resSelectEventCategory) {
			while ($rowSelectEventCategory=$resSelectEventCategory->FetchRow()) {
				$EventCategoryTxt.= '<option '.($EventEventCategory==$rowSelectEventCategory['PK_EventCategory']?' selected="selected" ':'').' value="'.$rowSelectEventCategory['PK_EventCategory'].'">'.$rowSelectEventCategory['Description'].'</option>';
				$EventCategoryTxt.=getEventCategoryChildsArray($rowSelectEventCategory['PK_EventCategory'],$rowSelectEventCategory['Description'],$EventEventCategory,$dbADO);
			}
		}
		$out.='
		
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		
		<form action="index.php" method="post" name="editEvent">
		<input type="hidden" name="section" value="editEvent">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="EventID" value="'.$selectedEvent.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>	
				<tr>
					<td>
						Event List Category:</td><td><select name="EventEventCategory">'.$EventCategoryTxt.'</select>							
					</td>
				</tr>		
				<tr>
					<td>Event List Name:</td><td><input type="text" maxlength="50" name="EventDescription" value="'.$EventDescription.'"></td>
				</tr>		
				<tr>
					<td colspan="2">
						<fieldset>
							<legend>Parameters</legend>
								<table>
									';
		$selectEventParams = "SELECT Event_EventParameter.Description,PK_EventParameter,EventParameter.Description AS EL_Description
				FROM Event_EventParameter INNER JOIN EventParameter ON PK_EventParameter = FK_EventParameter
			WHERE FK_Event = ?
			order by EL_Description asc
		";
		$resSelectEventParams = $dbADO->Execute($selectEventParams,array($selectedEvent));
		$displayedEventParams = array();
		$displayedEventParams[]=0;
		if ($resSelectEventParams) {
			while ($rowSelectEventParams=$resSelectEventParams->FetchRow()) {
				$out.='<tr><td>'.$rowSelectEventParams['EL_Description'].'<br />#'.$rowSelectEventParams['PK_EventParameter'].'</td><td><textarea cols="40" rows="4" name="eventParamDesc_'.$rowSelectEventParams['PK_EventParameter'].'">'.$rowSelectEventParams['Description'].'</textarea></td>
				<td>
					<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editEventParameterFromEvent&from=editEvent&eventParameterID='.$rowSelectEventParams['PK_EventParameter'].'\',\'width=500,height=400,toolbar=1\');">Edit</a>
					<br />
					<a href="javascript:void(0);" onClick="if (confirm(\'Are you sure you want to delete this parameter?\')) windowOpen(\'index.php?section=deleteEventParameterFromEvent&from=editEvent&EventID='.$selectedEvent.'&eventParameterID='.$rowSelectEventParams['PK_EventParameter'].'\',\'width=100,height=100,toolbar=1\');">Delete</a>
				</td>
				</tr>';
				$displayedEventParams[]=$rowSelectEventParams['PK_EventParameter'];
			}
		}
		
		$querySelRemainingEventParams = 'SELECT * FROM EventParameter WHERE PK_EventParameter NOT IN ('.join(",",$displayedEventParams).') order by Description Asc';
		$resSelRemainingEventParams = $dbADO->Execute($querySelRemainingEventParams);
		
		$eventParametersTxt = "<option value='0'>-please select-</option>";
			if ($resSelRemainingEventParams)  {
				while ($row = $resSelRemainingEventParams->FetchRow()) {
					$eventParametersTxt.='<option value="'.$row['PK_EventParameter'].'">'.$row['Description'].'</option>';
				}
			} else {
				$eventParametersTxt = "<option value='0'>-nothing to select-</option>";
			}
		
		$out.="<tr><td colspan='3'>Add new parameter <select name='addEventParameterToEvent'>{$eventParametersTxt}</select> &nbsp; <input type='submit' name='submitX' value='Add'></td></tr>";
		$out.='<tr><td colspan="3">If parameter not in the list <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createEventParameter&from=editEvent\',\'width=500,height=400,toolbar=1\');">create a new one</a></td></tr>';
		$out.='
								</table>
						</fieldset>
					</td>
				</tr>		
				<tr>
					<td colspan="2" align="center">
						<input type="hidden" name="displayedEventParams" value="'.join(",",$displayedEventParams).'">
						<input type="submit" class="button" name="submitX" value="Save">
					</td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editEvent");			
 			frmvalidator.addValidation("EventDescription","req","Please enter a name for this Event List !");
			frmvalidator.addValidation("EventEventCategory","dontselect=0","Please select a parent event list category!");
		</script>
		';
		
	} else {
		
		$EventDescription = isset($_POST['EventDescription'])?cleanString($_POST['EventDescription']):'';
		$category = cleanInteger($_POST['EventEventCategory']);
		
		$displayedEventParams = explode(",",$_POST['displayedEventParams']);
		if (!is_array($displayedEventParams)) {
			$displayedEventParams=array();
		}
		foreach ($displayedEventParams as $param) {
			$param = (int)$param;
			$description = cleanString(@$_POST['eventParamDesc_'.$param]);
			$selectOldValue = 'SELECT Description FROM  Event_EventParameter WHERE FK_Event = ? and FK_EventParameter = ?';
			$resOldValue = $dbADO->Execute($selectOldValue,array($selectedEvent,$param));
				if ($resOldValue && $resOldValue->RecordCount()==1)	{
					$rowOldValue = $resOldValue->FetchRow();
					 if ($rowOldValue['Description']!=$description) {
					 	$queryUpdateEventParameter = 'UPDATE Event_EventParameter SET Description = ? WHERE FK_Event = ? and FK_EventParameter = ?';
					 	$resUpd = $dbADO->Execute($queryUpdateEventParameter,array($description,$selectedEvent,$param));
					 	$lastAction = "eventParamDesc_".$param;
					 }
				}
		}
		
		$addEventParameterToEvent = cleanInteger($_POST['addEventParameterToEvent']);
		if ($addEventParameterToEvent!=0) {
			$queryInsertEventParameter = "INSERT INTO Event_EventParameter (FK_Event,FK_EventParameter) values(?,?)";
			$res = $dbADO->Execute($queryInsertEventParameter,array($selectedEvent,$addEventParameterToEvent));			
			$lastAction = "eventParamDesc_".$addEventParameterToEvent;
			
		}
		
		if ($category != 0 && $EventDescription!='' && $selectedEvent!=0) {
			$insertEvent = 'update Event set Description=?, FK_EventCategory=? where PK_Event = ?';
			$dbADO->Execute($insertEvent,array($EventDescription,$category,$selectedEvent));
		} else {				
			header("Location: index.php?section=editEvent&EventID=$selectedEvent");
		}		
		$out.="
			<script>
				alert('Event List modified!');
			    opener.document.forms.{$from}.action.value='form';				
				opener.document.forms.{$from}.lastAction.value='$lastAction';	
				opener.document.forms.{$from}.submit();
				document.location.href='index.php?section=editEvent&from=$from&EventID=$selectedEvent&unique=".uniqid("pluto")."';
			</script>
			";	
	}
	
	$onLoad='';
	
	if ($lastAction!=''?$onLoad.="if (document.forms.editEvent.{$lastAction}) {document.forms.editEvent.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {		
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

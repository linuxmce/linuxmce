<?php
function dceEvents($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/dceEvents.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	
	if($action=='form'){
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<form action="index.php" method="POST" name="dceEvents">
			<input type="hidden" name="section" value="dceEvents">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" align="center">
			<tr>
				<td colspan="4"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createEventCategory&from=dceCommands\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');"><B>'.$TEXT_ADD_EVENT_CATEGORY_CONST.'</B></a></td>
			</tr>		
			<tr class="tablehead">
				<td align="center" width="120"><B>'.$TEXT_EVENT_CATEGORY_CONST.'</B></td>
				<td align="center"><B>#</b></td>
				<td align="center"><B>'.$TEXT_EVENT_DESCRIPTION_CONST.'</b></td>
				<td align="center"><B>'.$TEXT_USED_BY_CONST.'</B></td>
			</tr>';
		$queryRootEC='
			SELECT Description,PK_EventCategory
			FROM EventCategory
			WHERE FK_EventCategory_Parent IS NULL
			ORDER BY Description ASC';
		$resRootEC=$dbADO->Execute($queryRootEC);
		$pos=0;
		while($rowRootEC=$resRootEC->FetchRow()){
			$pos++;
			$out.='
			<tr bgcolor="#EEEEEE">
				<td colspan="4"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=events&from=dceEvents\',\'width=600,height=800,toolbar=1\');"><B>'.$rowRootEC['Description'].'</B></a></td>
			</tr>';
			$resEvents=$dbADO->Execute('SELECT * FROM Event WHERE FK_EventCategory=?',$rowRootEC['PK_EventCategory']);
			while($rowEvents=$resEvents->FetchRow()){
				$queryDT_E='
					SELECT DeviceTemplate.Description, PK_DeviceTemplate,DeviceTemplate_Event.Description AS Tooltip
					FROM DeviceTemplate_Event
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE FK_Event=?';
				$dtLinksArray=array();
				$resDT_E=$dbADO->Execute($queryDT_E,$rowEvents['PK_Event']);
				while($rowDT_E=$resDT_E->FetchRow()){
					$dtLinksArray[]='<a href="index.php?section=editMasterDevice&model='.$rowDT_E['PK_DeviceTemplate'].'" title="'.$rowDT_E['Tooltip'].'">'.$rowDT_E['Description'].'</a>';
				}
				$out.='
				<tr>
					<td>&nbsp;</td>
					<td>'.$rowEvents['PK_Event'].'</td>
					<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editEvent&from=dceEvents&EventID='.$rowEvents['PK_Event'].'\',\'status=0,resizable=1,width=500,height=250,toolbar=1,scrollbars=1\');">'.$rowEvents['Description'].'<a></td>
					<td>'.join(', ',$dtLinksArray).'</td>
				</tr>';
			}
			$out.=getEventCategoryChilds($rowRootEC['PK_EventCategory'],$dbADO,1);
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=dceEvents');
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle('DCE '.$TEXT_EVENTS_CONST);
	$output->setNavigationMenu(array($TEXT_EVENTS_CONST=>'index.php?section=dceEvents'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}


function getEventCategoryChilds($eventCategory,$dbADO,$level)
{
	$out='';
	$queryEC='
			SELECT Description,PK_EventCategory
			FROM EventCategory
			WHERE FK_EventCategory_Parent=?
			ORDER BY Description ASC';
	$resEC=$dbADO->Execute($queryEC,$eventCategory);
	$pos=0;
	while($rowEC=$resEC->FetchRow()){
		$pos++;
		$indent='';
		for($i=0;$i<6*$level;$i++)
			$indent.='&nbsp;';
		$out.='
			<tr bgcolor="#EEEEEE">
				<td colspan="3">'.$indent.'<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=events&from=dceEvents\',\'width=600,height=800,toolbar=1\');"><B>'.$rowEC['Description'].'</B></a></td>
			</tr>';
		$resEvents=$dbADO->Execute('SELECT * FROM Event WHERE FK_EventCategory=?',$rowEC['PK_EventCategory']);
		while($rowEvents=$resEvents->FetchRow()){
			$queryDT_E='
					SELECT DeviceTemplate.Description, PK_DeviceTemplate,DeviceTemplate_Event.Description AS Tooltip
					FROM DeviceTemplate_Event
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE FK_Event=?';
			$dtLinksArray=array();
			$resDT_E=$dbADO->Execute($queryDT_E,$rowEvents['PK_Event']);
			while($rowDT_E=$resDT_E->FetchRow()){
				$dtLinksArray[]='<a href="index.php?section=editMasterDevice&model='.$rowDT_E['PK_DeviceTemplate'].'" title="'.$rowDT_E['Tooltip'].'">'.$rowDT_E['Description'].'</a>';
			}
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td>'.$rowEvents['PK_Event'].'</td>
					<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editEvent&from=dceEvents&EventID='.$rowEvents['PK_Event'].'\',\'status=0,resizable=1,width=500,height=250,toolbar=1,scrollbars=1\');">'.$rowEvents['Description'].'<a></td>
					<td>'.join(', ',$dtLinksArray).'</td>
				</tr>';
		}
		$out.=getEventCategoryChilds($rowEC['PK_EventCategory'],$dbADO,$level+1);
	}
	return $out;
}
?>
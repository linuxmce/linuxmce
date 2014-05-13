<?php
function dceDeviceData($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/dceDeviceData.lang.php');
	
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
			<form action="index.php" method="POST" name="dceDeviceData">
			<input type="hidden" name="section" value="dceDeviceData">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" align="center">
			<tr class="tablehead">
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td><B>'.$TEXT_TYPE_CONST.'</B></td>
				<td><B>'.$TEXT_USED_BY_CONST.'</B></td>
			</tr>';
		$queryDD='
			SELECT DeviceData.Description, ParameterType.Description AS Type, PK_DeviceData,DeviceTemplate.Description AS Template,PK_DeviceTemplate,DeviceTemplate_DeviceData.Description AS Tooltip
			FROM DeviceData
			INNER JOIN ParameterType ON FK_ParameterType=PK_ParameterType
			INNER JOIN DeviceTemplate_DeviceData ON FK_DeviceData=PK_DeviceData
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			ORDER BY DeviceData.Description ASC
			';
		$resDD=$dbADO->Execute($queryDD);
		$ddArray=array();
		while($row=$resDD->FetchRow()){
			$ddArray[$row['PK_DeviceData']]['Description']=$row['Description'];
			$ddArray[$row['PK_DeviceData']]['Type']=$row['Type'];
			$ddArray[$row['PK_DeviceData']]['PK_DeviceTemplate'][]=$row['PK_DeviceTemplate'];
			$ddArray[$row['PK_DeviceData']]['Template'][]=$row['Template'];
			$ddArray[$row['PK_DeviceData']]['Tooltip'][]=$row['Tooltip'];
			
			/*
			while($rowDT_DD=$resDT_DD->FetchRow()){
				$dtLinksArray[]='<a href="index.php?section=editMasterDevice&model='.$rowDT_DD['PK_DeviceTemplate'].'" title="'.$rowDT_DD['Tooltip'].'">'.$rowDT_DD['Description'].'</a>';
			}
			*/
		}

		$pos=0;
		foreach ($ddArray AS $ddID=>$values){
			$dtLinksArray=array();
			for($i=0;$i<count($values['PK_DeviceTemplate']);$i++){
				$dtLinksArray[]='<a href="index.php?section=editMasterDevice&model='.$values['PK_DeviceTemplate'][$i].'" title="'.$values['Tooltip'][$i].'">'.$values['Template'][$i].'</a>';
			}
			
			$pos++;
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'').'">
					<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editDeviceData&from=dceDeviceData&deviceDataID='.$ddID.'\',\'status=0,resizable=1,width=500,height=250,toolbar=1\');">'.$values['Description'].'</a></td>
					<td>'.$values['Type'].'</td>
					<td>'.join(', ',$dtLinksArray).'</td>
				</tr>';
			
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=dceDeviceData');
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle('DCE '.$TEXT_DEVICE_DATA_CONST);
	$output->setNavigationMenu(array($TEXT_DEVICE_DATA_CONST=>'index.php?section=dceDeviceData'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
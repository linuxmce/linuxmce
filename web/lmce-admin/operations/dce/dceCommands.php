<?php
function dceCommands($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/dceCommands.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	// $dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			
			<form action="index.php" method="POST" name="dceCommands">
			<input type="hidden" name="section" value="dceCommands">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" align="center">
			<tr class="tablehead">
				<td align="center" width="120"><B>'.$TEXT_COMMAND_CATEGORY_CONST.'</B></td>
				<td align="center"><B>#</b></td>
				<td align="center"><B>'.$TEXT_COMMAND_DESCRIPTION_CONST.'</b></td>
				<td align="center" colspan="2"><B>'.$TEXT_USED_BY_CONST.'</B></td>
			</tr>
		';
		
		$query='
			SELECT FK_Command,DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate, DeviceTemplate.Description AS TemplateName,PK_Device,Device.Description AS DeviceName
			FROM DeviceTemplate_DeviceCommandGroup
			INNER JOIN DeviceTemplate ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN Device ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=Device.FK_DeviceTemplate
			INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_DeviceCommandGroup=DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup
			WHERE Device.FK_Installation=?';
		$res=$dbADO->Execute($query,array($installationID));
		$templatesUsedByCommand=array();
		$templateNamesUsedByCommand=array();
		$devicesUsedByCommand=array();
		$deviceNamesUsedByCommand=array();
		while($row=$res->FetchRow()){
			if(@!in_array($row['FK_DeviceTemplate'],$templatesUsedByCommand[$row['FK_Command']])){
				$templatesUsedByCommand[$row['FK_Command']][]=$row['FK_DeviceTemplate'];
				$templateNamesUsedByCommand[$row['FK_Command']][]=$row['TemplateName'];
			}
			
			if(@!in_array($row['PK_Device'],$templatesUsedByCommand[$row['FK_Command']])){
				$devicesUsedByCommand[$row['FK_Command']][]=$row['PK_Device'];
				$deviceNamesUsedByCommand[$row['FK_Command']][]=$row['DeviceName'];
			}
		}
		//print_array($templatesUsedByCommand);
		
		$queryRootCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent IS NULL
			ORDER BY CommandCategory.Description ASC';
		$resRootCC=$dbADO->Execute($queryRootCC);
		$out.=formatOutput($resRootCC,$dbADO,0,$templatesUsedByCommand,$templateNamesUsedByCommand,$devicesUsedByCommand,$deviceNamesUsedByCommand);
		$out.='
			<tr>
				<td align="left" colspan="4"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandCategory&from=dceCommands\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">'.$TEXT_ADD_COMMAND_CATEGORY_CONST.'</a></td>
			</tr>
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=dceCommands');
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle('DCE '.$TEXT_COMMANDS_CONST);
	$output->setNavigationMenu(array($TEXT_COMMANDS_CONST=>'index.php?section=dceCommands'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}


function getCommandCategoryChilds($CommandCategory,$dbADO,$level,$templatesUsedByCommand,$templateNamesUsedByCommand,$devicesUsedByCommand,$deviceNamesUsedByCommand)
{
	$out='';
	$queryCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent=?
			ORDER BY CommandCategory.Description ASC';
	$resCC=$dbADO->Execute($queryCC,$CommandCategory);
	$out.=formatOutput($resCC,$dbADO,$level,$templatesUsedByCommand,$templateNamesUsedByCommand,$devicesUsedByCommand,$deviceNamesUsedByCommand);
	return $out;
}

function formatOutput($resRootCC,$dbADO,$level,$templatesUsedByCommand,$templateNamesUsedByCommand,$devicesUsedByCommand,$deviceNamesUsedByCommand)
{
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/dceCommands.lang.php');
	
	$installationID = (int)@$_SESSION['installationID'];
	$out='';
	$pos=0;
	while($rowRootCC=$resRootCC->FetchRow()){
		$pos++;
		$indent='';
		for($i=0;$i<3*$level;$i++)
			$indent.='&nbsp;-';

		$out.='
			<tr bgcolor="#EEEEEE">
				<td colspan="3">'.$indent.' <a href="#" onClick="windowOpen(\'index.php?section=editCommandCategory&from=dceCommands&ccID='.$rowRootCC['PK_CommandCategory'].'\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');"><B>'.$rowRootCC['Description'].'</B></a>
					<a href="#" onClick="windowOpen(\'index.php?section=addCommand&from=dceCommands&commandCateg='.$rowRootCC['PK_CommandCategory'].'\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">[ Add command ]</a>
				</td>
				<td align="center"><B>'.$TEXT_DEVICE_TEMPLATES_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DEVICES_CONST.'</B></td>
			</tr>';
		$resCommands=$dbADO->Execute('SELECT * FROM Command WHERE FK_CommandCategory=? ORDER BY Description ASC',$rowRootCC['PK_CommandCategory']);
		$cmdPos=0;
		while($rowCommands=$resCommands->FetchRow()){
			$cmdPos++;
			$dtLinks=array();
			for($i=0;$i<@count($templatesUsedByCommand[$rowCommands['PK_Command']]);$i++){
				$dtID=$templatesUsedByCommand[$rowCommands['PK_Command']][$i];
				$description=$templateNamesUsedByCommand[$rowCommands['PK_Command']][$i];
				
				$dtLinks[]='<a href="index.php?section=editMasterDevice&model='.$dtID.'&from=dceCommands">'.$description.'</a>';
			}
			
			$devicesLinks=array();
			for($i=0;$i<@count($devicesUsedByCommand[$rowCommands['PK_Command']]);$i++){
				$deviceID=$devicesUsedByCommand[$rowCommands['PK_Command']][$i];
				$description=$deviceNamesUsedByCommand[$rowCommands['PK_Command']][$i];
				
				$devicesLinks[]='<a href="index.php?section=editDeviceParams&deviceID='.$deviceID.'">'.$description.'</a>';
			}

			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.$rowCommands['PK_Command'].'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=dceCommands&commandID='.$rowCommands['PK_Command'].'\',\'width=400,height=300,toolbar=1,resizable=1,scrollbars=1\');">'.$rowCommands['Description'].'<a></td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.join(', ',$dtLinks).'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.join(', ',$devicesLinks).'</td>
				</tr>';
		}
		$out.=getCommandCategoryChilds($rowRootCC['PK_CommandCategory'],$dbADO,$level+1,$templatesUsedByCommand,$templateNamesUsedByCommand,$devicesUsedByCommand,$deviceNamesUsedByCommand);
	}
	return $out;
}
?>
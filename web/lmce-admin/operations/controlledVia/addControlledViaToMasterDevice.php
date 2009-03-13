<?php
function addControlledViaToMasterDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addControlledViaToMasterDevice.lang.php');

	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	
	
	
	
	
	if ($action=='form') {
		$alreadySelectedControlledVia = array();
		$alreadySelectedControlledVia[]=$deviceID;
		$querySelectControlledViaForDevice = "
			select MDL_CV.FK_DeviceTemplate_ControlledVia
								FROM DeviceTemplate_DeviceTemplate_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 
								INNER JOIN DeviceTemplate MDL2 on FK_DeviceTemplate_ControlledVia  = MDL2.PK_DeviceTemplate
								INNER JOIN DeviceCategory DC on MDL2.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ?
			";
		$rs = $dbADO->Execute($querySelectControlledViaForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedControlledVia[]=$row['FK_DeviceTemplate_ControlledVia'];
		}
		$rs->Close();
		$out.='
		<form action="index.php" method="post" name="addControlledViaToMasterDevice">
		<input type="hidden" name="section" value="addControlledViaToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		
		<div class="err">'.@$_GET['error'].'</div>
			<table>			
				<tr>
					<td>'.$TEXT_CONTROLLED_VIA_CONST.':</td>
					<td>
						'.(@(int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0
								?'You have selected : <b>'.$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'].'</b>&nbsp;&nbsp;<br />'
								:(
									isset($_GET['msg'])
									?'<b>'.strip_tags($_GET['msg']).'</b>'
									:'')
								 ).' 
								 <input type="text" name="ControlledViaDeviceTemplateID" size="20" value="'.(@(int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0?@$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']:'').'">
								 <input type="submit" class="button" name="searchDeviceTemplate" value="'.$TEXT_SEARCH_CONST.' [...]" >
					</td>
				</tr>		
				<tr>
					<td>'.$TEXT_REROUTE_MESSAGES_TO_PARENT_CONST.':</td>
					<td>
						<input name="RerouteMessagesToParent" value="1" type="checkbox">
					</td>
				</tr>								 
				<tr>
					<td>'.$TEXT_AUTO_CREATE_CHILDREN_CONST.':</td>
					<td>
						<input name="AutoCreateChildren" value="1" type="checkbox">
					</td>
				</tr>					
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		';
		
	} else {
		$controlledViaDeviceTemplateID = cleanInteger($_POST['ControlledViaDeviceTemplateID']);
		
		$controlledVia = isset($_POST['controlledVia'])?cleanString($_POST['controlledVia']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';		
		$reroute = cleanInteger(@$_POST['RerouteMessagesToParent']);
		$autocreate = cleanInteger(@$_POST['AutoCreateChildren']);
		
		
				
		$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] = 0;
		$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'] = '';
		
			$querySelectRemainingControlledViaForDevice = "
							select  PK_DeviceTemplate,Description from DeviceTemplate 
							where PK_DeviceTemplate = ?";
						$rs = $dbADO->Execute($querySelectRemainingControlledViaForDevice,array($controlledViaDeviceTemplateID));
						while ($row = $rs->FetchRow()) {
								$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] = $row['PK_DeviceTemplate'];
								$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'] = $row['Description'];
						}
			$rs->Close();
			
		if (isset($_POST['searchDeviceTemplate'])) {
			if ($_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] !=0) {
				header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID");
				exit();
			} else {
				header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID&msg=Invalid+ID");
				exit();				
			}
		} 
		
		
		//RerouteMessagesToParent AutoCreateChildren
		
		if ((int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0 && (int)$deviceID!=0) {
			$resIsControlled=$dbADO->Execute('SELECT * FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate=? AND FK_DeviceTemplate_ControlledVia=?',array($deviceID,$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']));
			if($resIsControlled->RecordCount()==0){
				$insertObjToDevice = 'insert into DeviceTemplate_DeviceTemplate_ControlledVia
						(FK_DeviceTemplate, FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren) 
						values(?,?,?,?)';
							
				$query = $dbADO->Execute($insertObjToDevice,array($deviceID,$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'],$reroute,$autocreate));
				$lastInsert = $dbADO->Insert_ID();
				
				if ($autocreate==1) {
					
					header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$lastInsert");
					exit();
				}
			}else{
				header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID&error=$TEXT_ERROR_AREADY_CONTROLLED_BY_DT_CONST");
				exit();
			}
						
			$out.="
			<script>
				alert('$TEXT_CONTROLLED_VIA_ADDED_CONST');			
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_CONTROLLED_VIA_DEVICE_TEMPLATE_CONST);			
	$output->output();
}
?>

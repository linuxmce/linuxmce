<?php
function tunerConfig($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/tunerConfig.lang.php');
	
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];
	$dtID=$_REQUEST['dtID'];
	$categoryID=$_REQUEST['categoryID'];


	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="tunerConfig">
	<input type="hidden" name="section" value="tunerConfig">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="dtID" value="'.$dtID.'">	
	<input type="hidden" name="categoryID" value="'.$categoryID.'">	
		
	<div align="center"><B>'.$TEXT_TUNER_SETUP_CONST.'</B></div>	
	<br><br>';
	if($categoryID!=$GLOBALS['AmpsReceivers']){
		$out.='
	<table>
		<tr>
			<td align="center" colspan="2" class="err"><B>'.$TEXT_TUNER_WARNING_CONST.'</B></td>
		</tr>	
		<tr>
			<td align="right"><B>'.$TEXT_TUNNERS_QUESTION_CONST.'</B></td>
			<td><select name="tunerNo">
				<option value="1">1</option>
				<option value="2">2</option>
				<option value="3">3</option>
			</select>
		</td>
		</tr>	
		<tr>
			<td colspan="2">'.$TEXT_TUNERS_INFO_CONST.'</td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_SAVE_CONST.'"></td>
		</tr>		
	</table>';
	}else{
	$out.='	<table>
		<tr>
			<td align="center" colspan="2" class="err"><B>'.$TEXT_TUNER_WARNING_CONST.'</B></td>
		</tr>	
		<tr>
			<td align="center" colspan="2"><B>'.$TEXT_TUNER_CONST.':</B> <input type="checkbox" name="command[]" value="'.$GLOBALS['AM_Command'].'" checked> AM <input type="checkbox" name="command[]" value="'.$GLOBALS['FM_Command'].'" checked> FM <input type="checkbox" name="command[]" value="'.$GLOBALS['WB_Command'].'"> WB <input type="checkbox" name="command[]" value="'.$GLOBALS['SW_Command'].'"> SW</td>
		</tr>	
		<tr>
			<td colspan="2">'.$TEXT_BUILTIN_TUNER_CONST.'</td>
		</tr>	
		<tr>
			<td colspan="2" align="center">&nbsp;</td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="'.$TEXT_TUNER_CONFIRMATION_CONST.'"> <input type="button" class="button" onClick="self.close();" value="'.$TEXT_TUNER_INFIRMATION_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
		</tr>		
	</table>';
	}
		
$out.='		
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=tunerConfig&dtID=$dtID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		$controlPipeQuery='
			INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
				(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
			VALUES 
				(?,?,?)';
		$dt_InputQuery='INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)';

		if($categoryID!=$GLOBALS['AmpsReceivers']){
			$tunerNo=(int)$_POST['tunerNo'];
		
			for($i=1;$i<=$tunerNo;$i++){
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
						(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
					VALUES
						(?,?,1,1)',array($GLOBALS['Tuner'],$dtID));
				$insertID=$dbADO->Insert_ID();
				$GLOBALS['tuner_'.$i]=($GLOBALS['tuner_'.$i]=='' || $GLOBALS['tuner_'.$i]==0)?NULL:$GLOBALS['tuner_'.$i];
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
						(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
					VALUES 
						(?,?,?)',array($insertID,1,$GLOBALS['tuner_'.$i]));
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
						(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
					VALUES 
						(?,?,?)',array($insertID,2,$GLOBALS['tuner_'.$i]));
				$dbADO->Execute($dt_InputQuery,array($dtID,$GLOBALS['tuner_'.$i]));
			}
		}else{
			$commandArray=$_POST['command'];
			foreach ($commandArray AS $command){
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
						(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
					VALUES
						(?,?,1,1)',array($GLOBALS['Tuner'],$dtID));
				$insertID=$dbADO->Insert_ID();
				
				$command=($command=='')?NULL:$command;
				$dbADO->Execute($controlPipeQuery,array($insertID,1,$command));
				$dbADO->Execute($controlPipeQuery,array($insertID,2,$command));
				$dbADO->Execute($dt_InputQuery,array($dtID,$command));
			}
		}	
		$out.='
		<script>
			self.close();
		</script>';
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

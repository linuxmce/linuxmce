<?
function infraredCommands($output,$publicADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/infraredCommands.lang.php');
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$GLOBALS['excludedCategories']=array($GLOBALS['DSPModeCommandCategory'], $GLOBALS['InputsCommandCategory'],$GLOBALS['OutputsCommandCategory']);
	$GLOBALS['excludedCommands']=array($GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand'],$GLOBALS['powerCommand']);
	
	// if page was called from Ruby codes, no restrictions
	if(isset($_REQUEST['norestrict'])){
		$GLOBALS['excludedCategories']=array(0);
		$GLOBALS['excludedCommands']=array(0);
	}
	$GLOBALS['infraredGroup']=(int)@$_REQUEST['infraredGroup'];	
	$GLOBALS['deviceID']=((int)@$_REQUEST['deviceID']!=0)?(int)@$_REQUEST['deviceID']:NULL;	
	$GLOBALS['dtID']=(int)@$_REQUEST['dtID'];	
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<h3 align="center">'.$TEXT_COMMANDS_CONST.'</h3>
			<form action="index.php" method="POST" name="infraredCommands">
			<input type="hidden" name="section" value="infraredCommands">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="infraredGroup" value="'.$GLOBALS['infraredGroup'].'">
			<input type="hidden" name="deviceID" value="'.$GLOBALS['deviceID'].'">
			<input type="hidden" name="dtID" value="'.$GLOBALS['dtID'].'">
		
		<table cellpadding="3" align="center" class="normaltext">
			<tr class="tablehead">
				<td align="center" width="120"><B>'.$TEXT_COMMAND_CATEGORY_CONST.'</B></td>
				<td><B>#</B></td>
				<td align="center"><B>'.$TEXT_COMMAND_DESCRIPTION_CONST.'</b></td>
				<td align="center"><B>'.$TEXT_ACTION_CONST.'</b></td>
			</tr>
		';
		$GLOBALS['oldCheckedCommands']=array();
		$queryOld='SELECT DISTINCT FK_Command FROM InfraredGroup_Command WHERE FK_InfraredGroup=?';
		$resOld=$publicADO->Execute($queryOld,array($GLOBALS['infraredGroup']));
		while($row=$resOld->FetchRow()){
			$GLOBALS['oldCheckedCommands'][]=$row['FK_Command'];
		}

		$GLOBALS['displayedCommands']=array();
		//$publicADO->debug=true;
		$query='
			SELECT DISTINCT
				Command.Description,
				PK_Command, 
				CommandCategory.PK_CommandCategory, 
				CommandCategory.Description AS CommandCategory ,
				Parent.Description AS Parent,if(Parent.Description IS NULL,CommandCategory.Description,concat(Parent.Description,\'-\',CommandCategory.Description)) AS Categ
			FROM Command
			INNER JOIN CommandCategory ON FK_CommandCategory=CommandCategory.PK_CommandCategory 
			LEFT JOIN CommandCategory Parent ON CommandCategory.FK_CommandCategory_Parent=Parent.PK_CommandCategory
			WHERE '.((isset($_REQUEST['rootNode']))?'1=1':'CommandCategory.FK_CommandCategory_Parent='.$GLOBALS['AVCommandCategory']).' AND CommandCategory.PK_CommandCategory NOT IN ('.join(',',$GLOBALS['excludedCategories']).') 
			ORDER BY Categ,CommandCategory.Description ASC';
		$resRootCC=$publicADO->Execute($query);
		$cc=0;
		$cmdPos=0;
		while($rowRootCC=$resRootCC->FetchRow()){
			$GLOBALS['displayedCommands'][]=$rowRootCC['PK_Command'];
			if($cc!=$rowRootCC['PK_CommandCategory']){
				$cc=$rowRootCC['PK_CommandCategory'];
				$cmdPos=0;
				$out.='
				<tr bgcolor="#EEEEEE">
					<td colspan="4"><B>'.$rowRootCC['Categ'].'</B></td>
				</tr>';
			}
			$cmdPos++;
			
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.$rowRootCC['PK_Command'].'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.$rowRootCC['Description'].'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'" align="center"><input type="checkbox" name="command_'.$rowRootCC['PK_Command'].'" value="1" '.((in_array($rowRootCC['PK_Command'],$GLOBALS['oldCheckedCommands']))?'checked':'').'></td>
				</tr>';
		}

		$out.='
			<tr>
				<td colspan="4" align="center"><input type="submit" name="submit" class="button" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>		
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="oldCheckedCommands" value="'.join(',',$GLOBALS['oldCheckedCommands']).'">
		</form>';
	}else{
	// process area
		$displayedCommands=explode(',',$_POST['displayedCommands']);
		$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);

		foreach ($displayedCommands AS $commandID){
			if(isset($_POST['command_'.$commandID])){
				$GLOBALS['infraredGroup']=($GLOBALS['infraredGroup']==0)?NULL:$GLOBALS['infraredGroup'];
				if(!in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command) VALUES (?,?)',array($GLOBALS['infraredGroup'],$commandID));
					
					$igcID=$publicADO->Insert_ID();
				}
			}else{
				if(in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE FK_InfraredGroup=? AND FK_Command=? ',array($GLOBALS['infraredGroup'],$commandID));
				}
			}
		}
		
		$out.='
			<script>
				opener.location.reload();
				self.close();
			</script>';
		//header('Location: index.php?section=infraredCommands&infraredGroup='.$GLOBALS['infraredGroup'].'&msg=InfraredGroup - Commands updated');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_COMMANDS_CONST);
	$output->output();
}
?>
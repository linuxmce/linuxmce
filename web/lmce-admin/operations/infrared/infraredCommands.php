<?php
function infraredCommands($output,$publicADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/infraredCommands.lang.php');
	
	define('SORT_CATEGORY', 0);
	define('SORT_COMMAND', 	1);
	define('SORT_ID', 	2);
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$GLOBALS['excludedCategories']=array($GLOBALS['DSPModeCommandCategory'], $GLOBALS['InputsCommandCategory'],$GLOBALS['OutputsCommandCategory']);
	$GLOBALS['excludedCommands']=array($GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand'],$GLOBALS['powerCommand']);
	
	$sort_options=(isset($_REQUEST['sort_options'])?$_REQUEST['sort_options']:0);
	$sortOptions=array('Command Category','Command Description','ID Number');

	$GLOBALS['newCheckedCommands']=array();
	$GLOBALS['newCheckedCommands']=explode(',',$_REQUEST['newCheckedCommands']);

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
			';
		$out.='<form action="index.php" method="POST" name="infraredCommands">
			<input type="hidden" name="section" value="infraredCommands">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="infraredGroup" value="'.$GLOBALS['infraredGroup'].'">
			<input type="hidden" name="deviceID" value="'.$GLOBALS['deviceID'].'">
			<input type="hidden" name="dtID" value="'.$GLOBALS['dtID'].'">
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">'.$TEXT_COMMANDS_CONST.'</h3>
			<div align="center"><b>Sort by:</b> '.pulldownFromArray($sortOptions,"sort_options",$sort_options,'style="width:175px;"').'<input type="submit" name="submitSort" class="button" value="Go"></div><br>
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
			WHERE '.((isset($_REQUEST['rootNode']))?'1=1':'CommandCategory.FK_CommandCategory_Parent='.$GLOBALS['AVCommandCategory']).' AND CommandCategory.PK_CommandCategory NOT IN ('.join(',',$GLOBALS['excludedCategories']).') '; 
		switch ($sort_options) {			
			case SORT_CATEGORY:	
				$query.='ORDER BY Categ,Command.Description ASC';
				$out.=CategoryTable($publicADO,$query) ;
				break;
			case SORT_COMMAND:
				$query.='ORDER BY Command.Description ASC';
				$out.=CommandTable($publicADO,$query) ;
				break;
			case SORT_ID:
				$query.='ORDER BY PK_Command ASC';
				$out.=IDTable($publicADO,$query) ;
				break;
		}
		
		
		$out.='
			<tr>
				<td colspan="4" align="center"><input type="submit" name="submit" class="button" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>		
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="oldCheckedCommands" value="'.join(',',$GLOBALS['oldCheckedCommands']).'">
			<input type="hidden" name="newCheckedCommands" value="'.join(',',$GLOBALS['newCheckedCommands']).'">
		</form>';
	}else{
		$displayedCommands=explode(',',$_POST['displayedCommands']);
		$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);
		$newCheckedCommands=explode(',',$_POST['newCheckedCommands']);
		$deviceID=$_POST[deviceID];
		$dtID=$_POST['dtID'];
		if(isset($_POST['submit'])) {
			// Save Button Was Clicked
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
		}else if(isset($_POST['sort_options'])) {
			//Sort Button Was Clicked
			$GLOBALS['newCheckedCommands']=array();
			foreach ($displayedCommands AS $commandID){

				if( isset($_POST['command_'.$commandID]) && !in_array($commandID,$oldCheckedCommands)  ){
					$GLOBALS['newCheckedCommands'][]=$commandID;
				}
			}

			$sort_options=(isset($_POST['sort_options'])?$_POST['sort_options']:0);

			header('Location: index.php?section=infraredCommands&infraredGroup='.$GLOBALS['infraredGroup'].'&deviceID='.$deviceID.'&dtID='.$dtID.'&sort_options='.$sort_options.'&oldCheckedCommands='.$_POST['oldCheckedCommands'].'&newCheckedCommands='.join(',',$GLOBALS['newCheckedCommands']).'&msg=');
			exit();
		}
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_COMMANDS_CONST);
	$output->output();
}

function CategoryTable($publicADO,$query) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/infraredCommands.lang.php');

	$out='<table cellpadding="3" align="center" class="normaltext">
		<tr class="tablehead">
		<td align="center" width="175"><B>'.$TEXT_COMMAND_CATEGORY_CONST.'</B></td>
		<td align="center" width="30"><B>ID</B></td>
		<td align="center" width="175"><B>'.$TEXT_COMMAND_DESCRIPTION_CONST.'</b></td>
		<td align="center" width="20"><B>'.$TEXT_ACTION_CONST.'</b></td>
		<td align="left" width="10">New?</td>
		</tr>';
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
		
		$existingCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['oldCheckedCommands']) || in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands']) ?true:false);	
		$newCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands'])?true:false);
		$out.='
		<tr>
			<td>&nbsp;</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['PK_Command'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['Description'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center"><input type="checkbox" name="command_'.$rowRootCC['PK_Command'].'" value="1" '.( $existingCommand ?'checked':'').'></td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center" class="err"><B>'.($newCommand?'*':'&nbsp;').'</b></td>
		</tr>';
		}
	return $out;
}

function CommandTable($publicADO,$query) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/infraredCommands.lang.php');

	$out='<table cellpadding="3" align="center" class="normaltext">
		<tr class="tablehead">
		<td align="center" width="175"><B>'.$TEXT_COMMAND_DESCRIPTION_CONST.'</B></td>
		<td align="center" width="175"><B>'.$TEXT_COMMAND_CATEGORY_CONST.'</b></td>
		<td align="center" width="30"><B>ID</B></td>
		<td align="center" width="20"><B>'.$TEXT_ACTION_CONST.'</b></td>
		<td align="left" width="10">New?</td>
		</tr>';
	$resRootCC=$publicADO->Execute($query);
	$cc=0;
	$cmdPos=0;
	while($rowRootCC=$resRootCC->FetchRow()){
		$GLOBALS['displayedCommands'][]=$rowRootCC['PK_Command'];
		$cmdPos++;
		$existingCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['oldCheckedCommands']) || in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands']) ?true:false);	
		$newCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands'])?true:false);
		$out.='
		<tr>
			
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'"><B>'.$rowRootCC['Description'].'</B></td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['Categ'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['PK_Command'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center"><input type="checkbox" name="command_'.$rowRootCC['PK_Command'].'" value="1" '.( $existingCommand ?'checked':'').'></td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center" class="err"><B>'.($newCommand?'*':'&nbsp;').'</b></td>	
		</tr>';
		}
	return $out;
}

function IDTable($publicADO,$query) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/infraredCommands.lang.php');

	$out='<table cellpadding="3" align="center" class="normaltext">
		<tr class="tablehead">
		<td align="center" width="30"><B>ID</B></td>
		<td align="center" width="175"><B>'.$TEXT_COMMAND_DESCRIPTION_CONST.'</B></td>
		<td align="center" width="175"><B>'.$TEXT_COMMAND_CATEGORY_CONST.'</b></td>
		<td align="center" width="20"><B>'.$TEXT_ACTION_CONST.'</b></td>
		<td align="left" width="10">Mew?</td>
		</tr>';
	$resRootCC=$publicADO->Execute($query);
	$cc=0;
	$cmdPos=0;
	
	while($rowRootCC=$resRootCC->FetchRow()){
		$GLOBALS['displayedCommands'][]=$rowRootCC['PK_Command'];
		$cmdPos++;
		$existingCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['oldCheckedCommands']) || in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands']) ?true:false);	
		$newCommand=(in_array($rowRootCC['PK_Command'],$GLOBALS['newCheckedCommands'])?true:false);
		$out.='
		<tr>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'"><B>'.$rowRootCC['PK_Command'].'</B></td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['Description'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'">'.$rowRootCC['Categ'].'</td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center"><input type="checkbox" name="command_'.$rowRootCC['PK_Command'].'" value="1" '.( $existingCommand ?'checked':'').'></td>
			<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#DDE7DD').'" align="center" class="err"><B>'.($newCommand?'*':'&nbsp;').'</b></td>
		</tr>';
		}
	return $out;
}
?>

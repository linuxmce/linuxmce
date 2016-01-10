<?php
function myScenarios($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/myScenarios.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'showBasicInfo';
$out='';
$installationID = (int)@$_SESSION['installationID'];

$out.='<div align="left" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br><br>';
if ($action == 'showBasicInfo') {
	$out.=$TEXT_MY_SCENARIOS_NOTE_CONST;	

	$out.='<hr><input type="button" class="button" name="sortScenarios" value="'.$TEXT_SORT_SCENARIOS_CONST.'"  onclick="self.location=\'index.php?section=sortScenarios&sortBy=Room&from=myScenarios\'">';
	
} elseif($action=='showArrayCommands') {
	$arrayID = cleanInteger(@$_REQUEST['array']);	
	$queryGetCommands = 'SELECT * FROM CommandGroup WHERE FK_Installation = ? AND FK_Array = ?';
	$resGetCommands = $dbADO->Execute($queryGetCommands,array($installationID,$arrayID));

	while ($rowGetCommands = $resGetCommands->FetchRow()) {
		$out.='<a href="index.php?section=editCommandGroup&cgID='.$rowGetCommands['PK_CommandGroup'].'">'.$rowGetCommands['Description'].'</a> <br />';
	}
	
	$out.='
	<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
	</script>
	<input type="button" class="button" name="addCommandGroup" value="'.$TEXT_ADD_MY_SCENARIO_CONST.'" onClick="windowOpen(\'index.php?section=addCommandGroupAsScenario&from=myScenarios&arrayID='.$arrayID.'\',\'width=600,height=600,toolbar=1,resizable=1,scrollbars=1\');"">
	';
							
}
	



	$output->setNavigationMenu(array($TEXT_MY_SCENARIOS_CONST=>'index.php?section=myScenarios'));
	if(isset($_REQUEST['msg']))
		$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setScriptCalendar('null');
	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_MY_SCENARIOS_CONST);
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MY_SCENARIOS_CONST);			
	$output->output(); 
}
?>

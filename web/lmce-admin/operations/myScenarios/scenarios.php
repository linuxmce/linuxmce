<?php
function scenarios($output,$dbADO) {
	// obsolete file
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'showBasicInfo';
$out='';
$installationID = (int)@$_SESSION['installationID'];


if ($action == 'showBasicInfo') {
	$out='
		You can write something about what he can do here, in my scenarios area.
	';	
}elseif($action == 'del'){
	// process delete here
	// ??? what else should be deleted
	$out.="
	<script>
		self.location='index.php?section=scenarios&action=showArrayCommands&array=1';
	</script>
	";
} 
elseif($action=='showArrayCommands') {
	$arrayID = cleanInteger(@$_REQUEST['array']);	
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();
	
	$out.='<h2>'.$rowArray['Description'].'</h2>';

	$out.='<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	$queryRooms='
		SELECT Room.*, Room.Description AS RoomName FROM Room
		WHERE FK_Installation=?
			ORDER BY Room.Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	while($rowRooms=$resRooms->FetchRow()){
		$out.='
		<tr bgcolor="#D1D9EA">
			<td colspan="3">'.$rowRooms['RoomName'].'</td>
		</tr>';
		$queryGetCommands = '
			SELECT 
				CommandGroup.*,
				CommandGroup.Description AS CommandGroupDescription
			FROM CommandGroup
				INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
				INNER JOIN Room ON FK_Room=PK_Room
			WHERE CommandGroup.FK_Installation = ? AND FK_Array = ? AND PK_Room=?
				ORDER BY CommandGroup.Description ASC';
		//$dbADO->debug=true;
		$lineCount=0;
		$resGetCommands = $dbADO->Execute($queryGetCommands,array($installationID,$arrayID,$rowRooms['PK_Room']));
		while ($rowGetCommands = $resGetCommands->FetchRow()) {
			$lineCount++;
			$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;'.$rowGetCommands['CommandGroupDescription'].'</td>
				<td><a href="index.php?section=wizardScenarios&cgID='.$rowGetCommands['PK_CommandGroup'].'&installationID='.$installationID.'">Edit</a> </td>
				<td><a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete '.$rowGetCommands['CommandGroupDescription'].' from '.$rowRooms['RoomName'].'?\')) self.location=\'index.php?section=scenarios&cgID='.$rowGetCommands['PK_CommandGroup'].'&action=del\';">Delete</a> </td>
			</tr>
		';
		}
		$out.='
			<tr>
				<td colspan="3"><input type="button" class="button" name="addCommandGroup" value="Add '.$rowArray['Description'].'" onClick="windowOpen(\'index.php?section=addCommandGroupAsScenario&from=myScenarios&arrayID='.$arrayID.'\',\'width=600,height=600,toolbar=1,resizable=1,scrollbars=1\');""></td>
			</tr>
		';
	}
	
	$out.='
		</table>
	<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
	</script>
	';
							
}
	



	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Lightning Scenarios"=>'index.php?section=scenarios&action=showArrayCommands&array=1'));

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();

}
?>

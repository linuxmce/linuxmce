<?php
function entertainArea($output,$dbADO) {
	// obsolete file
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
/* @var $resRoomTypes ADORecordSet */

$out='';
$locationGoTo='';
//$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';

if (@$_SESSION['userLoggedIn']!=true) {
	header("Location: index.php?section=login&from=rooms");
}

$installationID = cleanInteger($_SESSION['installationID']);

$out.='<h3>Entertain Area</h3>';

if ($action=='form') {
	

$queryEntertainArea = 'SELECT EntertainArea.Description as EA_Desc,PK_EntertainArea,FK_Room,Room.Description as R_Desc 
				FROM Room INNER JOIN EntertainArea ON PK_Room=FK_Room
			   WHERE Room.FK_Installation = ?
			  ';

$resEntertainArea= $dbADO->Execute($queryEntertainArea,array($installationID));


$queryRooms = 'SELECT PK_Room,Description FROM Room WHERE FK_Installation = ? ORDER BY Description ASC';
$resRooms = $dbADO->Execute($queryRooms,array($installationID));

	if ($resEntertainArea) {
		$out.="
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,'',attributes);
			}
		</script>";
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.="
		<table>
			<form action='index.php' method='post' name='entertainArea'>
			<input type='hidden' name='section' value='entertainArea'>
			<input type='hidden' name='action' value='add'>
			<input type='hidden' name='lastAction' value=''>
		";
		
		$entertainAreaFormValidation='';
		$displayedEntertainArea=array();
		while ($rowEntertainArea = $resEntertainArea->FetchRow()) {
			
			$out.='<tr><td><input type="text" name="entAreaDesc_'.$rowEntertainArea['PK_EntertainArea'].'" value="'.$rowEntertainArea['EA_Desc'].'"></td><td>';
			$displayedEntertainArea[]=$rowEntertainArea['PK_EntertainArea'];
			
			$resRooms->MoveFirst();
			$out.='<select name="room_'.$rowEntertainArea['PK_EntertainArea'].'">';
			while ($rowRooms = $resRooms->FetchRow()) {
				$out.='<option '.($rowRooms['PK_Room']==$rowEntertainArea['FK_Room']?' selected ':'').' value="'.$rowRooms['PK_Room'].'">'.$rowRooms['Description'].'</option>';
			}			
			$out.='</select>';
			$out.="</td><td>
				<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteRoomFromEntertainArea&from=entertainArea&roomID={$rowEntertainArea['PK_EntertainArea']}','status=0,resizable=1,width=200,height=200,toolbar=1');\">Delete</a>
			</td>";
			$out.='</tr>';
			$entertainAreaFormValidation.='
				frmvalidator.addValidation("room_'.$rowEntertainArea['PK_EntertainArea'].'","req","Please enter a description");
			';
			
		}
		
		$resRooms->MoveFirst();			
		$roomsRemainingTxt='<select name="addNewRoomToEntertainArea">
			<option value="0">-please select-</option>
		';
			while ($rowRooms= $resRooms->FetchRow()) {				
				$roomsRemainingTxt.='<option value="'.$rowRooms['PK_Room'].'">'.$rowRooms['Description'].'</option>';				
			}			
		$resRooms->Close();
		
		$out.='</select>';
			
		$out.='
			<tr><td colspan="2"> '.$roomsRemainingTxt.' <input type="submit" class="button" name="submitX" value="Add"  ></td></tr>
			<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="Submit"  ></td></tr>
			<input type="hidden" name="displayedEntertainArea" value="'.join(",",$displayedEntertainArea).'">
			</form>
		
		<script>
		 	var frmvalidator = new formValidator("entertainArea"); 		
			'.$entertainAreaFormValidation.'
		</script>
		
		</table>';
	}
} else {
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=entertainArea&error=You are not authorised to change this installation!");
		exit(0);
	}
	
	//process
	$addNewRoomToEntertainArea = cleanInteger($_POST['addNewRoomToEntertainArea']);
	
	if ($addNewRoomToEntertainArea!=0) {
		$queryInsertRoom = 'INSERT INTO EntertainArea (FK_Room) values(?)';
		$res = $dbADO->Execute($queryInsertRoom,array($addNewRoomToEntertainArea));
		$lastInsert = $dbADO->Insert_ID();
		$locationGoTo = "entAreaDesc_{$lastInsert}";		
	}
	
	$displayedEntertainArea = cleanString($_POST['displayedEntertainArea']);

	$displayedEntertainAreaArray = explode(",",$displayedEntertainArea);
	if (!is_array($displayedEntertainAreaArray) || $displayedEntertainAreaArray===array()) {
		$displayedEntertainAreaArray=array();
	}
	
	foreach ($displayedEntertainAreaArray as $room) {
		$queryOldValue = "SELECT Description, FK_Room FROM EntertainArea WHERE PK_EntertainArea = ?";
		$resOldValue = $dbADO->Execute($queryOldValue,array($room));
		$oldDesc = '';
		$oldRoomType = '';

		if ($resOldValue) {
			$rowOldValue = $resOldValue->FetchRow();
			$oldDesc = $rowOldValue['Description'];
			$oldRoomType = $rowOldValue['FK_Room'];
		}

		$newDesc = cleanString(@$_POST['entAreaDesc_'.$room]);
		$newRoomType = cleanString(@$_POST['room_'.$room]);

		if ($newDesc!=$oldDesc || $oldRoomType!=$newRoomType)  {
			$query = 'UPDATE EntertainArea set Description=?,FK_Room=? WHERE PK_EntertainArea = ?';
			$resUpdRoom = $dbADO->Execute($query,array($newDesc,$newRoomType,$room));
			$locationGoTo = "entAreaDesc_".$room;
		}
	}
	
	if (strstr($locationGoTo,"#")) {
		header("Location: index.php?section=entertainArea&msg=Saved!".$locationGoTo);
	} else {
		header("Location: index.php?section=entertainArea&msg=Saved!&lastAction=".$locationGoTo);
	}
	
}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.entertainArea.{$lastAction}) {document.forms.entertainArea.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setNavigationMenu(array("Rooms"=>'index.php?section=rooms',"Entertain Area"=>'index.php?section=entertainArea'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Entertain area');			
	$output->output();
	
}
?>

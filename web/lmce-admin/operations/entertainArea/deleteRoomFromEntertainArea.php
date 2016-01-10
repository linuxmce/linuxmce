<?php
function deleteRoomFromEntertainArea($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);	
	$entertainAreaID = (int)$_REQUEST['roomID'];
	$installationID = cleanInteger($_SESSION['installationID']);
	
		
	if ($installationID!=0 && $entertainAreaID!=0) {
			
			$deleteRoomFromEntertainArea= 'delete from EntertainArea where PK_EntertainArea = ?';
			$query = $dbADO->Execute($deleteRoomFromEntertainArea,array($entertainAreaID));
			
			$out.="
			<script>
				alert('Room removed from entertain area!');
			    	opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}		

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
	
}


?>
<?php
function deleteRoomFromInstallation($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('deleteRoomFromInstallation.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);	
	$roomID = (int)$_REQUEST['roomID'];
	$installationID = cleanInteger($_SESSION['installationID']);
	
	
	if ($installationID!=0 && $roomID!=0) {
			
			$deleteRoomFromInstallation = 'delete from Room where PK_Room = ?';
			$query = $dbADO->Execute($deleteRoomFromInstallation,array($roomID));
			
			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
					
			$out.="
			<script>
				alert('".translate('TEXT_ROOM_DELETED_CONST')."');
			    	opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = '<a href="javascript:window.close();">'.translate('TEXT_CLOSE_CONST').'</a>';
		}		

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
	
}


?>
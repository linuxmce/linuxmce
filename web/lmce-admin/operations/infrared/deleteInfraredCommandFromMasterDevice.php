<?php
function deleteInfraredCommandFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$commandID = (int)$_REQUEST['commandID'];
	$infraredGroupID = (int)$_REQUEST['infraredGroupID'];
	
		
	if ($commandID!=0 && $infraredGroupID!=0) {
			$deleteObjFromDevice = 'delete from InfraredGroup_Command where FK_Command = ? and FK_InfraredGroup= ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($commandID,$infraredGroupID));
			$out.="
			<script>
				alert('Comand deleted!');
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
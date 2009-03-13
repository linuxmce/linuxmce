<?php
function deletePackageDistro($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$FK_Package = cleanInteger(@$_REQUEST['Package']);
	$FK_Distro = cleanInteger(@$_REQUEST['Distro']);
	
	$deviceID = (int)@$_REQUEST['deviceID'];

	$deleteDirectoryFile = 'DELETE FROM Package_Distro WHERE FK_Package=? AND FK_Distro=?';
	$dbADO->Execute($deleteDirectoryFile,array($FK_Package,$FK_Distro));
			
	$out.="
				<script>
					alert('Distro deleted from package!');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.subAction.value='edit';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function deleteDirectoryFile($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteDirectoryFile.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$FK_Package_Directory = cleanInteger(@$_REQUEST['Package_Directory']);
	$oldFile=cleanString(@$_REQUEST['oldFile'],30);
	
	$deviceID = (int)@$_REQUEST['deviceID'];

	$deleteDirectoryFile = 'DELETE FROM Package_Directory_File WHERE FK_Package_Directory=? AND File=?';
	$dbADO->Execute($deleteDirectoryFile,array($FK_Package_Directory,$oldFile));
			
	$out.="
				<script>
					alert('$TEXT_FILE_DELETED_CONST');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
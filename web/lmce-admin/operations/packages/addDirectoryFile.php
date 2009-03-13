<?php
function addDirectoryFile($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addDirectoryFile.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$FK_Package_Directory = cleanInteger(@$_REQUEST['Package_Directory']);
	
	$deviceID = (int)@$_REQUEST['deviceID'];
		
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="addDirectoryFile">
		<input type="hidden" name="section" value="addDirectoryFile">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="Package_Directory" value="'.$FK_Package_Directory.'">
			<h3>'.$TEXT_ADD_FILE_TO_DIRECTORY_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_FILE_CONST.' *</td>
					<td><input type="text" size="15" name="File" value=""></td>
				</tr>
				<tr>
					<td>'.$TEXT_SEARCH_CONST.'</td>
					<td><textarea name="Search" rows="10" colls="30"></textarea></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addDirectoryFile");
 			frmvalidator.addValidation("File","req","'.$TEXT_FILENAME_REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$File = cleanString(@$_POST['File'],30);
		$Search = cleanString(@$_POST['Search'],1000);
		
		if ($File!='') {
			$queryInsertDirectoryFile = 'INSERT INTO Package_Directory_File (FK_Package_Directory,File,Search) values(?,?,?)';
			$dbADO->Execute($queryInsertDirectoryFile,array($FK_Package_Directory,$File,$Search));
			
			$out.="
				<script>
					alert('$TEXT_FILE_ADDED_CONST');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=addPackageToMasterDevice&from=$from&deviceID=$deviceID&subAction=edit");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_FILE_TO_DIRECTORY_CONST);			
	$output->output();
}
?>

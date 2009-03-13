<?php
function editDirectoryFile($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDirectoryFile.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$PK_Package_Directory_File = cleanInteger(@$_REQUEST['Package_Directory_File']);
	
	$deviceID = (int)@$_REQUEST['deviceID'];

	$queryPackage_Directory_File='SELECT * FROM Package_Directory_File WHERE PK_Package_Directory_File=?';
	$resPackage_Directory_File=$dbADO->Execute($queryPackage_Directory_File,$PK_Package_Directory_File);
	$rowPackage_Directory_File = $resPackage_Directory_File->FetchRow();
	
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editDirectoryFile">
		<input type="hidden" name="section" value="editDirectoryFile">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="Package_Directory_File" value="'.$PK_Package_Directory_File.'">
		
			<h3>'.$TEXT_EDIT_DIRECTORY_FILE_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_FILE_CONST.' *</td>
					<td><input type="text" size="15" name="File" value="'.$rowPackage_Directory_File['File'].'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_SEARCH_CONST.':</td>
					<td><textarea name="Search" rows="10" colls="30">'.$rowPackage_Directory_File['Search'].'</textarea></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("editDirectoryFile");
 			frmvalidator.addValidation("File","req","'.$TEXT_FILENAME_REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$File = cleanString(@$_POST['File'],30);
		$Search = cleanString(@$_POST['Search'],1000);
		
		if ($File!='') {
			$updateDirectoryFile = 'UPDATE Package_Directory_File SET File=?,Search=? WHERE PK_Package_Directory_File=?';
			$dbADO->Execute($updateDirectoryFile,array($File,$Search,$PK_Package_Directory_File));
			
			$out.="
				<script>
					alert('$TEXT_FILE_UPDATED_CONST');					
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
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DIRECTORY_FILE_CONST);			
	$output->output();
}
?>

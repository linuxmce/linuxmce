<?php
function addDirectory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addDirectory.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
		
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="addDirectory">
		<input type="hidden" name="section" value="addDirectory">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<h3>'.$TEXT_ADD_DIRECTORY_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_DIRECTORY_DESCRIPTION_CONST.' *</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addDirectory");
 			frmvalidator.addValidation("Description","req","'.$TEXT_DIRECTORY_DESCRIPTION_REQUIRED_CONST.'");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$description = cleanString(@$_POST['Description'],30);
		
		if ($description!='') {
			$queryInsertDistro = 'INSERT INTO Directory (Description) values(?)';
			$dbADO->Execute($queryInsertDistro,array($description));
			
			$out.="
				<script>
					alert('Directory added!');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=addDirectory");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_DIRECTORY_CONST);			
	$output->output();
}
?>

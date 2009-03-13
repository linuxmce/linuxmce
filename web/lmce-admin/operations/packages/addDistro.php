<?php
function addDistro($output,$dbADO) {
	// obsolete file
	
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
		<form action="index.php" method="post" name="addDistro">
		<input type="hidden" name="section" value="addDistro">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Distro description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>
				<tr>
					<td>Operating System:</td>
					<td><select name="FK_OperatingSystem">
						<option value="0">-please select-</option>
						';
						$queryOperatingSystem = "
							SELECT * FROM OperatingSystem ORDER BY Description ASC";
						$rs = $dbADO->_Execute($queryOperatingSystem);
						while ($row = $rs->FetchRow()) {
								$out.='<option value="'.$row['PK_OperatingSystem'].'">'.$row['Description']."</option>";
						}
						$rs->Close();
					$out.='
						</select></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addDistro");
 			frmvalidator.addValidation("Description","req","Please enter a package description");
			//frmvalidator.addValidation("FK_OperatingSystem","req","Please enter a package name");
			//frmvalidator.addValidation("Source","req","Please enter a source name");
			//frmvalidator.addValidation("Repository","req","Please enter a repository");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$description = cleanString(@$_POST['Description'],30);
		$FK_OperatingSystem = cleanInteger(@$_POST['FK_OperatingSystem']);
		
		if ($description!='') {
			$queryInsertDistro = 'INSERT INTO Distro(Description,FK_OperatingSystem) values(?,?)';
			$dbADO->Execute($queryInsertDistro,array($description,$FK_OperatingSystem));
			
			$out.="
				<script>
					alert('Distro added!');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=addPackage&from=$from&deviceID=$deviceID&subAction=edit");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>

<?php
function addOS($output,$dbADO) {
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addOS.lang.php');

	//$dbADO->debug=true;
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'addedit';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$PK_OperatingSystem = isset($_REQUEST['PK_OperatingSystem'])?cleanInteger($_REQUEST['PK_OperatingSystem']):0;

	if ($action=='close') {
		$out.='<script>window.close();</script>';
	} elseif ($action=='addedit') {
		$out.='
		<form action="index.php" method="post" name="addOS">
		<input type="hidden" name="section" value="addOS">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="PK_OperatingSystem" value="'.$PK_OperatingSystem.'">
		';

		if ($PK_OperatingSystem!=0) {
			$queryOS='SELECT * FROM OperatingSystem WHERE PK_OperatingSystem=?';
			$resOS=$dbADO->Execute($queryOS,array($PK_OperatingSystem));
			$rowOS = $resOS->FetchRow();
			$out.='			<h3>'.$TEXT_EDIT_OPERATING_SYSTEM_CONST.'</h3>';
		} else {
			$out.='			<h3>'.$TEXT_ADD_OPERATING_SYSTEM_CONST.'</h3>';
		}
		$out.='
			<table>
				<tr>
					<td>'.$TEXT_OPERATING_SYSTEM_CONST.' *</td>
					<td><input type="text" size="30" name="Description" value="'.($PK_OperatingSystem?$rowOS['Description']:'').'"></td>
				</tr>
				<tr>
					<td>'.$TEXT_DEFINE_CONST.'</td>
					<td><input type="text" size="25" name="Define" value="'.($PK_OperatingSystem?$rowOS['Define']:'').'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center">
						<input type="submit" class="button" name="addOS" value="'.($PK_OperatingSystem?$TEXT_SAVE_CONST:$TEXT_ADD_CONST).'"> 
						<input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">
					</td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addOS");
 			frmvalidator.addValidation("Description","req","'.$TEXT_DESCRIPTION_REQUIRED_CONST.'");
		</script>
		';

	} else {
		$Description = isset($_REQUEST['Description'])?cleanString($_REQUEST['Description'],30):'';
		$Define = isset($_REQUEST['Define'])?cleanString($_REQUEST['Define'],25):'';

		if ($Description!='') {
			if ($PK_OperatingSystem!=0) {
				$queryUpdateOS = 'UPDATE OperatingSystem SET Description=?, Define=?
							WHERE PK_OperatingSystem=?';
				$dbADO->Execute($queryUpdateOS, array($Description, $Define, $PK_OperatingSystem));
			} else {
				$queryUpdateOS = 'INSERT INTO OperatingSystem(Description, Define)
							VALUES (? , ?)';
				$dbADO->Execute($queryUpdateOS, array($Description, $Define));
				$PK_OperatingSystem = $dbADO->Insert_ID();
			}

			$out.="
			<script>
				opener.document.location='index.php?section={$from}';
				self.location='index.php?section=addOS&PK_OperatingSystem={$PK_OperatingSystem}&from={$from}';
			</script>
			";
		} else {
			header("Location: index.php?section=addOS&PK_OperatingSystem={$PK_OperatingSystem}&from={$from}");
		}

	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_DISTRIBUTION_CONST);
	$output->output();
}
?>

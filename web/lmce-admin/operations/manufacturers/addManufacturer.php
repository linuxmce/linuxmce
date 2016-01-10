<?php
function addManufacturer($output,$dbADO) {
	// include language files
	/* @var $dbADO ADOConnection */
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addManufacturer.lang.php');
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$dcID=(int)@$_REQUEST['dcID'];
	
	if ($action=='form') {
		$out.='
		<form action="index.php" method="post" name="addManufacturer">
		<input type="hidden" name="section" value="addManufacturer">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="dcID" value="'.$dcID.'">
		
			<h3>'.$TEXT_ADD_MANUFACTURER_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
<a href="index.php?section=deviceTemplatePicker&dcSelected='.$dcID.'&manufSelected='.@$_REQUEST['mID'].'">'.$TEXT_BACK_CONST.'</a>				
		
			<div class="err" align="center">'.@$_GET['error'].'</div>
			<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
		
			<table>			
				<tr>
					<td>'.$TEXT_MANUFACTURER_CONST.' *</td><td><input type="text" maxlength="50" name="manufacturerDescription" value="'.@$_SESSION['addManufacturer']['manufacturerDescription'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_ADD_CONST.'"  > <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"> <input type="reset" class="button" name="closeBtn" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addManufacturer");
 			frmvalidator.addValidation("manufacturerDescription","req","'.$TEXT_VALIDATE_MANUFACTURER_NAME_CONST.'");
 			frmvalidator.addValidation("manufacturerDescription","maxlen=50","'.$TEXT_VALIDATE_MANUFACTURER_LENGTH_CONST.'"); 			
		</script>
		';
		$_SESSION['addManufacturer']['manufacturerDescription']='';
	} else {
		
		$manufacturerDescription = isset($_POST['manufacturerDescription'])?cleanString($_POST['manufacturerDescription']):'';
		if ($manufacturerDescription!='') {
			$insertManufacturer = 'insert into Manufacturer(Description) values(?)';
			$query = $dbADO->Execute($insertManufacturer,array($manufacturerDescription));
			$mID=$dbADO->Insert_ID();
			
			header("Location: index.php?section=addManufacturer&mID=$mID&msg=".urlencode($TEXT_MANUFACTURER_ADDED_NOTIFICATION_CONST));
			exit();
		} else {
			
			header("Location: index.php?section=addManufacturer");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_MANUFACTURER_CONST);			
	$output->output();
}
?>

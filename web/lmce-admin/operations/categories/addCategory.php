<?
function addCategory($output,$dbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editDeviceCategory.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$parentID = (int)$_REQUEST['parentID'];

	if ($action == 'form') {
		$deviceCategoriesArray=getHierachicalCategories($dbADO);
		$deviceCategoryFormElement=pulldownFromArray($deviceCategoriesArray,'parentID',$parentID,'class="input_big"');
	
	$out='	
	<br>
		<h3>'.$TEXT_ADD_DEVICE_CATEGORY_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
	<form method="post" action="index.php" name="addCategory">	
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="section" value="addCategory">
	
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	
	<table border="0">
		<tr>
			<td>'.$TEXT_DESCRIPTION_CONST.' *</td><td><input type="text" name="categoryDescription" value="" size="40"></td>
		</tr>
		<tr>
			<td>'.$TEXT_PARENT_CONST.':</td>
			<td>'.$deviceCategoryFormElement.'</td>
		</tr>			
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
		</tr>
	</table>
	</form>
	<script>
		 	var frmvalidator = new formValidator("addCategory");
 			frmvalidator.addValidation("categoryDescription","req","'.$TEXT_PLEASE_ENTER_A_DESCRIPTION_CONST.'");
		</script>
	<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
	';
	
	} else {
		$description = cleanString($_POST['categoryDescription']);		
		$parentID = (int)$_POST['parentID']>0?(int)$_POST['parentID']:NULL;
		if (trim($description)!='') {
			$categArr=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$dbADO,'WHERE UPPER(Description)=\''.strtoupper(addslashes($description)).'\' AND FK_DeviceCategory_Parent '.(is_null($parentID)?'IS NULL':'='.$parentID));
			if(count($categArr)!=0){
				header("Location: index.php?section=addCategory&parentID=".$parentID."&error=".urlencode($TEXT_DEVICE_CATEGORY_ALREADY_EXISTS_CONST));
				exit();
			}

			$dbADO->Execute('INSERT INTO DeviceCategory (Description,FK_DeviceCategory_Parent) VALUES (?,?)',array($description,$parentID));
			$out='
			<script>
				    opener.document.forms[0].action.value="form";
					opener.document.forms[0].submit();
					self.location="index.php?section=addCategory&parentID='.$parentID.'&msg='.urlencode($TEXT_DEVICE_CATEGORY_ADDED_CONST).'";
				</script>
			';
		} else {		
			header("Location: index.php?section=addCategory&dcID=$parentID");
			exit();
		}
	}
				
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>
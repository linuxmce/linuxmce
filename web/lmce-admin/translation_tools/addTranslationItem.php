<?php
function addTranslationItem($output,$dbADO) {
	global $dbPlutoMainType,$dbPlutoMainServer,$dbPlutoMainUser,$dbPlutoMainPass;
	
	/* @var $dbADO ADOConnection */
	// todo: move records in pluto_main database
	$trADO = &ADONewConnection($dbPlutoMainType);
	$trADO->NConnect($dbPlutoMainServer,urlencode($dbPlutoMainUser),urlencode($dbPlutoMainPass),'translation'); 
	
	$selectedLang=(isset($_REQUEST['language']))?$_REQUEST['language']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$languagesArray=getAssocArray('Language','PK_Language','Description',$dbADO,'WHERE PK_Language!=1');

	if($action=='form'){
		$out='<h3>Add translated item</h3>
		<form action="index.php" method="post" name="addTranslationItem">
		<input type="hidden" name="section" value="addTranslationItem">
		<input type="hidden" name="action" value="edit">

		<div class="err" align="center">'.@$_GET['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<a href="index.php?section=translationItems&language='.$selectedLang.'">Back</a>
		<table cellpadding=3" cellspacing="0">
			<tr>
				<td><B>Filename</B></td>
				<td><input type="text" name="filename" value="'.@$_SESSION['t_filename'].'"></td>
			</tr>
			<tr>
				<td><B>Variable name</B></td>
				<td><input type="text" name="variable" value="'.@$_SESSION['t_variable'].'"></td>
			</tr>
			<tr>
				<td><B>English value</B></td>
				<td><textarea name="translated">'.@$_SESSION['t_translated'].'</textarea></td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="add" value="Add item"></td>
			</tr>	
		</table>
		</form>	
		<script>
			var frmvalidator = new formValidator("addTranslationItem");
 			frmvalidator.addValidation("translated","req","Please type the value for translated text.");
		</script>		
		';
	}else{
		$_SESSION['t_filename']=stripslashes($_POST['filename']);
		$_SESSION['t_variable']=stripslashes($_POST['variable']);
		$_SESSION['t_translated']=stripslashes($_POST['translated']);
		
		$existingItem=getAssocArray('Translation','Variable','Value',$trADO,'WHERE Filename=\''.$_SESSION['t_filename'].'\'');
		if(count($existingItem)>0){
			if(@$existingItem[$_SESSION['t_variable']]==$_SESSION['t_translated']){
				header('Location: index.php?section=addTranslationItem&error=The item already exists.');
				exit();
			}
		}
		$trADO->Execute('INSERT INTO Translation (Filename,Variable,Value) VALUES (?,?,?)',array($_SESSION['t_filename'],$_SESSION['t_variable'],$_SESSION['t_translated']));
		unset($_SESSION['t_filename']);
		unset($_SESSION['t_variable']);
		unset($_SESSION['t_translated']);
		$trID=$trADO->Insert_ID();
		
		header('Location: index.php?section=editTranslationItem&trID='.$trID.'&msg=The item was added');
		exit();
	}
	
	
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Translation items');
	$output->output();  
}
?>

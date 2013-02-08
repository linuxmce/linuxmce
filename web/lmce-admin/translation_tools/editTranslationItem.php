<?
function editTranslationItem($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/translation.lang.php');
	
	/* @var $dbADO ADOConnection */
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$textID=(int)$_REQUEST['textID'];

	$textData=getFieldsAsArray('Text','FK_TextCategory, Text.Description,Text.FK_TextCategory ,Parent.PK_TextCategory AS Parent',$dbADO,'
		INNER JOIN TextCategory ON Text.FK_TextCategory=TextCategory.PK_TextCategory
		LEFT JOIN TextCategory Parent ON  TextCategory.FK_TextCategory_Parent=Parent.PK_TextCategory 
		WHERE PK_Text='.$textID);
	$parentCateg=(!is_null($textData['Parent'][0]))?$textData['Parent'][0]:$textData['FK_TextCategory'][0];
	$translatedText=getAssocArray('Text_LS','FK_Language','Description',$dbADO,'WHERE FK_Text='.$textID);
		
	$languagesArray=getAssocArray('Language','PK_Language','Description',$dbADO);

	if($action=='form'){
		$out='<h3>'.$TEXT_EDIT_ITEM_CONST.'</h3>
		<form action="index.php" method="post" name="editTranslationItem">
		<input type="hidden" name="section" value="editTranslationItem">
		<input type="hidden" name="action" value="edit">
		<input type="hidden" name="textID" value="'.$textID.'">
		
		<div class="err" align="center">'.@$_GET['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<a href="index.php?section=translationItems&category='.$parentCateg.'&subcategory='.$textData['FK_TextCategory'][0].'">'.$TEXT_BACK_CONST.'</a>
		<table cellpadding=3" cellspacing="0" width="600">
			<tr>
				<td width="80"><B>'.$TEXT_ITEM_NAME_CONST.'</B></td>
				<td><input type="text" name="description" value="'.$textData['Description'][0].'" style="width:100%"></td>
			</tr>';

		$jsValidation='';
		foreach ($languagesArray AS $langID=>$langName){
			$jsValidation.='frmvalidator.addValidation("text_'.$langID.'","req","'.$TEXT_TEXT_REQUIRED_FOR_LANGUAGE_CONST.' '.$langName.'");
			';
			$out.='
			<tr bgcolor="#F0F3F8">
				<td><B>'.$TEXT_LANGUAGE_CONST.'</B></td>
				<td>'.$langName.'</td>
			</tr>
			<tr>
				<td colspan="2"><textarea style="width:100%" name="text_'.$langID.'">'.@$translatedText[$langID].'</textarea></td>
			</tr>						
			';
		}
		$out.='
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
			</tr>	
		</table>
		</form>	
		<script>
			var frmvalidator = new formValidator("editTranslationItem");
 			'.$jsValidation.'
		</script>		
		';
	}else{
		if(isset($_POST['update'])){
			$description=stripslashes($_POST['description']);
			if($description!=$textData['Description'][0]){
				$existingText=getAssocArray('Text','PK_Text','Description',$dbADO,'WHERE Description=\''.$description.'\' AND FK_TextCategory='.$textData['FK_TextCategory'][0]);
				if(count($existingText)>0){
					$error=$TEXT_ERROR_VARIABLE_ALREADY_USED_CONST;
				}
			}
			
			foreach ($languagesArray AS $langID=>$langName){
				$translated=stripslashes($_POST['text_'.$langID]);
				$dbADO->Execute('REPLACE INTO Text_LS (FK_Language,FK_Text,Description) VALUES (?,?,?)',array($langID,$textID,$translated));
			}
		}
		
		header('Location: index.php?section=editTranslationItem&textID='.$textID.'&msg='.$TEXT_ITEM_UPDATED_CONST.'&error='.@$error);
		exit();
	}
	
	
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_ITEM_CONST);
	$output->output();  
}
?>
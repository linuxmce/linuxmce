<?php
function translationItems($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/translation.lang.php');
	
	/* @var $dbADO ADOConnection */
	$category=@$_REQUEST['category'];
	$rootcategories=getAssocArray('TextCategory','PK_TextCategory','Description',$dbADO,'WHERE FK_TextCategory_Parent IS NULL','ORDER BY Description ASC');

	$categoryToDisplay=(isset($_REQUEST['subcategory']) && (int)$_REQUEST['subcategory']!=0)?(int)$_REQUEST['subcategory']:(int)@$_REQUEST['category'];
	if($categoryToDisplay==0){
		$categoriesIDs=array_keys($rootcategories);
		$category=$categoriesIDs[0];
		$categoryToDisplay=$categoriesIDs[0];
	}
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$subcategIDs=geTextCategorytDescendants($category,$dbADO);
	$subcategs=getAssocArray('TextCategory','PK_TextCategory','Description',$dbADO,'WHERE FK_TextCategory_Parent IN ('.join(',',(array($category)+$subcategIDs)).')');
	$categoryToDisplay=(!in_array(@$_REQUEST['subcategory'],$subcategIDs))?$category:$_REQUEST['subcategory'];
	
	$out='';
	
	if($action=='form'){
		$out='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<h3>Translation items</h3>
		
		<form action="index.php" method="post" name="translationItems">
		<input type="hidden" name="section" value="translationItems">
		<input type="hidden" name="action" value="form">
		<table>
			<tr>
				<td><B>'.$TEXT_CATEGORY_CONST.'</B></td>
				<td>'.pulldownFromArray($rootcategories,'category',$category,'onChange="document.translationItems.action.value=\'form\';document.translationItems.submit();"').'</td>
			</tr>	
			<tr>
				<td><B>'.$TEXT_SUB_CATEGORY_CONST.'</B></td>
				<td>'.pulldownFromArray($subcategs,'subcategory',(int)@$_REQUEST['subcategory'],'onChange="document.translationItems.action.value=\'form\';document.translationItems.submit();"').'</td>
			</tr>	
			<tr>
				<td colspan="2">'.subcategoriesHTML($categoryToDisplay,$dbADO).'</td>
			</tr>	
			<tr>
				<td colspan="2"><a href="index.php?section=addTranslationItem">'.$TEXT_ADD_ITEM_CONST.'</a></td>
			</tr>	
		</table>
		</form>	
	';
	}else{
		if(isset($_REQUEST['did']) && (int)$_REQUEST['did']>0){
			$did=(int)$_REQUEST['did'];
			$dbADO->Execute('DELETE FROM Text_LS WHERE FK_Text=?',$did);
			$dbADO->Execute('DELETE FROM Text WHERE PK_Text=?',$did);
			
			header('Location: index.php?section=translationItems&subcategory='.$_REQUEST['subcategory'].'&msg='.$TEXT_ITEM_DELETED_CONST);
			exit();
		}
		
	}
	
	
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_TRANSLATION_ITEMS_CONST);
	$output->output();  
}


function geTextCategorytDescendants($categoryID,$dbADO){
	if($categoryID==0){
		return array();
	}
	$childsDC=array();
	$res=$dbADO->Execute('
		SELECT  
		IF(PK_TextCategory=?,1,0) AS pos,
		TextCategory.*
		FROM TextCategory
		ORDER BY pos desc,PK_TextCategory ASC',$categoryID);
	while($row=$res->FetchRow()){
		if($row['PK_TextCategory']==$categoryID){
			$childsDC[]=$row['PK_TextCategory'];
		}
		if(in_array($row['FK_TextCategory_Parent'],$childsDC)){
			$childsDC[]=$row['PK_TextCategory'];
		}
	}

	return $childsDC;
}

function subcategoriesHTML($categoryToDisplay,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/translation.lang.php');
	
	if($categoryToDisplay==0){
		return '';
	}
	
	$subcategIDs=geTextCategorytDescendants($categoryToDisplay,$dbADO);
	
	$textsArray=getAssocArray('Text','PK_Text','Description',$dbADO,'WHERE FK_TextCategory IN ('.join(',',$subcategIDs).')','ORDER BY Description ASC');
	$out='<table cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td align="center"><B>'.$TEXT_ITEM_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
		</tr>';
	$pos=0;
	if(count($textsArray)==0){
		$out.='
		<tr>
			<td align="center" colspan="2">'.$TEXT_NO_ITEMS_CONST.'</td>
		</tr>';
	}
	foreach ($textsArray AS $textID=>$text){
		$pos++;
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>'.$text.'</td>
			<td align="center"><a href="index.php?section=editTranslationItem&textID='.$textID.'">'.$TEXT_EDIT_CONST.'</a> <a href="javascript:if(confirm(\''.$TEXT_CONFIRM_DELETE_TRANSLATION_ITEM_CONST.'\'))self.location=\'index.php?section=translationItems&action=del&subcategory='.$categoryToDisplay.'&did='.$textID.'\'">'.$TEXT_DELETE_CONST.'</a></td>
		</tr>';
	}
	$out.='
	</table>';
	
	return $out;
}
?>
<?php
function deleteCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/deleteCategory.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);		
	$categSelected = (int)$_REQUEST['categSelected'];
	
	$action = (isset($_REQUEST['action']) && cleanString($_REQUEST['action'])!='null')?cleanString($_REQUEST['action']):'';

		
	if ($action=='commands') {
		$label = $TEXT_COMMANDS_CATEGORIES_CONST;
		$tableName = 'CommandCategory';		
		$columnNamePK = 'PK_CommandCategory';	
		$columnNameFK = 'FK_CommandCategory_Parent';
	} elseif ($action=='events') {
		$label = $TEXT_EVENT_CATEGORIES_CONST;
		$tableName = 'EventCategory';
		$columnNamePK = 'PK_EventCategory';	
		$columnNameFK = 'FK_EventCategory_Parent';
	}

	$GLOBALS['childsFromAnyCategory'] = array();
	$x=getChildsForCategory($tableName,$columnNamePK,$columnNameFK,$categSelected,$dbADO);
	$childsToDeleteAray = cleanArray($GLOBALS['childsFromAnyCategory']);

	$GLOBALS['childsFromAnyCategory'][]=$categSelected;
	

	if ((int)$categSelected!=0) {
			$deleteObjFromCateg = 'delete from '.$tableName.' where '.$columnNamePK.' IN 
			('.join(",",$GLOBALS['childsFromAnyCategory']).')';
			$query = $dbADO->_Execute($deleteObjFromCateg);
			$out.="
			<script>
				alert('$TEXT_RECORD_DELETED_CONST');
			    opener.document.forms.{$from}.actionX.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">'.$TEXT_CLOSE_CONST.'</a>';
		}		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
<?php
function manageCategories($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/manageCategories.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;

	$selectedCateg= (int)@$_REQUEST['categSelected'];

	$action = (isset($_REQUEST['action']) && cleanString($_REQUEST['action'])!='null')?cleanString($_REQUEST['action']):'form';
	$actionX = (isset($_REQUEST['actionX']) && cleanString($_REQUEST['actionX'])!='null')?cleanString($_REQUEST['actionX']):'form';

	if ($action=='commands') {
		$label = $TEXT_COMMANDS_CATEGORIES_CONST;
		$tableName = 'CommandCategory';
		$columnNamePK = 'PK_CommandCategory';
		$columnNameFK = 'FK_CommandCategory_Parent';
		$tableName_Sec = 'Command';
		$columnName_FK = 'FK_CommandCategory';
		$columnName_PK = 'PK_Command';
		$actionToGoForEdit = 'editCommand';
		$fieldNameToGoForEdit = 'commandID';
	} elseif ($action=='events') {
		$label = $TEXT_EVENT_CATEGORIES_CONST;
		$tableName = 'EventCategory';
		$columnNamePK = 'PK_EventCategory';
		$columnNameFK = 'FK_EventCategory_Parent';
		$tableName_Sec = 'Event';
		$columnName_FK = 'FK_EventCategory';
		$columnName_PK = 'PK_Event';
		$actionToGoForEdit = 'editEvent';
		$fieldNameToGoForEdit = 'EventID';
	}

	if ($actionX=='form') {

		$jsTree = '';

		$selectCategories = "SELECT * FROM {$tableName} WHERE {$columnNameFK} IS NULL ORDER BY Description";
		$rs = $dbADO->_Execute($selectCategories);
		while ($row = $rs->FetchRow()) {
			$jsTree.='
		auxS'.$row[$columnNamePK].' = insFld(foldersTree, gFld("'.$row['Description'].'", "javascript:manageCategories.categSelected.value='.$row[$columnNamePK].';manageCategories.actionX.value=null;manageCategories.submit();"));
		auxS'.$row[$columnNamePK].'.xID = '.$row[$columnNamePK].';
		';
			$jsTree.=getChildsForCategory($tableName,$columnNamePK,$columnNameFK,$row[$columnNamePK],$dbADO);
		}
		$rs->Close();


		$selectModels = '<option value="0">'.$TEXT_NONE_CONST.'</option>';
		if ($selectedCateg!=0)	{

			$GLOBALS['childsFromAnyCategory'] = array();
			$x=getChildsForCategory($tableName,$columnNamePK,$columnNameFK,$selectedCateg,$dbADO);
			$childsToSearchInto = cleanArray($GLOBALS['childsFromAnyCategory']);
			$childsToSearchInto[]=$selectedCateg;

			$selectModels='';
			$queryModels = "SELECT $columnName_PK,Description FROM $tableName_Sec WHERE $columnName_FK IN (".join(",",$childsToSearchInto).") Order By Description ASC";
			$rs = $dbADO->_Execute($queryModels);

			while ($row = $rs->FetchRow()) {
				$selectModels.="<option ".($selectedCateg==$row[$columnName_PK]?" selected ":"")." value='{$row[$columnName_PK]}'>{$row['Description']}</option>";
			}

			$rs->Close();
		}



		$scriptInHead = "

<script>
function windowOpen(locationA,attributes) {
	window.open(locationA,'',attributes);
}
</script>

<!-- As in a client-side built tree, all the tree infrastructure is put in place
     within the HEAD block, but the actual tree rendering is trigered within the
     BODY -->

<!-- Code for browser detection -->
<script src=\"scripts/treeview/ua.js\"></script>

<!-- Infrastructure code for the tree -->
<script src=\"scripts/treeview/ftiens4.js\"></script>

<!-- Execution of the code that actually builds the specific tree.
     The variable foldersTree creates its structure with calls to
	 gFld, insFld, and insDoc -->
<script>

USETEXTLINKS = 1
STARTALLOPEN = 0
USEFRAMES = 0
USEICONS = 0
WRAPTEXT = 1
PRESERVESTATE = 1
ICONPATH = 'scripts/treeview/' 
HIGHLIGHT = 1


foldersTree = gFld('<b>$label</b>', \"javascript:manageCategories.categSelected.value=0;manageCategories.actionX.value=null;manageCategories.submit();\");
foldersTree.xID = 1001635872
$jsTree

foldersTree.treeID = 't2'

function getObj(obj) {
		if (document.layers) {
			if (typeof obj == 'string') {
				return document.layers[obj];
			} else 	{
				return obj;
			}
		}
		if (document.all) {
			if (typeof obj == 'string') {
				return document.all(obj);
			} else {
				return obj;
			}
		}
		if (getObj) {
			if (typeof obj == 'string') {
				return getObj(obj);
			} else {
				return obj;
			}
		}
		return null;	
	} 

	function showHideObject(obj) {
		obj = getObj(obj);					
		if(document.all || getObj) {  			
				if (obj.style.visibility == \"visible\") {
					obj.style.visibility = \"hidden\";
					obj.style.display = \"none\";
				} else {
				    obj.style.visibility = \"visible\";
					obj.style.display = \"block\";
				}
		} else if (document.layers) {
				if (obj.style.visibility == \"visible\") {
					obj.visibility = \"hide\";	
				} else {
				obj.visibility = \"show\";	
				}
		}
	} 

</script>
";
		$output->setScriptInHead($scriptInHead);


		$out.='
<form action="'.$_SERVER['PHP_SELF'].'" method="POST" name="manageCategories">
<input type="hidden" name="section" value="manageCategories">
<input type="hidden" name="actionX" value="add">
<input type="hidden" name="action" value="'.$action.'">
<input type="hidden" name="categSelected" value="'.$selectedCateg.'">

<table cellpadding="5" cellspacing="0" border="0" align="center">

			<tr>
				<th width="25%">'.$label.'</th>
				<th width="25%">Elements</th>				
			</tr>
			<tr valign="top">				
				<td width="25%" align="center" valign="top">
					<span style="font-color:#FFFFFF"><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></span>
					<span>
					<script>
						initializeDocument();						
					</script>
					<noscript>
					A tree for site navigation will open here if you enable JavaScript in your browser.
					</noscript>
					</span>
					<br /><br />
					<input type="text" name="Category_Description" size="15" />
					<input type="submit" class="button" name="addCategory" value="'.($selectedCateg==0?$TEXT_ADD_TOP_LEVEL_CHILD_CONST:$TEXT_ADD_CHILD_CONST).'"  />
					'.($selectedCateg!=0?'<input type="button" class="button" name="editCategory" value="'.$TEXT_EDIT_CONST.'" onClick="javascript: windowOpen(\'index.php?section=editCategory&action='.$action.'&from=manageCategories&categSelected='.$selectedCateg.'\',\'status=0,resizable=1,width=600,height=250,toolbar=1\');" />':'<input type="submit" class="button" name="editCategory" value="'.$TEXT_EDIT_CONST.'" disabled="disabled"  />').'';

		//getDeviceCategoryChildsNo($selectedDevice,$dbADO);
		$GLOBALS['childsFromAnyCategory'] = array();
		$x=getChildsForCategory($tableName,$columnNamePK,$columnNameFK,$selectedCateg,$dbADO);
		$childsToDelete = count(cleanArray($GLOBALS['childsFromAnyCategory']));

		$out.='
					&nbsp;&nbsp;'.($selectedCateg!=0?'<input type="button" class="button" name="deletCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) windowOpen(\'index.php?section=deleteCategory&from=manageCategories&action='.$action.'&categSelected='.$selectedCateg.'\',\'status=0,resizable=1,width=100,height=100,toolbar=0\');" />':'<input type="submit" class="button" name="deleteCategory" value="Delete" disabled="disabled"  />').'
				</td>
				<td width="25%" align="center"  valign="top">
					<script>
						function checkEdit(objF) {	
								windowOpen(\'index.php?section='.$actionToGoForEdit.'&'.$fieldNameToGoForEdit.'=\'+objF.model[objF.model.selectedIndex].value,\'status=0,resizable=1,width=600,height=800,toolbar=0\');								
						}						
					
					</script>
					<select name="model" id="model" size="10">					
							'.$selectModels.'	
					</select>
					<input type="button" class="button" name="edit_Category" value="'.$TEXT_EDIT_CONST	.'" onClick="javascript:checkEdit(this.form);" /> <input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();">
				</td>
			</tr>
		</table>					
</form>
';
	} else {

		$addCategory = @cleanString($_POST['addCategory']);
		$category_Desc = @cleanString($_POST['Category_Description']);
		$categSelected = cleanInteger($_POST['categSelected']);

		/*$deviceCategSelected = (int)$_POST['deviceCategSelected'];
		$deviceSelected = (int)$_POST['deviceSelected'];
		$manufacturerSelected = (int)$_POST['manufacturers'];*/

		$justAddedNode = 0;
		if (strstr($addCategory,'Add')) {
			if ($category_Desc!='') {
				if ($categSelected==0) {
					$queryInsertCategory = "insert into $tableName($columnNameFK,Description)
	 				values(NULL,?)";
					$dbADO->Execute($queryInsertCategory,array($category_Desc));
				} else {
					$queryInsertCategoryDesc = "insert into $tableName($columnNameFK,Description)
	 				values(?,?)";
					$dbADO->Execute($queryInsertCategoryDesc,array($categSelected,$category_Desc));
					$justAddedNode = $categSelected;
				}
			}
		}
		$out="
				<script>
				    opener.location.reload();				
					self.location='index.php?section=manageCategories&categSelected=$categSelected&justAddedNode=$justAddedNode&action=$action';
				</script>
				";	
	}

	$output->setNavigationMenu(array($TEXT_DEVICE_TEMPLATES_CONST=>'index.php?section=deviceTemplates',$label=>'index.php?section=manageCategories&action='.$action));

	$output->setScriptInBody("");
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MANAGE_CATEGORIES_CONST);
	$output->output();
}
?>

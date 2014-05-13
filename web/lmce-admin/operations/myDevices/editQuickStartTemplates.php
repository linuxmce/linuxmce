<?php
function editQuickStartTemplates($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	if ($action == 'form') {
		
		
	
		
	$tree=getQuickStartTree($dbADO);
	
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="editQuickStartTemplates" enctype="multipart/form-data">
		<input type="hidden" name="section" value="editQuickStartTemplates">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="currentPos" value="">
		
		<h3>Edit Quick Start templates</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>	
	
	'.$tree.'
	</form>
	<div align="center"><input type="button" class="button" name="close" value="Close" onClick="self.close();"></div>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editQuickStartTemplates&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$msg='Quick start templates were updated.';
		$categoriesArray=explode(',',$_POST['categories']);
		foreach ($categoriesArray AS $category){
			$description=@$_POST['category_'.$category];
			$parent=((int)@$_POST['parent_'.$category]>0)?(int)$_POST['parent_'.$category]:NULL;
			$dbADO->Execute('UPDATE QuickStartCategory SET Description=?,FK_QuickStartCategory_Parent=? WHERE PK_QuickStartCategory=?',array($description,$parent,$category));
		}
		
		$templates=explode(',',@$_POST['templates']);
		foreach ($templates AS $qst){
			if((int)$qst!=0){
				$description=cleanString($_POST['description_'.$qst]);
				$package=((int)$_POST['package_'.$qst]>0)?(int)$_POST['package_'.$qst]:NULL;
				$binary=$_POST['binary_'.$qst];
				$arguments=cleanString(str_replace(" ","\t",$_POST['arguments_'.$qst]));
				$homepage=cleanString($_POST['homepage_'.$qst]);
				$icon=cleanString($_POST['icon_'.$qst]);
				$windowclass=cleanString($_POST['windowclass_'.$qst]);
				$designobj=((int)$_POST['designobj_'.$qst]>0)?(int)$_POST['designobj_'.$qst]:NULL;
				$osd=((int)$_POST['osd_'.$qst]>0)?(int)$_POST['osd_'.$qst]:NULL;

				$dbADO->Execute('
						UPDATE QuickStartTemplate SET 
							Description=?,  
							FK_Package=?,
							`Binary`=?,
							Arguments=?,
							Homepage=?,
							Icon=?,
							FK_DesignObj=?,
							FK_DesignObj_OSD=?,
							WindowClass=?
						WHERE PK_QuickStartTemplate=?',
				array($description,$package,$binary,$arguments,$homepage,$icon,$designobj,$osd,$windowclass,$qst));
			}
		}

		
		
		$newCategory=stripslashes($_POST['newCategory']);
		$newParent=((int)$_POST['newParent']>0)?(int)$_POST['newParent']:NULL;
		if($newCategory!=''){
			$dbADO->Execute('INSERT INTO QuickStartCategory (Description,FK_QuickStartCategory_Parent) VALUES (?,?)',array($newCategory,$newParent));
			$msg.='<br>New quick start category was added.';
		}
		
		
		$newTemplate=stripslashes($_POST['newTemplate']);
		$newTemplateCategory=((int)$_POST['newTemplateCategory']>0)?(int)$_POST['newTemplateCategory']:NULL;
		if($newTemplate!=''){
			if((int)$newTemplateCategory!=0){
				$dbADO->Execute('INSERT INTO QuickStartTemplate (Description,FK_QuickStartCategory) VALUES (?,?)',array($newTemplate,$newTemplateCategory));
				$msg.='<br>New quick start template was added.';
			}else{
				$err='Please select category type for quick start template.';
			}
		}
		
		$deleteCategory=(int)@$_POST['deleteCategory'];
		if($deleteCategory>0){
			$dbADO->Execute('DELETE FROM QuickStartTemplate WHERE FK_QuickStartCategory=?',$deleteCategory);
			$dbADO->Execute('UPDATE QuickStartCategory SET FK_QuickStartCategory_Parent=NULL WHERE FK_QuickStartCategory_Parent=?',$deleteCategory);
			$dbADO->Execute('DELETE FROM QuickStartCategory WHERE PK_QuickStartCategory=?',$deleteCategory);
			$msg.='<br>The category and the templates from it had been deleted.';
		}

		$deleteTemplate=(int)@$_POST['deleteTemplate'];
		if($deleteTemplate>0){
			$dbADO->Execute('DELETE FROM QuickStartTemplate WHERE PK_QuickStartTemplate=?',$deleteTemplate);
			$msg.='<br>The quick start template had been deleted.';
		}
		
		$script='
		<script>
			opener.location.reload();
			self.location="index.php?section=editQuickStartTemplates&msg='.$msg.'&error='.@$err.'";
		</script>';
		die($script);		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Quick start templates');
	$output->output();
}


function getQuickStartTemplates($dbADO){
	$res=$dbADO->Execute('SELECT QuickStartTemplate.*, Package.Description AS Package,DesignObj.Description AS DesignObj,DesignObj_osd.Description AS Osd FROM QuickStartTemplate LEFT JOIN Package ON FK_Package=PK_Package LEFT JOIN DesignObj ON FK_DesignObj=DesignObj.PK_DesignObj LEFT JOIN DesignObj DesignObj_osd ON FK_DesignObj_OSD=DesignObj_osd.PK_DesignObj');
	$qsTemplates=array();
	while($row=$res->FetchRow()){
		$qsTemplates[$row['FK_QuickStartCategory']][]=$row;
	}
	
	return $qsTemplates;
}

function getQuickStartTree($dbADO){
	global $gstArray;
	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$nodes=getNodesArray('QuickStartCategory','PK_QuickStartCategory','FK_QuickStartCategory_Parent',$dbADO);
	$qstArray=getQuickStartTemplates($dbADO);

	$out='
		<input type="hidden" name="deleteCategory" value="">
		<input type="hidden" name="deleteTemplate" value="">
	<table cellpadding="3" cellspacing="0" border="0">
		<tr class="tablehead">
			<td><B>#</B></td>
			<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
			<td><B>'.$TEXT_ACTION_CONST.'</B></td>
		</tr>
		<tr class="alternate_back">
			<td>-</td>
			<td><B>Undefined category</b></td>
			<td align="center">-</td>
		</tr>
		<tr>
			<td></td>
			<td colspan="2">'.qstTable(NULL,$qstArray).'</td>
		</tr>			';
	
	// added to tree templates without category
	
	$templates=array();
	if(@count($nodes['root_node'])!=0){
		foreach ($nodes['root_node'] AS $rootNode){
			$out.='
			<tr class="alternate_back">
				<td>'.$rootNode.'</td>
				<td><a href="index.php?section=editQuickStartCategory&id='.$rootNode.'"><B>'.@$nodes['description'][$rootNode].'</b></a></td>
				<td align="right"><input type="button" class="button" name="qsc_'.$rootNode.'" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start category? All templates from this category will be deleted too.\')){document.editQuickStartTemplates.deleteCategory.value=\''.$rootNode.'\';document.editQuickStartTemplates.submit();}"></td>
			</tr>
			<tr>
				<td></td>
				<td colspan="2">'.qstTable($rootNode,$qstArray).'</td>
			</tr>		
			';		
			$out.=getQuickStartChilds($nodes,$rootNode,$qstArray);		
			}
	}
	$out.='
	</table>
	<br><a href="index.php?section=addQuickStartTemplate">Add quick start template</a>';
	
	return $out;
}

function getQuickStartChilds($nodes,$selectedCategory,$qstArray){
	$out='';
	if(isset($nodes[$selectedCategory])){
		foreach ($nodes[$selectedCategory] AS $childNode){
	
			$out.='
			<tr class="alternate_back">
				<td>'.$childNode.'</td>
				<td><a href="index.php?section=editQuickStartCategory&id='.$childNode.'"><B>'.(($nodes['description'][$selectedCategory]!='')?$nodes['description'][$selectedCategory].' &gt;&gt;':'').' '.@$nodes['description'][$childNode].'</b></a></td>
				<td align="right"><input type="button" class="button" name="qsc_'.$childNode.'" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start category? All templates from this category will be deleted too.\')){document.editQuickStartTemplates.deleteCategory.value=\''.$childNode.'\';document.editQuickStartTemplates.submit();}"></td>
			</tr>
			<tr>
				<td></td>
				<td colspan="2">'.qstTable($childNode,$qstArray).'</td>
			</tr>			
		';		
	

	
			$out.=getQuickStartChilds($nodes,$childNode,$qstArray);		
		}
	}	
	return $out;
}

function getQSCategories($nodes){
	$reorderedArray=array();
	if(isset($nodes['root_node'])){
		foreach ($nodes['root_node'] AS $rootNode){
			$reorderedArray[$rootNode]=$nodes['description'][$rootNode];
			
			$reorderedArray=$reorderedArray+getReorderedChilds($nodes,$rootNode,$nodes['description'][$rootNode]);
		}
	}
		
	return $reorderedArray;
}

function getReorderedChilds($nodes,$rootNode,$description){
	$reorderedArray=array();
	foreach ($nodes[$rootNode] as $childNode) {
		$reorderedArray[$childNode]=$description.' - '.$nodes['description'][$childNode];
		
		$reorderedArray=$reorderedArray+getReorderedChilds($nodes,$childNode,$nodes['description'][$childNode]);
	}
	
	return $reorderedArray;
}

function qstTable($node,$qstArray){
	if(count(@$qstArray[$node])==0){
		$out='<table border="0" cellpadding="2" cellspacing="0" width="100%">
				<tr>
					<td>No records</td>
				</tr>
			</table>';
		return $out;
	}
	
	$out='<table class="regular_table" border="1" cellpadding="2" cellspacing="0" width="100%">
				<tr class="alternate_back">
					<td><B>Description</B></td>
					<td><B>Package</B></td>
					<td><B>Binary</B></td>
					<td><B>Arguments</B></td>
					<td><B>Homepage</B></td>
					<td><B>Icon</B></td>
					<td><B>Window Class</B></td>
					<td><B>Design Obj</B></td>
					<td><B>Osd</B></td>
					<td align="center"><B>Action</B></td>
				</tr>	';
	foreach (@$qstArray[$node] AS $qsTemplate=>$data){
		$out.='
				<tr>
					<td><a href="index.php?section=editQuickStartTemplate&id='.$data['PK_QuickStartTemplate'].'">'.@$data['Description'].'</a></td>
					<td>'.@$data['Package'].'&nbsp;</td>
					<td>'.@$data['Binary'].'&nbsp;</td>
					<td>'.@$data['Arguments'].'</td>
					<td>'.@$data['Homepage'].'&nbsp;</td>
					<td>'.((strpos(@$data['Icon'],'http://')!==false)?'<img src="'.$data['Icon'].'" border="0">':'').'&nbsp;</td>
					<td>'.@$data['WindowClass'].'&nbsp;</td>
					<td>'.@$data['DesignObj'].'&nbsp;</td>
					<td>'.@$data['Osd'].'&nbsp;</td>
					<td align="center"><input type="button" class="button" name="del" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this quick start template?\')){document.editQuickStartTemplates.deleteTemplate.value=\''.$data['PK_QuickStartTemplate'].'\';document.editQuickStartTemplates.submit();}"></td>
				</tr>
				';
	}
	$out.='</table>';
	
	return $out;
}
?>

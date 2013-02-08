<?
function editQuickStartCategory($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$id=(int)$_REQUEST['id'];
	
	if ($action == 'form') {
		$data=getFieldsAsArray('QuickStartCategory','Description,FK_QuickStartCategory_Parent',$dbADO,'WHERE PK_QuickstartCategory='.$id);
		$parents=getAssocArray('QuickStartCategory','QuickStartCategory.PK_QuickStartCategory AS PK_QuickStartCategory',"IF(QParent.PK_QuickStartCategory IS NULL,QuickStartCategory.Description,CONCAT(QParent.Description,' > ',QuickStartCategory.Description)) AS descr",$dbADO,'LEFT JOIN QuickStartCategory QParent ON QParent.PK_QuickStartCategory=QuickStartCategory.FK_QuickStartCategory_Parent WHERE QuickStartCategory.PK_QuickStartCategory!='.$id,'ORDER BY descr ASC');
		
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="editQuickStartCategory" enctype="multipart/form-data">
		<input type="hidden" name="section" value="editQuickStartCategory">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="id" value="'.$id.'">	
		
		<h3>Edit Quick Start Category #'.$id.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>	
	
		<table>
			<tr>
				<td><B>Description</B></td>
				<td><input type="text" name="description" value="'.$data['Description'][0].'"></td>
			</tr>
			<tr>
				<td><B>Child of</B></td>
				<td>'.pulldownFromArray($parents,'parent',$data['FK_QuickStartCategory_Parent'][0]).'</td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Update"></td>
			</tr>			
		</table>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editQuickStartCategory&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		if(isset($_POST['save'])){
			$description=cleanString($_POST['description']);
			$parent=(int)$_POST['parent'];
			$parent=($parent==0)?NULL:$parent;
			
			$dbADO->Execute('UPDATE QuickStartCategory SET Description=?,FK_QuickStartCategory_Parent=? WHERE PK_QuickStartCategory=?',array($description,$parent,$id));
			header("Location: index.php?section=editQuickStartTemplates&msg=The quick start category was updated.");
			exit(0);
		}
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Quick Start Category #'.$id);
	$output->output();
}
?>

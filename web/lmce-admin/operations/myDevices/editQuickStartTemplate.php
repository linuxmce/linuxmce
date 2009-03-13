<?
function editQuickStartTemplate($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$id=(int)@$_REQUEST['id'];
	
	if ($action == 'form') {
		$data=getFieldsAsArray('QuickStartTemplate','Description,FK_QuickStartCategory,FK_Package,`Binary`,Homepage,Arguments,Icon,WindowClass,FK_DesignObj,FK_DesignObj_OSD',$dbADO,'WHERE PK_QuickstartTemplate='.$id);
		$packages=getAssocArray('Package','PK_Package','Description',$dbADO,'','ORDER BY Description ASC');
		$designobj=getAssocArray('DesignObj','PK_DesignObj','Description',$dbADO,'','ORDER BY Description ASC');
		$categories=getAssocArray('QuickStartCategory','QuickStartCategory.PK_QuickStartCategory AS PK_QuickStartCategory',"IF(QParent.PK_QuickStartCategory IS NULL,QuickStartCategory.Description,CONCAT(QParent.Description,' > ',QuickStartCategory.Description)) AS descr",$dbADO,'LEFT JOIN QuickStartCategory QParent ON QParent.PK_QuickStartCategory=QuickStartCategory.FK_QuickStartCategory_Parent','ORDER BY descr ASC');
		
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="editQuickStartTemplate" enctype="multipart/form-data">
		<input type="hidden" name="section" value="editQuickStartTemplate">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="id" value="'.$id.'">	
		
		<h3>Edit Quick Start template #'.$id.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>	
	
		<table>
			<tr>
				<td><B>Quick start category</B></td>
				<td>'.pulldownFromArray($categories,'category',$data['FK_QuickStartCategory'][0]).'</td>
			</tr>		
			<tr>
				<td><B>Description</B></td>
				<td><input type="text" name="description" value="'.$data['Description'][0].'"></td>
			</tr>
			<tr>
				<td><B>Package</B></td>
				<td>'.pulldownFromArray($packages,'package',$data['FK_Package'][0]).'</td>
			</tr>
			<tr>
				<td><B>Binary</B></td>
				<td><input type="text" name="binary" value="'.$data['Binary'][0].'"></td>
			</tr>
			<tr>
				<td><B>Arguments</B></td>
				<td><input type="text" name="arguments" value="'.$data['Arguments'][0].'"></td>
			</tr>
			<tr>
				<td><B>Homepage</B></td>
				<td><input type="text" name="homepage" value="'.$data['Homepage'][0].'"></td>
			</tr>
			<tr>
				<td><B>Icon</B></td>
				<td><input type="text" name="icon" value="'.$data['Icon'][0].'"></td>
			</tr>
			<tr>
				<td><B>Window class</B></td>
				<td><input type="text" name="windowclass" value="'.$data['WindowClass'][0].'"></td>
			</tr>
			<tr>
				<td><B>DesignObj</B></td>
				<td>'.pulldownFromArray($designobj,'designobj',$data['FK_DesignObj'][0]).'</td>
			</tr>
			<tr>
				<td><B>OSD</B></td>
				<td>'.pulldownFromArray($designobj,'osd',$data['FK_DesignObj_OSD'][0]).'</td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Update"></td>
			</tr>			
		</table>
	</form>
		<script>
		 	var frmvalidator = new formValidator("editQuickStartTemplate"); 			
			frmvalidator.addValidation("description","req","Required field");
		</script>	
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editQuickStartTemplate&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$description=cleanString($_POST['description']);
		$category=(int)$_POST['category'];
		$category=($category==0)?NULL:$category;
		$package=(int)$_POST['package'];
		$package=($package==0)?'NULL':$package;
		$binary=cleanString($_POST['binary']);
    	$arguments=cleanString($_POST['arguments']);
    	$homepage=cleanString($_POST['homepage']);
    	$icon=cleanString($_POST['icon']);
		$windowclass=cleanString($_POST['windowclass']);
		$designobj=(int)$_POST['designobj'];
		$designobj=($designobj==0)?'NULL':$designobj;
    	$osd=(int)$_POST['osd'];
    	$osd=($osd==0)?'NULL':$osd;
		
    	if(isset($_POST['save'])){
    		$dbADO->Execute('UPDATE QuickStartTemplate QuickStartTemplate SET 
    			Description=?, 
    			FK_QuickStartCategory=?, 
    			FK_Package=?,
    			`Binary`=?,
    			Arguments=?,
    			Homepage=?,
    			Icon=?,
    			WindowClass=?,
    			FK_DesignObj=?,
    			FK_DesignObj_OSD=?
    			WHERE PK_QuickStartTemplate=?',
    			array(
    				$description,
    				$category,
    				$package,
    				$binary,
    				$arguments,
    				$homepage,
    				$icon,
    				$windowclass,
    				$designobj,
    				$osd,
    				$id
    			));
    	}
		
		header("Location: index.php?section=editQuickStartTemplates&msg=The quick start template was updated.");
		exit(0);    	
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Quick start templates');
	$output->output();
}
?>

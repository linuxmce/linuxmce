<?
function addQuickStartTemplate($output,$dbADO) {
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
		$packages=getAssocArray('Package','PK_Package','Description',$dbADO,'','ORDER BY Description ASC');
		$designobj=getAssocArray('DesignObj','PK_DesignObj','Description',$dbADO,'','ORDER BY Description ASC');
		$categories=getAssocArray('QuickStartCategory','QuickStartCategory.PK_QuickStartCategory AS PK_QuickStartCategory',"IF(QParent.PK_QuickStartCategory IS NULL,QuickStartCategory.Description,CONCAT(QParent.Description,' > ',QuickStartCategory.Description)) AS descr",$dbADO,'LEFT JOIN QuickStartCategory QParent ON QParent.PK_QuickStartCategory=QuickStartCategory.FK_QuickStartCategory_Parent','ORDER BY descr ASC');
		
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="addQuickStartTemplate" enctype="multipart/form-data">
		<input type="hidden" name="section" value="addQuickStartTemplate">
		<input type="hidden" name="action" value="add">	
		
		<h3>Add Quick Start template</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>	
	
		<table>
			<tr>
				<td><B>Quick start category</B></td>
				<td>'.pulldownFromArray($categories,'category',0).'</td>
			</tr>		
			<tr>
				<td><B>New quick start category *</B></td>
				<td><input type="text" name="newcategory" value=""></td>
			</tr>
			
			<tr>
				<td><B>Description **</B></td>
				<td><input type="text" name="description" value=""></td>
			</tr>
			<tr>
				<td><B>Package</B></td>
				<td>'.pulldownFromArray($packages,'package',0).'</td>
			</tr>
			<tr>
				<td><B>Binary</B></td>
				<td><input type="text" name="binary" value=""></td>
			</tr>
			<tr>
				<td><B>Arguments</B></td>
				<td><input type="text" name="arguments" value=""></td>
			</tr>
			<tr>
				<td><B>Homepage</B></td>
				<td><input type="text" name="homepage" value=""></td>
			</tr>
			<tr>
				<td><B>Icon</B></td>
				<td><input type="text" name="icon" value=""></td>
			</tr>
			<tr>
				<td><B>Window class</B></td>
				<td><input type="text" name="windowclass" value=""></td>
			</tr>
			<tr>
				<td><B>DesignObj</B></td>
				<td>'.pulldownFromArray($designobj,'designobj',0).'</td>
			</tr>
			<tr>
				<td><B>OSD</B></td>
				<td>'.pulldownFromArray($designobj,'osd',0).'</td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Add"></td>
			</tr>			
		</table>
		<em>* Type the new quick start category if it\'s not in the list<br>
		** Required field</em>

	</form>
		<script>
		 	var frmvalidator = new formValidator("addQuickStartTemplate"); 			
			frmvalidator.addValidation("description","req","Required field");
		</script>	
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=addQuickStartTemplate&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$description=cleanString($_POST['description']);
		$category=(int)$_POST['category'];
		$category=($category==0)?NULL:$category;
		$newcategory=cleanString($_POST['newcategory']);
		if($newcategory!=''){
			$dbADO->Execute('INSERT INTO QuickStartCategory (Description) VALUES (?)',array($newcategory));
			$category=$dbADO->Insert_ID();
		}
		
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
    		$dbADO->Execute('INSERT INTO QuickStartTemplate 
    			(Description, FK_QuickStartCategory, FK_Package, `Binary`, Arguments, Homepage, Icon, WindowClass, FK_DesignObj, FK_DesignObj_OSD)
    			VALUES (?,?,?,?,?,?,?,?,?,?)',
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
    				$osd
    			));
    	}
		
		header("Location: index.php?section=editQuickStartTemplates&msg=The quick start template was added.");
		exit(0);    	
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Quick start templates');
	$output->output();
}
?>

<?php
function createInstallation($output,$dbADO) {
	// obsolete page
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="createInstallation">
		<input type="hidden" name="section" value="createInstallation">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>
				<tr>
					<td>Name:</td>
					<td><input type="text" size="15" name="Name" value=""></td>
				</tr>
				<tr>
					<td>Address:</td>
					<td><input type="text" size="15" name="Address" value=""></td>
				</tr>
				<tr>
					<td>City:</td>
					<td><input type="text" size="15" name="City" value=""></td>
				</tr>
				<tr>
					<td>State:</td>
					<td><input type="text" size="15" name="State" value=""></td>
				</tr>
				<tr>
					<td>Zip:</td>
					<td><input type="text" size="15" name="Zip" value=""></td>
				</tr>
				<tr>
					<td>Is Active:</td>
					<td><input type="checkbox" name="IsActive" value="1"></td>
				</tr>		
				<tr>
					<td>Is Monitored:</td>
					<td><input type="checkbox" name="IsMonitored" value="1"></td>
				</tr>	
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
				
		<script>
		 	var frmvalidator = new formValidator("createInstallation");
 			frmvalidator.addValidation("Description","req","Please enter a description");
			frmvalidator.addValidation("Name","req","Please enter a name");			
		</script>
		';
		
	} else {
		
		
		
		$description = cleanString(@$_POST['Description'],50);
		$name = cleanString(@$_POST['Name'],50);
		$address = cleanString(@$_POST['Address'],50);
		$city = cleanString(@$_POST['City'],50);
		$state = cleanString(@$_POST['State'],50);
		$zip = cleanString(@$_POST['Zip'],50);
		$isActive = cleanInteger(@$_POST['IsActive']);
		$isMonitored = cleanInteger(@$_POST['IsMonitored']);
		
		if ($description!='' && $name!='') {
			
			$queryUpdate = 'INSERT INTO Installation 
							(Description,Name,Address,City,State,Zip,isActive,isMonitored)
					  values(?,?,?,?,?,?,?,?)';			 
			$dbADO->Execute($queryUpdate,array($description,$name,$address,$city,$state,$zip,$isActive,$isMonitored));
			$lastInsert = $dbADO->Insert_ID();
			
			$_SESSION['installationIDs'][]=$lastInsert;
			
			$insertUserToInstallation = "
				INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
					VALUES(?,?,1)
				";			
			$query=$dbADO->Execute($insertUserToInstallation,array($lastInsert,$_SESSION['userID']));				
				
			$out.="
				<script>
					alert('Installation added!');
					top.frames['treeframe'].location='index.php?section=leftMenu';
				</script>
				";				
			//header("Location: index.php?section=createInstallation&from=$from");
		} else {
			header("Location: index.php?section=createInstallation&saved=yes");
		}
				
	}
	
	$output->setNavigationMenu(array("Settings"=>"index.php?section=installationSettings","Create new installation"=>"index.php?section=createInstallation"));
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Create installation');			
	$output->output();
}
?>

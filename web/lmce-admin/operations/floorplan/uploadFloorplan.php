<?php
function uploadFloorplan($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/uploadFloorplan.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$page=(isset($_REQUEST['fpID']))?cleanInteger($_REQUEST['fpID']):0;
	$type=(isset($_REQUEST['type']))?cleanInteger($_REQUEST['type']):1;
	$strError=(isset($_GET['error']))?$_GET['error']:'';
	
	$installationID = (int)@$_SESSION['installationID'];
	$path=$GLOBALS['floorplansPath'].'/inst'.$installationID;
	$floorplanImage='floorplans/image_not_found.gif';
	
	if(file_exists($path.'/'.$page.'.png')){
		$randNumber=rand(0,99999);
		$floorplanImage='include/image.php?imagepath='.$GLOBALS['floorplansPath'].'/inst'.$installationID.'/'.$page.'.png&rand='.$randNumber;
	}

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$out='';
	
	$queryFloorplan='SELECT Description FROM Floorplan WHERE FK_Installation=? AND Page=?';
	$resFloorplan=$dbADO->Execute($queryFloorplan,array($installationID,$page));

	if($resFloorplan->RecordCount()==0 	&& $page!=0){
		header("Location: index.php?section=floorplanWizard&error=$TEXT_INVALID_FLOORPLAN_SPECIFIED_CONST");		
		exit();
	}
	$rowFloorplan=$resFloorplan->FetchRow();
	$oldDescription=$rowFloorplan['Description'];

	if($action=='form'){
		$out='
			<form action="index.php" method="POST" name="uploadFloorplan" enctype="multipart/form-data">
			<input type="hidden" name="section" value="uploadFloorplan">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="fpID" value="'.$page.'">		
			<input type="hidden" name="type" value="'.$type.'">		
		
			<table bgcolor="#EEEEEE" cellpadding="0" cellspacing="0" border="1" align="center">
				<tr>
					<td>
					<table cellpadding="20">
						<tr>
							<td>&nbsp;&nbsp;&nbsp;</td>
							<td>';

								if($strError != ''){ 
									$out.='<p class="normalBold">'.$strError.'</p>';
								} 
								if($page != 0)
								{ 
									$out.='<img src="'.$floorplanImage.'"  border="1"><br /><br />'; 
								}
							$out.='	
								<table>
									<tr>
										<td class="normal">'.$TEXT_DESCRIPTION_CONST.'&nbsp;&nbsp;&nbsp;</td>
										<td><input type="text" name="txtDescription" value="'.@$oldDescription.'"></td>
									</tr>
									<tr>
										<td class="normal">
											'.$TEXT_CHOOSE_FILE_CONST.':<br />&nbsp; &nbsp; ('.$TEXT_FILETYPE_RESTRICTION_CONST.')
										</td>
										<td><input type="file" name="fileImage"></td>
									</tr>
									<tr>
										<td colspan="2" align="center"><br />
											<input type="submit" class="button" value="'.(($page==0)?$TEXT_ADD_CONST:$TEXT_UPDATE_CONST).' '.$TEXT_FLOORPLAN_CONST.'">
											<input type="button" class="button" onclick="JavaScript:self.location=\'index.php?section=floorplanWizard&page='.$page.'&type='.$type.'\';" value="'.$TEXT_CANCEL_CONST.'">&nbsp; &nbsp;
										</td>
									</tr>
								</table>
							</td>
						</tr>
					</table>
					</td>
				</tr>
			</table>
		</form>
		';
	}else{
		
		$newDescription=cleanString($_POST['txtDescription']);
		$path=$GLOBALS['floorplansPath'].'/inst'.$installationID;

		if($page!=0){
			$updateFloorplan='UPDATE Floorplan SET Description=? WHERE FK_Installation=? AND Page=?';
			$dbADO->Execute($updateFloorplan,array($newDescription,$installationID,$page));
		}
		
		if($_FILES['fileImage']['name']!=''){
			switch($_FILES['fileImage']['type']){
				case 'image/x-png':
				case 'image/png': $extension = 'png'; break;
				default:
					$invalidType=true;
				break;
			}
			
			if(isset($invalidType)){
				header("Location: index.php?section=uploadFloorplan&error=$TEXT_ERROR_INVALID_FILE_TYPE_CONST&page=$page&fpID=$page");
				exit();
			}
			
			if(!file_exists($path) && !@mkdir($path)){
				header("Location: index.php?section=uploadFloorplan&error=$TEXT_ERROR_CANNOT_CREATE_DIRECTORY_CONST&page=$page&fpID=$page");
				exit();
			}
			
			if($page==0){
				$selectPage='SELECT (MAX(Page)+1) AS newPage FROM Floorplan WHERE FK_Installation=?';
				$res=$dbADO->Execute($selectPage,$installationID);
				$row=$res->FetchRow();
				$newPage=(isset($row['newPage']))?$row['newPage']:1;
				$newPicName=$newPage.'.'.$extension;
			}else
				$newPicName=$page.'.'.$extension;
			
			if(!move_uploaded_file($_FILES['fileImage']['tmp_name'],$path.'/'.$newPicName)){
				header("Location: index.php?section=uploadFloorplan&error=$TEXT_ERROR_PERMISSIONS_CONST $path&page=".$page);
				exit();
			}
			
			if($page==0){
				$insertFloorplan='INSERT INTO Floorplan (FK_Installation, Page, Description) VALUES (?,?,?)';
				$dbADO->Execute($insertFloorplan,array($installationID,$newPage,$newDescription));
				$page=$newPage;
			}
		}

		header("Location: index.php?section=floorplanWizard&page=".$page."&type=".$type);
	}
	$output->setNavigationMenu(array("Floorplans Wizard"=>'index.php?section=floorplanWizard&page='.$page.'&type='.$type));	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_UPLOAD_FLOORPLAN_CONST);
	$output->output();
}
?>

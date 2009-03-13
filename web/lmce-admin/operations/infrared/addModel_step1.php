<?
	$mID=$_REQUEST['mID'];
	$dcID=$_REQUEST['dcID'];

	$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$publicADO,'WHERE PK_Manufacturer='.$mID,'ORDER BY Description ASC');
	$categArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$publicADO,'WHERE PK_DeviceCategory='.$dcID,'ORDER BY Description ASC');
	
	$commMethodArray=getAssocArray('CommMethod','PK_CommMethod','Description',$publicADO,'','ORDER BY PK_CommMethod ASC');
	$commMethodRadioButtons='';
	$selectedCommMethod=(isset($_SESSION['selectedCommMethod']))?$_SESSION['selectedCommMethod']:1;
	foreach ($commMethodArray AS $key=>$value){
		$commMethodRadioButtons.='<input type="radio" name="commMethod" value="'.$key.'" '.(($key==$selectedCommMethod)?'checked':'').'>'.$value.'<br>';
	}
	
	
	if($action=='form'){
		$out='
		<h3 align="center">'.$TEXT_ADD_MODEL_CONST.'</h3><br>
		<B>'.$TEXT_QUESTION_1_TITLE_CONST.'</B><br>
		<p class="normaltext">'.$TEXT_Q1_INFO_CONST.' <a href="http://plutohome.com/wiki/index.php/Tell_Pluto_about_your_home">('.strtolower($TEXT_DETAILS_CONST).')</a> 
		 '.$TEXT_Q1_NOTE_CONST.'
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="mID" value="'.$mID.'">
			<input type="hidden" name="dcID" value="'.$dcID.'">

		<table class="normaltext" align="center">
			<tr>
				<td align="right"><B>'.$TEXT_MANUFACTURER_CONST.'</B></td>
				<td>'.$manufArray[$mID].'</td>
				<td><B>'.$TEXT_DEVICE_CATEGORY_CONST.'</B></td>
				<td>'.$categArray[$dcID].'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_Q1_MODEL_NAME_CONST.' *</B></td>
				<td colspan="3"><input type="text" name="description" value=""></td>
			</tr>
			<tr>
				<td><B>'.$TEXT_Q1_CONTROL_CONST.'</B></td>
				<td colspan="3">'.$commMethodRadioButtons.'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_CREATE_DEVICE_INSTANCE_CONST.'</B></td>
				<td colspan="3"><input type="checkbox" name="create_device" value="1" checked></td>
			</tr>			
			<tr>
				<td colspan="4" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_NEXT_CONST.'"> <input type="button" class="button" name="close" value="'.$TEXT_CANCEL_CONST.' & '.$TEXT_CLOSE_CONST.'" onclick="self.close();"></td>
			</tr>
		</table><br>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
			var frmvalidator = new formValidator("addModel");
 			frmvalidator.addValidation("description","req","'.$TEXT_DEVICE_TEMPLATE_NAME_REQUIRED_CONST.'");
		</script>		
		';
	}else{
		// process
		$description=stripslashes($_POST['description']);
		
		if($dcID!=0 && $mID!=0 && $description!=''){
			$_SESSION['selectedCommMethod']=(int)$_POST['commMethod'];
			if($_SESSION['selectedCommMethod']==1){
				$parentCategory=$GLOBALS['InfraredInterface'];
				$implementDCE=0;
				$commandLine=NULL;
				$fk_package=NULL;
			}else{
				$parentCategory=$GLOBALS['rootComputerID'];
				$implementDCE=1;
				$commandLine='Generic_Serial_Device';
				$fk_package=307;
			}
			
			$publicADO->Execute('INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,psc_user,ImplementsDCE,CommandLine,FK_Package,FK_CommMethod) values(?,?,?,?,?,?,?,?)',array($description,$dcID,$mID,$userID,$implementDCE,$commandLine,$fk_package,$_SESSION['selectedCommMethod']));
			$dtID=$publicADO->Insert_ID();
			
			// add record to DeviceTemplate_AV
			$publicADO->Execute('INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate,psc_user) VALUES (?,?)',array($dtID,$userID));			

			// add controlled via infrared interface or computers
			$publicADO->Execute('INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia (FK_DeviceTemplate,FK_DeviceCategory) values(?,?)',array($dtID,$parentCategory));

			// hardcoded corespondence between AV categories cd, casette, dvd etc. and media types
			$dc_mtArray=array(106=>19,107=>12,108=>14,105=>15,135=>17,104=>25);

			if(isset($dc_mtArray[$dcID])){
				$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($dtID,$dc_mtArray[$dcID]));
			}
			// hardcoded: add embedded tv and vcr if device category is TV/VCR combo
			
			if($dcID==109 || $dcID==98){
				$publicADO->Execute('INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,161));
				createEmbeddedDeviceTemplate('TV - LiveTV',$mID,$dcID,$userID,$dtID,161,11,$publicADO);
				
				$publicADO->Execute('INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,282));
				createEmbeddedDeviceTemplate('VCR-1 - Video Tape',$mID,$dcID,$userID,$dtID,282,16,$publicADO);
			}
			
			
			$create_device=(int)@$_REQUEST['create_device'];
			if(session_name()=='Web-admin' && $create_device==1){
				// add the device
				global $dbPlutoMainDatabase;
				$installationID=(int)$_SESSION['installationID'];

				$insertID=createDevice($dtID,$installationID,0,NULL,$dbADO);
				$script='
				<script>
				opener.location.reload();
				self.location="index.php?section=addModel&step=2&dtID='.$dtID.'&isDef=1&deviceID='.@$insertID.'";
				</script>
				';
				die($script);
			}
			
			header('Location: index.php?section=addModel&step=2&dtID='.$dtID.'&isDef=1&deviceID='.@$insertID);
			exit();
		}
		
		header('Location: index.php?section=addModel&step=1&mID='.$mID.'&dcID='.$dcID.'&deviceID='.@$insertID);
	}
?>
<?
	// the document with instructions for option 1, I'll write C++ code
	$writeCodeDocument=1;
	
	// the document for virtual device, option 3
	$virtualDeviceDocument=1;
	
	$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$publicADO,'WHERE PK_Manufacturer='.$mID,'ORDER BY Description ASC');
	$categArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$publicADO,'WHERE PK_DeviceCategory='.$dcID,'ORDER BY Description ASC');
	
	if($action=='form'){
		$out='
		<h3 align="center">Add model</h3><br>
		<p class="normaltext">
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="mID" value="'.$mID.'">
			<input type="hidden" name="dcID" value="'.$dcID.'">
			<input type="hidden" name="from" value="'.$from.'">
		
		
		<table class="normaltext" align="center">
			<tr>
				<td align="right"><B>Manufacturer</B></td>
				<td>'.$manufArray[$mID].'</td>
				<td align="right"><B>Device Category</B></td>
				<td>'.$categArray[$dcID].'</td>
			</tr>';
		if($from!='genericSerialDevices'){
			$out.='
			<tr>
				<td><B>What type of device will this be?  Are you going to be writing code for it?</B></td>
				<td colspan="3">
					<input type="radio" name="opt" value="3" checked> No, this device doesn\'t require programming.  Examples: A/V equipment controlled by infrared, remote controls, and virtual devices are children of a dce device, like security sensors, light switches, etc.<br>
					<input type="radio" name="opt" value="1"> I\'m going to write c++ code, or use another language that can handle socket communications.  Examples: Software media players, phone systems, Java/Perl applications, etc.<br>
					<input type="radio" name="opt" value="2"> It\'s a simple, generic serial device controlled by RS232, Ethernet or USB, which you\'ll control with LinuxMCE\'s GSD and embedded Ruby snippets (very easy).  Examples: Lighting control, alarm panels, surveillance cameras, thermostats, weather stations, a/v equipment with RS232 ports, etc.<br>
				</td>
			</tr>			

			
			';
		}
			$out.='
			<tr>
				<td><B>What is the name or model # *</B></td>
				<td colspan="3"><input type="text" name="description" value=""> <input type="submit" class="button" name="add" value="Add"></td>
			</tr>
		</table><br>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
			var frmvalidator = new formValidator("addModel");
 			frmvalidator.addValidation("description","req","Please type the name of the model.");
		</script>		
		';
	}else{
		// process
		$opt=(int)@$_POST['opt'];
		$description=stripslashes($_POST['description']);
		if($from=='genericSerialDevices'){
			$opt=2;
		}

		switch ($opt){
			case 1:
				$implementDCE=1;
				$commandLine=NULL;
				$redirectJS='
				<script>
					window.open(\'index.php?section=editMasterDevice&model=&msg=The device template was added.\',\'_blank\',\'\')
					self.close();
				</script>
				';
			break;

			case 2:
				$implementDCE=1;
				$commandLine='Generic_Serial_Device';
				$fk_package=307;
				$addInstance=1;
				$addCommandGroup=1;
				if($from=='genericSerialDevices'){
					$redirectJS='
					<script>
					try{
						opener.location=\'index.php?section=genericSerialDevices&msg=The device was added.\';
						self.close();
					}catch(e){
						self.location=\'index.php?section=deviceTemplates&msg=The device template was added.\';
					}
					</script>
					';
				}else{
				$redirectJS='
				<script>
					opener.location=\'index.php?section=genericSerialDevices&msg=The device was added.\';
					self.close();
				</script>
				';

				}
			break;
			
			case 3:
				$implementDCE=0;
				$commandLine=NULL;
				$fk_package=NULL;
				$redirectJS='
				<script>
					try{
						window.open(\'index.php?section=editMasterDevice&model=&msg=The device template was added.\',\'_blank\',\'\')
						self.close();
					}catch(e){
						//
						self.location=\'index.php?section=deviceTemplates&msg=The device was added.\';
					}
				</script>
				';				
			break;
		}
		
		if($dcID!=0 && $mID!=0 && $description!=''){
			$publicADO->Execute('INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,psc_user,ImplementsDCE,CommandLine,FK_Package,Define) values(?,?,?,?,?,?,?,"")',array($description,$dcID,$mID,$userID,$implementDCE,$commandLine,$fk_package));
			$dtID=$publicADO->Insert_ID();
			
			if($dtID>0 && isset($addInstance)){
				// create the instance
				global $dbPlutoMainDatabase;
				$installationID=(int)$_SESSION['installationID'];
				//$deviceID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$dtID.' -i '.$installationID);
				$insertID=createDevice($dtID,$installationID,0,NULL,$dbADO);
			}
			
			$out=($opt==2)?$redirectJS:str_replace('model=','model='.$dtID,$redirectJS);				
				
		}else{
			header('Location: index.php?section=addModel&dcID='.$dcID.'&mID='.$mID.'&from='.$from.'&error=Please type the description.');
			exit();
		}
	}
?>
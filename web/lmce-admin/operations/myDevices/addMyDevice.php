<?php
function addMyDevice($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addMyDevice.lang.php');
		
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];


	if ($action == 'form') {
		$parentID = (int)@$_SESSION['parentID'];
		$parentID = isset($_REQUEST['parentID'])?cleanInteger($_REQUEST['parentID']):$parentID;
		
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		'.@$Alert.'
	</script>
		
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="addMyDevice">
	<input type="hidden" name="section" value="addMyDevice">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="parentID" value="'.$parentID.'">
		
		<h3>'.(($parentID==0)?$TEXT_ADD_TOP_LEVEL_DEVICE_CONST:$TEXT_ADD_CHILD_DEVICE_CONST).'</h3>
		<table>			
				<tr>
					<td>'.$TEXT_DESCRIPTION_CONST.':</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>				
				<tr>
					<td>'.$TEXT_IP_ADDRESS_CONST.':</td>
					<td><textarea name="IPaddress"></textarea></td>
				</tr>
				<tr>
					<td>'.$TEXT_MAC_ADDRESS_CONST.':</td>
					<td><textarea name="MACaddress"></textarea></td>
				</tr>
				<tr>
					<td>'.$TEXT_IGNORE_ON_OFF_CONST.':</td>
					<td>'.$TEXT_ON_CONST.':<input type="radio" value="1" name="IgnoreOnOff"> &nbsp; '.$TEXT_OFF_CONST.':<input type="radio" value="0" name="IgnoreOnOff" checked="checked"></td>
				</tr>
				<tr>
					<td>'.$TEXT_DEVICE_TEMPLATE_CONST.'</td>
					<td><input type="button" class="button" name="pick" value="'.$TEXT_PICK_DEVICE_TEMPLATE_CONST.'" onclick="document.addMyDevice.submit();windowOpen(\'index.php?section=deviceTemplatePicker&categoryID=0&from='.urlencode('addMyDevice&parentID='.$parentID).'&allowAdd=1\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=addMyDevice&parentID=$parentID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}


		if (isset($_REQUEST['deviceTemplate']) && (int)$_REQUEST['deviceTemplate']!=0) {
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			$descriptionMyDevice = $_SESSION['Description'];
			$IPaddressMyDevice = $_SESSION['IPaddressMyDevice'];
			$MACaddressMyDevice = $_SESSION['MACaddressMyDevice'];
			$ignoreOnOff = $_SESSION['ignoreOnOff'];
			$parentID = $_SESSION['parentID'];
			
			//$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID.(($IPaddressMyDevice!='')?' -I '.$IPaddressMyDevice:'').(($MACaddressMyDevice!='')?' -M '.$MACaddressMyDevice:'').(($parentID!=0)?' -C '.$parentID:''));
			$insertID=createDevice($deviceTemplate,$installationID,$parentID,NULL,$dbADO,0,$IPaddressMyDevice,$MACaddressMyDevice);				
			
			if($descriptionMyDevice!='')
				$dbADO->Execute('UPDATE Device SET Description=?, IgnoreOnOff=? WHERE PK_Device=?',array($descriptionMyDevice,$ignoreOnOff,$insertID));
				
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			
			if(isPhone($deviceTemplate,$dbADO)){
				$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.$insertID;
				exec($cmd);
			}
			
			unset($_SESSION['Description']);
			unset($_SESSION['IPaddressMyDevice']);
			unset($_SESSION['MACaddressMyDevice']);
			unset($_SESSION['ignoreOnOff']);

			$suffix=(isOrbiter($insertID,$dbADO) || isMediaDirector($insertID,$dbADO,1))?'&showNote=1':'';
			
			$out.="<script>
			top.frames['treeframe'].location='index.php?section=leftMenu&deviceID=$insertID';
			self.location.href=\"index.php?section=editDeviceParams&deviceID=$insertID&parentID=$parentID&msg=$TEXT_NEW_DEVICE_ADDED_CONST.$suffix\";
		</script>";
		}else{
			$_SESSION['Description']=cleanString(@$_POST['Description']);
			$_SESSION['IPaddressMyDevice'] = cleanString(@$_POST['IPaddress']);
			$_SESSION['MACaddressMyDevice'] = cleanString(@$_POST['MACaddress']);
			$_SESSION['ignoreOnOff'] = cleanInteger(@$_POST['IgnoreOnOff']);
			$_SESSION['parentID']=(int)@$_POST['parentID'];
			$parentID = isset($_REQUEST['parentID'])?cleanInteger($_REQUEST['parentID']):0;			
			
			header("Location: index.php?section=addMyDevice&parentID=$parentID");
			exit();
		}

	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

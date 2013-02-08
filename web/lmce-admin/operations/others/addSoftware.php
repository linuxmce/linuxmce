<?
function addSoftware($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addSoftware.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$compSelected=(isset($_REQUEST['computer']))?(int)$_REQUEST['computer']:0;
	
	
	if($action=='form'){
	$computers=getDevicesArrayFromCategory($GLOBALS['rootComputerID'],$dbADO,' AND FK_Device_ControlledVia IS NULL');

	$getXML_cmd='/usr/pluto/bin/getxmls';
	
	$out.='
	<script>
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}	
	</script>
	
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
			
		<form action="index.php" method="POST" name="addSoftware">
		<input type="hidden" name="section" value="addSoftware">
		<input type="hidden" name="action" value="form">
		
		<table>
			<tr>
				<td colspan="2"><input type="button" class="button" value="Update Software List" onClick="windowOpen(\'operations/logs/executeLog.php?script=4&cmd='.urlencode($getXML_cmd).'\',\'width=1024,height=768,scrollbars=1,resizable=1\');"></td>
			</tr>
			<tr>
				<td>'.$TEXT_ADD_SOFTWARE_ON_WHICH_COMPUTER_CONST.'</td>
				<td>'.pulldownFromArray($computers,'computer',$compSelected,'onChange="document.addSoftware.action.value=\'form\';document.addSoftware.submit();"').'</td>
			</tr>
			<tr>
				<td colspan="2">'.softwareList($compSelected,$dbADO).'</td>
			</tr>			
		</table>
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=addSoftware&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		if(isset($_REQUEST['sID'])){
			$sID=(int)@$_REQUEST['sID'];
			//	To install/remove the packege you have to send command to General_Info_Plugin
			//	CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software,string &sCMD_Result,Message *pMessage)
			$true_false=($action=='install')?1:0;
			
			$cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 27 1 813 2 '.$compSelected.' 229 '.$sID.' 119 '.$true_false;
			exec_batch_command($cmd);				
		}
		
		header('Location: index.php?section=addSoftware&computer='.$compSelected);
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_ADD_SOFTWARE_CONST);
	$output->setNavigationMenu(array($TEXT_ADD_SOFTWARE_CONST=>'index.php?section=addSoftware'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_SOFTWARE_CONST);
	$output->output();
}

function softwareList($compSelected,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addSoftware.lang.php');

	if($compSelected==0){
		return $TEXT_ERROR_NO_COMPUTER_SELECTED_CONST;		
	}
	
	$distroVersion=str_replace(array('LMCE_CORE_','LMCE_MD_'),'',getDeviceData($compSelected,$GLOBALS['DD_model'],$dbADO));
	$version=getVersion();
	
	$res=$dbADO->Execute('
		SELECT
			PK_Software,
			PackageName,
			`Status`
		FROM Software
		LEFT JOIN Software_Source ON Software_Source.FK_Software=PK_Software
		LEFT JOIN Software_Device ON Software_Device.FK_Software=PK_Software AND FK_Device=?
		WHERE Distro=?  AND Required_Version_Min<=? AND Required_Version_Max>=? AND Virus_Free=1 
		GROUP BY PK_Software, PackageName, `Status` 
		ORDER BY PackageName
		',array($compSelected,$distroVersion,$version,$version));
	
	$out='<table>
		<tr class="tablehead">
			<td align="center">'.$TEXT_ICON_CONST.'</td>
			<td align="center">'.$TEXT_PACKAGENAME_CONST.'</td>
			<td align="center">'.$TEXT_DISTRO_CONST.'</td>
			<td align="center">'.$TEXT_IS_INSTALLED_CONST.'</td>
			<td align="center">'.$TEXT_ACTION_CONST.'</td>
		</tr>
	';
	if($res->RecordCount()==0){
		$out.='
		<tr class="alternate_back">
			<td colspan="10">'.$TEXT_NO_RECORDS_CONST.'</td>
		</tr>';
	}
	$installStatusArray=array(
		'I'=>'Installed',
		'i'=>'Installing',
		'R'=>'Not installed',
		'r'=>'Removing',
		'N'=>"Installation aborted",
		''=>'Not installed'
	);
	
	$pos=0;
	while($row=$res->FetchRow()){
		switch ($row['Status']){
			case 'N':
			case 'r':
			case 'R':
			case null:
				$button='<input type="button" class="button" name="btn_'.$row['PK_Software'].'" value="'.$TEXT_INSTALL_CONST.'" onClick="self.location=\'index.php?section=addSoftware&action=install&computer='.$compSelected.'&sID='.$row['PK_Software'].'\'">';
			break;
			case 'i':
			case 'I':
				$button='<input type="button" class="button" name="btn_'.$row['PK_Software'].'" value="'.$TEXT_REMOVE_CONST.'" onClick="self.location=\'index.php?section=addSoftware&action=remove&computer='.$compSelected.'&sID='.$row['PK_Software'].'\'">';
			break;
			default:
				$button=' - ';
			break;
		}
		
		$class=($pos%2==0)?'alternate_back':'';
		$out.='
		<tr class="'.$class.'">
			<td align="center"><img src="softwareIcon.php?sID='.$row['PK_Software'].'"></td>
			<td align="center">'.$row['PackageName'].'</td>
			<td align="center">'.$distroVersion.'</td>
			<td align="center">'.$installStatusArray[@$row['Status']].'</td>
			<td align="center">'.$button.'</td>
		</tr>';
		$pos++;
	}
	$out.='<table>';
	
	return $out;
}

function getVersion(){
	$iniData=parse_ini_file('/etc/pluto.conf');
	
	return @$iniData['PlutoVersion'];
}
?>
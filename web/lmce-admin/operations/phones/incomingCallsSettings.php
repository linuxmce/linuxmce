<?
	ini_set('display_errors', 1); 
	error_reporting(E_ALL);

function incomingCallsSettings($output,$dbADO,$telecomADO,$asteriskADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/incomingCallsSettings.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$id=$_REQUEST['id'];
	
	if($action=='form'){
		$selectedValues=getSelectedValues($id,$telecomADO);

		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<a href="index.php?section=phoneLines">'.$TEXT_BACK_CONST.'</a>
		<form action="index.php" method="POST" name="incomingCallsSettings">
			<input type="hidden" name="section" value="incomingCallsSettings">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="id" value="'.$id.'">
			
			'.getPhoneLineDetails($id,$asteriskADO).'<br>
			'.getPhoneLinesSettings($id,$dbADO,$telecomADO,$selectedValues).'
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=incomingCallsSettings&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}	
		
		processSubmit($telecomADO);
		
		header('Location: index.php?section=incomingCallsSettings&id='.$id.'&msg='.$TEXT_ICS_UPDATED_CONST);
	}

	$output->setMenuTitle($TEXT_PHONE_LINES_CONST.' |');
	$output->setPageTitle($TEXT_INCOMMING_CALLS_SETTINGS_CONST);
	$output->setNavigationMenu(array($TEXT_PHONE_LINES_CONST=>'index.php?section=phoneLines',$TEXT_INCOMMING_CALLS_SETTINGS_CONST=>'index.php?section=incomingCallsSettings&id='.$id));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getPhoneLineDetails($id,$astADO){
	$count=0;
	$res=$astADO->Execute("SELECT * FROM phonelines WHERE id='$id'");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td align="center"><B>Name</B></td>
			<td align="center"><B>Protocol</B></td>
			<td align="center"><B>Phonenumber</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>'.$row['name'].'</td>
			<td>'.$row['protocol'].'</td>
			<td>'.$row['phonenumber'].'</td>
		</tr>';
	}
	
	$out.='</table><br>';
	if($count==0){
		$out='';
	}
	
	return $out;
}


// grab the settings from pluto_telecom table
function getPhoneLinesSettings($id,$dbADO,$telecomADO,$oldValues){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
		
	$houseModes=getAssocArray('HouseMode','PK_HouseMode','Description',$dbADO,'','ORDER BY Description ASC');
	$routingArray=array(
		'ring'=>'Ring extension(s)',
		'user'=>'Always transfer to a particular user',
		'voicemail'=>'Go directly to a user\'s voicemail',
		'prompt'=>'Prompt the caller to choose an extension or user',
		'transfer'=>'Transfer to an outside number');

	$pos=0;
	$out='<table align="center" cellpadding="2" cellspacing="0">';
	foreach ($houseModes AS $mode=>$label){
		$pos++;
		$color=($pos%2==1)?'#EEEEEE':'#FFFFFF';
		
		$databaseHM=(strpos(@$oldValues[$mode],',')!==false)?substr(@$oldValues[$mode],0,strpos(@$oldValues[$mode],',')):@$oldValues[$mode];
		$selectedHM=(isset($_REQUEST['routing_'.$mode]))?$_REQUEST['routing_'.$mode]:$databaseHM;
		$selectedValue=(isset($_REQUEST['routing_'.$mode]) && $_REQUEST['routing_'.$mode]!=$databaseHM)?'':substr(@$oldValues[$mode],strpos(@$oldValues[$mode],',')+1);

		$out.='
			<tr bgcolor="'.$color.'">
				<td><B>'.$label.'</B></td>
				<td>'.pulldownFromArray($routingArray,'routing_'.$mode,$selectedHM,'onChange="document.incomingCallsSettings.action.value=\'form\';document.incomingCallsSettings.submit();"').'</td>
			</tr>
			<tr bgcolor="'.$color.'">
				<td>&nbsp;</td>
				<td>'.routing_options($mode,$selectedHM,$selectedValue,$dbADO).'</td>
			</tr>		
		';
	}
	$out.='
		<tr bgcolor="#EEEEEE">
			<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
		</tr>
	</table>
	<input type="hidden" name="houseModes" value="'.join(',',array_keys($houseModes)).'">
	';
	
	
	return $out;
}

// retrive routing options based on value selected
// return blank html space if nothing is selected
function routing_options($mode,$selectedHM,$selectedValues,$dbADO){
	if($selectedHM=='0'){
		return '&nbsp;';
	}
	
	switch ($selectedHM){
		case 'ring':
			$phones=getDevicesArrayFromCategory($GLOBALS['rootPhones'],$dbADO);
			$selectedValuesArray=explode(',',$selectedValues);
			$out='';
			foreach ($phones AS $phoneID=>$phoneName){
				$out.='<input type="checkbox" name="phone_'.$mode.'_'.$phoneID.'" value="1" '.((in_array($phoneID,$selectedValuesArray))?'checked':'').'> '.$phoneName.'<br>';
			}
			$out.='<input type="hidden" name="phones_'.$mode.'" value="'.join(',',array_keys($phones)).'">';
		break;
		case 'user':
			$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);
			$out=pulldownFromArray($users,'user_'.$mode,$selectedValues);
		break;		
		case 'voicemail':
			$users=getAssocArray('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.(int)$_SESSION['installationID']);
			$out=pulldownFromArray($users,'voicemail_'.$mode,$selectedValues);
		break;		
		case 'prompt':
			$out='';
		break;			
		case 'transfer':
			$out='<input type="text" name="transfer_phone" value="'.$selectedValues.'">';
		break;		
		default:
			$out='&nbsp;';
		break;
	}
	
	
	return $out;
}

// extract selected values from line_HouseMode from table pluto_telecom
function getSelectedValues($id,$telecomADO){
	$res=$telecomADO->Execute('SELECT EK_HouseMode,Routing FROM Line_HouseMode WHERE ID='.$id);
	if($res->RecordCount()==0){
		return array();
	}
	
	$values=array();
	while($row=$res->fetchRow()){
		$values[$row['EK_HouseMode']]=$row['Routing'];
	}
	
	return $values;
}

function processSubmit($telecomADO){
	$id=$_REQUEST['id'];
	$selectedValues=getSelectedValues($id,$telecomADO);
	$houseModes=explode(',',$_REQUEST['houseModes']);

	foreach ($houseModes AS $mode){
		$mode_selected=$_REQUEST['routing_'.$mode];
		switch ($mode_selected) {
			case 'ring':
				$phones=explode(',',$_REQUEST['phones_'.$mode]);
				$phonesToRing=array();
				foreach ($phones As $phoneID){
					if(isset($_REQUEST['phone_'.$mode.'_'.$phoneID])){
						$phonesToRing[]=$phoneID;
					}
				}
				$routingValue='ring,'.join(',',$phonesToRing);				
			break;
			case 'user':
				$user=$_REQUEST['user_'.$mode];
				$routingValue='user,'.$user;
			break;
			case 'voicemail':
				$user=$_REQUEST['voicemail_'.$mode];
				$routingValue='voicemail,'.$user;
			break;			
			case 'transfer':
				$routingValue='transfer,'.$_REQUEST['transfer_phone'];
			break;			
			case 'prompt':
			default:
				$routingValue='prompt';
			break;
		}

		if(isset($selectedValues[$mode])){
			// update
			$telecomADO->Execute('UPDATE Line_HouseMode SET Routing=? WHERE ID=? AND EK_HouseMode=?',array($routingValue,$id,$mode));
		}else{
			// insert
			$telecomADO->Execute('INSERT INTO Line_HouseMode (ID,EK_HouseMode,Routing) VALUES (?,?,?)',array($id,$mode,$routingValue));
		}
	}

	$cmd='sudo -u root /usr/pluto/bin/db_create_dialplan.sh';
	exec($cmd);
}
?>

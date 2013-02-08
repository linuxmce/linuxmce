<?
	// TODO: Remove (debug)
	//error_reporting(E_ALL);
	//ini_set("display_errors", 1);

	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('fax.lang.php');

function fax($output,$astADO,$dbADO,$telecomADO) {
	global $wikiHost, $editedID, $editdata;
	
	// check if database asterisk exists
	$databases=$dbADO->MetaDatabases();
	if(!in_array('asterisk',$databases)){
		error_redirect(translate('TEXT_ASTERISK_DB_NOT_FOUND_CONST'),'index.php?section=phoneLines');
	}	
	
	$out=setLeftMenu($dbADO);
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	if($action=='form'){
		
		$out.='
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		
		'.
		// Form to send new fax
		sendFaxForm($telecomADO,$astADO)
				
		// Form to process already sent / received faxes
		.faxTable($astADO)

		// Form to show running sessions
		.faxSessions();
	}
	else{ // process area		
		// action is send new fax
		if(isset($action) && $action == 'sendFax') {
			// test if file type is supported
			if ($_FILES["file"]["type"] == "application/pdf") {			
				// test if upload was successfull
				if ($_FILES["file"]["error"] > 0) {
			    	$msg="Error: " . $_FILES["file"]["error"] . "<br />";
			  	// if yes then process file
			  	} else {
				    if (file_exists("/home/fax/outgoing/" . $_FILES["file"]["name"])){
						$msg=$_FILES["file"]["name"] . " already exists. ";
					} else {
						// send uploaded file as fax
						move_uploaded_file($_FILES["file"]["tmp_name"],"/home/fax/outgoing/" . $_FILES["file"]["name"]);
						// put fax into table and fetch assigned PK_Fax ID	
						$res=$astADO->Execute("INSERT INTO fax_list (Incoming,FileName,Result) VALUES ('0','".$_FILES["file"]["name"]."','processing')");
						$res=$astADO->Execute('SELECT LAST_INSERT_ID();');
						$row=$res->FetchRow();
						$id=$row[0];
						$cmd='sudo -u root /usr/pluto/bin/Fax_Send.sh file "/home/fax/outgoing/'.$_FILES["file"]["name"].'" '.$_REQUEST['faxNumber'].' 20408720 "LinuxMCE fax" '.$id;
						exec_batch_command($cmd,1);
						$msg="Submitted";						
					}
			    }
			} else {
				$msg=translate('TEXT_FAX_INVALID_FILE_TYPE_CONST').": ".$_FILES["file"]["type"];
			}
			if($msg != "") $suffix='&msg='.$msg;
			header('Location: index.php?section=fax'.@$suffix);
			exit();
		}
		
		header('Location: index.php?section=fax');
		exit();
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_FAX_MANAGEMENT_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_FAX_MANAGEMENT_CONST')=>'index.php?section=fax'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_FAX_MANAGEMENT_CONST'));
	$output->output();
}

// Display top form to send new fax
function sendFaxForm($telecomADO,$astADO) {
	$installationID = (int)@$_SESSION['installationID'];	

	// get list of fax contacts from phonebook
	$res=$telecomADO->Execute("SELECT PK_PhoneNumber,Name,PhoneNumber FROM Contact,PhoneNumber WHERE PK_Contact=FK_Contact AND FK_PhoneType=5 ORDER BY Name");
	$FaxContactsList['0']='Manualy entered';
	while($row=$res->FetchRow()) $FaxContactsList[$row['PhoneNumber']]=$row['Name'];

	// get list of fax lines
	$res=$astADO->Execute("SELECT phonenumber,protocol,name,isfax FROM `phonelines` WHERE enabled='yes' ORDER BY name ASC;");
	while($row=$res->FetchRow()) {
		$linename=$row['name'];
		if($row['isfax'] == 'yes') $linename.=" (FAX)";
		$FaxLinesList[$row['protocol'].'/'.$row['phonenumber']]=$linename;
	}
	
	$out='
	<script type="text/javascript">
		function chooseReceiver() 	{
			receiver=document.faxSend.faxReceiver.value;
			if(receiver != "0") {
				document.faxSend.faxNumber.value=receiver;
			} else {
				document.faxSend.faxNumber.value="";
			}
		}
		function modifyNumber() 	{
			// TODO: This should redraw the dropdown list
			document.faxSend.faxReceiver.selectedIndex=0;
		}
	</script>
	<form action="index.php" method="POST" name="faxSend" enctype="multipart/form-data">
		<input type="hidden" name="section" value="fax">
		<input type="hidden" name="action" value="sendFax">
		
		<table cellpadding="3" cellspacing="0">
			<tr class="tablehead">
				<td colspan="4"><B>'.translate('TEXT_FAX_SEND_NEW_CONST').'</B></td>
			</tr>
			<tr>
				<td><B>'.translate('TEXT_FAX_FILE_CONST').'</B></td>
				<td><input type="file" name="file" id="file" /></td>
				<td><B>'.translate('TEXT_FAX_DESTINATION_CONST').'</B></td>
				<td>
					<select name="faxReceiver"  style="width: auto" onchange="chooseReceiver();">';
					foreach ($FaxContactsList as $id => $number) $out.='<option value="'.$id.'" '.((@$ddArray['297'] ==				$id)?'selected="selected"':'').'>'.$number.'</option>';
					$out .='</select>
					<input type="text" name="faxNumber" value="" onchange="modifyNumber();">
				</td>
			</tr>
			<tr>
				<td><B>'.translate('TEXT_USE_PHONELINE_CONST').'</B></td>
				<td>
					<select name="faxLine"  style="width: auto">';
					foreach ($FaxLinesList as $id => $number) $out.='<option value="'.$id.'" '.((@$ddArray['297'] ==				$id)?'selected="selected"':'').'>'.$number.'</option>';
					$out .='</select>
				<td><B>'.translate('TEXT_FAX_RESOLUTION_CONST').'</B></td>
				<td>
					<select name="faxResolution"  style="width: auto">
						<option value="0" selected="selected">'.translate('TEXT_FAX_RESOLUTION_STANDARD_CONST').'</option>
						<option value="1">'.translate('TEXT_FAX_RESOLUTION_FINE_CONST').'</option>
					</select>
					&nbsp;&nbsp;
					<input type="submit" class="button" style="width: 100px" value="'.translate('TEXT_SEND_CONST').'">
				</td>
			</tr>	
		</table>
	</form>';	
	return $out;
}

// display table of sent and received faxes
function faxTable($astADO){
	global $editedID;
	global $editdata;
	$GLOBALS['count']=0;
	
	// read incoming fax list
	
	$res=$astADO->Execute("SELECT * FROM fax_list ORDER BY DateTime DESC");
	$out='
	<form action="index.php" method="POST" name="faxProcess" enctype="multipart/form-data">
		<input type="hidden" name="section" value="fax">
		<input type="hidden" name="action" value="">
	<table cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td><B>&nbsp;</B></td>
			<td><B>'.translate('TEXT_FAX_DATETIME_CONST').'</B></td>
			<td><B>'.translate('TEXT_FAX_RESULT_CONST').'</B></td>
			<td><B>'.translate('TEXT_FAX_REMOTECID_CONST').'</B></td>
			<td><B>'.translate('TEXT_FAX_FILENAME_CONST').'</B></td>
		</tr>	';

	while($row=$res->FetchRow()){
		$GLOBALS['count']++;
		$color=($GLOBALS['count']%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
		<td>'.($row['Incoming']=='1'
			?'<img src="/'.APPDIRECTORY.'/img/incoming.gif" height="15" alt="[->]">'
			:'<img src="/'.APPDIRECTORY.'/img/outgoing.gif" height="15" alt="[<-]">').'</td>
		<td>'.$row['DateTime'].'</td>
		<td align="center">';
		switch ($row['Result']) {
			case 'OK':
				$out.='<img src="/'.APPDIRECTORY.'/img/enabled.png" height="15" alt="[*]">';
				break;
			case 'processing':
				$out.='<img src="/'.APPDIRECTORY.'/img/processing.gif" height="15" alt="[*]">';
				break;
			default:
				$out.=$row['Result'];
				break;
		}
		$out .='</td>
		<td>'.$row['remoteCID'].'</td>
		<td>';
		if($row['Incoming']==1) {
			$out.='<a href="/fax-incoming/'.$row['FileName'].'">'.$row['FileName'].'</a>';
		} else {
			$out.='<a href="/fax-outgoing/'.$row['FileName'].'">'.$row['FileName'].'</a>';
		}
		$out.='</td>
		</tr>';
	}
	$out.='</table></form>';
	return $out;
}

// display running fax sessions
function faxSessions() {
	$cmd='sudo -u root /usr/sbin/asterisk -rx "fax show sessions" | grep Active';
	$response=exec_batch_command($cmd,1);
	$lines=explode("\n",$response);
	$out="";
	if(count($lines) > 0 AND $lines[0] != "") {
		$out.='
			<table cellpadding="3" cellspacing="0">
			<tr class="tablehead">
				<td><B>'.translate('TEXT_FAX_RUNNING_SESSIONS_CONST').'</B></td>
			</tr>';		
		foreach ($lines as $line) $out.='<tr><td>'.$line.'</td></tr>';
	}
	return $out;
}
?>

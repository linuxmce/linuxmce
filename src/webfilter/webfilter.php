<?PHP
function webfilterSettings($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');

	
$out='';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$accessFile=$GLOBALS['pluto.conf'];

	// check if proxy is disabled
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	if(count($retArray)==0){
		$_GET['error'].='Insuffient rights: pluto.conf file cannot be opened.';
	}
	
	//$dbADO->SetFetchMode(ADODB_FETCH_ARRAY);
	$res=$dbADO->Execute('SELECT var,val FROM webfilter_proxy WHERE conf="webfilter" AND conf_type="global"');
	 //$row=$res->FetchRow();
	 $key=array();
	 $value=array();
	while ($row=$res->FetchRow()) {
	$key[]=$row['var'];
	$value[]=$row['val'];
	}
	$keyval=array_combine($key, $value);
	//print_r($keyval);
if ($action == 'form') {
$loglevel=array(
		"0"=>"none",
		"1"=>"just denied",
		"2"=>"all text based",
		"3"=>"all requests"
		);
		
		$logfileformat=array(
		"1"=>"DansGuardian format (space delimited)",
		"2"=>"CSV-style format",
		"3"=>"Squid Log File Format",
		"4"=>"Tab delimited"
		);
$out.='	<script>
	function confirmDisableproxy()
	{
		if(document.webfilter.change_webfilter_status.checked){
			if(confirm("'.translate('TEXT_DISABLE_WEBFILTER_CONFIRMATION_CONST').'")){
				document.webfilter.submit();
			}else{
			document.webfilter.change_webfilter_status.checked=false;
			}
		}else{
			document.webfilter.submit();
		}
		</script>
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>	
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0" align="center">
	<tr>
	<td>
	<form action="index.php" method="POST" name="webfilter">
		<input type="hidden" name="section" value="webfilter">
	<input type="hidden" name="action" value="Enable_Disable">
	<h3 class="err">'.((@$DisableWebfilter!=1)?'':'- '.translate('TEXT_WEBFILTER_DISABLED_CONST').' - ').'</h3>
		<input type="checkbox" name="change_webfilter_status" value="1" '.((@$DisableWebfilter!=1)?'':'checked').' onClick="confirmDisablewebfilter()"> '.translate('TEXT_WEBFILTER_DISABLED_CONST').'<br />
		<input type="submit" class="button" name="save" value="Enable_Disable" />
	</form>
	</td>
	</tr>
	<tr>
	<td>
	<table>
	<tr>
	<td>
				<form action="index.php" method="POST" name="webfilter">
	<input type="hidden" name="section" value="webfilter" />
	<input type="hidden" name="action" value="save" />
	</td>
	</tr>
	<tr>
	<td>global settings:</td>
	</tr>
	<!--<tr>
	<td>Dansguardian filterip:</td>
	<td><input type="text" name="filterip" value="'.$keyval["filterip"].'" /></td>
	<td>(If left blank will listen on all IPs.)</td>
	</tr>
	<tr>
	<td>Dansguardian port:</td>
	<td><input type="text" name"filterport" value="'.$keyval["filterport"].'" /></td>
	</tr>
	<tr>
	<td>proxy ipaddress:</td>
	<td><input type="text" name="proxyip" value="'.$keyval["proxyip"].'"/></td>
	</tr>
	<tr>
	<td>proxy port:</td>
	<td><input type="text" name="proxyport" value="'.$keyval["proxyport"].'" /></td>
	</tr>-->
	<tr>
		<td>
		loglevel:</td>
		<td>
		<select name="loglevel">
		<option value="'.$keyval["loglevel"].'" selected>'.$loglevel[$keyval["loglevel"]].'</option>
		<option value="0">'.$loglevel["0"].'</option>
		<option value="1">'.$loglevel["1"].'</option>
		<option value="2">'.$loglevel["2"].'</option>
		<option value="3">'.$loglevel["3"].'</option>
		</select></td>
		</tr>
		<tr>
		<td>
		Format of logfile:</td>
		<td><select name="logfileformat">
		<option value="'.$keyval["logfileformat"].'" selected>'.$logfileformat[$keyval["logfileformat"]].'</option>
		<option value="1">'.$logfileformat["1"].'</option>
		<option value="2">'.$logfileformat["2"].'</option>
		<option value="3">'.$logfileformat["3"].'</option>
		<option value="4">'.$logfileformat["4"].'</option>
		</select></td>
		</tr>
		<tr>
		<td>Denied page:</td>
		<td><input type="text" name="accessdeniedaddress" value="'.$keyval["accessdeniedaddress"].'" /></td>
		<td>(With a \' at begin and end)</td>
		</tr>
		<tr>
		<td colspan="2"><input type="submit" class="button" name="save" value="'.translate('TEXT_SAVE_CONST').'"><input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>
			</form>
		</td>	
		</tr>
		</table>
		</td>
		</tr>
		<tr>
		<td>
		<form action="index.php" method="POST" name="webfilter">
		<input type="hidden" name="section" value="webfilter">
		<input type="hidden" name="action" value="add">';
		$groups=$keyval["filtergroups"];
		$i=0;
		$groupsdetails='<table>
		<tr>
		<td>Description</td><td>Group</td><td>mode</td><td>block</td><td>Disable AntiVirus</td><td>Bypass link</td><td>settings</td><td>lists</td>
		</tr>';
		while ( $i <= $groups) {
		$fres=$dbADO->Execute('SELECT var,val FROM webfilter_proxy WHERE conf="webfilter" AND conf_type="f'.$i.'"');
		$key=array();
	 $value=array();
	while ($frow=$fres->FetchRow()) {
	$key[]=$row['var'];
	$value[]=$row['val'];
	}
	$fkeyval=array_combine($key, $value);
		if ($fkeyval["bypass"] == 0) {
		$bypass='off';
		} else {
		$bypass='on';
		}
			if ($i==0) {
			$groupsdetails.='<tr><td>In Common, f'.$i.'</td><td>'.$fkeyval["groupname"].'</td><td>'.$fkeyval["groupmode"].'</td><td></td><td>'.$fkeyval["disablecontentscan"].'</td><td>'.$bypass.'</td><td></td><td><a href="index.php?section=webfilter_list&group=0">Modify</a></td></tr>';
			} elseif ($i==1) {
			$groupsdetails.='<tr><td>Default group, f'.$i.'</td><td>'.$keyval["groupname"].'</td><td>'.$fkeyval["groupmode"].'</td><td></td><td>'.$fkeyval["disablecontentscan"].'</td><td>'.$bypass.'</td><td><a href="index.php?section=webfilter_groups&group='.$i.'">Modify</a></td><td><a href="index.php?section=webfilter_list&group='.$i.'">Modify</a></td><tr>';
			} else {
			$groupsdetails.='<tr><td>Filter group, f'.$i.'</td><td>'.$fkeyval["groupname"].'</td><td>'.$fkeyval["groupmode"].'</td><td></td><td>'.$fkeyval["disablecontentscan"].'</td><td>'.$bypass.'</td><td><a href="index.php?section=webfilter_groups&group='.$i.'">Modify</a></td><td><a href="index.php?section=webfilter_list&group='.$i.'">Modify</a></td><tr>';
			}
		$i++;
		}
		$groupsdetails.='</table>';
		$out.=$groupsdetails.'
		</td>
		</tr>
		<tr>
			<td colspan="7" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_CONST').'"></td>
		</tr>
		</form>
	</td>
    </tr>
		</table>';
}else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=webfilter&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
	if(isset($_POST['action'])){
		if($_POST['action']=='add'){
			$oldgroups=$keyval["filtergroups"];
			$newgroups=$oldgroups;
			$newgroups++;
			if (!copy('/etc/dansguardian/dansguardianf0.conf','/etc/dansguardian/dansguardianf'.$newgroups.'.conf')) {
				header("Location: index.php?section=webfilter&error=TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST");
			} else {
				$dbADO->Execute('UPDATE webfilter_proxy SET val='.$newgroups.' WHERE conf="webfilter" AND conf_type="global" AND var="filtergroups" AND val="'.$oldgroups.'"');
				$dbADO->Execute('INSERT INTO webfilter_proxy (conf, conf_type, var, val) SELECT conf,"f'.$newgroups.'",var,val FROM webfilter_proxy WHERE conf_type="f0"');
				//exec_batch_command('sudo -u root /usr/pluto/bin/webfilter_proxy.sh webfilter global');
				header("Location: index.php?section=webfilter_groups&group=".$newgroups);
			}
		}
		
		if($_POST['action']=='save') {
			foreach($_POST as $key=>$val) {
				if ($key == 'section' || $key=='save' || $key=='action') {
				} else {
				$dbADO->Execute('UPDATE webfilter_proxy SET val="'.$val.'" WHERE conf="webfilter" AND conf_type="global" AND var="'.$key.'" AND val="'.$keyval[$key].'"');
				}
			}
			exec_batch_command('sudo -u root /usr/pluto/bin/webfilter_proxy.sh global');
			header("Location: index.php?section=webfilter&msg=".translate('TEXT_WEBFILTER_UPDATED_CONST'));
		}
	
		if ($_POST['action'] == 'Enable_Disable') {
			if ($_POST['change_webfilter_status']==1){ 
				writeConf($accessFile, 'DisableWebfilter',@$DisableWebfilter,1);
			}	else { 
				writeConf($accessFile, 'DisableWebfilter',@$DisableWebfilter,0);
			}
			exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
			header("Location: index.php?section=webfilter&msg=".translate('TEXT_WEBFILTER_UPDATED_CONST'));
		}
	}
}	
$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_WEBFILTER_GLOBAL_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_WEBFILTER_GLOBAL_CONST')=>'index.php?section=webfilter'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_WEBFILTER_GROUPS_CONST'));
	$output->output();
}



function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=webfilter&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
		exit();
	}
	$oldFile=implode('',$oldFileArray);
	$stringToReplace=$variable.'='.$oldValue;
	if(ereg($stringToReplace,$oldFile)){
		$newFile=str_replace($stringToReplace,$variable.'='.$newValue,$oldFile);
	}
	else
		$newFile=$oldFile.$variable.'='.$newValue."\n";
	if(!is_writable($accessFile)){
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=webfilter&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>
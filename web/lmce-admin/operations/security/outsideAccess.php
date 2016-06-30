<?
function outsideAccess($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('outsideAccess.lang.php');
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	$accessFile=$GLOBALS['pluto.conf'];

	// check if firewall is disabled
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	if(count($retArray)==0){
		$_GET['error'].='Insufficient rights: pluto.conf file cannot be opened.';
	}

	function getMAC($int){
		$mac = exec("ip link show $int | awk '/ether/ {print $2}'");
		return $mac;
	}
	
	function getIP($int) {
		$ip = exec("ip addr show $int| awk '/inet / {print $2}'");
		$pieces = explode("/", $ip);
		return $pieces[0];
	}
	
	function getMASK($int) {
		$ip = exec("ip addr show $int| awk '/inet / {print $2}'");	
		$pieces = explode("/", $ip);
		$netmask = str_split(str_pad(str_pad('', $pieces[1], '1'), 32, '0'), 8);    
		foreach ($netmask as &$element) $element = bindec($element);    
		return join('.', $netmask);
	}
	
	function getGW() {
		$gw = exec("ip -f inet route show|awk '/default/ {print($3)}'");
		return $gw;
	}
	
	function getDNS() {
		$dns = shell_exec("cat /etc/resolv.conf|awk '/nameserver/ {print $2}'");
		$dns = implode(", ",explode("\n", trim($dns)));
		return $dns;
	}
	
		$queryDevice='
		SELECT Device.*
			FROM Device
		INNER JOIN DeviceTemplate ON 
			FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND Device.FK_Installation=?
		';
	$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['CategoryCore'],$installationID));
	
	if($resDevice->RecordCount()!=0){
		$rowDevice=$resDevice->FetchRow();
		$coreID=$rowDevice['PK_Device'];
	}
	
	// extract NIC IPv4 settings from core
	$queryNC='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
	$resNC=$dbADO->Execute($queryNC,array($coreID,$GLOBALS['NetworkInterfaces']));
	
	if($resNC->RecordCount()>0){
		$rowNC=$resNC->FetchRow();
		$interfacesArray=explode('|',$rowNC['IK_DeviceData']);
		
		// get external NIC IPv4 settings
		$externalInterfaceArray=explode(',',$interfacesArray[0]);
		$externalMAC=getMAC($externalInterfaceArray[0]);
		$coreIPv4Status=$externalInterfaceArray[1];

		switch ($coreIPv4Status) {
		    case 'disabled':
		    	$coreIPv4='disabled';
		        break;

		    case 'dhcp':
				$ipFromDHCP=1;
	            $coreIPv4=getIP($externalInterfaceArray[0]);
	            $coreIPv4NetMask=getMASK($externalInterfaceArray[0]);
				$coreIPv4GW=getGW();
				$DNS=explode(',',getDNS());
				$coreIPv4DNS1=trim($DNS[0]);
				$coreIPv4DNS2=trim($DNS[1]);
				$coreIPv4DNS3=trim($DNS[2]);
				break;
				
		    case 'static';
		    default:
		    	$coreIPv4Status='static';
				$ipFromDHCP=0;
				$coreIPv4=$externalInterfaceArray[1];
				$coreIPv4NetMask=$externalInterfaceArray[2];
				$coreIPv4GW=$externalInterfaceArray[3];
				$coreIPv4DNS1=$externalInterfaceArray[4];
				$coreIPv4DNS2=$externalInterfaceArray[5];
				break;			
		}
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$queryDSS='
		SELECT Device_StartupScript.Enabled, PK_Device, Parameter,FK_Device 
		FROM Device 
		INNEr JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_StartupScript ON FK_Device=PK_Device AND FK_StartupScript=?
		WHERE FK_DeviceCategory=? AND FK_Installation=?';

	$resDSS=$dbADO->Execute($queryDSS,array($GLOBALS['ProcessLogs'],$GLOBALS['CategoryCore'],$installationID));
	if($resDSS->RecordCount()>0){
		$rowDSS=$resDSS->FetchRow();
		$sendErrorsToPluto=($rowDSS['Enabled']==1 && $rowDSS['Parameter']!='0')?1:0;
		$coreID=$rowDSS['PK_Device'];
	}
	
	$resAllow80=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="webadmin default port" AND DestinationPort="80:NULL"');
	$allowAccessOn80=($resAllow80->RecordCount()>0)?1:0;
	$rowAccessOn80=$resAllow80->FetchRow();
	$WebId1=isset($rowAccessOn80['Place'])?$rowAccessOn80['Place']:'0';		
	$resAllowPort=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="webadmin non default port" AND DestinationPort!="80"');
	$allowAccessOnPort=($resAllowPort->RecordCount()>0)?1:0;
	$rowAccessOnPort=$resAllowPort->FetchRow();
	$port=$rowAccessOnPort['SourcePort'];
	$port=explode(':', $port);
	$port=$port[0];
	$WebId2=isset($rowAccessOnPort['Place'])?$rowAccessOnPort['Place']:'0';

	$resAllowSSL443=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="SSL webadmin default port" AND DestinationPort="443:NULL"');
	$allowSSLAccessOn443=($resAllowSSL443->RecordCount()>0)?1:0;
	$rowAccessOn443=$resAllowSSL443->FetchRow();
	$SslId1=isset($rowAccessOn443['Place'])?$rowAccessOn443['Place']:'0';

	$resAllowSSLPort=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="SSL webadmin non default port" AND DestinationPort!="443"');
	$allowSSLAccessOnPort=($resAllowSSLPort->RecordCount()>0)?1:0;
	$rowSSLAccessOnPort=$resAllowSSLPort->FetchRow();
	$sslport=$rowSSLAccessOnPort['SourcePort'];
	$sslport=explode(':', $sslport);
	$sslport=$sslport[0];
	$SslId2=isset($rowSSLAccessOnPort['Place'])?$rowSSLAccessOnPort['Place']:'0';
	
	$resAllow22=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="SSH default port" AND DestinationPort="22:NULL"');
	$allowAccessOn22=($resAllow22->RecordCount()>0)?1:0;
	$rowAccessOn22=$resAllow22->FetchRow();
	$SshId1=isset($rowAccessOn22['Place'])?$rowAccessOn22['Place']:'0';

	$resAllowSSHPort=$dbADO->Execute('SELECT * FROM Firewall WHERE Description="SSH non default port" AND DestinationPort!="22"');
	$allowSSHAccessOnPort=($resAllowSSHPort->RecordCount()>0)?1:0;
	$rowSSHAccessOnPort=$resAllowSSHPort->FetchRow();
	$sshport=$rowSSHAccessOnPort['SourcePort'];
	$sshport=explode(':', $sshport);
	$sshport=$sshport[0];
	$SshId2=$rowSSHAccessOnPort['Place']?$rowSSHAccessOnPort['Place']:'0';
	
	if ($action=='form') {
		

		if(count($retArray)==0){
			$out.='<div class="err">'.translate('TEXT_ERROR_OPENING_PLUTO_CONF_CONST').'</div>';
		}
		$out.=setLeftMenu($dbADO).'<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
	<script>
	function validateInput()
	{
		portValue=parseInt(document.outsideAccess.port.value);
		if(document.outsideAccess.allowOnPort.checked && (isNaN(portValue) || portValue==80 || portValue<=0)){
			alert("Please type the port, other value than 80.");
			document.outsideAccess.port.focus();
			return false;
		}
		portValue=parseInt(document.outsideAccess.sslport.value);
		if(document.outsideAccess.allowSSLOnPort.checked && (isNaN(portValue) || portValue==443 || portValue<=0)){
			alert("Please type the port, other value than 443.");
			document.outsideAccess.sslport.focus();
			return false;
		}
		portValue=parseInt(document.outsideAccess.sshport.value);
		if(document.outsideAccess.allowSSHOnPort.checked && (isNaN(portValue) || portValue==22 || portValue<=0)){
			alert("Please type the port, other value than 22.");
			document.outsideAccess.sshport.focus();
			return false;
		}
		
		if(document.outsideAccess.allowOnPassword.checked && document.outsideAccess.password.value!=document.outsideAccess.password1.value){
			alert("Please type the same password.");
			document.outsideAccess.password.focus();
			return false;
		}
		return true;
	}
		
	function setAntiHanging()
	{
		document.outsideAccess.RA_CheckRemotePort.checked=(document.outsideAccess.allowOnPassword.checked)?true:false;
	}
	</script>	
		
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="post" name="outsideAccess" onSubmit="return validateInput();">
	<input type="hidden" name="section" value="outsideAccess">
	<input type="hidden" name="action" value="add">
	<span class="err"><B>'.(isset($remote)?translate('TEXT_PROVIDE_PASSWORD_AND_INSTALLATION_CONST').' '.$installationID.'-'.$remote:'').'</B></span>
	<table width="600">
		<tr>
			<td colspan="3">'.translate('TEXT_REMOTE_ASSISTANCE_CONST').': <B>'.(isset($remote)?translate('TEXT_ENABLED_CONST'):translate('TEXT_DISABLED_CONST')).'</B> <input type="submit" name="'.(isset($remote)?'deactivate':'activate').'" class="button" value="'.(isset($remote)?translate('TEXT_DISABLE_CONST'):translate('TEXT_ENABLE_CONST')).'"></td>
		</tr>	
		<tr>
			<td><input type="hidden" name="WebId1" value="'.$WebId1.'" /><input type="checkbox" name="allow80" value="1" '.(($allowAccessOn80==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_80_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="hidden" name="WebId2" value="'.$WebId2.'" /><input type="checkbox" name="allowOnPort" value="1" '.(($allowAccessOnPort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_OTHER_CONST').'</td>
			<td><input type="text" name="port" value="'.@$port.'"></td>
		</tr>
		<tr>
			<td><input type="hidden" name="SslId1" value="'.$SslId1.'" /><input type="checkbox" name="allow443" value="1" '.(($allowSSLAccessOn443==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_SSL443_CONST').'</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td><input type="hidden" name="SslId2" value="'.$SslId2.'" /><input type="checkbox" name="allowSSLOnPort" value="1" '.(($allowSSLAccessOnPort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_SSLOTHER_CONST').'</td>
			<td><input type="text" name="sslport" value="'.@$sslport.'"></td>
		</tr>
		<tr>
			<td><input type="hidden" name="SshId1" value="'.$SshId1.'" /><input type="checkbox" name="allow22" value="1" '.(($allowAccessOn22==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_22_CONST').'</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
		<td><input type="hidden" name="SshId2" value="'.$SshId2.'" /><input type="checkbox" name="allowSSHOnPort" value="1" '.(($allowSSHAccessOnPort==1)?'checked':'').'></td>
		<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_22OTHER_CONST').'</td>
		<td><input type"text" name="sshport" value="'.@$sshport.'" /></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="RAport" value="1" '.(((int)@$RAport==22)?'checked':'').'></td>
			<td>'.translate('TEXT_USE_PORT_22_FOR_REMOTE_ASSISTANCE_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="RA_CheckRemotePort" value="1" '.(((int)@$RA_CheckRemotePort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ENABLE_ANTI_HANGING_MEASURE_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="sendErrorsToPluto" value="1" '.((@$sendErrorsToPluto==1)?'checked':'').'></td>
			<td>'.translate('TEXT_SEND_ERRORS_TO_PLUTO_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td colspan="2">'.translate('TEXT_OUTSIDE_ACCESS_NOTES_CONST').'</td>
		</tr>		
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="save" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>
	</table>
		<input type="hidden" name="oldAllow80" value="'.$allowAccessOn80.'">
		<input type="hidden" name="oldAllowOnPort" value="'.$allowAccessOnPort.'">
		<input type="hidden" name="oldPort" value="'.@$port.'">
		<input type="hidden" name="oldAllow443" value="'.$allowSSLAccessOn443.'">
		<input type="hidden" name="oldAllowSSLOnPort" value="'.$allowSSLAccessOnPort.'">
		<input type="hidden" name="oldSSLPort" value="'.@$sslport.'">
		<input type="hidden" name="oldAllow22" value="'.$allowAccessOn22.'">
		<input type="hidden" name="oldAllowSSHOnPort" value="'.$allowSSHAccessOnPort.'">
		<input type="hidden" name="oldSSHPort" value="'.@$sshport.'">
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=outsideAcces&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}
		
		if(isset($_POST['activate'])){
			// /usr/pluto/bin/RA-handler.sh {--enable|--disable
			$cmd='sudo -u root /usr/pluto/bin/RA-handler.sh --enable';
			exec_batch_command($cmd);
		}

		if(isset($_POST['deactivate'])){
			$cmd='sudo -u root /usr/pluto/bin/RA-handler.sh --disable';
			exec_batch_command($cmd);
		}		
		
		if(isset($_POST['save'])){
			
			$oldSendErrorsToPluto=(int)@$sendErrorsToPluto;
			$newSendErrorsToPluto=(int)@$_POST['sendErrorsToPluto'];
			if($oldSendErrorsToPluto==0){
				if($newSendErrorsToPluto==1){
					if($rowDSS['FK_Device']!=''){
						$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=1, Parameter=? WHERE FK_Device=? AND FK_StartupScript=?',array('',$coreID,$GLOBALS['ProcessLogs']));
					}else{
						$dbADO->Execute('INSERT INTO Device_StartupScript (FK_Device, FK_StartupScript,Enabled,Parameter) VALUES (?,?,1,?)',array($coreID,$GLOBALS['ProcessLogs'],''));
					}
				}
			}else{
				if($newSendErrorsToPluto==0){
					if(isset($coreID)){
						$dbADO->Execute('UPDATE Device_StartupScript SET Parameter=0 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ProcessLogs']));
					}
				}
			}
		
			

			$allow80=(int)@$_POST['allow80'];
			$oldAllow80=(int)@$_POST['oldAllow80'];
			$WebId1=(int)$_POST['WebId1'];
			$allow443=(int)@$_POST['allow443'];
			$oldAllow443=(int)@$_POST['oldAllow443'];
			$SslId1=(int)$_POST['SslId1'];
			$allow22=(int)@$_POST['allow22'];
			$oldAllow22=(int)@$_POST['oldAllow22'];
			$SshId1=(int)$_POST['SshId1'];
			
			$oldAllowOnPort=@(int)$_POST['oldAllowOnPort'];
			$allowOnPort=@(int)$_POST['allowOnPort'];
			$WebId2=(int)$_POST['WebId2'];
			$oldPort=(int)@$_POST['oldPort'];
			$port=(int)@$_POST['port'];
			$oldAllowSSLOnPort=@(int)$_POST['oldAllowSSLOnPort'];
			$allowSSLOnPort=@(int)$_POST['allowSSLOnPort'];
			$SslId2=(int)$_POST['SslId2'];
			$oldSSLPort=(int)@$_POST['oldSSLPort'];
			$sslport=(int)@$_POST['sslport'];
			$oldAllowSSHOnPort=@(int)$_POST['oldAllowSSHOnPort'];
			$allowSSHOnPort=@(int)$_POST['allowSSHOnPort'];
			$SshId2=(int)$_POST['SshId2'];
			$oldSSHPort=(int)@$_POST['oldSSHPort'];
			$sshport=(int)@$_POST['sshport'];

			if($oldAllow80==0){
				if($allow80==1){
					$options='-L Rule -H local -t filter -A add -p '.$WebId1.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S NULL -s NULL -r '.$coreIPv4.' -D 80 -d NULL -T ACCEPT -c \'webadmin default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allow80==0){
					$options='-L Rule -H local -t filter -A Del -p '.$WebId1.' -C INPUT -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			if($oldAllowOnPort==0){
				if($allowOnPort==1){
					$options='-L Rule -H local -t nat -A add -p '.$WebId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$port.' -s NULL -r 127.0.0.1 -D 80 -d NULL -T ACCEPT -c \'webadmin non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allowOnPort==0){
					$options='-L Rule -H local -t nat -A Del -p '.$WebId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}elseif($port!=$oldPort){
					$options='-L Rule -H local -t nat -A Del -p '.$WebId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
					$options='-L Rule -H local -t filter -A add -p '.$WebId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$port.' -s NULL -s NULL -r 127.0.0.1 -D 80 -d NULL -T ACCEPT -c \'webadmin non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			if($oldAllow443==0){
				if($allow443==1){
					$options='-L Rule -H local -t filter -A add -p '.$SslId1.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S NULL -s NULL -r '.$coreIPv4.' -D 443 -d NULL -T ACCEPT -c \'SSL webadmin default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allow443==0){
					$options='-L Rule -H local -t filter -A Del -p '.$SslId1.' -C INPUT -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			if($oldAllowSSLOnPort==0){
				if($allowSSLOnPort==1){
					$options='-L Rule -H local -t filter -A add -p '.$SslId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$sslport.' -s NULL -r 127.0.0.1 -D 443 -d NULL -T ACCEPT -c \'SSL webadmin non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allowSSLOnPort==0){
					$options='-L Rule -H local -t nat -A Del -p '.$SslId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}elseif($sslport!=$oldSSLPort){
					$options='-L Rule -H local -t filter -A Del -p '.$SslId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
					$options='-L Rule -H local -t filter -A add -p '.$SslId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$sslport.' -s NULL -r 127.0.0.1 -D 443 -d NULL -T ACCEPT -c \'SSL webadmin non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			if($oldAllow22==0){
				if($allow22==1){
					$options='-L Rule -H local -t filter -A add -p '.$SshId1.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S NULL -s NULL -r '.$coreIPv4.' -D 22 -d NULL -T ACCEPT -c \'SSH default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allow22==0){
					$options='-L Rule -H local -t filter -A Del -p '.$SshId1.' -C INPUT -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			if($oldAllowSSHOnPort==0){
				if($allowSSHOnPort==1){
					$options='-L Rule -H local -t filter -A add -p '.$SshId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$sshport.' -s NULL -r 127.0.0.1 -D 22 -d NULL -T ACCEPT -c \'SSH non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}else{
				if($allowSSHOnPort==0){
					$options='-L Rule -H local -t nat -A Del -p '.$SshId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}elseif($sshport!=$oldSSHPort){
					$options='-L Rule -H local -t nat -A Del -p '.$SshId2.' -C PREROUTING -P tcp-ipv4';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
					$options='-L Rule -H local -t filter -A add -p '.$SshId2.' -C AUTO -I NULL -O NULL -M NULL -P tcp-ipv4 -R NULL -S '.$sshport.' -s NULL -r 127.0.0.1 -D 22 -d NULL -T ACCEPT -c \'SSH non default port\'';
					exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh '.$options);
				}
			}
			echo $options;

			if(isset($_POST['RAport'])){
				if(@$RAport!=22){
					writeToFile($accessFile, 'RAport',0,22);
					$suffix=' restart';
				}
			}else{
				if(@$RAport==22){
					removeFromFile('RAport',$accessFile);
					$suffix=' restart';
				}
			}
						
			if((int)@$RA_CheckRemotePort==1){
				if(!isset($_POST['RA_CheckRemotePort'])){
					writeToFile($accessFile, 'RA_CheckRemotePort',1,0);	
				}
			}else{
				if(isset($_POST['RA_CheckRemotePort'])){
					writeToFile($accessFile, 'RA_CheckRemotePort',0,1);
				}
			}
			$cmd='sudo -u root /usr/pluto/bin/SetupRemoteAccess.sh'.@$suffix;
			exec($cmd);
			header("Location: index.php?section=outsideAccess&msg=".translate('TEXT_REMOTE_ACCESS_UPDATED_CONST'));
			exit();
		}
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_OUTSIDE_ACCESS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_OUTSIDE_ACCESS_CONST')=>'index.php?section=outsideAccess'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_OUTSIDE_ACCESS_CONST'));			
	$output->output();  		
}

function writeToFile($accessFile, $variable,$oldValue,$newValue)
{	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile." ");
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
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}

function removeFromFile($variable,$accessFile)
{
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
		exit();
	}
	$oldFile='';
	foreach($oldFileArray AS $line)
		if(!ereg($variable,$line))
			$oldFile.=$line;
	if(!is_writable($accessFile)){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $oldFile)) {
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}


function getRemotePorts($installationID){
	global $LinuxMCEHost;
	$cmd='wget \''.$LinuxMCEHost.'get_ra_ports.php?installationID='.$installationID.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	$out=exec_batch_command($cmd,1);	
	
	// output like Ports=SSH=10003&Web=10004&SSHnomon=10005#
	if(substr($out,0,5)!='Ports' && substr($out,-1)!='#'){
		return false;
	}
	return substr($out,6,-1);
}

function freeRemotePorts($installationID){
	global $LinuxMCEHost;
	$cmd='wget \''.$LinuxMCEHost.'get_ra_ports.php?installationID='.$installationID.'&action=del\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	$out=exec_batch_command($cmd,1);	
	
	return ($out=='Success')?true:false;
	
}
?>

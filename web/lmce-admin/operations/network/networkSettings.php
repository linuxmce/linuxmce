<?php
function getMAC($int){
	$mac = exec("ip link show $int | awk '/ether/ {print $2}'");
 	return $mac;
}

function getIP($int) {
	$ip = exec("ip addr show $int| awk '/inet / {print $2}'");
	$pieces = explode("/", $ip);
	return $pieces[0];
}

function getIPv6IP($int) {
	$ip = exec("ip -f inet6 addr show $int| awk '/scope global/ {print $2}'");
	$pieces = explode("/", $ip);
	return $pieces[0];
}

function getGW() {
	$gw = exec("ip -f inet route show|awk '/default/ {print($3)}'");
	return $gw;
}

function getIPv6GW() {
	$gw = exec("ip -f inet6 route show|awk '/default/ {print($3)}'");
	return $gw;
}

function getMASK($int) {
 	$ip = exec("ip addr show $int| awk '/inet / {print $2}'");	
    $pieces = explode("/", $ip);
	$netmask = str_split(str_pad(str_pad('', $pieces[1], '1'), 32, '0'), 8);    
	foreach ($netmask as &$element) $element = bindec($element);    
	return join('.', $netmask);
}

function getIPv6MASK($int) {
 	$ip = exec("ip -f inet6 addr show $int| awk '/scope global/ {print $2}'");	
    $pieces = explode("/", $ip);
	return $pieces[1];
}

function getOutSideIP() {
	$ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://automation.whatismyip.com/n09230945.asp');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $internetip = curl_exec($ch);
	if(!filter_var($internetip, FILTER_VALIDATE_IP)) {
		$internetip='';
	}
	return $internetip;
}

function getDNS() {
    $dns = shell_exec("cat /etc/resolv.conf|awk '/nameserver/ {print $2}'");
    $dns = implode(", ",explode("\n", trim($dns)));
    return $dns;
}

function getIPv6DNS() {
    $dns = shell_exec("cat /etc/resolv.conf|awk '/nameserver/ {print $2}'");
    $dns = implode(", ",explode("\n", trim($dns)));
    return $dns;
}

function networkSettings($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
	includeLangFile('networkSettings.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//	$dbADO->debug=true;
	// # TODO - We should take wlan adapters into account as well, especially for the external network
	$number_of_cards = exec('/sbin/ifconfig -s | awk \'$1 != "Iface" && $1 != "lo" { print $1 }\' | wc -l');
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	$OfflineMode=exec('sudo -u root /usr/pluto/bin/OfflineMode.sh get');

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
	$domain=getDeviceData($coreID,$GLOBALS['ComputerDomain'],$dbADO);
	$cname=getDeviceData($coreID,$GLOBALS['ComputerName'],$dbADO);
	
	$queryDHCP='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
	$resDHCP=$dbADO->Execute($queryDHCP,array($coreID,$GLOBALS['DHCPDeviceData']));
	$rowDHCP=$resDHCP->FetchRow();
	$enableDHCP=($rowDHCP['IK_DeviceData']!='')?1:0;
	
	$nonPlutoIP=0;
	$oldCoreDHCP = $rowDHCP['IK_DeviceData'];
	if(ereg(',',$oldCoreDHCP)){
		$nonPlutoIP=1;
		$ipArray=explode(',',$oldCoreDHCP);
		$coreDHCPArray=explode('-',$ipArray[0]);
		$coreDHCPStart=$coreDHCPArray[0];
		$coreDHCPEnd=explode('.',$coreDHCPArray[1]);
		$coreDHCPEnd=$coreDHCPEnd[3];
		$nonPlutoIPArray=explode('-',$ipArray[1]);
		$nonPlutoIPStart=$nonPlutoIPArray[0];
		$nonPlutoIPEnd=explode('.',$nonPlutoIPArray[1]);
		$nonPlutoIPEnd=$nonPlutoIPEnd[3];		
	}else{
		$coreDHCPArray=explode('-',$oldCoreDHCP);
		$coreDHCPStart=$coreDHCPArray[0];
		$coreDHCPEnd=explode('.',$coreDHCPArray[1]);
		$coreDHCPEnd=$coreDHCPEnd[3];
	}

	// extract radvd and public prefix from IPv6 tunnel settings for now
	$resRA=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292');
	$rowRA=$resRA->FetchRow();
	$ipv6_data=explode(",", $rowRA['IK_DeviceData']);
	$IPv6Prefix=$ipv6_data[5];
	$IPv6Netmask = $ipv6_data[6];
	$enableRA = ($ipv6_data[11]=='on')?1:0;
	
	// extract VPN settings from DeviceData
	$resVPN=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=295');
	$rowVPN=$resVPN->FetchRow();
	$VPN_data=explode(",", $rowVPN['IK_DeviceData']);
	$enableVPN = ($VPN_data[0]=='on')?1:0;
	$VPNRange=explode('-',$VPN_data[1]);
	$VPNRangeStart=$VPNRange[0];
	$VPNRangeEnd=explode('.',$VPNRange[1]);
	$VPNRangeEnd=$VPNRangeEnd[3];
	$VPNPSK = $VPN_data[2];
	
	// extract PPPoE settings from core
	$resPPPoE=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData='.$GLOBALS['PPPoeData']);
	$rowPPPoE=$resPPPoE->FetchRow();
	$PPPoEData=explode(",", $rowPPPoE['IK_DeviceData']);
	$PPPoEEnabled = ($PPPoEData[0]=='on')?1:0;
	$PPPoEUser=$PPPoEData[1];
	$PPPoEPass=$PPPoEData[2];
	$PPPoEIPv6Enabled = ($PPPoEData[3]=='on')?1:0;

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
			   
		// get internal NIC IPv4 settings
		$internalInterfaceArray=explode(',',$interfacesArray[1]);
		$internalMAC=getMAC($internalInterfaceArray[0]);
		$oldInternalCoreIP=$internalInterfaceArray[1];
		
		switch ($internalCoreIPv4Status) {
		    case 'disabled':
		    	$internalCoreIPv4IP='disabled';
		        break;

			case 'static';
		    default:
		    	$internalCoreIPv4Status='static';
				$internalCoreIPv4IP=$internalInterfaceArray[1];
				$internalCoreIPv4NM=$internalInterfaceArray[2];
			break;
		}
		$outsideIP = getOutsideIP();
		
		$externalInterface=$externalInterfaceArray[0];
		$internalInterface=$internalInterfaceArray[0];
		
		// extract NIC IPv6 settings from core
		$queryNC='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$resNC=$dbADO->Execute($queryNC,array($coreID,$GLOBALS['IPv6NetworkInterfaces']));
		$rowNC=$resNC->FetchRow();
		
		$IPv6interfacesArray=explode('|',$rowNC['IK_DeviceData']);

		// get external NIC IPv6 settings
		$externalIPv6InterfaceArray=explode(',',$IPv6interfacesArray[0]);
		$coreIPv6Status=$externalIPv6InterfaceArray[1];
		switch ($coreIPv6Status) {
			case 'disabled':
				break;
			case 'ra':
				$coreIPv6=getIPv6IP($externalIPv6InterfaceArray[0]);
				$coreIPv6GW=getIPv6GW();
				$coreIPv6NetMask=getIPv6MASK($externalIPv6InterfaceArray[0]);
				break;
			case 'dhcp':
				$coreIPv6=getIPv6IP($externalIPv6InterfaceArray[0]);
				$coreIPv6GW=getIPv6GW();
				$coreIPv6NetMask=getIPv6MASK($externalIPv6InterfaceArray[0]);
				break;
			case 'static':
			default:
				$coreIPv6Status='static';
				$coreIPv6=$externalIPv6InterfaceArray[1];
				$coreIPv6NetMask=$externalIPv6InterfaceArray[2];
				$coreIPv6GW=$externalIPv6InterfaceArray[3];
				$coreIPv6DNS1=$externalIPv6InterfaceArray[4];
				$coreIPv6DNS2=$externalIPv6InterfaceArray[5];
				break;
		}
		
		// get internal NIC IPv6 settings
		$internalIPv6InterfaceArray=explode(',',$IPv6interfacesArray[1]);
		$internalCoreIPv6Status=$internalIPv6InterfaceArray[1];
		switch ($internalCoreIPv6Status) {
			case 'disabled':
				break;
			case 'ra':
				$internalCoreIPv6=getIPv6IP($internalIPv6InterfaceArray[0]);
				$internalCoreIPv6NetMask=getIPv6MASK($internalIPv6InterfaceArray[0]);
				break;
			case 'static':
			default:
				$internalCoreIPv6Status='static';
				$internalCoreIPv6=$internalIPv6InterfaceArray[1];
				$internalCoreIPv6NetMask=$internalIPv6InterfaceArray[2];
				break;
		}

	}else{
		$fatalError='No record in Device_DeviceData for Network Interfaces.';
	}	

	$swaphtml = "";
  if (!empty($externalInterfaceArray[0]) && !empty($internalInterfaceArray[0]))

	{
		$swaphtml = '<input type="submit" class="button" name="swap" value="'.translate('TEXT_SWAP_INTERFACES_CONST').'"';
	}
	if ($action == 'form') {
		if(!isset($fatalError)){
			$out.='
	<script>
	function ipFromDHCP()
	{
		newVal=(!document.networkSettings.ipForAnonymousDevices.checked)?true:false;
		newColor=(document.networkSettings.ipForAnonymousDevices.checked)?"#000000":"#999999";

		eval("document.networkSettings.nonPlutoIPStart.disabled="+newVal+";");
		eval("document.networkSettings.nonPlutoIPEnd.disabled="+newVal+";");
		
		document.getElementById("nonPlutoIPText").style.color=newColor;
		document.getElementById("nonPlutoIPStart").style.color=newColor;
		document.getElementById("nonPlutoIPEnd").style.color=newColor;
		
	}

	function validateElement(elementName,errorMsg)
	{
		// check for valid ip address
	
		ipaddr = eval(\'document.networkSettings.\'+elementName+\'.value\');
		ipaddr = ipaddr.replace( /\s/g, "") //remove spaces for checking
		var re = /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/; //regex. check for digits and in all 4 quadrants of the IP
		
		if (re.test(ipaddr)) {
			//split into units with dots "."
			var parts = ipaddr.split(".");
			//if the first unit/quadrant of the IP is zero
			if (parseInt(parseFloat(parts[0])) == 0) {
				alert(errorMsg);
				return false;
			}
			//if any part is greater than 255
			for (var i=0; i<parts.length; i++) {
				if (parseInt(parseFloat(parts[i])) > 255){
					alert(errorMsg);
					return false;
				}
			}
			return true;
		} else {
			alert(errorMsg);
			return false;
		}
	}
			
	function validateForm()
	{
		var shouldSubmit=false;
		
		if(document.networkSettings.extv4[2].checked ){
			validIP=validateElement("coreIPv4IP","Please enter Core\'s IP address.");
			if(validIP)
				validNetMask=validateElement("coreIPv4NM","Please enter Subnet Mask address.");
			if(validNetMask)
				validGW=validateElement("coreIPv4GW","Please enter Gateway address.");
			if(validGW)
				validDNS1=validateElement("coreIPv4DNS1","Please enter first DNS address.");
			if(validDNS1)
				shouldSubmit=true;
		}else {
			shouldSubmit=true;
		}
		
		if(shouldSubmit==true) {
			document.getElementById("loading").style.display = "block";
			
			// Enable fields to make variables flow into POST request
			// disabled elements are not transmitted
			document.networkSettings.PPPoEUser.disabled=false;
			document.networkSettings.PPPoEPass.disabled=false;
			document.networkSettings.coreDHCPStart.disabled=false;
			document.networkSettings.coreDHCPEnd.disabled=false;
			document.networkSettings.nonPlutoIPStart.disabled=false;
			document.networkSettings.nonPlutoIPEnd.disabled=false;
			document.networkSettings.VPNRangeStart.disabled=false;
			document.networkSettings.VPNRangeEnd.disabled=false;
			document.networkSettings.VPNPSK.disabled=false;
			document.networkSettings.IPv6Prefix.disabled=false;
			document.networkSettings.IPv6Netmask.disabled=false;
		
			document.networkSettings.submit();
		}
	}
		
	function setIPRange()
	{
		newVal=(!document.networkSettings.enableDHCP.checked)?true:false;
		newColor=(document.networkSettings.enableDHCP.checked)?"#000000":"#999999";

		document.networkSettings.coreDHCPStart.disabled=newVal;
		document.networkSettings.coreDHCPEnd.disabled=newVal;
		
		document.networkSettings.ipForAnonymousDevices.disabled=newVal;
		if(!document.networkSettings.enableDHCP.checked)
			document.networkSettings.ipForAnonymousDevices.checked=false;
		
		document.getElementById("coreDHCPText").style.color=newColor;
		document.networkSettings.coreDHCPStart.style.color=newColor;
		document.networkSettings.coreDHCPEnd.style.color=newColor;
		
		ipFromDHCP();
	}
	
	function setVPNRange()
	{
		newVal=(!document.networkSettings.enableVPN.checked)?true:false;
		newColor=(document.networkSettings.enableVPN.checked)?"#000000":"#999999";

		document.networkSettings.VPNRangeStart.disabled=newVal;
		document.networkSettings.VPNRangeEnd.disabled=newVal;
		document.networkSettings.VPNPSK.disabled=newVal;
		
		document.getElementById("VPNEnabledText").style.color=newColor;
		document.getElementById("VPNRangeText").style.color=newColor;
		
		document.networkSettings.VPNRangeStart.style.color=newColor;
		document.networkSettings.VPNRangeEnd.style.color=newColor;
		document.networkSettings.VPNPSK.style.color=newColor;
	}	
	
	function setIPv6Range()
	{
		newVal=(!document.networkSettings.enableRA.checked)?true:false;
		newColor=(document.networkSettings.enableRA.checked)?"#000000":"#999999";

		document.networkSettings.IPv6Prefix.disabled=newVal;
		document.networkSettings.IPv6Netmask.disabled=newVal;

		document.getElementById("IPv6PrefixText").style.color=newColor;
		document.networkSettings.IPv6Prefix.style.color=newColor;
		document.networkSettings.IPv6Netmask.style.color=newColor;
	}
		
	function setPPPoE()
	{
		checked=document.getElementById("PPPoEEnabled").checked;
		newColor=(checked)?"#000000":"#999999";
		
		document.getElementById("PPPoEUserText").style.color=newColor;
		document.getElementById("PPPoEPassText").style.color=newColor;
		document.getElementById("PPPoEIPv6EnabledText").style.color=newColor;
		document.networkSettings.PPPoEUser.style.color=newColor;
		document.networkSettings.PPPoEPass.style.color=newColor;
		
		document.networkSettings.PPPoEUser.disabled=!checked;
		document.networkSettings.PPPoEPass.disabled=!checked;
		document.networkSettings.PPPoEIPv6Enabled.disabled=!checked;
	}
	
	function setStaticIP(newVal,ipVersion)
	{
		newVal=!newVal;
		newColor=(newVal==false)?"#000000":"#999999";

		eval("document.getElementById(\"coreIPv"+ipVersion+"IPtext\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"IP\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"IP\").disabled="+newVal+";");

		eval("document.getElementById(\"coreIPv"+ipVersion+"NMtext\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"NM\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"NM\").disabled="+newVal+";");

		eval("document.getElementById(\"coreIPv"+ipVersion+"GWtext\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"GW\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"GW\").disabled="+newVal+";");

		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS1text\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS1\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS1\").disabled="+newVal+";");

		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS2text\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS2\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"coreIPv"+ipVersion+"DNS2\").disabled="+newVal+";");
	}
	
	function setStaticIntIP(newVal,ipVersion)
	{
		newVal=!newVal;
		newColor=(newVal==false)?"#000000":"#999999";

		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"IPtext\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"IP\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"IP\").disabled="+newVal+";");

		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"NMtext\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"NM\").style.color=\""+newColor+"\";");
		eval("document.getElementById(\"internalCoreIPv"+ipVersion+"NM\").disabled="+newVal+";");
	}
	
	</script>
	<div id="loading">
		<div id="loading_picture">
			<img src="img/loader.gif" /> 
		</div>
		<div id="loading_text">
			'.translate('TEXT_PLEASE_WAIT_CONST').'
		</div>
	</div>

	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="networkSettings">
	<input type="hidden" name="section" value="networkSettings">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0">
		<tr>
			<td colspan="6" class="tablehead"><B>'.translate('TEXT_CORE_IDENTIFICATION_CONST').':</B></td>
		</tr>
		<tr>
			<!-- CORE name -->
			<td colspan="2"><B>'.translate('TEXT_COMPUTER_NAME_CONST').'</B></td>
			<td><input type="text" name="cname" value="'.$cname.'"></td>
			<td colspan="2"><B>'.translate('TEXT_DOMAIN_CONST').'</B></td>
			<td><input type="text" name="domain" value="'.$domain.'"></td>
		</tr>
		<tr><td colspan="6">&nbsp;</td></tr>
       	<tr>
       		<td colspan="6" class="tablehead"><B>'.translate('TEXT_CORE_SEEN_FROM_INTERNET_CONST').':</B></td>
       	</tr>
		<tr>
			<td colspan="2"><b>'.translate('TEXT_OUTSIDE_IP_CONST').'</b></td>
			<td><input type="text" name="outsideip" disabled value="'.$outsideIP.'"></td>
			<td colspan="2"><b>'.translate('TEXT_OUTSIDE_HOSTNAME_CONST').'</b></td>
			<td><input type="text" name="outsideip" disabled size=50 value="'.gethostbyaddr($outsideIP).'"></td>
		</tr>
		<tr><td colspan="6">&nbsp;</td></tr>
		<tr>
			<td colspan="6" class="tablehead"><B>'.translate('TEXT_DHCP_SERVERS_CONST').':</B></td>
		</tr>
		<tr>
			<!-- IPv4 DHCP range for LinuxMCE devices) -->
			<td colspan="3">
				<input type="checkbox" name="enableDHCP" value="1" '.(($enableDHCP==1)?'checked':'').' onclick="setIPRange();">
				<span id="coreDHCPText" style="color:'.(($enableDHCP!=1)?'#999999':'').'">'.translate('TEXT_IPV4_DHCP_ENABLED_CONST').'</span>
			</td>
			<td colspan="3">
				<input type="text" maxlength="15" id="coreDHCPStart" name="coreDHCPStart" size="16" style="text-align: right" value="'.@$coreDHCPStart.'" '.(($enableDHCP!=1)?'disabled':'').'> - 
			  	<input type="text" maxlength="3" id="coreDHCPEnd" name="coreDHCPEnd" size="4" value="'.@$coreDHCPEnd.'" '.(($enableDHCP!=1)?'disabled':'').'>
			</td>
		</tr>
		<tr>
			<!-- IPv4 DHCP range for non-LinuxMCE devices) -->		
			<td>&nbsp;</td>
			<td colspan="2">
				<input type="checkbox" name="ipForAnonymousDevices" value="1" '.((@$nonPlutoIP==1)?'checked':'').' onClick="ipFromDHCP()" '.(($enableDHCP==1)?'':'disabled').'>
				<span id="nonPlutoIPText" style="color:'.(($nonPlutoIP!=1)?'#999999':'').'"> '.translate('TEXT_IPV4_DHCP_NONLMCE_ENABLED_CONST').'</span>
			</td>
			<td colspan="3">
				<input type="text" maxlength="15" name="nonPlutoIPStart" size="16" style="text-align: right" value="'.@$nonPlutoIPStart.'" '.(($enableDHCP==1)?'':'disabled').'> -
				<input type="text" maxlength="3" name="nonPlutoIPEnd" size="4" value="'.@$nonPlutoIPEnd.'" '.(($enableDHCP==1)?'':'disabled').'>
			</td>
		</tr>
		<tr><td colspan="6"><hr></td></tr>
		
		<tr>
			<!-- IPv6 RA daemon settings -->
			<td colspan="3">
				<input type="checkbox" name="enableRA" '.(($enableRA==1)?'checked':'').' onclick="setIPv6Range();">
				<span id="IPv6PrefixText" style="color:'.(($enableRA==0)?'#999999':'').'">'.translate('TEXT_IPV6_RA_ENABLED_CONST').':</span>
			</td>
			<td colspan="3">
				<input type="text" maxlength="38" style="color:'.(($enableRA==0)?'#999999':'').'" name="IPv6Prefix" size="39" value="'.@$IPv6Prefix.'" '.(($enableRA==1)?'':'disabled').'> / 
				<input type="text" maxlength="2" style="color:'.(($enableRA==0)?'#999999':'').'" name="IPv6Netmask" size="2" value="'.@$IPv6Netmask.'" '.(($enableRA==1)?'':'disabled').'>
			</td>
		</tr>
		<tr><td colspan="6"><hr></td></tr>
		<tr>
			<!-- VPN SETTINGS -->
			<td colspan="3">
				<input type="checkbox" name="enableVPN" '.($enableVPN?'checked':'#000000').' onclick="setVPNRange();">
				<span id="VPNEnabledText" style="color:'.(!$enableVPN?'#999999':'#000000').'">'.translate('TEXT_VPN_ENABLED_CONST').':</span>
			</td>
			<td colspan="3"><input type="password" maxlength="25" style="color:'.(!$enableVPN?'#999999':'#000000').'" name="VPNPSK" size="26" value="'.@$VPNPSK.'" '.(($enableVPN!=1)?'disabled':'').'></td>		
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2">
				<span id="VPNRangeText" style="color:'.(!$enableVPN?'#999999':'#000000').'">'.translate('TEXT_VPN_CLIENT_RANGE_CONST').':</span>
			</td>
			<td colspan="3">
				<input type="text" maxlength="15" style="text-align: right; color:'.(!$enableVPN?'#999999':'#000000').'" name="VPNRangeStart" size="16" value="'.@$VPNRangeStart.'" '.(($enableVPN!=1)?'disabled':'').'> - 
				<input type="text" maxlength="3" style="color:'.(!$enableVPN?'#999999':'#000000').'" name="VPNRangeEnd" size="4" value="'.@$VPNRangeEnd.'" '.(($enableVPN!=1)?'disabled':'').'>
			</td>
		</tr>
		
		<tr><td colspan="6">&nbsp;</td></tr>
		<tr>
			<td colspan="6" class="tablehead"><B>'.translate('TEXT_NUMBER_OF_NIC_CONST').': '.$number_of_cards.'</B></td>
		</tr>
		<tr>
			<td colspan="6"><B>1. '.translate('TEXT_EXTERNAL_NIC_CONST').' '.@$externalInterfaceArray[0].'</B> (MAC: '
			.@$externalMAC.')</td>
		</tr>
		<tr style="background-color: #CCFFCC;" >
		<!-- PPPoE xDSL configuration -->
			<td colspan="2"><input type="checkbox" onclick="setPPPoE();" name="PPPoEEnabled" id="PPPoEEnabled" value="1" '.(($PPPoEEnabled)?'checked':'').'> '.translate('TEXT_PPPOE_ENABLED_CONST').':</td>
			<td colspan="4">
				<B><span id="PPPoEUserText" style="color:'.(!$PPPoEEnabled?'#999999':'').'">'.translate('TEXT_PPPOE_USER_CONST').'</span></B> &nbsp; <input type="text" id="PPPoEUser" name="PPPoEUser" style="color:'.(!$PPPoEEnabled?'#999999':'').'" value="'.$PPPoEUser.'" '.(!$PPPoEEnabled?'disabled':'').'> &nbsp; 
				<B><span id="PPPoEPassText" style="color:'.(!$PPPoEEnabled?'#999999':'').'">'.translate('TEXT_PPPOE_PASS_CONST').'</span></B> &nbsp; <input type="password" id="PPPoEPass" name="PPPoEPass" style="color:'.(!$PPPoEEnabled?'#999999':'').'" value="'.$PPPoEPass.'" '.(!$PPPoEEnabled?'disabled':'').'>
				<input type="checkbox" name="PPPoEIPv6Enabled" id="PPPoEIPv6Enabled" value="1" '.(($PPPoEIPv6Enabled==1)?'checked':'').' '.((!$PPPoEEnabled)?'disabled':'').'> <span id="PPPoEIPv6EnabledText" style="color:'.(!$PPPoEEnabled?'#999999':'').'">'.translate('TEXT_PPPOE_IPV6_CONST').'</span>
			</td>		
		</tr>
		<tr>
			<!-- IPv4 NIC mode) -->
			<td colspan="3"><B>IPv4:</B>&nbsp;
				<input type="radio" name="extv4" disabled value="disabled" onclick="setStaticIP(false,4);"'.(($coreIPv4Status=='disabled')?'checked':'').'> '.translate('TEXT_INTERFACE_DISABLED').''.'
				&nbsp;&nbsp;<input type="radio" name="extv4" value="dhcp" onclick="setStaticIP(false,4);"'.(($coreIPv4Status=='dhcp')?'checked':'').'> '.translate('TEXT_INTERFACE_DHCP').''.'
				&nbsp;&nbsp;<input type="radio" name="extv4" value="static" onclick="setStaticIP(true,4);"'.(($coreIPv4Status=='static')?'checked':'').'> '.translate('TEXT_INTERFACE_STATIC').'
			</td>
			<!-- IPv6 NIC mode) -->
			<td colspan="3"><B>IPv6:</B>&nbsp;
				<input type="radio" name="extv6" value="disabled" onclick="setStaticIP(false,6);"'.(($coreIPv6Status=='disabled')?'checked':'').'> '.translate('TEXT_INTERFACE_DISABLED').''.'
				&nbsp;&nbsp;<input type="radio" name="extv6" value="ra" onclick="setStaticIP(false,6);"'.(($coreIPv6Status=='ra')?'checked':'').'> '.translate('TEXT_INTERFACE_RA').''.'
				&nbsp;&nbsp;<input type="radio" name="extv6" value="dhcp" onclick="setStaticIP(false,6);"'.(($coreIPv6Status=='dhcp')?'checked':'').'> '.translate('TEXT_INTERFACE_DHCP').''.'
				&nbsp;&nbsp;<input type="radio" name="extv6" value="static" onclick="setStaticIP(true,6);"'.(($coreIPv6Status=='static')?'checked':'').'> '.translate('TEXT_INTERFACE_STATIC').'
			</td>
		</tr>
		<tr>
			<!-- IPv4 address -->
			<td>&nbsp;</td>
			<td width="150"><span id="coreIPv4IPtext" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_IP_ADDRESS_CONST').':</span></td>
			<td><input type="text" maxlength="15" id="coreIPv4IP" name="coreIPv4IP" size="20" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'" value="'.@$coreIPv4.'" '.(($coreIPv4Status!='static')?'disabled':'').'></td>			
			<!-- IPv6 address -->
			<td>&nbsp;</td>
			<td width="150"><span id="coreIPv6IPtext" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_IP_ADDRESS_CONST').':</span></td>
			<td><input type="text" maxlength="39" id="coreIPv6IP" name="coreIPv6IP" size="50" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'" value="'.@$coreIPv6.'" '.(($coreIPv6Status!='static')?'disabled':'').'></td>	
		</tr>						
		<tr>
			<!-- IPv4 netmask -->
			<td>&nbsp;</td>
			<td><span id="coreIPv4NMtext" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_NETMASK_CONST').':</span></td>
			<td><input type="text" maxlength="15" id="coreIPv4NM" name="coreIPv4NM" size="20" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'" value="'.@$coreIPv4NetMask.'" '.(($coreIPv4Status!='static')?'disabled':'').'></td>
			<!-- IPv6 prefix length -->
			<td>&nbsp;</td>
			<td><span id="coreIPv6NMtext" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_PREFIX_LENGTH_CONST').':</span></td>
			<td><input type="text" maxlength="2" id="coreIPv6NM" name="coreIPv6NM" size="5" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'" value="'.@$coreIPv6NetMask.'" '.(($coreIPv6Status!='static')?'disabled':'').'></td>
		</tr>
		<tr>
			<!-- IPv4 gateway -->
			<td>&nbsp;</td>
			<td><span id="coreIPv4GWtext" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_GATEWAY_CONST').':</span></td>
			<td><input type="text" maxlength="15" id="coreIPv4GW" name="coreIPv4GW" size="20" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'" value="'.@$coreIPv4GW.'" '.(($coreIPv4Status!='static')?'disabled':'').'></td>		
			<!-- IPv6 gateway -->
			<td>&nbsp;</td>
			<td><span id="coreIPv6GWtext" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_GATEWAY_CONST').':</span></td>
			<td><input type="text" maxlength="39" id="coreIPv6GW" name="coreIPv6GW" size="50" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'" value="'.@$coreIPv6GW.'" '.(($coreIPv6Status!='static')?'disabled':'').'></td>
		</tr>		
		<tr>
			<!-- IPv4 DNS1 -->
			<td>&nbsp;</td>
			<td><span id="coreIPv4DNS1text" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_NAMESERVER_CONST').' #1:</span></td>
			<td><input type="text" maxlength="15" id="coreIPv4DNS1" name="coreIPv4DNS1" size="20" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'" value="'.@$coreIPv4DNS1.'" '.(($coreIPv4Status!='static')?'disabled':'').'></td>
			<!-- IPv6 DNS1 -->
			<td>&nbsp;</td>
			<td><span id="coreIPv6DNS1text" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_NAMESERVER_CONST').' #1:</span></td>
			<td><input type="text" maxlength="39" id="coreIPv6DNS1" name="coreIPv6DNS1" size="50" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'" value="'.@$coreIPv6DNS1.'" '.(($coreIPv6Status!='static')?'disabled':'').'></td>
		</tr>		
		<tr>
			<!-- IPv4 DNS2 -->
			<td>&nbsp;</td>
			<td><span id="coreIPv4DNS2text" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_NAMESERVER_CONST').' #2:</span></td>
			<td><input type="text" maxlength="15" id="coreIPv4DNS2" name="coreIPv4DNS2" size="20" style="color:'.(($coreIPv4Status!='static')?'#999999':'').'" value="'.@$coreIPv4DNS2.'" '.(($coreIPv4Status!='static')?'disabled':'').'></td>	
			<!-- IPv6 DNS2 -->
			<td>&nbsp;</td>
			<td><span id="coreIPv6DNS2text" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_NAMESERVER_CONST').' #2:</span></td>
			<td><input type="text" maxlength="39" id="coreIPv6DNS2" name="coreIPv6DNS2" size="50" style="color:'.(($coreIPv6Status!='static')?'#999999':'').'" value="'.@$coreIPv6DNS2.'" '.(($coreIPv6Status!='static')?'disabled':'').'></td>
		</tr>		
		<tr><td colspan="6"><hr></td></tr>
		<tr>
			<td colspan="6"><B>2. '.translate('TEXT_INTERNAL_NIC_CONST').' '.@$internalInterfaceArray[0].'</B> (MAC: '.@$internalMAC.')</td>
		<tr>
			<!-- Internal IPv4 NIC mode) -->
			<td colspan="3"><B>IPv4:</B>&nbsp;
				<input type="radio" name="intv4" disabled value="disabled" onclick="setStaticIntIP(false,4);"'.(($internalCoreIPv4Status=='disabled')?'checked':'').'> '.translate('TEXT_INTERFACE_DISABLED').''.'
				&nbsp;&nbsp;<input type="radio" name="intv4" value="static" onclick="setStaticIntIP(true,4);"'.(($internalCoreIPv4Status=='static')?'checked':'').'> '.translate('TEXT_INTERFACE_STATIC').'
			</td>
			<!-- Internal IPv6 NIC mode) -->
			<td colspan="3"><B>IPv6:</B>&nbsp;
				<input type="radio" name="intv6" value="disabled" onclick="setStaticIntIP(false,6);"'.(($internalCoreIPv6Status=='disabled')?'checked':'').'> '.translate('TEXT_INTERFACE_DISABLED').''.'
				&nbsp;&nbsp;<input type="radio" name="intv6" value="ra" onclick="setStaticIntIP(false,6);"'.(($internalCoreIPv6Status=='ra')?'checked':'').'> '.translate('TEXT_INTERFACE_RA').''.'
				&nbsp;&nbsp;<input type="radio" name="intv6" value="static" onclick="setStaticIntIP(true,6);"'.(($internalCoreIPv6Status=='static')?'checked':'').'> '.translate('TEXT_INTERFACE_STATIC').'
			</td>
		</tr>

		<tr>
			<!-- Internal IPv4 address -->
			<td>&nbsp;</td>
			<td width="150"><span id="internalCoreIPv4IPtext" style="color:'.(($internalCoreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_IP_ADDRESS_CONST').':</span></td>
			<td><input type="text" maxlength="15" name="internalCoreIPv4IP" size="20" value="'.@$internalCoreIPv4IP.'"></td>
			<!-- Internal IPv6 address -->
			<td>&nbsp;</td>
			<td width="150"><span id="internalCoreIPv6IPtext" style="color:'.(($internalCoreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_IP_ADDRESS_CONST').':</span></td>
			<td><input type="text" maxlength="39" id="internalCoreIPv6IP" name="internalCoreIPv6IP" size="50" style="color:'.(($internalCoreIPv6Status!='static')?'#999999':'').'" value="'.@$internalCoreIPv6.'" '.(($internalCoreIPv6Status!='static')?'disabled':'').'></td>	

		</tr>						
		<tr>
			<!-- Internal IPv4 netmask -->
			<td>&nbsp;</td>
			<td><span id="internalCoreIPv4NMtext" style="color:'.(($internalCoreIPv4Status!='static')?'#999999':'').'">'.translate('TEXT_NETMASK_CONST').':</span></td>
			<td><input type="text" maxlength="15" name="internalCoreIPv4NM" size="20" value="'.@$internalCoreIPv4NM.'"></td>
			<!-- Internal IPv6 netmask -->
			<td>&nbsp;</td>
			<td><span id="internalCoreIPv6NMtext" style="color:'.(($internalCoreIPv6Status!='static')?'#999999':'').'">'.translate('TEXT_NETMASK_CONST').':</span></td>
			<td><input type="text" maxlength="2" id="internalCoreIPv6NM" name="internalCoreIPv6NM" size="5" style="color:'.(($internalCoreIPv6Status!='static')?'#999999':'').'" value="'.@$internalCoreIPv6NetMask.'" '.(($internalCoreIPv6Status!='static')?'disabled':'').'></td>
		</tr>
		
		<tr><td colspan="6"><hr></td></tr>
		<tr>
			<td colspan="3">
				<input type="checkbox" name="OfflineMode" value="1" '.(($OfflineMode=='true')?'checked':'').'>
				<B>'.translate('TEXT_OFFLINEMODE_CONST').'&nbsp;'.$swaphtml.'</B>	
			</td>
			<td colspan="3" bgcolor="#EEEEEE">
				<input type="button" class="button" name="update" value="'.translate('TEXT_UPDATE_CONST').'" onClick="validateForm()"> <input type="reset" class="button" name="reset" value="'.translate('TEXT_RESET_CONST').'">
			</td>
		</tr>			
	<tr><td colspan="6">'.translate('TEXT_OPEN_FIREWALL_CONST').'</td></tr>	'
	.'</table>
	</form>
		<script>
		 	var frmvalidator = new formValidator("networkSettings");
 			frmvalidator.addValidation("internalCoreIP_1","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreIP_2","req","Please enter an IP address.");			
			frmvalidator.addValidation("internalCoreIP_3","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreIP_4","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreNetMask_1","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_2","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_3","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_4","req","Please enter the subnet mask.");
		</script>
	';
		}else{
			$out.='<h2 style="color:red;">'.$fatalError.'</h2>';
		}
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=networkSettings&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		$newEnableDHCP=(isset($_POST['enableDHCP']))?1:0;
		
		// new ip range
		$coreDHCPStart=$_POST['coreDHCPStart'];
		$coreDHCPEnd=explode('.',$_POST['coreDHCPStart']);
		$coreDHCPEnd[3]=$_POST['coreDHCPEnd'];
		$coreDHCPEnd=join('.',$coreDHCPEnd);
		$coreDHCP=$coreDHCPStart.'-'.$coreDHCPEnd;
		// new internal core ip
		$internalCoreIP=$_POST['internalCoreIPv4IP'];
		$isChanged=0;
		$ipForAnonymousDevices=isset($_POST['ipForAnonymousDevices'])?1:0;
		if($ipForAnonymousDevices==1){
			$nonPlutoIPStart=$_POST['nonPlutoIPStart'];
			$nonPlutoIPEnd=explode('.',$_POST['nonPlutoIPStart']);
			$nonPlutoIPEnd[3]=$_POST['nonPlutoIPEnd'];
			$nonPlutoIPEnd=join('.',$nonPlutoIPEnd);
			$nonPlutoIP=$nonPlutoIPStart.'-'.$nonPlutoIPEnd;			
			$coreDHCP.=','.$nonPlutoIP;
		}

		if($newEnableDHCP==1){
			if($coreDHCP!=$oldCoreDHCP){
				$SQL='REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_Devicedata) VALUES(?,?,?)';
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['DHCPDeviceData'],$coreDHCP));
			}
		}
		else{
			$SQL='REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_Devicedata) VALUES(?,?,?)';
			$dbADO->Execute($SQL,array($coreID,$GLOBALS['DHCPDeviceData'],''));
		}
		$needReboot=0;
		$willReboot=0;

		if(isset($_POST['swap'])){
			$externalInterface=$internalInterfaceArray[0];
			$internalInterface=$externalInterfaceArray[0];
			$needReboot=1;
		}
				
		if($resNC->RecordCount()>0){
			// Configure IPv4 external interface
			$networkInterfaces=$externalInterface;
			if($_POST['extv4']=='disabled'){
				$networkInterfaces.=',disabled|';
			}
			else if($_POST['extv4']=='dhcp'){
				$networkInterfaces.=',dhcp|';
			}
			else{
				$dns1 = $_POST['coreIPv4DNS1'];
				$dns2 = $_POST['coreIPv4DNS2'];
				$dns_string = $dns1 . ($dns2 === "" ? "" : ",$dns2");
				$networkInterfaces.=','.$_POST['coreIPv4IP'].','.$_POST['coreIPv4NM'].','.$_POST['coreIPv4GW'].','.$dns_string.'|';
			}
			// Configure IPv4 internal interface
			if ($internalInterface !== ""){
				$networkInterfaces.=$internalInterface.','.$internalCoreIP.','.$_POST['internalCoreIPv4NM'];
			}
			if($networkInterfaces!=$rowNC['IK_DeviceData']){
				$SQL="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(?,?,?)";
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['NetworkInterfaces'],$networkInterfaces));
			}

			// Configure IPv6 external interface
			$networkInterfaces=$externalInterface;
			switch ($_POST['extv6']) {
				case 'static';
					$dns1 = $_POST['coreIPv6DNS1'];
					$dns2 = $_POST['coreIPv6DNS2'];
					$dns_string = $dns1 . ($dns2 === "" ? "" : ",$dns2");
					$networkInterfaces.=','.$_POST['coreIPv6IP'].','.$_POST['coreIPv6NM'].','.$_POST['coreIPv6GW'].','.$dns_string.'|';
				break;
				
				case 'dhcp':
				case 'ra':
					$networkInterfaces.=','.$_POST['extv6'].'|';
				break;

				default:
				case 'disabled':
					$networkInterfaces.=',disabled|';
				break;
			}

			// Configure IPv6 internal interface
			if ($internalInterface !== ""){
				switch ($_POST['intv6']) {
					case 'static';
						$networkInterfaces.=$internalInterface.','.$_POST['internalCoreIPv6IP'].','.$_POST['internalCoreIPv6NM'];
					break;
					
					case 'ra';
						$networkInterfaces.=$internalInterface.',ra';
					break;
					
					default:
					case 'disabled':
						$networkInterfaces.=$internalInterface.',disabled';
					break;
				}
			}
			if($networkInterfaces!=$rowNC['IK_DeviceData']){
				$SQL="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(?,?,?)";
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['IPv6NetworkInterfaces'],$networkInterfaces));
			}
		}

		// tokenize xDSL PPPoE values to put in CORE device data
		$resPPPoE=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData='.$GLOBALS['PPPoeData']);
		$rowPPPoE=$resRA->FetchRow();
		$PPPoEData[0]=($_POST['PPPoEEnabled']?'on':'off');
		$PPPoEData[1]= $_POST['PPPoEUser'];
		$PPPoEData[2]= $_POST['PPPoEPass'];
		$PPPoEData[3]=($_POST['PPPoEIPv6Enabled']?'on':'off');
		$token = join(',',$PPPoEData);
	 	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData='".$token."' WHERE FK_Device=1 AND FK_DeviceData=".$GLOBALS['PPPoeData']) 
	 		or die('ERROR: Invalid query: '.mysql_error());

		// tokenize IPv6 values to put in CORE device data
		$resRA=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292');
		$rowRA=$resRA->FetchRow();
		$ipv6_data=explode(",", $rowRA['IK_DeviceData']);
		$ipv6_data[5]= $_POST['IPv6Prefix']; 
		$ipv6_data[6]= $_POST['IPv6Netmask'];
		$ipv6_data[11]= $_POST['enableRA'];
		$token = join(',',$ipv6_data);
	 	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData='".$token."' WHERE FK_Device=1 AND FK_DeviceData=292") 
	 		or die('ERROR: Invalid query: '.mysql_error());

		// tokenize VPN settings to put in CORE device data
		$resVPN=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=295');
		$rowVPN=$resVPN->FetchRow();
		$VPN_data=explode(",", $rowVPN['IK_DeviceData']);
		$VPN_data[0]= $_POST['enableVPN'];
		$VPNRangeStart=$_POST['VPNRangeStart'];
		$VPNRangeEnd=explode('.',$_POST['VPNRangeStart']);
		$VPNRangeEnd[3]=$_POST['VPNRangeEnd'];
		$VPNRangeEnd=join('.',$VPNRangeEnd);
		$VPNRange=$VPNRangeStart.'-'.$VPNRangeEnd;		
		$VPN_data[1]= $VPNRange;
		$VPN_data[2]= $_POST['VPNPSK'];
		$token = join(',',$VPN_data);
	 	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData='".$token."' WHERE FK_Device=1 AND FK_DeviceData=295") 
	 		or die('ERROR: Invalid query: '.mysql_error());

		// NOTE: Please don't reboot the computer before these commands have completed, OK?
		// NOTE: Well, unless you like breaking /etc/network/interfaces randomly because the mysql server shutdown was faster than the scripts
		$commands = array('Network_Setup.sh', 'Network_Firewall.sh');
		for ($i = 0; $i < count($commands); $i++)
		{
			$cmd = "sudo -u root /usr/pluto/bin/{$commands[$i]}";
			exec_batch_command($cmd);
		}
		
		$OfflineMode=(int)@$_REQUEST['OfflineMode'];
		$OfflineMode=($OfflineMode==1)?'true':'false';
		exec_batch_command('sudo -u root /usr/pluto/bin/OfflineMode.sh set '.$OfflineMode);
		
		// set domain & computer name
		$domain=cleanString($_POST['domain']);
		if($domain!=''){
			set_device_data($coreID,$GLOBALS['ComputerDomain'],$domain,$dbADO);
		}
		$cname=cleanString($_POST['cname']);
		if($cname!=''){
			set_device_data($coreID,$GLOBALS['ComputerName'],$cname,$dbADO);
		}

		// NOTE: Serial stuff completed. Now it's OK to reboot.
		if($coreDHCP !=$oldCoreDHCP){
			writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tIP range changed from $oldCoreDHCP to $coreDHCP\n",'a+');
		}
		
		if($internalCoreIP!=$oldInternalCoreIP){
			writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tIP changed from $oldInternalCoreIP to $internalCoreIP\n",'a+');
			// v-- THIS THING REBOOTS THE COMPUTER WITHOUT ASKING!!!
			exec_batch_command('sudo -u root /usr/pluto/bin/InternalIPChange.sh');
			// ^-- THIS THING REBOOTS THE COMPUTER WITHOUT ASKING!!!
			$willReboot=1;
		}
		
		// TODO: call a script who will set the domain and computer name
		
		if ($needReboot)
			$willReboot=1;

		if ($willReboot)
			$ipchanged_msg=' The system will reboot in the following moments.';
		$msg=urlencode("Network settings updated.".@$ipchanged_msg);
		
		header("Location: index.php?section=networkSettings&msg=".$msg);
		if ($needReboot)
			exec_batch_command("sudo -u root /sbin/reboot");
	}

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_NETWORK_SETTINGS_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_NETWORK_SETTINGS_CONST')=>'index.php?section=networkSettings'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Network Settings');
	$output->output();
}

function getIpFromParts($partName,$startIndex=1)
{
	$ipArray=array();
	for($i=$startIndex;$i<($startIndex+4);$i++)
	{
		$part = @$_POST[$partName.$i];
		if ($part === "")
			return "";
		$ipArray[] = $part;
	}
	return join('.',$ipArray);
}

function getIPv6FromParts($partName,$startIndex=1)
{
	$ipArray=array();
	for($i=$startIndex;$i<($startIndex+8);$i++)
	{
		$part = @$_POST[$partName.$i];
		if ($part != "") $ipArray[] = $part;
	}
	$tmp=join(':',$ipArray);
	if(count($ipArray) < 8) $tmp.='::'; 
	return $tmp;
}

?>

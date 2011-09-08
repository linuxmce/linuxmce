<?
function getMAC($int){
	$mac = exec("ip link show $int | awk '/ether/ {print $2}'");
 	return $mac;
}

function getIP($int) {
	$ip = exec("ip addr show $int| awk '/inet / {print $2}'");
	$pieces = explode("/", $ip);
	return $pieces[0];
}

function getGW() {
	$gw = exec("ip -f inet route show|awk '/default/ {print($3)}'");
	return $gw;
}

function getMASK($int) {
 	$ip = exec("ip addr show $int| awk '/inet / {print $2}'");	
    $pieces = explode("/", $ip);
	$netmask = str_split(str_pad(str_pad('', $pieces[1], '1'), 32, '0'), 8);    
	foreach ($netmask as &$element) $element = bindec($element);    
	return join('.', $netmask);
}

function getOutSideIP() {
	$ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://automation.whatismyip.com/n09230945.asp');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $internetip = curl_exec($ch);
	return $internetip;
}

function getDNS() {
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
		$coreDHCPArray=explode('.',str_replace('-','.',$ipArray[0]));
		$nonPlutoIPArray=explode('.',str_replace('-','.',$ipArray[1]));
	}else{
		$coreDHCPArray=explode('.',str_replace('-','.',$oldCoreDHCP));
	}

	// extract radvd and public prefix from IPv6 tunnel settings for now
	$resRA=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292');
	$rowRA=$resRA->FetchRow();
	$ipv6_data=explode(",", $rowRA['IK_DeviceData']);
	$IPv6PrefixArray=explode(':',$ipv6_data[5]);
	$IPv6Netmask = $ipv6_data[6];
	$enableRA = ($ipv6_data[11]=='on')?1:0;
	
	// extract VPN settings from DeviceData
	$resVPN=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=295');
	$rowVPN=$resVPN->FetchRow();
	$VPN_data=explode(",", $rowVPN['IK_DeviceData']);
	$enableVPN = ($VPN_data[0]=='on')?1:0;
	$VPNRange=explode('.',str_replace('-','.',$VPN_data[1]));
	$VPNPSK = $VPN_data[2];
	
	$queryNC='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
	$resNC=$dbADO->Execute($queryNC,array($coreID,$GLOBALS['NetworkInterfaces']));
	if($resNC->RecordCount()>0){
		$rowNC=$resNC->FetchRow();
		$interfacesArray=explode('|',$rowNC['IK_DeviceData']);
		$externalInterfaceArray=explode(',',$interfacesArray[0]);
		$externalMAC=getMAC($externalInterfaceArray[0]);
		if($externalInterfaceArray[1]!='dhcp'){
			$coreIPArray=explode('.',$externalInterfaceArray[1]);
			$coreNetMaskArray=explode('.',$externalInterfaceArray[2]);
			$coreGWArray=explode('.',$externalInterfaceArray[3]);
			$coreDNS1Array=explode('.',$externalInterfaceArray[4]);
			$coreDNS2Array=explode('.',@$externalInterfaceArray[5]);
			$ipFromDHCP=0;
		}else{
			$ipFromDHCP=1;
            $externalIP=getIP($externalInterfaceArray[0]);
            $externalMASK=getMASK($externalInterfaceArray[0]);
			$defaultGW=getGW();
			$DNS=getDNS();
		}
		$internalInterfaceArray=explode(',',$interfacesArray[1]);
		$internalMAC=getMAC($internalInterfaceArray[0]);
		// old internal core ip
		$oldInternalCoreIP=$internalInterfaceArray[1];

		$internalCoreIPArray=explode('.',$internalInterfaceArray[1]);
		$internalCoreNetMaskArray=explode('.',$internalInterfaceArray[2]);
		$outsideIP = getOutsideIP();
	}else{
		$fatalError='No record in Device_DeviceData for Network Interfaces.';
	}	

	$swaphtml = "";
  if (!empty($externalInterfaceArray[0]) && !empty($internalInterfaceArray[0]))

	{
		$swaphtml = '<br><input type="submit" class="button" name="swap" value="'.translate('TEXT_SWAP_INTERFACES_CONST').'"><br>';
	}
	if ($action == 'form') {
		if(!isset($fatalError)){
			$out.='
	<script>
	function ipFromDHCP()
	{
		newVal=(!document.networkSettings.ipForAnonymousDevices.checked)?true:false;
		newColor=(document.networkSettings.ipForAnonymousDevices.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++){
			eval("document.networkSettings.nonPlutoIP_"+i+".disabled="+newVal+";");
		}
		document.getElementById("nonPluto").style.color=newColor;
		
	}

	function validateElement(elementNameArray,errorMsg)
	{
		for(i=0;i<elementNameArray.length;i++){
			tmp=eval(\'document.networkSettings.\'+elementNameArray[i]+\'.value==""\');
			if(tmp){
				eval(\'document.networkSettings.\'+elementNameArray[i]+\'.focus()\');
				alert(errorMsg);
				return false;
			}
		}
		return true;
	}
			
	function validateForm()
	{
		if(document.networkSettings.ipFrom[1].checked ){
			validIP=validateElement(new Array("coreIP_1","coreIP_2","coreIP_3","coreIP_4"),"Please enter Core\'s IP address.");
			if(validIP)
				validNetMask=validateElement(new Array("coreNetMask_1","coreNetMask_2","coreNetMask_3","coreNetMask_4"),"Please enter Subnet Mask address.");
			if(validNetMask)
				validGW=validateElement(new Array("coreGW_1","coreGW_2","coreGW_3","coreGW_4"),"Please enter Gateway address.");
			if(validGW)
				validDNS1=validateElement(new Array("coreDNS1_1","coreDNS1_2","coreDNS1_3","coreDNS1_4"),"Please enter first DNS address.");
			if(validDNS1)
				document.networkSettings.submit();
		}else
			document.networkSettings.submit();
	}
		
	function setIPRange()
	{
		newVal=(!document.networkSettings.enableDHCP.checked)?true:false;
		newColor=(document.networkSettings.enableDHCP.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++){
			eval("document.networkSettings.coreDHCP_"+i+".disabled="+newVal+";");
		}
		document.networkSettings.ipForAnonymousDevices.disabled=newVal;
		if(!document.networkSettings.enableDHCP.checked)
			document.networkSettings.ipForAnonymousDevices.checked=false;
		document.getElementById("range").style.color=newColor;
		document.getElementById("provide").style.color=newColor;
		ipFromDHCP();
	}
	
	function setVPNRange()
	{
		newVal=(!document.networkSettings.enableVPN.checked)?true:false;
		newColor=(document.networkSettings.enableVPN.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++) eval("document.networkSettings.VPNRange_"+i+".disabled="+newVal+";");

		eval("document.networkSettings.VPNPSK.disabled="+newVal+";");
		document.getElementById("VPNrange").style.color=newColor;
		document.getElementById("VPNPSKlabel").style.color=newColor;
	}	
	
	function setIPv6Range()
	{
		newVal=(!document.networkSettings.enableRA.checked)?true:false;
		newColor=(document.networkSettings.enableRA.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++){
			eval("document.networkSettings.IPv6Prefix_"+i+".disabled="+newVal+";");
		}
		eval("document.networkSettings.IPv6Netmask.disabled="+newVal+";");
		document.getElementById("IPv6Prefix").style.color=newColor;
	}
		
	function setStaticIP(newVal)
	{
		for(i=1;i<5;i++){
			eval("document.networkSettings.coreIP_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreNetMask_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreGW_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreDNS1_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreDNS2_"+i+".disabled="+newVal+";");
		}
		newColor=(newVal==false)?"#4E6CA6":"#CCCCCC";
		document.getElementById("coreIPtext").style.color=newColor;
		document.getElementById("coreNMtext").style.color=newColor;
		document.getElementById("coreGWtext").style.color=newColor;
		document.getElementById("coreDNS1text").style.color=newColor;
		document.getElementById("coreDNS2text").style.color=newColor;
	}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="networkSettings">
	<input type="hidden" name="section" value="networkSettings">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0">
		<tr>
			<td colspan="3" class="tablehead"><B>'.translate('TEXT_CORE_IDENTIFICATION_CONST').':</B></td>
		</tr>
		<tr>
			<td colspan="4"><B>'.translate('TEXT_COMPUTER_NAME_CONST').'</B> &nbsp; <input type="text" name="cname" value="'.$cname.'"> &nbsp; <B>'.translate('TEXT_DOMAIN_CONST').'</B> &nbsp; <input type="text" name="domain" value="'.$domain.'"></td>
		</tr>
		<tr><td>&nbsp;</td></tr>
        	<tr>
                        <td colspan="3" class="tablehead"><B>'.translate('TEXT_CORE_SEEN_FROM_INTERNET_CONST').':</B></td>
                </tr>
		<tr><td colspan="3"><b>'.translate('TEXT_OUTSIDE_IP_CONST').'</b>&nbsp<input type="text" name="outsideip" disabled value="'.$outsideIP.'">&nbsp;&nbsp;<b>'.translate('TEXT_OUTSIDE_HOSTNAME_CONST').'</b>&nbsp;<input type="text" name="outsideip" disabled size=50 value="'.gethostbyaddr($outsideIP).'"></td></tr>
		<tr><td>&nbsp;</td></tr>
		<tr>
			<td colspan="3" class="tablehead"><B>'.translate('TEXT_DHCP_SERVERS_CONST').':</B></td>
		</tr>
		<tr>
			<td colspan="3"><input type="checkbox" name="enableDHCP" value="1" '
				.(($enableDHCP==1)?'checked':'').' onclick="setIPRange();"> '.translate('TEXT_IPV4_DHCP_ENABLED_CONST').'</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><span id="range" style="color:'.(($enableDHCP!=1)?'#CCCCCC':'').'">'.translate('TEXT_IPV4_DHCP_LMCE_RANGE_CONST').': <input type="text" maxlength="3" name="coreDHCP_1" size="3" value="'.@$coreDHCPArray[0].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_2" size="3" value="'.@$coreDHCPArray[1].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_3" size="3" value="'.@$coreDHCPArray[2].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_4" size="3" value="'.@$coreDHCPArray[3].'" '.(($enableDHCP!=1)?'disabled':'').'> - <input type="text" maxlength="3" name="coreDHCP_5" size="3" value="'.@$coreDHCPArray[4].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_6" size="3" value="'.@$coreDHCPArray[5].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_7" size="3" value="'.@$coreDHCPArray[6].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_8" size="3" value="'.@$coreDHCPArray[7].'" '.(($enableDHCP!=1)?'disabled':'').'></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><input type="checkbox" name="ipForAnonymousDevices" value="1" '.((@$nonPlutoIP==1)?'checked':'').' onClick="ipFromDHCP()" '.(($enableDHCP==1)?'':'disabled').'><span id="provide" style="color:'.(($enableDHCP!=1)?'#CCCCCC':'').'"> '.translate('TEXT_IPV4_DHCP_NONLMCE_ENABLED_CONST').'</span></td>
		</tr>
		<tr><td>&nbsp;</td><td colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="nonPluto" style="color:'
			.(($enableDHCP!=1)?'#CCCCCC':'').'">'.translate('TEXT_IPV4_DHCP_NONLMCE_RANGE_CONST').': </span>
			<input type="text" maxlength="3" name="nonPlutoIP_1" size="3" value="'.@$nonPlutoIPArray[0].'" '.(($enableDHCP==1)?'':'disabled').'>			.<input type="text" maxlength="3" name="nonPlutoIP_2" size="3" value="'.@$nonPlutoIPArray[1].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_3" size="3" value="'.@$nonPlutoIPArray[2].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_4" size="3" value="'.@$nonPlutoIPArray[3].'" '.(($enableDHCP==1)?'':'disabled').'> - <input type="text" maxlength="3" name="nonPlutoIP_5" size="3" value="'.@$nonPlutoIPArray[4].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_6" size="3" value="'.@$nonPlutoIPArray[5].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_7" size="3" value="'.@$nonPlutoIPArray[6].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_8" size="3" value="'.@$nonPlutoIPArray[7].'" '.(($enableDHCP==1)?'':'disabled').'>
		</td></tr>
		<tr>
		<td colspan="3"><input type="checkbox" name="enableRA" '
			.(($enableRA==1)?'checked':'').' onclick="setIPv6Range();"> '.translate('TEXT_IPV6_RA_ENABLED_CONST').'</td>
		</tr>
		<tr><td>&nbsp;</td><td colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="IPv6Prefix" style="color:'
			.(($enableRA!=1)?'#CCCCCC':'').'">'.translate('TEXT_IPV6_PUBLIC_PREFIX_CONST').': </span>
			<input type="text" maxlength="3" name="IPv6Prefix_1" size="3" value="'.@$IPv6PrefixArray[0].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_2" size="3" value="'.@$IPv6PrefixArray[1].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_3" size="3" value="'.@$IPv6PrefixArray[2].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_4" size="3" value="'.@$IPv6PrefixArray[3].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_5" size="3" value="'.@$IPv6PrefixArray[4].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_6" size="3" value="'.@$IPv6PrefixArray[5].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_7" size="3" value="'.@$IPv6PrefixArray[6].'" '.(($enableRA==1)?'':'disabled').'>:<input type="text" maxlength="3" name="IPv6Prefix_8" size="3" value="'.@$IPv6PrefixArray[7].'" '.(($enableRA==1)?'':'disabled').'> / <input type="text" maxlength="3" name="IPv6Netmask" size="3" value="'.@$IPv6Netmask.'" '.(($enableRA==1)?'':'disabled').'>
		</td></tr>
		
		<tr>
			<td colspan="3"><input type="checkbox" name="enableVPN" '.(($enableVPN==1)?'checked':'').' onclick="setVPNRange();">'.translate('TEXT_VPN_ENABLED_CONST').'</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><span id="VPNPSKlabel" style="color:'.(($enableVPN!=1)?'#CCCCCC':'').'">'.translate('TEXT_VPN_PSK_CONST').': <input type="text" maxlength="15" name="VPNPSK" size="20" value="'.@$VPNPSK.'" '.(($enableVPN!=1)?'disabled':'').'></td>		
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><span id="VPNrange" style="color:'.(($enableVPN!=1)?'#CCCCCC':'').'">'.translate('TEXT_VPN_CLIENT_RANGE_CONST').': <input type="text" maxlength="3" name="VPNRange_1" size="3" value="'.@$VPNRange[0].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_2" size="3" value="'.@$VPNRange[1].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_3" size="3" value="'.@$VPNRange[2].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_4" size="3" value="'.@$VPNRange[3].'" '.(($enableVPN!=1)?'disabled':'').'> - <input type="text" maxlength="3" name="VPNRange_5" size="3" value="'.@$VPNRange[4].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_6" size="3" value="'.@$VPNRange[5].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_7" size="3" value="'.@$VPNRange[6].'" '.(($enableVPN!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="VPNRange_8" size="3" value="'.@$VPNRange[7].'" '.(($enableVPN!=1)?'disabled':'').'></td>
		</tr>
		
		<tr><td>&nbsp;</td></tr>
		<tr>
			<td colspan="3" class="tablehead"><B>'.translate('TEXT_NUMBER_OF_NIC_CONST').': '.$number_of_cards.'</B></td>
		</tr>
		<tr>
			<td colspan="3"><B>1. '.translate('TEXT_EXTERNAL_NIC_CONST').' '.@$externalInterfaceArray[0].'</B> (MAC: '
			.@$externalMAC.')</td>
		</tr>
		<tr>
			<td colspan="3"><input type="radio" name="ipFrom" value="DHCP" onclick="setStaticIP(true);"'.(($ipFromDHCP==1)?'checked':'')
				.'> '.translate('TEXT_GET_IP_FROM_DHCP_CONST').''
				.'<br><input type="radio" name="ipFrom" value="static" onclick="setStaticIP(false);" '
				.(($ipFromDHCP==0)?'checked':'').'> '.translate('TEXT_USE_STATIC_IP_CONST').'</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td width="150"><span id="coreIPtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">'.translate('TEXT_IP_ADDRESS_CONST').':</span></td>
			<td><input type="text" maxlength="3" name="coreIP_1" size="3" value="'.@$coreIPArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_2" size="3" value="'.@$coreIPArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_3" size="3" value="'.@$coreIPArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_4" size="3" value="'.@$coreIPArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'>'
		.(($ipFromDHCP==1)?'&nbsp&nbspDHCP IP  : '.@$externalIP:'')
		.'</td>	
		</tr>						
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreNMtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">'.translate('TEXT_NETMASK_CONST').':</span></td>
			<td><input type="text" maxlength="3" name="coreNetMask_1" size="3" value="'.@$coreNetMaskArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_2" size="3" value="'.@$coreNetMaskArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_3" size="3" value="'.@$coreNetMaskArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_4" size="3" value="'.@$coreNetMaskArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'>'
		.(($ipFromDHCP==1)?'&nbsp&nbspDHCP MASK: '.@$externalMASK:'')
		.'</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreGWtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">'.translate('TEXT_GATEWAY_CONST').':</span></td>
			<td><input type="text" maxlength="3" name="coreGW_1" size="3" value="'.@$coreGWArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_2" size="3" value="'.@$coreGWArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_3" size="3" value="'.@$coreGWArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_4" size="3" value="'.@$coreGWArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'>'
		 .(($ipFromDHCP==1)?'&nbsp&nbspDHCP GW  : '.@$defaultGW:'')
		.'</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreDNS1text" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">'.translate('TEXT_NAMESERVER_CONST').' (DNS) #1:</span></td>
			<td><input type="text" maxlength="3" name="coreDNS1_1" size="3" value="'.@$coreDNS1Array[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_2" size="3" value="'.@$coreDNS1Array[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_3" size="3" value="'.@$coreDNS1Array[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_4" size="3" value="'.@$coreDNS1Array[3].'" '.(($ipFromDHCP==1)?'disabled':'').'>'
			.(($ipFromDHCP==1)?'&nbsp&nbspDHCP DNS : '.@$DNS:'')
			.'</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreDNS2text" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">'.translate('TEXT_NAMESERVER_CONST').' (DNS) #2:</span></td>
			<td><input type="text" maxlength="3" name="coreDNS2_1" size="3" value="'.@$coreDNS2Array[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_2" size="3" value="'.@$coreDNS2Array[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_3" size="3" value="'.@$coreDNS2Array[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_4" size="3" value="'.@$coreDNS2Array[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>		
		<tr><td colspan="3"><hr></td></tr>
		<tr>
			<td colspan="3"><B>2. '.translate('TEXT_INTERNAL_NIC_CONST').' '.@$internalInterfaceArray[0].'</B> (MAC: '.@$internalMAC.')</td>
		<tr>
		<tr>
			<td>&nbsp;</td>
			<td width="150">'.translate('TEXT_IP_ADDRESS_CONST').':</td>
			<td><input type="text" maxlength="3" name="internalCoreIP_1" size="3" value="'.@$internalCoreIPArray[0].'">.<input type="text" maxlength="3" name="internalCoreIP_2" size="3" value="'.@$internalCoreIPArray[1].'">.<input type="text" maxlength="3" name="internalCoreIP_3" size="3" value="'.@$internalCoreIPArray[2].'">.<input type="text" maxlength="3" name="internalCoreIP_4" size="3" value="'.@$internalCoreIPArray[3].'"></td>
		</tr>						
		<tr>
			<td>&nbsp;</td>
			<td>'.translate('TEXT_NETMASK_CONST').':</td>
			<td><input type="text" maxlength="3" name="internalCoreNetMask_1" size="3" value="'.@$internalCoreNetMaskArray[0].'">.<input type="text" maxlength="3" name="internalCoreNetMask_2" size="3" value="'.@$internalCoreNetMaskArray[1].'">.<input type="text" maxlength="3" name="internalCoreNetMask_3" size="3" value="'.@$internalCoreNetMaskArray[2].'">.<input type="text" maxlength="3" name="internalCoreNetMask_4" size="3" value="'.@$internalCoreNetMaskArray[3].'"></td>
		</tr>
		<tr><td colspan="3"><hr></td></tr>
		<tr>
			<td colspan=3>'.$swaphtml.'</td>
		</tr>
		<tr>
			<td><input type="checkbox" name="OfflineMode" value="1" '.(($OfflineMode=='true')?'checked':'').'></td>
			<td colspan="2"><B>'.translate('TEXT_OFFLINEMODE_CONST').'</B></td>
		</tr>			
		<tr>
			<td colspan="3" align="center" bgcolor="#EEEEEE"><input type="button" class="button" name="update" value="'.translate('TEXT_UPDATE_CONST').'" onClick="validateForm()"> <input type="reset" class="button" name="reset" value="'.translate('TEXT_RESET_CONST').'"></td>
		</tr>		
	<tr><td colspan="3">'.translate('TEXT_OPEN_FIREWALL_CONST').'</td></tr>	'
	//<tr><td colspan="3" class="tablehead">Current settings:</td></tr>
	//<tr><td colspan="3" class="alternate_back">'.get_network_settings().'</td></tr>
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
		$coreDHCP=getIpFromParts('coreDHCP_',1).'-'.getIpFromParts('coreDHCP_',5);

		// new internal core ip
		$internalCoreIP=getIpFromParts('internalCoreIP_');


		$isChanged=0;
		$ipForAnonymousDevices=isset($_POST['ipForAnonymousDevices'])?1:0;
		if($ipForAnonymousDevices==1){
			$coreDHCP.=','.getIpFromParts('nonPlutoIP_').'-'.getIpFromParts('nonPlutoIP_',5);
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

		$externalInterface=$externalInterfaceArray[0];
		$internalInterface=$internalInterfaceArray[0];
		if(isset($_POST['swap'])){
      $externalInterface=$internalInterfaceArray[0];
      $internalInterface=$externalInterfaceArray[0];

			$needReboot=1;
		}
		if($resNC->RecordCount()>0){
			$networkInterfaces=$externalInterface;
			if($_POST['ipFrom']=='DHCP'){
				$networkInterfaces.=',dhcp|';
			}else{
				$dns1 = getIpFromParts('coreDNS1_');
				$dns2 = getIpFromParts('coreDNS2_');
				$dns_string = $dns1 . ($dns2 === "" ? "" : ",$dns2");
				$networkInterfaces.=','.getIpFromParts('coreIP_').','.getIpFromParts('coreNetMask_').','.getIpFromParts('coreGW_').','.$dns_string.'|';
			}
			if ($internalInterface !== ""){
				$networkInterfaces.=$internalInterface.','.$internalCoreIP.','.getIpFromParts('internalCoreNetMask_');
			}
			if($networkInterfaces!=$rowNC['IK_DeviceData']){
				$SQL="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(?,?,?)";
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['NetworkInterfaces'],$networkInterfaces));
			}
		}

		// tokenize IPv6 values to put in CORE device data
		$resRA=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292');
		$rowRA=$resRA->FetchRow();
		$ipv6_data=explode(",", $rowRA['IK_DeviceData']);
		$ipv6_data[5]= getIPv6FromParts('IPv6Prefix_'); 
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
		$VPN_data[1]= $coreDHCP=getIpFromParts('VPNRange_',1).'-'.getIpFromParts('VPNRange_',5);
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

function get_network_settings(){
	$command='sudo -u root /usr/pluto/bin/Network_DisplaySettings.sh --all';
	$ret=exec_batch_command($command,1);
	
	$lines=explode("\n",$ret);

	if(count($lines)==0){
		return '';
	}
	$out='<table>';
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, 'http://automation.whatismyip.com/n09230945.asp');
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	$internetip = curl_exec($ch);
	if($internetip) {	
		$internetname=gethostbyaddr($internetip);
		$out.='<tr><td><b>CORE SEEN FROM OUTSIDE</b></td><td>'.$internetip.' ('.$internetname.')</td></tr>';
		$out.='<td colspan=2><hr></td>';
	}
	foreach ($lines AS $line){
		$parts=explode('=',$line);
		if($parts[0]=='INTERNAL_IFACE' || $parts[0]=='GATEWAY') $out.='<td colspan=2><hr></td>';
		if(count($parts)==2){
			$out.='
			<tr>
				<td><b>'.trim($parts[0]).'</b></td>
				<td>'.trim($parts[1]).'</td>
			</tr>';
		}
	}
	$out.='</table>';
	
	return $out;
}
?>

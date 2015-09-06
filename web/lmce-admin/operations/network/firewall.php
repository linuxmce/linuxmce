<?php
function firewall($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('firewall.lang.php');
	
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$accessFile=$GLOBALS['pluto.conf'];

	// check if firewall is disabled
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	if(count($retArray)==0){
		$_GET['error'].='Insuffient rights: pluto.conf file cannot be opened.';
	}
	// grep all interfaces
	exec('cat /proc/net/dev | tail -n +3 | cut -d":" -f 1  | sed -e \'s/^[ \t]*//\'',$ifArray);
	
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
	}
	
	$i=0;
	if (@$AdvancedFirewall == 1){
			$res=$dbADO->Execute('SELECT PK_Firewall, Matchname FROM Firewall WHERE Protocol=\'chain-ipv4\' ORDER BY PK_Firewall');
		while ($row=$res->FetchRow()) {
			$key[]=$i;
			$value[]=$row['Matchname'];
			$value1[]=$row['Matchname'].'!'.$row['PK_Firewall'];
			$i++;
		}
	$more_chains=array_combine($key, $value);
	$more_chains_id=array_combine($key, $value1);
	}
	
	if ($fwVersion == "ipv6") {
		$start_chains=Array(
		"0"=>"input",
		"1"=>"forward",
		"2"=>"output",
		);
	} else {
		$start_chains=Array(
		"0"=>"input",
		"1"=>"forward",
		"2"=>"port_forward (NAT)",
		"3"=>"output",
		);
	};
	
	$RuleTYPE=Array(
	"0"=>"",
	"1"=>"PREROUTING",
	"2"=>"POSTROUTING",
	"3"=>"OUTPUT",
	);
	
	$protocolarr=Array(
	"0"=>"tcp",
	"1"=>"udp",
	"2"=>"tcp & udp",
	"3"=>"icmp",
	"4"=>"ip",
	);
	
	$start_RPolicy=Array(
	"0"=>"ACCEPT",
	"1"=>"DROP",
	"2"=>"REJECT",
	"3"=>"MASQUERADE",
	"4"=>"RETURN",
	"5"=>"REDIRECT",
	"6"=>"LOG",
	);
	
	if(count($more_chains)!=0){
	$chains=array_merge($start_chains, $more_chains);
	$chains_id=array_merge($start_chains, $more_chains_id);
	$save_chains=array_merge($start_chains, $more_chains);
	$RPolicy=array_merge($start_RPolicy, $more_chains);
	} else {
	$chains=$start_chains;
	$save_chains=$start_chains;
	$RPolicy=$start_RPolicy;
	}
	
		foreach ($chains as $chainnr => $chain) {
		$selected='';
		$chain_options.= "<option value='$chain' $selected >$chain</option>";
	}
	
	foreach ($RPolicy as $RulepPolicy => $Policy) {
			$selected=$row['RPolicy'] == $RPolicy?'selected':'';
			$RulePolicy_options .= "<option value='$Policy' $selected >$Policy</option>";
		}
	
	foreach ($RuleTYPE as $RuleTYPEnr => $Ruletype) {
		$selected='';
		$Ruletype_options.= "<option value='$Ruletype' $selected >$Ruletype</option>";
	}
		
	if ($action == 'form') {

	$out.='
	<script>
	function fwIPVersion()
	{
	document.getElementById(\'firewall\').submit();
	}	
	function enableDestination()
	{
		if(document.getElementById(\'firewall\').Chain.value=="input"){
			document.getElementById(\'firewall\').IntIf.disabled=false;
			document.getElementById(\'firewall\').ExtIf.disabled=true;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=true;
		} else if(document.getElementById(\'firewall\').Chain.value=="output"){
			document.getElementById(\'firewall\').IntIf.disabled=true;
			document.getElementById(\'firewall\').ExtIf.disabled=false;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=true;
		} else if(document.getElementById(\'firewall\').Chain.value=="forward"){
			document.getElementById(\'firewall\').IntIf.disabled=false;
			document.getElementById(\'firewall\').ExtIf.disabled=false;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=true;
		} else if(document.getElementById(\'firewall\').Chain.value=="VPN"){
			document.getElementById(\'firewall\').IntIf.disabled=false;
			document.getElementById(\'firewall\').ExtIf.disabled=false;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=true;
		} else if(document.getElementById(\'firewall\').Chain.value=="mds"){
			document.getElementById(\'firewall\').IntIf.disabled=false;
			document.getElementById(\'firewall\').ExtIf.disabled=false;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=true;
		} else{
			document.getElementById(\'firewall\').IntIf.disabled=false;
			document.getElementById(\'firewall\').ExtIf.disabled=false;
			document.getElementById(\'firewall\').DestinationPort.disabled=false;
			document.getElementById(\'firewall\').DestinationIP.disabled=false;
			document.getElementById(\'firewall\').RuleType.disabled=false;
		}
	}
        function confirmDisableFirewall(ver)
        {
                if((document.getElementById(\'firewall\').change_ipv4_firewall_status.checked && ver=="ipv4")
                        || (document.getElementById(\'firewall\').change_ipv4_firewall_status.checked && ver=="ipv6")){
                        if(confirm("("+ver+") '.translate('TEXT_DISABLE_FIREWALL_CONFIRMATION_CONST').'")){
                                document.getElementById(\'firewall\').submit();
                        }else{
                                if(ver=="ipv4") document.getElementById(\'firewall\').change_ipv4_firewall_status.checked=false;
                                else if(ver=="ipv6") document.getElementById(\'firewall\').change_ipv6_firewall_status.checked=false;
                        }
                }else{
                        document.getElementById(\'firewall\').submit();
                }
        }
	function confirmAdvanced()
	{
		if (document.getElementById(\'firewall\').advanced_firewall.checked){
			if(confirm("Advanced Settings?")){
				document.getElementById(\'firewall\').submit();
			}
		}else{
			document.getElementById(\'firewall\').submit();
		}
	}
	function confirmResetFirewall()
	{
			if(confirm("Are you sure you want to reset the Firewall to defaults?")){
				document.getElementById(\'firewall\').IDIOT.type="submit";
				document.getElementById(\'firewall\').submit();
			}
	}
	function confirmDisableRule(RuleID)
	{
		document.getElementById(\'firewall\').disableRule.value=RuleID;
		document.getElementById(\'firewall\').submit();
	}
	function confirmEnableRule(RuleID)
	{
		document.getElementById(\'firewall\').enableRule.value=RuleID;
		document.getElementById(\'firewall\').submit();
	}

	function show_allRules()
	{
		if (document.getElementById(\'firewall\').show_all_rules.checked){
			if(confirm("Show all unprocessed rules?")){
				document.getElementById(\'firewall\').submit();
			}
		}else{
			document.getElementById(\'firewall\').submit();
		}
	}
	function fw_block()
	{
		if (document.getElementById(\'firewall\').fw_blocklist.checked){
			document.getElementById(\'firewall\').submit();
		}else{
			if(confirm("Disable automatic blocking of TOP20 known attackers?")){
				document.getElementById(\'firewall\').submit();
			}
		}
	}
	
	function edit_Chain(ID)
	{
		document.getElementById(\'firewall\').edit_chain.value;
		document.getElementById(\'firewall\').Chain_Id.value=ID;
		document.getElementById(\'firewall\').submit();
	}
	
	function add_Chain()
	{
		document.getElementById(\'firewall\').add_Chain.type="submit";
		document.getElementById(\'firewall\').New_Chain.value;
		document.getElementById(\'firewall\').submit();
	}
	
	function delete_chain(Del_ID)
	{
		document.getElementById(\'firewall\').del_chain.value;
		document.getElementById(\'firewall\').Chain_Id.value=Del_ID;
		document.getElementById(\'firewall\').submit();
	}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form id="firewall" action="index.php" method="POST" name="firewall">
	<input type="hidden" name="section" value="firewall">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="disableRule" value="add">
	<input type="hidden" name="enableRule" value="add">
		<input type="hidden" name="Chain_Id" value="add" />
	<input type="hidden" name="Chain_url_Id" value="add" />
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0" align="center">
		<tr>
			<td colspan="100%" align="center"><h3 class="err">'.((@$DisableIPv4Firewall!=1)?'':'- '.translate('TEXT_FIREWALL_DISABLED_WARNING_CONST').' - ')
                                        .((@$DisableIPv6Firewall!=1)?'':' - '.translate('TEXT_IPV6_FIREWALL_DISABLED_WARNING_CONST').' -').'</h3></td>
 		</tr>
		<tr>
			<td colspan="4" align="left">
			<input type="checkbox" name="change_ipv4_firewall_status" value="1" '.((@$DisableIPv4Firewall!=1)?'':'checked').' onClick="confirmDisableFirewall(\'ipv4\');"> IPv4 '.translate('TEXT_FIREWALL_DISABLED_CONST').'<br />
			<input type="checkbox" name="change_ipv6_firewall_status" value="1" '.((@$DisableIPv6Firewall!=1)?'':'checked').' onClick="confirmDisableFirewall(\'ipv6\');"> IPv6 '.translate('TEXT_FIREWALL_DISABLED_CONST').'<br />
			<input type="checkbox" name="advanced_firewall" value="1" '.((@$AdvancedFirewall!=1)?'':'checked').' onClick="confirmAdvanced();" > Advanced Firewall Settings<br />';
			if ($AdvancedFirewall == 1){
				$out.='<input type="checkbox" name="show_all_rules" value="1" '.((@$Show_all_rules!=1)?'':'checked').' onClick="show_allRules();"> Show all unprocessed rules<br />';
				$out.='<input type="checkbox" name="fw_blocklist" value="1" '.((@$fw_blocklist!=1)?'':'checked').' onClick="fw_block();"> Block TOP20 known attackers';
			}
			$out.='</td><td colspan="3" align="left">Select Firewall : <select name="fwVersion" onChange="fwIPVersion()"">
                                <option value="ipv4" '.((@$fwVersion!="ipv4")?'':'selected').'>IPv4 Firewall</option>
                                <option value="ipv6" '.((@$fwVersion!="ipv6")?'':'selected').'>IPv6 Firewall</option>
                        </select>
			<td colspan="30%" align="right"><input type="button" class="button" name="IDIOT" value="Reset '.@$fwVersion.' firewall to defaults" onclick="confirmResetFirewall()"></td>
		</tr>
		<tr><td colspan="100%"><hr></td></tr>';
		$i=0;
		if (@$AdvancedFirewall == 1){
		$chain=count($chains);
		}else{
		$chain=1;
		}
		while ( $i < $chain) {
		if (isset($_GET['edit_Chain']) && $_GET['edit_Chain'] == $explode['1']) {
			$out.= '<tr><td colspan="100%"></td></tr>
			<tr><td colspan="4" align="left"><input type="text" name="chain_name" value="'.$chains[$i].'" />';
			$out.= '</B></td><td colspan="5" align="Left">';
		} else {
			$explode=explode("!", $chains_id[$i]);
			$out.= '<tr><td colspan="100%"></td></tr>';
			if (isset($_GET['edit_Chain']) && $_GET['edit_Chain'] == $explode['1']) {
				$out.='<tr><td colspan="4" align="left"><input type="text" name="edit_chain" value="'.$explode['0'].'" /></td><td colspan="5" align="Left">';
			} else {
				$out.='<tr><td colspan="4" align="left"><B>'.$chains[$i].'</B></td><td colspan="5" align="Left">';
			}
		if ($chains[$i] == 'input' || $chains[$i] == 'forward' || $chains[$i] == 'output'){
				if (@$AdvancedFirewall == 1){
					$out.='Default Firewall Policy : ';
					$res=$dbADO->Execute('SELECT Matchname FROM Firewall where Protocol = \''.$chains[$i].'\' AND RuleType = "policy'.$fwVersion.'"');
						while ($row=$res->FetchRow()) {
					        $out.= $row['Matchname'].'</td>';
							$DPolicy=$row['Matchname'];
						}
					$out.='<td colspan="30%" align="right" >Set default policy :
					<input type="submit" class="button" name="'.$chains[$i].'Policy" value="ACCEPT">
					<input type="submit" class="button" name="'.$chains[$i].'Policy" value="DROP">
					<input type="submit" class="button" name="'.$chains[$i].'Policy" value="REJECT">';
				}

		} elseif (strpos($chains[$i], 'port_forward (NAT)') !== false || strpos($chains[$i], 'VPN') !== false || strpos($chains[$i], 'LOGGING') !== false || strpos($chains[$i], 'MD\'s') !== false || strpos($chains[$i], 'fail2ban-') !== false) {
			
			} else {
					$out.='<td colspan="30%" align="right" >';
						if (isset($_GET['edit_Chain']) && $_GET['edit_Chain'] == $explode['1']) {
							$out.='<input type="submit" class="button" name="save" value="save" onclick="edit_Chain('.$explode['1'].')" />
							<input type="submit" class="button" name="del_chain" value="DELETE" onClick="delete_chain('.$explode['1'].')" />';
						} else {
							$out.='<input type="button" class="button" name="edit" value="Edit" onclick="window.location.href=\'index.php?section=firewall&edit_Chain='.$explode['1'].'\';" />
							<input type="submit" class="button" name="del_chain" value="DELETE" onClick="delete_chain('.$explode['1'].')" />';
						}
				}
		}
		$out.='</tr><tr class="tablehead">
			<td align="center"><B>'.translate('TEXT_DISABLED_CONST').'</B></td>';
			if (@$AdvancedFirewall == 1){
				$out.='<td align="center"><B>'.translate('TEXT_POSITION_CONST').'</B></td>';
			}
			$out.='<td align="center"><B>'.translate('TEXT_IPVERSION_CONST').'</B></td>';
				$out.='<td align="center"><B>'.translate('TEXT_RULE_TYPE_CONST').'</B></td>';
				if (@$AdvancedFirewall == 1){
				if ( $chains[$i] == 'input' ) {
			                $out.='<td align="center"><B>'.translate('TEXT_INT_IF_CONST').'</B></td>
					<td></td>';
				} elseif ( $chains[$i] == 'output' ) {
                			$out.='<td></td><td align="center"><B>'.translate('TEXT_EXT_IF_CONST').'</B></td>';
				} else {
					$out.='<td align="center"><B>'.translate('TEXT_INT_IF_CONST').'</B></td>
					<td align="center"><B>'.translate('TEXT_EXT_IF_CONST').'</B></td>';
				}
		        	$out.='<td align="crnter"><B>'.translate('TEXT_MATCH_CONST').'</B></td>';
			}
			$out.='<td align="center"><B>'.translate('TEXT_PROTOCOL_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_SOURCE_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_IP_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_LIMIT_TO_IP_CONST').'</B></td>';
			if (@$AdvancedFirewall == 1){
				$out.='<td align="center"><B>'.translate('TEXT_RULE_POLICY_CONST').'</B></td>';
			}
			$out.='<td align="center"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
			<td>&nbsp;</td>
		</tr>
		';
		if (@$AdvancedFirewall == 1) {
			if (@$Show_all_rules == 1 || $chains[$i] != 'input' || $chains[$i] != 'forward' || $chains[$i] != 'output') {
				$res=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType LIKE "'.$chains[$i].'%" AND Protocol LIKE \'%-'.$fwVersion.'\' ORDER BY Place, PK_Firewall');
			} else {
				$res=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType="'.$chains[$i].'" AND RPolicy!=\''.$DPolicy.'\' AND Protocol LIKE \'%-'.$fwVersion.'\' ORDER BY  Place, PK_Firewall');
			}
		} else {
			$res=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType="'.$chains[$i].'" AND RPolicy!=\''.$DPolicy.'\' AND Protocol LIKE \'%-'.$fwVersion.'\' ORDER BY Place, SourcePort');
		}
		$pos=0;
		while($row=$res->FetchRow()){
			++$pos;
			$protocol=explode("-",$row['Protocol']);
			if ($protocol[0] == "all") {
				$protocol[0]='tcp & udp';
			}
			$RuleTypearr=explode("-",$row['RuleType']);
				$save_chain_options="";
				foreach ($save_chains as $chainnr => $save_chain) {
					$selected=$RuleTypearr[0] == $save_chain?'selected':'';
					$save_chain_options.= "<option value='$save_chain' $selected >$save_chain</option>";
				}
				$save_Ruletype_options="";
				foreach ($RuleTYPE as $save_RuleTYPEnr => $save_Ruletype) {
					$selected=$RuleTypearr[1] == $save_Ruletype?'selected':'';
					$save_Ruletype_options.= "<option value='$save_Ruletype' $selected >$save_Ruletype</option>";
				}
			if (isset($_GET['rule']) && $row['PK_Firewall'] == $_GET['rule']) {
				$Disabled=$row['Disabled'];
				$tr='<tr class="'.(($pos%2==0)?'alternate_back':'');
				if ( $row['Disabled'] == 1 ) {
						$tr.=' err';
					}
				if ($chains[$i] == 'input' || $chains[$i] == 'forward' || $chains[$i] == 'output'){
					if ( $DPolicy == $row['RPolicy'] ) {
						$tr.='" style="color: #808080';
					}
				}
				$tr.='">';
				$out.=$tr;
				$out.='<td align="center"><input type="checkbox" name="disable" value="'.$row['PK_Firewall'].'" onclick="if (this.checked) { confirmDisableRule('.$row['PK_Firewall'].') } else { confirmEnableRule('.$row['PK_Firewall'].') }" '.((@$Disabled!=1)?'':'checked').' /></td>';
					if (@$AdvancedFirewall == 1){
						$out.='<td align="center"><a href="index.php?section=firewall&action=move&moverule='.$prevrule.'" style="text-decoration: none;">&#8679;</a><a href="index.php?section=firewall&action=move&moverule='.$row['PK_Firewall'].'" style="text-decoration: none;">&#8681;</a></td>';
						$prevrule=$row['PK_Firewall'];
					}
					$out.='<script>
						function saveRule()
	{	
		document.getElementById(\'firewall\').save_Rule.value;
		document.getElementById(\'firewall\').save_place.value;
		document.getElementById(\'firewall\').save_IntIf.value;
		document.getElementById(\'firewall\').save_ExtIf.value;
		document.getElementById(\'firewall\').save_Matchname.value;
		document.getElementById(\'firewall\').save_protocol.value;
		document.getElementById(\'firewall\').save_IPVersion.value;
		document.getElementById(\'firewall\').save_SourcePort.value;
		document.getElementById(\'firewall\').save_SourcePortEnd.value;
		document.getElementById(\'firewall\').save_DestinationPort.value;
		document.getElementById(\'firewall\').save_DestinationIP.value;		  
		document.getElementById(\'firewall\').save_Chain.value;
		document.getElementById(\'firewall\').save_RuleType.value;
		document.getElementById(\'firewall\').save_SourceIP.value;
		document.getElementById(\'firewall\').save_RPolicy.value;
		document.getElementById(\'firewall\').save_Description.value;
		document.getElementById(\'firewall\').submit();
		
	}
	</script>
					<td align="center"><select name="save_IPVersion" STYLE="width:70px">
						<option value="ipv4" '.($protocol[1]=='ipv4'?'selected':'').'>IPv4</option>
						<option value="ipv6" '.($protocol[1]=='ipv6'?'selected':'').'>IPv6</option>
						<!--<option value="both">both</option>-->
					</select>';
						if ($AdvancedFirewall == 1) {
							$out.='
							place<br />
							<select name="save_place" STYLE="width:70px">
								<option value="2">Middle</option>
								<option value="1">First</option>
								<option value="3">Last</option>
							</select></td>';
						}
						$out.='<input type="hidden" name="save_place" value="'.$row['Place'].'" />
						</td><td align="center"><select name="save_Chain">
							'.$save_chain_options.'
							
							</select>';
							
					if (@$AdvancedFirewall == 1){	
						if ($RuleTypearr[1] == 'PREROUTING' || $RuleTypearr[1] == 'POSTROUTING' || $RuleTypearr[1] == 'OUTPUT' ) {
							$out.='<select name="save_RuleType">
										'.$save_Ruletype_options.'
									</select></td>';
						} else {
							$out.='<input type="hidden" name="save_RuleType" value="" />';
						}
        	                                if ( $chains[$i] == 'input'  || $chains[$i]=='nat' || $RuleTypearr[1] == 'PREROUTING')  {
	        	                                $out.='<td><select name="save_IntIf" STYLE="width:70px">
				                        <option value=""></option>';
			        	                foreach ($ifArray as $string){
											if ($string=="ipv6tunnel"){
												if ($row['IntIF']==="ipv6tunn") {
												$out.='<option value="ipv6tunn" selected>'.$string.'</option>';
												} else {
													$out.='<option value="ipv6tunn">'.$string.'</option>';
												}
											} else {
												$out.= '<option value="'.$string.'" ';
												if ($row['IntIF']==$string) {
													$out.='selected';
												}
												$out.='>'.$string.'</option>';
											}
										}
                        				$out.= '</select></td>
							<td><input type="hidden" name="save_ExtIf" value="" /></td>';
                                        	} elseif ($chains[$i] =='output') {
												     $out.='<td><input type="hidden" name="save_IntIf" value="" /></td><td><select name="save_ExtIf" STYLE="width:70px">
														<option value=""></option>';
														foreach ($ifArray as $string){
															if ($string=="ipv6tunnel"){
																if ($row['ExtIF']==="ipv6tunn") {
																	$out.='<option value="ipv6tunn" selected>'.$string.'</option>';
																} else {
																	$out.='<option value="ipv6tunn">'.$string.'</option>';
																}
															} else {
																$out.= '<option value="'.$string.'" ';
																if ($row['ExtIF']==$string) {
																		$out.='selected';
																}
																$out.='>'.$string.'</option>';
															}
														}
        	                			$out.= '</select></td>';
											} else {
		                                        $out.='<td><select name="save_IntIf" STYLE="width:70px">
				                        <option value=""></option>';
				                        foreach ($ifArray as $string){
												if ($string=="ipv6tunnel"){
													if ($row['IntIF']==="ipv6tunn") {
														$out.='<option value="ipv6tunn" selected>'.$string.'</option>';
													} else {
														$out.='<option value="ipv6tunn">'.$string.'</option>';
													}
												} else {
													$out.= '<option value="'.$string.'" '; 
													if ($row['IntIF']==$string) {
														$out.='selected';
													}
													$out.='>'.$string.'</option>';
												}
				                        }
                	        			$out.= '</select></td>';
	                	                        $out.='<td><select name="save_ExtIf" STYLE="width:70px">
			        	                <option value=""></option>';
			                	        foreach ($ifArray as $string){
											if ($string=="ipv6tunnel"){
												if ($row['ExtIF']==="ipv6tunn") {
													$out.='<option value="ipv6tunn" selected>'.$string.'</option>';
												} else {
													$out.='<option value="ipv6tunn">'.$string.'</option>';
												}
											} else {
                        				        $out.= '<option value="'.$string.'" ';
                                                if ($row['ExtIF']==$string) {
                                                   	$out.='selected';
	                                            }
        	                                    $out.='>'.$string.'</option>';
											}
										}
                        				$out.= '</select></td>';
                                	        }
						$out.='<td><input type="text" name="save_Matchname" value="'.$row['Matchname'].'" STYLE="width:100%" /></td>';
					} else {
                			    $out.='<input type="hidden" name="save_IntIf" value="'.$row['IntIF'].'" />
								<input type="hidden" name="save_ExtIf" value="'.$row['ExtIF'].'" />
								<input type="hidden" name="save_Matchname" value="'.$row['Matchname'].'" />';   
		                        }
					$out.='<td align="center"><select name="save_protocol" STYLE="width:70px">';
						foreach ($protocolarr as $string){
							if ($string=="tcp & udp"){
								if ($protocol[0]===$string) {
									$out.='<option value"all" selected>'.$string.'</option>';
								} else {
									$out.='<option value"all">'.$string.'</option>';
								}
							} else {
								$out.='<option value"'.$string.'" ';
								if ($protocol[0]===$string) {
									$out.='selected';
								}$out.='>'.$string.'</option>';
							}
						}
						$save_RuleType=explode('-', $row['RuleType']);
						$out.='</select></td>
					<input type="hidden" name="save_RuleType" value="'.$save_RuleType[1].'" />
					<td align="center"><input type="text" name="save_SourcePort" value="'.$row['SourcePort'].'" size="4" /> to <input type="text" name="save_SourcePortEnd" value="'.$row['SourcePortEnd'].'" size="4" /></B></td>
                    <td align="center"><input type="text" name="save_DestinationPort" value="'.$row['DestinationPort'].'" size="4" /></td>
					<td align="center"><input type="text" name="save_DestinationIP" value="'.$row['DestinationIP'].'" size="8" /></td>
					<td align="center"><input type="text" name="save_SourceIP" value="'.$row['SourceIP'].'" size="8"></td>';
					if (@$AdvancedFirewall == 1){
						$out.='<td align="center"><select name="save_RPolicy" STYLE="width:70px">';
						foreach ($RPolicy as $string){
							$out.='<option value"'.$string.'" ';
							if ($row['RPolicy']==$string) {
								$out.='selected';
							}$out.='>'.$string.'</option>';
						}
						$out.='</select></td></td>';
					} else {
                			        $out.='<input type="hidden" name="save_RPolicy" value="ACCEPT">';
		                        }
					$out.='<td align="center"><textarea rows="2" cols="4" name="save_Description">'.$row['Description'].'</textarea></td>
					<td align="center"><input type="hidden" name="save_Rule" value="'.$row['PK_Firewall'].'" /><span style="text-decoration: underline; cursor:pointer; color:#009108; font-size: 12px;" onclick="saveRule()">'.translate('TEXT_SAVE_CONST').'</span>&nbsp;<a href="index.php?section=firewall&action=del&delid='.$row['PK_Firewall'].'">'.translate('TEXT_DELETE_CONST').'</a></td>
					</tr>';
			} else {
				$Disabled=$row['Disabled'];
				$tr='<tr class="'.(($pos%2==0)?'alternate_back':'');
				if ( $row['Disabled'] == 1 ) {
						$tr.=' err';
					}
				if ($chains[$i] == 'input' || $chains[$i] == 'forward' || $chains[$i] == 'output'){
					if ( $DPolicy == $row['RPolicy'] ) {
						$tr.='" style="color: #808080';
					}
				}
				$tr.='">';
				$out.=$tr;
				$out.='<td align="center"><input type="checkbox" name="disable" value="'.$row['PK_Firewall'].'" onclick="if (this.checked) { confirmDisableRule('.$row['PK_Firewall'].') } else { confirmEnableRule('.$row['PK_Firewall'].') }" '.((@$Disabled!=1)?'':'checked').' /></td>';
				if (@$AdvancedFirewall == 1){
					$out.='<td align="center"><a href="index.php?section=firewall&action=move&moverule='.$prevrule.'" style="text-decoration: none;">&#8679;</a><a href="index.php?section=firewall&action=move&moverule='.$row['PK_Firewall'].'" style="text-decoration: none;">&#8681;</a></td>';
					$prevrule=$row['PK_Firewall'];
				}
				$out.='<td align="center">'.$protocol[1].'</td>
				<td align="center">'.$row['RuleType'].'</td>';
						if (@$AdvancedFirewall == 1){
							if ($row['IntIF']=="ipv6tunn"){
								$IntIf="ipv6tunnel";
							} else {
								$IntIf=$row['IntIF'];
							}
							if ($row['ExtIF']=="ipv6tunn"){
								$ExtIf="ipv6tunnel";
							} else {
								$ExtIf=$row['ExtIF'];
							}
        	                if ( $chains[$i] == 'input' )  {
                	            $out.='<td>'.$IntIf.'
								<td></td>';
                            } elseif ( $chains[$i] == 'output' ) {
        	                      $out.='<td></td>';
								$out.='<td>'.$ExtIf.'</td>';
                        	} else {
								$out.='<td>'.$IntIf.'</td>
								<td>'.$ExtIf.'</td>';
							}
							$out.='<td>'.$row['Matchname'].'</td>';
						}
						$out.='<td align="center">'.$protocol[0].'</td>';
                                               	if ( $row['SourcePort'] == ''){
                                                       	$out.='<td align="center"></td>';
                                                }else{
       	                                                $out.='<td align="center">'.($protocol[0] == 'ip' ? 'protocol: ' : '').
						$row['SourcePort'].($row['SourcePortEnd'] > 0 ? ' to '.$row['SourcePortEnd'] : '').'</B></td>';
                       	                        }
                               	                $out.='<td align="center">'.($row['DestinationPort'] > 0 ? $row['DestinationPort']:'').'</td>
						<td align="center">'.$row['DestinationIP'].'</td>
						<td align="center">'.$row['SourceIP'].'</td>';
						if (@$AdvancedFirewall == 1){
							$out.='<td align="center">'.$row['RPolicy'].'</td>';
						}
						$out.='<td align="center">'.$row['Description'].'</td>
						<td align="center"><a href="index.php?section=firewall&rule='.$row['PK_Firewall'].'" />'.translate('TEXT_EDIT_CONST').'</a>&nbsp;<a href="index.php?section=firewall&action=del&delid='.$row['PK_Firewall'].'">'.translate('TEXT_DELETE_CONST').'</a></td>
					</tr>';

			}
		}
		$i++;
		$out.='<tr><td colspan="100%"><hr></td></tr>';
		}
		$out.='
		<tr>
			<td colspan="100%" align="center">&nbsp;</td>
		</tr>
		<tr>
			<td colspan="100%" align="center" bgcolor="#EEEEEE"><B>'.translate('TEXT_ADD_NEW_FIREWALL_RULE_CONST').'</B></td>
		</tr>
		<tr class="tablehead">';
		if (@$AdvancedFirewall == 1){
			$out.='<td colspan="3" align="center"><B>'.translate('TEXT_IPVERSION_CONST').'</B></td>';
		} else {
			$out.='<td colspan="2" align="center"><B>'.translate('TEXT_IPVERSION_CONST').'</B></td>';
		}
				$out.='<td align="center"><B>'.translate('TEXT_RULE_TYPE_CONST').'</B></td>';
			if (@$AdvancedFirewall == 1){
				$out.='<td align="center"><B>'.translate('TEXT_INT_IF_CONST').'*</B></td>
				<td align="center"><B>'.translate('TEXT_EXT_IF_CONST').'*</B></td>
				<td align="center"><B>'.translate('TEXT_MATCH_CONST').'*</B></td>';
			}
			$out.='<td align="center"><B>'.translate('TEXT_PROTOCOL_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_SOURCE_PORT_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_PORT_CONST').'**</B></td>
			<td align="center"><B>'.translate('TEXT_DESTINATION_IP_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_LIMIT_TO_IP_CONST').'*</B></td>';
			if (@$AdvancedFirewall == 1){
				$out.='<td align="center"><B>'.translate('TEXT_RULE_POLICY_CONST').'</B></td>';
			}
			$out.='<td align="center"><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
			<td>&nbsp;</td>
		</tr>				
		<tr>';
		if (@$AdvancedFirewall == 1) {
			$out.='<td colspan="3" align="center"><select name="IPVersion" STYLE="width:70px">';
		} else {
			$out.='<td colspan="2" align="center"><select name="IPVersion" STYLE="width:70px">'; 
		}
		    $out.='<option value="ipv4" '.((@$fwVersion!="ipv4")?'':'selected').'>IPv4</option>
                <option value="ipv6" '.((@$fwVersion!="ipv6")?'':'selected').'>IPv6</option>
			</select><br />';
			if ($AdvancedFirewall == 1) {
			$out.='
			place<br />
			<select name="place" STYLE="width:70px">
			<option value="2">Middle</option>
			<option value="1">First</option>
			<option value="3">Last</option>
			</select></td>';
			}
			$out.='<td align="center" width="110"><select name="Chain" onChange="enableDestination()">
				'.$chain_options.'
			</select>';
			if (@$AdvancedFirewall == 1 && $fwVersion != "ipv6"){
				$out.='<select name="RuleType" disabled>
				'.$Ruletype_options.'
				</select></td>';
			} else {
				$out.='</td>';
			}
			if (@$AdvancedFirewall == 1) {
				$out.='<td><select name="IntIf" STYLE="width:70px">
				<option value=""></option>';
				foreach ($ifArray as $string){
					$out.= '<option value="'.$string.'">'.$string.'</option>';
				}
				$out.= '</select></td>
				<td><select name="ExtIf" STYLE="width:70px" disabled>
				<option value=""></option>';
				foreach ($ifArray as $string){
					$out.= '<option value="'.$string.'">'.$string.'</option>';
				}
				$out.= '</select></td>
				<td><input type="text" name="Matchname" STYLE="width:200px">
				</select></td>';
			}
			
			$out.='<td align="center"><select name="protocol" STYLE="width:70px">';
				foreach ($protocolarr as $string){
					if ($string==='tcp & udp') {
						$out.='<option value"all">'.$string.'</option>';
					} else {
						$out.='<option value="'.$string.'">'.$string.'</option>';
					}
				}
			$out.='</select></td>
			<td align="center" width="120"><input type="text" name="SourcePort" size="4" /> to <input type="text" name="SourcePortEnd" size="2" /></td>
			<td align="center"><input type="text" name="DestinationPort" size="4" /></td>
			<td align="center"><input type="text" name="DestinationIP" size="8" /></td>
			<td align="center"><input type="text" name="SourceIP" size="8"></td>';
			if (@$AdvancedFirewall == 1){
				$out.='<td align="center"><select name="RPolicy" STYLE="width:70px">">'.$RulePolicy_options.'</select></td>';
			} else {
			$out.='<input type="hidden" name="RPolicy" value="ACCEPT">';
			}
			$out.='<td align="center"><textarea rows="2" cols="4" name="Description" value="" /></textarea></td>
			<td align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="100%" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="'.translate('TEXT_ADD_CONST').'" /> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'" /></td>
		</tr>';
		if (@$AdvancedFirewall == 1){
		$out.='<tr>
			<td colspan="100%" align="center" bgcolor="#EEEEEE">'.translate('TEXT_NAME_OF_NEW_CHAIN_CONST').': <input type="TEXT" name="New_Chain" /><input type="submit" class="button" name="add_Chain" value="'.translate('TEXT_ADD_CONST').'" onClick="add_Chain()" /></td>
		</tr>';
		}
		$out.='<tr>
			<td colspan="100%" align="left">* '.translate('TEXT_OPTIONAL_FIELD_CONST').'</td>
		</tr>';
		if ($fwVersion == "ipv6") {
			$out.='<tr>
				<td colspan="100%" align="left">** This field is '.translate('TEXT_OPTIONAL_FIELD_CONST').' only not with port_forward (NAT), input is then port 9000 on the core to go to port 9000 = 9000:9000</td>
			</tr>';
		}
	$out.='</table>	
	</form>
		<script>
		 	var frmvalidator = new formValidator("firewall");
// 			frmvalidator.addValidation("internalCoreIP_1","req","'.translate('TEXT_IP_REQUIRED_CONST').'");
		</script>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=firewall&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		if(isset($_POST['add'])){
			$IntIF=@$_POST['IntIf'];
			$ExtIF=@$_POST['ExtIf'];
			$Matchname=@$_POST['Matchname'];
			if (!isset($_POST['place'])) {
				$place='2';
			} else {
				$place=$_POST['place'];
			}
			if ($_POST['protocol'] == 'tcp & udp') {
				$Protocol='all-'.@$_POST['IPVersion'];
			} else {
				$Protocol=@$_POST['protocol'].'-'.@$_POST['IPVersion'];
			}
			$SourcePort=@$_POST['SourcePort'];
			$SourcePortEnd=@$_POST['SourcePortEnd'];
			$DestinationPort=isset($_POST['DestinationPort'])?$_POST['DestinationPort']:0;
			$DestinationIP=isset($_POST['DestinationIP'])?$_POST['DestinationIP']:0;
			$Chain=@$_POST['Chain'];
			if (@$AdvancedFirewall == 1){
				if (isset($_POST['RuleType']) && $_POST['RuleType'] != "" ) {
					$RuleType=@$_POST['RuleType'];
					$Chain.="-".$RuleType;
				}
			} else {
					if ($_POST['Chain'] == "port_forward (NAT)") {
						$RuleType="PREROUTING";
						$Chain.="-".$RuleType;
					}
			}
			$SourceIP=@$_POST['SourceIP'];
			$RPolicy=@$_POST['RPolicy'];
			$Description=@$_POST['Description'];
			if ( $RuleType != "PREROUTING"){ // check if Ruletype is prerouting if not input one rule.
				$insert='INSERT INTO Firewall (Place,IntIF,ExtIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)';
				$dbADO->Execute($insert,array($place,$IntIF,$ExtIF,$Matchname, $Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$Chain,$SourceIP,$RPolicy,$Description));
			} else {
					// Check for destinationip of it is the external interface ip then input or a system on the network(s) then forward.
					 if ($coreIPv4 == $DestinationIP) {
						//Insert the input rule.
						$input='input';
						$insert='INSERT INTO Firewall (Place,IntIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)';
						$dbADO->Execute($insert,array('1',$IntIF,$Matchname, $Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$input,$SourceIP,$RPolicy,$Description));
					} else {
						if ($_POST['Chain'] == "port_forward (NAT)") {
							$forward='forward';
						} else {
							$forward=$_POST['Chain'];
							$queryChain='SELECT * FROM Firewall WHERE RuleType=? AND RPolicy=?';
							$resChain=$dbADO->Execute($queryChain,array("forward",$forward));
							if($resChain->RecordCount() ==0){
								$insert='INSERT INTO Firewall (Place,Protocol, RuleType,RPolicy,Description) VALUES (?,?,?,?,?)';
								$dbADO->Execute($insert,array('0','all-'.$_POST['IPVersion'],'forward',$forward,'LINK TO '.$forward));
							}
						}
						//Insert the forward rule.
						$msg.=$forward;
						$insert='INSERT INTO Firewall (Place,IntIF,ExtIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)';
						$dbADO->Execute($insert,array('1',$IntIF,$ExtIF,$Matchname, $Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$forward,$SourceIP,$RPolicy,$Description));
					}
				//Insert the prerouting rule.
				$insert='INSERT INTO Firewall (Place,IntIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)';
				$dbADO->Execute($insert,array('1',$IntIF,$Matchname, $Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$Chain,$SourceIP,$RPolicy,$Description));
			}
		}
		
		if(isset($_POST['save_Rule'])){
			$rule_nr=$_POST['save_Rule'];
			$IntIF=@$_POST['save_IntIf'];
			$ExtIF=@$_POST['save_ExtIf'];
			$Matchname=@$_POST['save_Matchname'];
			if (!isset($_POST['save_place'])) {
				$place='2';
			} else {
				$place=$_POST['save_place'];
			}
			if ($_POST['save_protocol'] == 'tcp & udp') {
				$Protocol='all-'.@$_POST['save_IPVersion'];
			} else {
				$Protocol=@$_POST['save_protocol'].'-'.@$_POST['save_IPVersion'];
			}
			$SourcePort=@$_POST['save_SourcePort'];
			$SourcePortEnd=@$_POST['save_SourcePortEnd'];
			$DestinationPort=isset($_POST['save_DestinationPort'])?$_POST['save_DestinationPort']:0;
			$DestinationIP=isset($_POST['save_DestinationIP'])?$_POST['save_DestinationIP']:0;
			if (isset($_POST['save_RuleType']) && $_POST['save_RuleType'] != "") {
				$Chain=@$_POST['save_Chain'];
				$RuleType=@$_POST['save_RuleType'];
				$Chain=$Chain."-".$RuleType;
			} else {
				$Chain=@$_POST['save_Chain'];
			}
			$SourceIP=@$_POST['save_SourceIP'];
			$RPolicy=@$_POST['save_RPolicy'];
			$Description=@$_POST['save_Description'];
			$update="UPDATE Firewall SET Place='".$place."', IntIF='".$IntIF."' , ExtIF='".$ExtIF."' ,Matchname='".$Matchname."' , Protocol='".$Protocol."', SourcePort='".$SourcePort."', SourcePortEnd='".$SourcePortEnd."', DestinationPort='".$DestinationPort."', DestinationIP='".$DestinationIP."', RuleType='".$Chain."',SourceIP='".$SourceIP."',RPolicy='".$RPolicy."',Description='".$Description."' WHERE PK_Firewall='".$rule_nr."'";
			$dbADO->Execute($update);
		}
		
				
		if (isset($_POST['add_Chain'])) {
			$Chain=@$_POST['New_Chain'];
			$Protocol="chain-ipv4";
			$insert='INSERT INTO Firewall (Matchname, Protocol) VALUES (?,?)';
			$dbADO->Execute($insert,array($Chain, $Protocol));
		}
		
		if (isset($_POST['edit_chain'])) {
			$chain_name=$_POST['edit_chain'];
			$ID=$_POST['Chain_Id'];
			//Select the old name of the chain to rename the rules for this chain and the connection rules to this chain. 
			$res=$dbADO->execute('SELECT Matchname FROM Firewall WHERE PK_Firewall='.$ID);
			while($row=$res->FetchRow()){
				$update='UPDATE Firewall SET Matchname=? WHERE PK_Firewall=?';
				$dbADO->execute($update,array($chain_name,$ID));
				//Rename Connections
				$update='UPDATE Firewall SET RPolicy=? WHERE RPolicy=?';
				$dbADO->execute($update,array($chain_name,$row['Matchname']));
				//Rename the rules
				$update='UPDATE Firewall SET RuleType=? WHERE RuleType=?';
				$dbADO->execute($update,array($chain_name,$row['Matchname']));
			}
			
		}
		
		if (isset($_POST['del_chain'])) {
			$delete='DELETE FROM Firewall WHERE PK_Firewall=?';
			$dbADO->Execute($delete,array($_POST['Chain_Id']));
		}
		
		if(isset($_POST['inputPolicy'])){
			$RuleType="policy";
			$RuleType.=$fwVersion;
			$Protocol="input";
			$Matchname=@$_POST['inputPolicy'];

			$insert='UPDATE Firewall SET Matchname=? WHERE Protocol=? AND RuleType=?';
			$dbADO->Execute($insert,array($Matchname, $Protocol,$RuleType));
		}

		if(isset($_POST['forwardPolicy'])){
			$RuleType="policy";
			$RuleType.=$fwVersion;
			$Protocol="forward";
			$Matchname=@$_POST['forwardPolicy'];

			$insert='UPDATE Firewall SET Matchname=? WHERE Protocol=? AND RuleType=?';
			$dbADO->Execute($insert,array($Matchname, $Protocol,$RuleType));
		}

		if(isset($_POST['outputPolicy'])){
			$RuleType="policy";
			$RuleType.=$fwVersion;
			$Protocol="output";
			$Matchname=@$_POST['outputPolicy'];

			$insert='UPDATE Firewall SET Matchname=? WHERE Protocol=? AND RuleType=?';
			$dbADO->Execute($insert,array($Matchname, $Protocol,$RuleType));
		}

                if(isset($_REQUEST['enableRule'])){
			$enable=$_REQUEST['enableRule'];
			$insert='UPDATE Firewall SET Disabled=? WHERE PK_Firewall=?';
			$dbADO->Execute($insert,array('0', $enable));
                }

                if(isset($_REQUEST['disableRule'])){
			$disable=$_REQUEST['disableRule'];
			$insert='UPDATE Firewall SET Disabled=? WHERE PK_Firewall=?';
			$dbADO->Execute($insert,array('1', $disable));
                }

                if(isset($_REQUEST['moverule'])){
			$rule=$_REQUEST['moverule'];
                        $select='Select RuleType, Protocol FROM Firewall WHERE PK_Firewall=?';
                        $temp=$dbADO->Execute($select,array( $rule));
                        while ($row=$temp->FetchRow()) {
                                $chain=$row['RuleType'];
                                $protocol=$row['Protocol'];
                        }
                        $ipversion = substr($protocol, strpos($protocol, "-")+1);
                        $select='Select PK_Firewall FROM Firewall WHERE RuleType=? AND Protocol LIKE ?';
                        $rules=$dbADO->Execute($select,array( $chain, '%-'.$ipversion));
                        while ($row=$rules->FetchRow()) {
                                if ($next == 1) {
                                        $nextrule=$row['PK_Firewall'];
                                }
                                if ($next == 1) {
                                $next=0;
                                }
                                if ($row['PK_Firewall'] == $rule) {
                                $next=1;
                                }
                        }
			if ($nextrule != ''){
				$dbADO->Execute('UPDATE Firewall SET PK_Firewall=? WHERE PK_Firewall=?',array( '0', $rule));
				$dbADO->Execute('UPDATE Firewall SET PK_Firewall=? WHERE PK_Firewall=?',array( $rule, $nextrule));
				$dbADO->Execute('UPDATE Firewall SET PK_Firewall=? WHERE PK_Firewall=?',array( $nextrule, '0'));
			}
                } else {
			if(isset($_REQUEST['delid'])){
				$delid=$_REQUEST['delid'];
				$dbADO->Execute('DELETE FROM Firewall WHERE PK_Firewall=?',$delid);
			}else{
				if(isset($_POST['IDIOT'])){
					writeConf($accessFile,'AdvancedFirewall',@$AdvancedFirewall,0);
					writeConf($accessFile,'Show_all_rules',@$Show_all_rules,0);
					$dbADO->Execute('DELETE FROM Firewall WHERE Protocol LIKE ?',array( "%-".$fwVersion));
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType LIKE ?',array( "policy".$fwVersion));
				}else{
					if(@$_REQUEST['change_ipv4_firewall_status']==1){
						writeConf($accessFile, 'DisableIPv4Firewall',@$DisableIPv4Firewall,1);
					}else{
						writeConf($accessFile, 'DisableIPv4Firewall',@$DisableIPv4Firewall,0);
					}
					if(@$_REQUEST['change_ipv6_firewall_status']==1){
						writeConf($accessFile, 'DisableIPv6Firewall',@$DisableIPv6Firewall,1);
					}else{
						writeConf($accessFile, 'DisableIPv6Firewall',@$DisableIPv6Firewall,0);				}
					if(@$_REQUEST['advanced_firewall']==1){
						writeConf($accessFile, 'AdvancedFirewall',@$AdvancedFirewall,1);
					}else{
						writeConf($accessFile, 'AdvancedFirewall',@$AdvancedFirewall,0);
					}
					if(@$_REQUEST['show_all_rules']==1){
						writeConf($accessFile, 'Show_all_rules',@$Show_all_rules,1);
					}else{
						writeConf($accessFile, 'Show_all_rules',@$Show_all_rules,0);
					}
					if(@$_REQUEST['fw_blocklist']==1){
						writeConf($accessFile, 'fw_blocklist',@$fw_blocklist,1);
					}else{
						writeConf($accessFile, 'fw_blocklist',@$fw_blocklist,0);
					}
					if(@$_REQUEST['fwVersion']=="ipv4"){
						writeConf($accessFile, 'fwVersion',@$fwVersion,'ipv4');
						}else{
						writeConf($accessFile, 'fwVersion',@$fwVersion,'ipv6');
					}
					
				}
			}
		}
		
			exec_batch_command('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
			header("Location: index.php?section=firewall&msg=".translate('TEXT_FIREWALL_RULES_UPDATED_CONST'));
	}

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_FIREWALL_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_FIREWALL_RULES_CONST')=>'index.php?section=firewall'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_FIREWALL_RULES_CONST'));
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=firewall&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('firewall.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
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
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=firewall&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>

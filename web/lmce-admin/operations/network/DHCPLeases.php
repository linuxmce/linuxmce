<?php
function DHCPLeases($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('dhcpleases.lang.php');

	include(APPROOT.'/include/dhcpd-tools/DHCPd-parse.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
//	$dbADO->debug=true;
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
	$accessFile = "/var/lib/dhcp/dhcpd.leases";
	$test = new Leases;
if (!isset($_GET['error']) && !$test->readLease($accessFile)) {
  header("Location: index.php?section=DHCPLeases&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." $accessFile");
	exit();
}
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<table cellpadding="3" cellspacing="0">
		<tr class="tablehead">
		<TD>#</TD>
			<td><B>'.translate('TEXT_IP_ADDRESS_CONST').'</B></td>
			<td><B>'.translate('TEXT_MAC_ADDRESS_CONST').'</B></td>
			<td><B>'.translate('TEXT_EXPIRES_CONST').'</B></td>
			<td><B>'.translate('TEXT_STATUS_CONST').'</B></td>
			<td><B>'.translate('TEXT_HOSTNAME_CONST').'</B></td>
			<td><B>'.translate('TEXT_DESCRIPTION_CONST').'</B></td>
			<td><B>'.translate('TEXT_DEVICE_CONST').'</B></td>
		</tr>';
		$pos=0;
		while($lease = $test->nextLease()){
		$class=($pos%2==0)?'alternate_back':'';
			$out.='
				<tr class="'.$class.'">
				<td>'.$pos.'</td>
					<td>'.$lease["ip_addr"].'</td>
					<td>'.$lease["mac"].'</td>
					<td>'.date("j-M-Y H:i", (int)$lease["ends"]).'</td>
					<td>'.$lease["status"].'</td>
					<td>'.$lease["hostname"].'</td><td></TD>';
				$res=$dbADO->Execute("SELECT * FROM Device where IPaddress='$lease[ip_addr]'");
				if($row=$res->FetchRow()){
				  $out .= "<td>$row[Description]</td>";
				  $out .= "<td><a href=index.php?section=editDeviceParams&deviceID=$row[PK_Device]>$row[PK_Device]</a></TD>";
				} else {
          $out .= "<td></td>";
        }
				$out .= '</tr>';
				$pos++;
		}
		$accessFile="/etc/dhcp/dhcpd.conf";
    $FileArray=@file($accessFile);	
    $File=implode('',$FileArray);
    preg_match_all('/host (\S+) { hardware ethernet (\S+); fixed-address (\S+);/',$File,$matches);
    for ($i=0;$i<=sizeof($matches);$i++) {
      $class=($pos%2==0)?'alternate_back':'';
      $out .= "<tr class='$class'>
        <td>$pos</td>
        <td>".$matches[3][$i]."</TD>
        <td>".$matches[2][$i]."</TD>
        <td align=center>--</TD>
        <td>static</TD>
        <td>".$matches[1][$i]."</TD>";
        $res=$dbADO->Execute("SELECT * FROM Device where IPaddress='".$matches[3][$i]."'");
				if($row=$res->FetchRow()){
				  $out .= "<td>$row[Description]</td>";
				  $out .= "<td><a href=index.php?section=editDeviceParams&deviceID=$row[PK_Device]>$row[PK_Device]</a></TD>";
				} else {
          $out .= "<td></td>";
        }
      $out .= "</TR>";
      $pos++;
    }
		$out.='
	</table>	
	';
	
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_DHCP_LEASES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_DHCP_LEASES_CONST')=>'index.php?section=DHCPLeases'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_DHCP_LEASES_CONST'));
	$output->output();
}

?>

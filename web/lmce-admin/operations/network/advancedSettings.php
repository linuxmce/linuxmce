<?php
function ipv6_save($connection) {
  	// tokenize values to put in CORE device data
  	$token=$_REQUEST['ipv6_tunnelbroker'].",".$_REQUEST['ipv6_tunnelID'].",".$_REQUEST['ipv6_endpoint']
      .",".$_REQUEST['ipv6_localaddr'].",".$_REQUEST['ipv6_localaddrNetmask']
      .",".$_REQUEST['ipv6_localnet'].",".$_REQUEST['ipv6_localnetNetmask']
      .",".$_REQUEST['ipv6_userid'].",".$_REQUEST['ipv6_password']
      .",".$_REQUEST['ipv6_active'].",".$_REQUEST['ipv6_dynamicIPv4']
      .",".$_REQUEST['ipv6_RAenabled'];
 	mysqli_query($connection, "UPDATE Device_DeviceData SET IK_DeviceData='".$token."' WHERE FK_Device=1 AND FK_DeviceData=292") 
 		or die('ERROR: Invalid query: '.mysqli_error($connection));
	$_REQUEST['msg']="IPv6 Settings saved";

	// disable firewall settings for now, will be next step
	//$commands = array('Network_Setup.sh', 'Network_Firewall.sh');
	$commands = array('Network_Setup.sh');
	for ($i = 0; $i < count($commands); $i++) {
		$cmd = "sudo -u root /usr/pluto/bin/{$commands[$i]}";
		exec_batch_command($cmd);
	}
}

function advancedSettings($output, $dbADO) {
	// Include language files
 	includeLangFile('common.lang.php');
	includeLangFile('advancedSettings.lang.php');

	// DB connection
	$mysqlhost="localhost";
	$mysqluser="root";
	$mysqlpwd="";
	$mysqllmcedb="pluto_main";

	// Init var's
  	$action = @$_REQUEST['action'];
	
	// Connect to database and switch to main db
	$connection=mysqli_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to database server!");
	mysqli_select_db($connection, $mysqllmcedb) or die("ERROR: could not select LinuxMCE main database!");

  	if($action == 'ipv6_save') {
    	ipv6_save($connection);
    }
	
	// TODO: Update ipv6 endpoint: https://ipv4.tunnelbroker.net/ipv4_end.php?ipv4b=AUTO&pass=md5($pass)&user_id=$userID9&tunnel_id=$tunnelID
	
	// Query current IPv6 tunnel settings
	$ipv6_query = mysqli_query($connection, "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292") 
      or die('ERROR: Invalid query: ' . mysqli_error($ipv6_query));
	$ipv6_data=mysqli_fetch_row($ipv6_query);
	$ipv6_data=explode(",", $ipv6_data[0]);
	
	$ipv6_tunnelBroker = $ipv6_data[0];
	$ipv6_tunnelID = $ipv6_data[1];
	$ipv6_endpoint = $ipv6_data[2];
	$ipv6_localaddr = $ipv6_data[3];
	$ipv6_localaddrNetmask = $ipv6_data[4];
	$ipv6_localnet = $ipv6_data[5];
	$ipv6_localnetNetmask = $ipv6_data[6];
	$ipv6_userID = $ipv6_data[7];
	$ipv6_password = $ipv6_data[8];
	$ipv6_active = $ipv6_data[9];
	$ipv6_dynamicIPv4 = $ipv6_data[10];
	$ipv6_RAenabled = $ipv6_data[11];

	$IPv6TunnelProviders=array(
        "- ".translate('TEXT_NONE_CONST')." -"                => "",
        "HE IPv6 Tunnel Broker"   => "he",
		"SIXXS IPv6 Tunnel Broker"   => "SIXXS"
  );

	$out='';
		foreach ($IPv6TunnelProviders as $providerName => $providerID) {
			$selected=$ipv6_tunnelBroker==$providerID?'selected':'';
			$IPv6Provider_options .= "<option value='$providerID' $selected >$providerName</option>";
		}
		//echo "Provider:".$providerName,"<br />";
		
		$out='
		<form action="index.php" method="POST" name="advancedSettings">
			<input type="hidden" name="section" value="advancedSettings">
			<input type="hidden" name="action" value="ipv6_save">
			<input type="hidden" name="ipv6_RAenabled" value="'.$ipv6_RAenabled.'">
			<div class="confirm"><B>'.(isset($_REQUEST['msg'])?strip_tags($_REQUEST['msg']):'').'</B></div>
			<table border="0">
				<tr><td colspan="2" class="tablehead"><b>'.translate('TEXT_IPV6TUNNEL_SETTINGS_CONST').'</b></td></tr>
				
				<tr><td width="150"><b>'.translate('TEXT_IPV6TUNNEL_PROVIDER_CONST').'</b></td><td>
					<select name="ipv6_tunnelbroker" STYLE="width:250px">'.$IPv6Provider_options.'</select></td></tr>
				
				<tr><td><b>'.translate('TEXT_IPv6TUNNEL_ENDPOINT_CONST').'</b></td>
					<td><input type="text" name="ipv6_endpoint" value="'.$ipv6_endpoint.'" STYLE="width:250px"></td></tr>
				
				<tr><td><b>'.translate('TEXT_IPV6TUNNEL_LOCAL_IPV6_IP_CONST').'</b></td>
					<td><input type="text" name="ipv6_localaddr" value="'.$ipv6_localaddr.'" STYLE="width:215px">
					/ <input type="text" name="ipv6_localaddrNetmask" value="'.$ipv6_localaddrNetmask.'" STYLE="width:25px"></td></tr>
				
				<tr><td><b>'.translate('TEXT_IPV6TUNNEL_LOCAL_IPV6_NET_CONST').'</b></td>
					<td><input type="text" name="ipv6_localnet" value="'.$ipv6_localnet.'" STYLE="width:215px">
					 / <input type="text" name="ipv6_localnetNetmask" value="'.$ipv6_localnetNetmask.'" STYLE="width:25px"></td></tr>
				
				<tr><td><b>'.translate('TEXT_IPV6TUNNEL_USERID_CONST').'</b></td>
					<td><input type="text" name="ipv6_userid" value="'.$ipv6_userID.'" STYLE="width:250px"></td></tr>
				
				<tr><td><b>'.translate('TEXT_PASSWORD_CONST').'</b></td>
					<td><input type="password" name="ipv6_password" value="'.$ipv6_password.'"STYLE="width:250px"></td></tr>
					
				<tr><td><b>'.translate('TEXT_IPV6TUNNEL_ID_CONST').'</b></td>
					<td><input type="text" name="ipv6_tunnelID" value="'.$ipv6_tunnelID.'" STYLE="width:250px"></td></tr>
				
				<tr><td><b>&nbsp;</b></td>
            	<td><input type="checkbox" name="ipv6_active" '.($ipv6_active=="on"?'checked':'').'>' 
						.translate('TEXT_IPV6TUNNEL_ACTIVATE_CONST').'
						<input type="checkbox" name="ipv6_dynamicIPv4" '.($ipv6_dynamicIPv4=="on"?'checked':'').'> 
						<font color="FF0000"><b>*</b></font>'.translate('TEXT_IPV6TUNNEL_DYNAMICIPV4_CONST').'</td></tr>
				
				<tr><td>&nbsp;</td><td><input type="submit" class="button" name="Save" value="'.translate('TEXT_SAVE_CONST').'"></td></tr>
				<tr><td colspan="2">&nbsp;</td></tr>
				<tr><td colspan="2">* '.translate('TEXT_IPV6TUNNEL_EXPLAINDYNAMIC_CONST').'</td></tr>
			</table>
		</form>
		';


	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_ADVANCED_SETTINGS_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_ADVANCED_SETTINGS_CONST')=>'index.php?section=advancedSettings'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_ADVANCED_SETTINGS_CONST'));
	$output->output();
								
}	
?>

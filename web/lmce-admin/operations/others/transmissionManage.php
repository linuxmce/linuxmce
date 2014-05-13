<?php
function transmissionManage() {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userChangePassword.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');

	$update = (isset($_REQUEST['updateTransmission']) && $_REQUEST['updateTransmission']!='')?cleanString($_REQUEST['updateTransmission']):'';
	$rpc_Auth_Req = (isset($_REQUEST['rpc-authentication-required']) && $_REQUEST['rpc-authentication-required']!='')?cleanString($_REQUEST['rpc-authentication-required']):'';
	$rpc_Username = (isset($_REQUEST['rpc-username']) && $_REQUEST['rpc-username']!='')?cleanString($_REQUEST['rpc-username']):'';
	$rpc_Password = (isset($_REQUEST['rpc-password']) && $_REQUEST['rpc-password']!='')?cleanString($_REQUEST['rpc-password']):'';

	exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getSettings rpc-username', $username);
	$username=$username[0];

	exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getSettings rpc-authentication-required', $authReq);
	$authReq=$authReq[0];

	$operation = (isset($_REQUEST['operation']) && $_REQUEST['operation']!='')?cleanString($_REQUEST['operation']):'';
	if ($operation!=''){
		exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission '.$operation);
	};

	exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission status', $status);
	$status=$status[0];

	if ($status==''){
		//Transmission-daemon is not running
		$TransStatus=$TEXT_TRANMSISSIONSTOPPED;
		$start='class="button"';
		$stop='disabled';
	}else{
		$TransStatus=$TEXT_TRANMSISSIONRUNNING;
		$start='disabled';
		$stop='class="button"';
	};

	if ($update!=''){
		//Stop the servce BEFORE updating settings! Transmission updates the settings file on stop.
		$change_pass=false;

		if ($status!=''){
			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission stop');
		};

		//Now update the settings file with the changes
		if ( $rpc_Auth_Req=='on' ){
			if ($authReq=='false'){
				//Was previously switched off
				exec('sudo -u root /usr/pluto/bin/transmission-settings.sh changeSetting "rpc-authentication-required" "true"');
				$authReq='true';
			};

			if ( $rpc_Username!=$username && $rpc_Username!='' ){
				//Change the Userx name
				exec('sudo -u root /usr/pluto/bin/transmission-settings.sh changeSetting "rpc-username" "'.$rpc_Username.'"');
			};

			if ( $rpc_Password!='' ){
				//Change the password
				exec('sudo -u root /usr/pluto/bin/transmission-settings.sh changeSetting "rpc-password" "'.$rpc_Password.'"');
				$change_pass=true;
			};
		}else{
			//No Authentication required, so no need to change username or password
			if ($authReq=='true'){
				exec('sudo -u root /usr/pluto/bin/transmission-settings.sh changeSetting "rpc-authentication-required" "false"');
				$authReq='false';
			};
		};

		//Finally, start the daemon with the new settings in place
		if ($change_pass){
			//Changing the password causes the un-encrypted plain text password to be displayed in the settings file, start and stop the service to force the encription
			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission start');
			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission stop');
		};

		if ($status!=''){
			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh startstoptransmission start');
		};
	};

	if ($authReq=='true'){
		$checked='checked';
		$setauth='';
	}else{
		$checked='';
		$setauth='disabled';
	};

	$out='
		<script>
			function changeAuthState(){
				if (document.getElementById("rpc-authentication-required").checked==true){
					document.getElementById("rpc-username").disabled=false;
					document.getElementById("rpc-password").disabled=false;
					document.getElementById("rpc-password-repeat").disabled=false;
				}else{
					document.getElementById("rpc-username").disabled=true;
					document.getElementById("rpc-password").disabled=true;
					document.getElementById("rpc-password-repeat").disabled=true;
				};
			}

			function checkform(){
				if (document.getElementById("rpc-authentication-required").checked==true){
					if ( document.getElementById("rpc-password").value==document.getElementById("rpc-password-repeat").value && document.getElementById("rpc-username").value!=""){
						document.getElementById("updateTransmission").value="update";
						document.transmission.submit();
					}else{
						if ( document.getElementById("rpc-password").value!=document.getElementById("rpc-password-repeat").value){
							alert("Passwords don\'t match");
						};

						if ( document.getElementById("rpc-username").value==""){
							alert("Transmission username MUST have a value!");
						};
					}
				}else{
					document.getElementById("updateTransmission").value="update";
					document.transmission.submit();
				};
			}
		</script>

		<input type="hidden" name="operation" id="operation" value="">
		<input type="hidden" name="updateTransmission" id="updateTransmission" value="">
		<center>'.$TEXT_MANAGETRANSMISSION.'!</center><br>
		<br>
		<table>
		<tbody>
		<tr><td class="tablehead">'.$TEXT_TRANMSISSIONSTATUS.'</td><td>'.$TransStatus.'</td></tr>
		<tr><td colspan=2><input type="submit" name="start" id="start" value="'.$TEXT_TRANSMISSIONSTART.'" '.$start.' onclick="document.getElementById(\'operation\').value=\'start\';"></td></tr>
		<tr><td colspan=2><input type="submit" name="stop" id="stop" value="'.$TEXT_TRANSMISSIONSTOP.'" '.$stop.' onclick="document.getElementById(\'operation\').value=\'stop\';"></td></tr>
		<tr><td colspan=2><input type="submit" name="restart" id="restart" value="'.$TEXT_TRANSMISSIONRESTART.'" '.$stop.' onclick="document.getElementById(\'operation\').value=\'restart\';"></td></tr>
		</tbody>
		<thead>
		<tr><td class="tablehead" colspan=2>'.$TEXT_TRANSMISSIONSERVICEACTION.'</td></tr>
		</thead>
		</table>
		<br>
		<table>
		<tbody>
		<tr><td class="tablehead">'.$TEXT_TRANMSISSIONLOGIN.'</td><td colspan=2><input type="text" name="rpc-username" id="rpc-username" value="'.$username.'" '.$setauth.'></td></tr>
		<tr><td class="tablehead">'.$TEXT_PASSWORD_CONST.'</td><td><input type="password" name="rpc-password" id="rpc-password" value="" '.$setauth.'></td><td>'.$TEXT_PASSWORD_BLANK.'<td></tr>
		<tr><td class="tablehead">'.$TEXT_USER_CONFIRM_PASSWORD_CONST.'</td><td colspan=2><input type="password" name="rpc-password-repeat" id="rpc-password-repeat" value="" '.$setauth.'></td></tr>
		</tbody>
		<thead>
		<tr><td class="tablehead" colspan=2>'.$TEXT_TRANMSISSIONAUTHREQUIRED.'</td><td class="tablehead"><input type="checkbox" name="rpc-authentication-required" id="rpc-authentication-required" '.$checked.' onchange="changeAuthState()"></td></tr>
		</thead>
		<tfoot>
		<tr><td colspan=3><input type="button" class="button" name="submit_Change" value="'.$TEXT_SAVE_CONST.'" onclick="checkform()"></td></tr>
		</tfoot>
		</table>
	';

	return $out;
}
?>

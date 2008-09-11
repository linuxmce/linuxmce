<?
function is_valid_user($userID,$passMD5,$dbADO){
	if($userID==0 || $passMD5==''){
		return false;
	}
	$res=$dbADO->Execute('SELECT * FROM Users WHERE PK_Users=? AND Password=?',array($userID,$passMD5));
	if($res->RecordCount()>0){
		return true;
	}
	return false;
}

function user_exists($username,$pass,$dbADO){
	if($username==''){
		return false;
	}
	$res=$dbADO->Execute('SELECT * FROM Users WHERE username=? AND Password=?',array($username,md5($pass)));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['PK_Users'];
	}
	return false;
	
}

function login_form($dbADO,$err=0){
	$error_message=($err>0)?'Login failed':'';
	$out='
	<form name="login_form" action="process_web_orbiter.php" method="POST">
	'.$error_message.'
	<table>
		<tr>
			<td>User</td>
			<td><input type="text" name="username" value=""></td>
		</tr>
		<tr>
			<td>Password</td>
			<td><input type="password" name="password" value=""></td>
		</tr>
		<tr>
			<td colspan="2"><input type="submit" name="login" value="Login"></td>
		</tr>
	</table>
	</form>
	';
	
	return $out;
}

function get_web_orbiter($installationID,$deviceID,$userID,$pass,$dbADO){
	if($deviceID==0){
		$out='
		<form name="worbiter" method="GET" action="">
			<input type="hidden" name="userID" value="'.$userID.'">
			<input type="hidden" name="pass" value="'.$pass.'">
		<table>
			<tr>
				<td>Installation: </td>
				<td>'.get_installation($userID,$dbADO,$installationID).'</td>
			</tr>
			<tr>
				<td>Device: </td>
				<td>'.get_devices($installationID,$userID,$dbADO,$deviceID).'</td>
			</tr>
			<tr>
				<td><input type="submit" name="submit" value="Submit"></td>
			</tr>		
		</table>
		</form>';
		return $out;
	}else{
		$out=get_web_orbiter_screen($deviceID,$dbADO);
	}
	
	return $out;
}

// return form element with installation(s): hidden field or pulldown
function get_installation($userID,$dbADO,$installationID=0){
	$instalationsArray=getAssocArray('Installation','PK_Installation','CONCAT(\'#\',PK_Installation,\' \',Description)',$dbADO,'INNER JOIN Installation_Users ON FK_Installation=PK_Installation WHERE FK_Users='.$userID);
	$no=count($instalationsArray);
	if($no==0){
		return 'ERROR: this user does not have any installation.'; 
	}
	if($no==1){
		$installationIDs=array_keys($instalationsArray);
		$instalation_form_element='# '.$installationIDs[0].'<input type="hidden" name="installationID" value="'.$installationIDs[0].'">';
		$GLOBALS['installationID']=$installationIDs[0];
	}else{
		$instalation_form_element=pulldownFromArray($instalationsArray,'installationID',$installationID,'onChange="document.worbiter.submit();"');
	}
	return $instalation_form_element;
}

// return weborbiter devices in pulldown
function get_devices($installationID,$userID,$dbADO,$deviceID=0){
	$installationID=($installationID==0)?$GLOBALS['installationID']:$installationID;
	$devicesArray=getAssocArray('Device','PK_Device','Description',$dbADO,'WHERE FK_DeviceTemplate='.$GLOBALS['GenericWebDevice'].' AND FK_Installation='.$installationID.' ORDER BY Description ASC');
	if(count($devicesArray)==0){
		return 'You don\'t have any generic web device in current installation.';
	}
	$out=pulldownFromArray($devicesArray,'deviceID',$deviceID,'onChange="document.worbiter.submit();"');
	
	return $out;
}

// retrieve device data for a device in a multidimensional array
function get_device_data($deviceID,$dbADO){
	$ddArray=array();
	if($deviceID==0){
		return $ddArray;
	}
	$res=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=?',$deviceID);
	while($row=$res->FetchRow()){
		$ddArray[$row['FK_DeviceData']]=$row['IK_DeviceData'];
	}
	
	return $ddArray;
}

// main function who display screen and handle buttons
function get_web_orbiter_screen($deviceID,$dbADO){
	
	$out='';
	
	// get proxy orbiter IP address 
	$ProxyOrbiterInfo=getFieldsAsArray('Device','Device.FK_Device_ControlledVia,DD.IK_DeviceData AS Port,Parent.IPAddress',$dbADO,'INNER JOIN Device_DeviceData DD ON DD.FK_Device=Device.PK_Device AND DD.FK_DeviceData='.$GLOBALS['ListenPort'].' LEFT JOIN Device Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device WHERE Device.FK_DeviceTemplate='.$GLOBALS['ProxyOrbiter'].' AND Parent.PK_Device IS NOT NULL AND Device.FK_Device_ControlledVia='.$deviceID);

	if(count($ProxyOrbiterInfo)!=0){
		$address=$ProxyOrbiterInfo['IPAddress'][0];
		if($address=='' || is_null($address)){
			$address=getDeviceIP($ProxyOrbiterInfo['FK_Device_ControlledVia'][0],$dbADO);

			if(is_null($address)){
				$address=getCoreIP($dbADO);
				write_log("\n\nGeneric Web Device does not have IP address\n");
			}
		}
		$port=$ProxyOrbiterInfo['Port'][0];
	}else{
		write_log("\n\nOrbiter proxy not found as child of Generic Web Device #$deviceID\n");
		return 'Orbiter proxy not found.';
	}

	// attempt to create socket
	$socket=web_socket_create($address,$port, AF_INET, SOCK_STREAM, SOL_TCP);
	if($socket===false){
		return socket_failed(1);
	}
	
	// connect on socket to proxy orbiter
	$result=web_proxy_connect($socket, $address, $port);
	if($result===false){
		web_socket_close($socket);
		return socket_failed(1);
	}
	
	write_log("Received parameters ".$_SERVER['QUERY_STRING']."\n");
	$command=(isset($_REQUEST['command']))?$_REQUEST['command']:'PLUTO_KEY 10';
	
	// get image
	if($command=='IMAGE'){
		$out=getImage($deviceID,$socket,$refresh);
	}
	
	// send touch command
	if(strpos($command,'TOUCH')!==false){
		$out=sendCommand($deviceID,$socket,$command,$refresh);
	}	

	// send PLUTO KEY command
	if(strpos($command,'PLUTO_KEY')!==false){
		$out=sendCommand($deviceID,$socket,$command,$refresh);
	}
	
	web_socket_close($socket);
	
	return $out;
}



// old proxyOrbiter functions
function getImage($deviceID,$socket,$refresh=''){
	$in = "IMAGE ".rand(10000,11000)."\n";
	$out='Trying to connect ...';

	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);

	$imageSize=(int)trim(str_replace('IMAGE ','',$outResponse));
	if($imageSize>0){
		$remaining=$imageSize;
		$outImage='';
		while($remaining>0){
			$linesize=($remaining<2048)?$remaining:2048;
			$chunk=@socket_read($socket, $linesize,PHP_BINARY_READ);
			$outImage.= $chunk;
			$remaining-=strlen($chunk);
			write_log("Retrieved image chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($outImage)){
			$isSaved=writeFile(getcwd().'/security_images/'.$deviceID.'_web.png',$outImage);
			write_log("Received image size $imageSize \n");
		}else{
			$out=socket_failed(1);
		}
	}
	if(file_exists(getcwd().'/security_images/'.$deviceID.'_web.png')){
		// header image=1 just return the png
		if(isset($GLOBALS['headerImage']) && $GLOBALS['headerImage']==1){
			$filepath=getcwd().'/security_images/'.$deviceID.'_web.png';
			
			header( "Content-type: image/png" );
			readfile( $filepath );
			exit();
		}
		$out=jsFunctions().'<img id="screen" src="include/image.php?imagepath='.getcwd().'/security_images/'.$deviceID.'_web.png&randno='.rand(10000,99999).'" onClick="sendTouch();"><br>'.buttons_bar();	
	}
	
	return $out;
}

function getXML($socket){
	$in = "XML ".rand(10000,11000)."\n";
	$out='';
	
	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);

	$xmlSize=(int)trim(str_replace('XML ','',$outResponse));
	if($xmlSize>0){
		$remaining=$xmlSize;
		$outXML='';
		while($remaining>0){
			$linesize=($remaining<2048)?$remaining:2048;
			$chunk=@socket_read($socket, $linesize,PHP_BINARY_READ);
			$outXML.= $chunk;
			$remaining-=strlen($chunk);
			write_log("Retrieved xml chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($outXML)){
			write_log("Received xml size $xmlSize \n");
		}
	}
	$out.=@$outXML;
	
	
	return $out;
}



function sendCommand($deviceID,$socket,$command,$refresh){
	$in = $command."\n";

	$out='';

	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);
	$out=getImage($deviceID,$socket,$refresh);
	
	return $out;
}

function write_log($log){
	$deviceID=(int)@$_REQUEST['deviceID'];
	writeFile('/var/log/pluto/'.$deviceID.'_web_orbiter.log',$log,'a+');
}

function getDeviceIP($deviceID,$dbADO){
	$arr=getFieldsAsArray('Device','IPAddress,FK_Device_ControlledVia',$dbADO,'WHERE PK_Device='.$deviceID);

	if($arr['IPAddress'][0]!=''){
		return $arr['IPAddress'][0];
	}

	if(is_null($arr['FK_Device_ControlledVia'][0])){
		return NULL;
	}
	
	return getDeviceIP($arr['FK_Device_ControlledVia'][0],$dbADO);
}

// socket functions
// connect to socket, write to log and return handler
function web_socket_create($address,$port,$domain,$type,$protocol){

	write_log("\n".date('d-m-Y H:i:s')."\nAttempting to create socket on host $address port $port ... ");
	$socket = socket_create($domain, $type, $protocol);
	if ($socket === false) {
		write_log("socket_create() failed: reason: " . socket_strerror($socket) . "\n");
	} else {
		write_log("OK.\n");
	}
	
	return $socket;
}

// function try to connect on socket to proxy orbiter
function web_proxy_connect($socket, $address, $port){
	$result = @socket_connect($socket, $address, $port);

	if ($result !==true) {
		write_log("socket_connect() failed.\nReason: (".socket_last_error().") " . socket_strerror(socket_last_error()) . "\n");
	} else {
		write_log("Connected on socket ... OK.\n");
	}
	
	return $result;
}

// close socket and log action
function web_socket_close($socket){
	
	write_log("Closing socket ... ");
	@socket_close($socket);
	write_log( "OK.\n");
}

// write to socket and log action
function web_socket_write($socket, $in){
	write_log("Sending command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log("Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}
	return $written;
}

// read from socket and log action
function web_socket_read($socket, $length,$type=PHP_BINARY_READ){
	$outResponse= @socket_read($socket, $length,$type);
	if($outResponse===false){
		write_log("Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n");
	}else{
		write_log("Read: ".$outResponse."\n");
	}
	
	return $outResponse;
}


function socket_failed($flag){
	switch($flag){
		case 1:
			$out='Connect to proxy orbiter failed, please <a href="javascript:self.location.reload()">try again</a>';
		break;
	}

	return $out;
}

function jsFunctions(){
	$url=clean_path();	
	
	$out='
		<script src="scripts/connectionWizard/connectionWizard.js" type="text/javascript" language="JavaScript"></script>
		<script>
		function findPosX(obj)
		{
			var curleft = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curleft += obj.offsetLeft
					obj = obj.offsetParent;
				}
			}
			else if (obj.x)
				curleft += obj.x;
			return curleft;
		}
		
		function findPosY(obj)
		{
			var curtop = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curtop += obj.offsetTop
					obj = obj.offsetParent;
				}
			}
			else if (obj.y)
				curtop += obj.y;
			return curtop;
		}
				
		function sendTouch()
		{
			if(touch_disabled==0){
				xRelative=xMousePos-findPosX(document.getElementById("screen"));
				yRelative=yMousePos-findPosY(document.getElementById("screen"));

				self.location="'.$url.'&command=TOUCH "+xRelative+"x"+yRelative;
			}
		}

		</script>';
	return $out;
}

function buttons_bar(){
	$url=clean_path();
	
	$out='

	<input type="button" class="button" name="go" value="Home" onClick="self.location=\''.$url.'&command=PLUTO_KEY 10\';"> 
	<input type="button" class="button" name="go" value="Back" onClick="self.location=\''.$url.'&command=PLUTO_KEY 11\';">
	<input type="button" class="button" name="go" value="Refresh" onClick="self.location=\''.$url.'&command=IMAGE\';">
	<input type="button" class="button" name="go" value="Exit" onClick="self.close();">
	';
	
	return $out;
}

function clean_path(){
	$path=parse_str($_SERVER['QUERY_STRING']);
	$url="weborbiter.php?userID=".@$userID."&pass=".@$pass."&installationID=".@$installationID."&deviceID=".@$deviceID;
	
	return $url;
}

function keyboard_codes(){
	$keys=array();
	for($i=48;$i<58;$i++){
		$keys[chr($i)]=$i;
	}
	
	for($i=97;$i<123;$i++){
		$keys[chr($i)]=$i;
	}
	
	return $keys;
}

function get_keyboard_codes_id($dbADO){
	$key_codes_ids=array();

	$res=$dbADO->Execute('SELECT PK_Button, Description,ASCII(Description) AS ascii FROM Button WHERE (((ASCII(Description)>=48 AND ASCII(Description)<58) || (ASCII(Description)>=97 AND ASCII(Description)<123)) AND CHAR_LENGTH(Description)=1) || (PK_Button IN (1,2,3,4,5))');
	while($row=$res->FetchRow()){
		$key_codes_ids[$row['ascii']]=$row['PK_Button'];
	}

	return $key_codes_ids;
}

function output_html($content,$dbADO,$refresh=0){
	$url=clean_path();
	
	$refresh_tag=($refresh!=0)?'<META HTTP-EQUIV=Refresh CONTENT="'.$refresh.'; URL='.$url.'&command=IMAGE">':'';
	$keyboard_codes=get_keyboard_codes_id($dbADO);
	$js_case='';
	
	// hard-coded: add up/down/left/right/enter commands
	$keyboard_codes[38]=1;	// up
	$keyboard_codes[40]=2;	// down
	$keyboard_codes[37]=3;	// left
	$keyboard_codes[39]=4;	// right
	$keyboard_codes[13]=5;	// enter
	
	foreach ($keyboard_codes AS $code=>$command_key){
		$js_case.='
			case '.$code.':
				command_to_send='.$command_key.';
			break;
		';
	}
	
	$out='
	<html>
	<head>
	'.$refresh_tag.'
	<script>
	touch_disabled=1;
	
	function sendKey(event){
		if (event.which){
			key_code=event.which;
		}else if(event.keyCode){
			key_code=event.keyCode;
		}

	';
	if($refresh!=0){
	$out.='
		if(key_code>58){
			key_code+=32;
		}
		switch(key_code){
			'.$js_case.'
			default:
				command_to_send=0;
			break
		}
		if(command_to_send!=0){
			self.location="'.$url.'&command=PLUTO_KEY "+command_to_send;
		}';
	}
	$out.='
	}	
	</script>
	
	<title><-mkr_t_name_mixed-> Web device</title>		
	<head>		
	<body onLoad="touch_disabled=0;" ONKEYDOWN="sendKey(event);">
	'.$content.'
	</body>
	</html>';
	
	print $out;
}
?>
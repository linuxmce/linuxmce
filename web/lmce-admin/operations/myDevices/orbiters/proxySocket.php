<?
function proxySocket($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/proxySocket.lang.php');
	
	// debug stuff
	//Header("Content-type: text/xml"); 
	//die(join("\n",file(getcwd().'/security_images/37_screen.xml')));

	
	global $port,$address;
	$GLOBALS['coreIP']=getCoreIP($dbADO);
	if(!isset($_REQUEST['address']) && !isset($_REQUEST['port'])){
		$phoneIPAddress=$_SERVER['REMOTE_ADDR'];
		$ProxyOrbiterInfo=getFieldsAsArray('Device','Device.PK_Device AS DeviceID, FK_Device_ControlledVia,IPAddress,DD1.IK_DeviceData AS PhoneIP,DD2.IK_DeviceData AS Port',$dbADO,'INNER JOIN Device_DeviceData DD1 ON DD1.FK_Device=PK_Device AND DD1.FK_DeviceData='.$GLOBALS['RemotePhoneIP'].' INNER JOIN Device_DeviceData DD2 ON DD2.FK_Device=PK_Device AND DD2.FK_DeviceData='.$GLOBALS['ListenPort'].' WHERE DD1.IK_DeviceData=\''.$phoneIPAddress.'\' AND FK_DeviceTemplate='.$GLOBALS['Cisco7970Orbiter']);
		if(count($ProxyOrbiterInfo)!=0){
			$address=$ProxyOrbiterInfo['IPAddress'][0];
			if($address=='' || is_null($address)){
				//$address=getDeviceIP($ProxyOrbiterInfo['FK_Device_ControlledVia'][0],$dbADO);
				$address=getCoreIP($dbADO);
				if(is_null($address)){
					$address=$GLOBALS['coreIP'];
					write_log("\n\n$TEXT_ERROR_MISSING_IP_ADDRESS_CONST\n");
				}
			}
			$port=$ProxyOrbiterInfo['Port'][0];
			$deviceID=$ProxyOrbiterInfo['DeviceID'][0];
		}else{
			$address=@$_REQUEST['address'];
			$port=@$_REQUEST['port'];
			$deviceID=(int)$_REQUEST['deviceID'];
			xml_die($deviceID,$address,$port,$command,"\n\n$TEXT_ERROR_ORBITER_PROXY_NOT_FOUND_FOR_IP_CONST ".$phoneIPAddress."\n",$TEXT_ERROR_INVALID_PHONE_IP_CONST.' '.$phoneIPAddress);
		}

	}else{
		$address=@$_REQUEST['address'];
		$port=@$_REQUEST['port'];
		$deviceID=(int)$_REQUEST['deviceID'];
	}
	$refresh=@$_REQUEST['refresh'];
	$command=$_REQUEST['command'];

	$out=$command.'<br>';
		
	/* Create a TCP/IP socket. */

	write_log("\n".miliseconds_date()."\nAttempting to create socket on host $address port $port ... ");
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false) {
		xml_die($deviceID,$address,$port,$command,"socket_create() failed: reason: " . socket_strerror($socket) . "\n",$TEXT_LOADING_CONST);
	} else {
		write_log("OK.\n");
	}

	$result = @socket_connect($socket, $address, $port);

	if ($result !==true) {
		@web_socket_close($socket);
		// if the orbiter is regenerating, query Orbiter table to check status

		$orbiterFields=getFieldsAsArray('Orbiter','RegenInProgress,RegenStatus,RegenPercent',$dbADO,'WHERE PK_Orbiter='.$deviceID);
		if(count($orbiterFields)>0){
			$orb_status='Regenerating '.trim(strtolower(substr($orbiterFields['RegenStatus'][0],0,strpos($orbiterFields['RegenStatus'][0],'-'))));
			$orb_status.=': '.$orbiterFields['RegenPercent'][0].'%';
			
			$err_msg=($orbiterFields['RegenInProgress'][0]==1)?$orb_status:$TEXT_LOADING_CONST;
		}else{
			$err_msg=$TEXT_LOADING_CONST;
		}
		
		xml_die($deviceID,$address,$port,$command,"socket_connect() failed.\nReason: (".socket_last_error().") " . socket_strerror(socket_last_error()) . "\n",$err_msg);
	} else {
		write_log(miliseconds_date()."Connected on socket ... OK.\n");
	}

	write_log(miliseconds_date()."Received parameters ".$_SERVER['QUERY_STRING']."\n");
	
	// get image
	if($command=='IMAGE'){
		$out.=getImage($deviceID,$socket,$dbADO,$refresh);
	}
	
	// send touch command
	if(strpos($command,'TOUCH')!==false){
		$out.=sendCommand($deviceID,$socket,$dbADO,$command,$refresh);
	}
	
	// send XML command
	if(strpos($command,'XML')!==false){
		
		$x=(int)@$_REQUEST['x'];
		$y=(int)@$_REQUEST['y'];
		if($x!=0 && $y!=0){
			$out.=sendCommand($deviceID,$socket,$dbADO,'TOUCH '.$x.'x'.$y,$refresh);
		}else{
			$key=@$_REQUEST['key'];
			if($key==''){
				getImage($deviceID,$socket,$dbADO);
			}else{
				$out.=sendCommand($deviceID,$socket,$dbADO,'PLUTO_KEY '.$key,$refresh);
			}
		}
		$XML=getXML($deviceID,$dbADO,$socket);
		web_socket_close($socket);
		$refreshURL="http://".$_SERVER['SERVER_ADDR']."/pluto-admin/index.php?section=proxySocket&address=$address&port=$port&command=XML&deviceID=$deviceID";
		
		//Header("Refresh: 5; url=$refreshURL");
		Header("Content-type: text/xml"); 
		write_log( miliseconds_date()."Redirecting to $refreshURL\n");
		die($XML);
	}	
	
	
	write_log(miliseconds_date()."Closing socket ... ");
	@web_socket_close($socket);
	write_log( "OK.\n");
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getImage($deviceID,$socket,$dbADO,$refresh=''){
	global $port,$address;
	$in = "IMAGE ".$deviceID."\n";
	$out='';

	write_log(miliseconds_date()."Sending to device $deviceID the command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log(miliseconds_date()."Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}

	$outResponse=web_socket_read($deviceID,$dbADO,$socket,2048,PHP_NORMAL_READ);

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
			$isSaved=writeFile(getcwd().'/security_images/'.$deviceID.'_cisco.png',$outImage);
			write_log(miliseconds_date()."Received image size $imageSize \n");
			
		}
	}
	if((int)$refresh>0){
		$out.=reloadImageJS($deviceID);
	}
	
	return $out;
}

function getXML($deviceID,$dbADO,$socket){
	$in = "XML $deviceID\n";
	$out='';
	
	write_log(miliseconds_date()."Sending to device $deviceID the command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log(miliseconds_date()."Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}

	$outResponse=web_socket_read($deviceID,$dbADO,$socket,2048,PHP_NORMAL_READ);

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
			write_log(miliseconds_date()."Received xml size $xmlSize \n");
		}
	}
	$out.=@$outXML;
	
	
	return $out;
}



function sendCommand($deviceID,$socket,$dbADO,$command,$refresh){
	$in = $command."\n";

	$out='';

	write_log(miliseconds_date()."Sending command to device $deviceID $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log("Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}
	

	$outResponse=web_socket_read($deviceID,$dbADO,$socket,2048,PHP_NORMAL_READ);
	$out.=$outResponse;

	$out.=getImage($deviceID,$socket,$dbADO,$refresh);
	
	return $out;
}

// reload the image "screen" from parent
function reloadImageJS($deviceID){
	$js='
		<script>
			parent.document.getElementById("screen").src="rooms/spacer.gif";
			parent.document.getElementById("screen").src="include/image.php?imagepath='.getcwd().'/security_images/'.$deviceID.'_cisco.png&randno='.rand(0,10000).'";
		</script>
	';
	
	return $js;
}

function write_log($log){
	writeFile(getcwd().'/security_images/socket.log',$log,'a+');
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

function xml_die($deviceID,$address,$port,$command,$message,$userFriendlyMessage=''){
	// the phone has 32 characters limit for values
	write_log(trim($message));
	write_log("\n$userFriendlyMessage");
	$xmlMessage=($userFriendlyMessage!='')?$userFriendlyMessage:str_replace("\n","",trim($message));
	$xml='
<CiscoIPPhoneGraphicFileMenu>
	<Title>Pluto Orbiter</Title>
	<Prompt>'.substr($xmlMessage,0,31).'</Prompt>
	<LocationX>0</LocationX>
	<LocationY>0</LocationY>
	<URL>http://'.$GLOBALS['coreIP'].'/pluto-admin/security_images/generic_xml_error.png</URL>
	<MenuItem>
		<Name>Button</Name>
			<URL>http://'.$GLOBALS['coreIP'].'/pluto-admin/security_images/generic_xml_error.png</URL>
		<TouchArea X1="0" Y1="0" X2="0" Y2="0"/>
	</MenuItem>	
</CiscoIPPhoneGraphicFileMenu>';

	$refreshURL="http://".$_SERVER['SERVER_ADDR']."/pluto-admin/index.php?section=proxySocket&address=$address&port=$port&command=$command&deviceID=$deviceID";
	Header("Refresh: 5; url=$refreshURL");
	//writeFile(getcwd().'/security_images/urls.txt',$refreshURL."\n\n",'a+');
	Header("Content-type: text/xml"); 
	
	write_log("\nRedirecting to $refreshURL\n");
	die($xml);	
}

function miliseconds_date($format='d-m-Y H:i:s'){
	list($usec, $sec) = explode(" ", microtime()); 
    
	$unix_time=(float)$sec;
	return date($format,$unix_time).'.'.substr((float)($usec),2).' ';
}

function web_socket_read($deviceID,$dbADO,$socket,$length,$type=PHP_BINARY_READ){
	global $port,$address;
	$outResponse= @socket_read($socket, $length,$type);

	if($outResponse===false || trim($outResponse)==='NOT_CONNECTED'){
		$last_error=socket_strerror(socket_last_error($socket));
		write_log(miliseconds_date()."Failed reading socket: ".$last_error."\n");
		// if the orbiter is regenerating, query Orbiter table to check status
		$orbiterFields=getFieldsAsArray('Orbiter','RegenInProgress,RegenStatus,RegenPercent',$dbADO,'WHERE PK_Orbiter='.$deviceID);
		if(count($orbiterFields)>0){
			$err_msg=($orbiterFields[0]['RegenInProgress']==1)?'Regeneration '.$orbiterFields[0]['RegenPercent']:$TEXT_LOADING_CONST;
		}else{
			$err_msg=$TEXT_LOADING_CONST;
		}
		xml_die($deviceID,$address,$port,'XML',"\n\nConnection reset by peer\n",$err_msg);
	}else{
		write_log(miliseconds_date()."Read: ".$outResponse."\n");
	}
	
	return $outResponse;
}

function web_socket_close($socket){
	$last_error=socket_strerror(socket_last_error($socket));
	write_log(miliseconds_date()."Closing socket: ".$last_error."\n");
	@socket_close($socket);
}
?>

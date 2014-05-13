<?php
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

function get_installation_array($userID,$dbADO,$installationID=0){
	$instalationsArray=getAssocArray('Installation','PK_Installation','CONCAT(\'#\',PK_Installation,\' \',Description)',$dbADO,'INNER JOIN Installation_Users ON FK_Installation=PK_Installation WHERE FK_Users='.$userID);
	return $instalationsArray;
}

function get_devices_array($installationID,$userID,$dbADO,$deviceID=0){
	$installationID=($installationID==0)?$GLOBALS['installationID']:$installationID;
	$devicesArray=getAssocArray('Device','PK_Device','Description',$dbADO,'WHERE FK_DeviceTemplate='.$GLOBALS['GenericWebDevice'].' AND FK_Installation='.$installationID.' ORDER BY Description ASC');
	return $devicesArray;
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

function get_web_orbiter_info($deviceID, $dbADO)
{
	$ProxyOrbiterInfo=getFieldsAsArray('Device','Device.PK_Device,Device.FK_Device_ControlledVia,DD.IK_DeviceData AS Port,Parent.IPAddress,Device.Description',$dbADO,'INNER JOIN Device_DeviceData DD ON DD.FK_Device=Device.PK_Device AND DD.FK_DeviceData='.$GLOBALS['ListenPort'].' LEFT JOIN Device Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device WHERE Device.FK_DeviceTemplate='.$GLOBALS['ProxyOrbiter'].' AND Parent.PK_Device IS NOT NULL AND Device.FK_Device_ControlledVia='.$deviceID);
	return $ProxyOrbiterInfo;
}

function openSocket($address, $port)
{
	$socket = web_socket_create($address,$port, AF_INET, SOCK_STREAM, SOL_TCP);
	if($socket === false){
		return false;
	}
	
	// connect on socket to proxy orbiter
	$result = web_proxy_connect($socket, $address, $port);
	if($result === false){
		web_socket_close($socket);
		return false;
	}

	return $socket;
}

function closeSocket($socket)
{
	web_socket_close($socket);
}

// old proxyOrbiter functions
function getImage($deviceID,$socket,$refresh=0){
	$in = "IMAGE ".rand(10000,11000)."\n";

	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);

	$imageSize=(int)trim(str_replace('IMAGE ','',$outResponse));
	$filename = false;
	if($imageSize>0){
		$remaining=$imageSize;
		$outImage='';
		while($remaining>0)
		{
			$linesize=($remaining<2048)?$remaining:2048;
			$chunk=@socket_read($socket, $linesize,PHP_BINARY_READ);
			$outImage.= $chunk;
			$remaining-=strlen($chunk);
			write_log("Retrieved image chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($outImage))
		{
			$filename = getcwd().'/security_images/'.$deviceID.'_web.png';
			$isSaved = writeFile($filename, $outImage);
			write_log("Received image size $imageSize \n");
		}
	}

	return $filename;
}

function getImageToRAM($deviceID,$socket)
{
	$in = "IMAGE " . rand(10000, 11000) . "\n";

	$written = web_socket_write($socket, $in);
	$outResponse = web_socket_read($socket, 2048, PHP_NORMAL_READ);

	$imageSize = (int)trim(str_replace('IMAGE ', '', $outResponse));
	$outImage = false;

	if ($imageSize > 0)
	{
		$remaining = $imageSize;
		$outImage = '';
		while ($remaining > 0)
		{
			$linesize = ($remaining < 2048) ? $remaining : 2048;
			$chunk = @socket_read($socket, $linesize, PHP_BINARY_READ);
			$outImage .= $chunk;
			$remaining -= strlen($chunk);
			write_log("Retrieved image chunk " . strlen($chunk) . ", remaining $remaining \n");
		}
		
		write_log("Received image size $imageSize \n");
	}

	return $outImage;
}

function getXML($socket){
	$in = "XML ".rand(10000,11000)."\n";
	$outXML = '';
	
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
	
	return $outXML;
}

function getNews($socket)
{
	$in = "ANYNEWS? ".rand(10000,11000)."\n";
	$out = '';
	
	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);

	$Size = (int)trim(str_replace('NEWS ', '', $outResponse));
	if ($Size > 0)
	{
		$remaining = $Size;
		$out = '';
		while ($remaining > 0)
		{
			$linesize = ($remaining < 2048) ? $remaining : 2048;
			$chunk = @socket_read($socket, $linesize, PHP_BINARY_READ);
			$out .= $chunk;
			$remaining -= strlen($chunk);
			write_log("Retrieved news chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($out))
		{
			write_log("Received news size $Size\n");
		}
	}
	
	return trim($out);
}


function sendCommand($deviceID,$socket,$command,$refresh=''){
	$in = $command."\n";

	$written=web_socket_write($socket, $in);
	$outResponse= web_socket_read($socket, 2048,PHP_NORMAL_READ);

	// ERROR and OK are sent stand-alone
	if (strpos($outResponse, "ERROR") === 0 || strpos($outResponse, "OK") === 0)
		return $outResponse;

	// everything else contains a reply size
	// use it to read the reply
	$space = strpos($outResponse, " ");
	if ($space === false)
		return "ERROR\n"; // something wrong happened

	$Size = (int)trim(substr($outResponse, $space + 1));
	if ($Size > 0)
	{
		$remaining = $Size;
		$out = '';
		while ($remaining > 0)
		{
			$linesize = ($remaining < 2048) ? $remaining : 2048;
			$chunk = @socket_read($socket, $linesize, PHP_BINARY_READ);
			$out .= $chunk;
			$remaining -= strlen($chunk);
			write_log("Retrieved news chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($out))
		{
			write_log("Received news size $Size\n");
		}
	}
	
	// send both the result message and its contents
	return $outResponse.$out;
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
?>

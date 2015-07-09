<?php

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
   
set_include_path('operations/cloudServices/google-api-php-client/src');	
//set_include_path('operations/cloudServices/includes');	
require_once('Google/autoload.php');
define('CREDENTIALS_PATH', '/tmp/user'.$_SESSION['userID'].'.json');
define('CLIENT_SECRET_PATH', '/var/www/lmce-admin/operations/cloudServices/includes/client_secret.json');
define('SCOPES', implode(' ', array(
  Google_Service_Drive::DRIVE_METADATA_READONLY)
));;

function refreshToken($token){	
	echo "Current Token:".$token;	
	return true;
}

function getClient($auth){
		
 $latestToken=file_get_contents(CLIENT_SECRET_PATH);
  $secret = file_get_contents(CLIENT_SECRET_PATH);
  if($latestToken==""){
  	$latestToken=$auth;
  }	
  $client = new Google_Client();
  $client->setApplicationName("LinuxMCE");
  $client->setScopes(SCOPES);  
  $client->setAuthConfigFile(CLIENT_SECRET_PATH);
  $client->setAccessType('offline');

  $client->setAccessToken(json_encode($auth));

  // Refresh the token if it's expired.
  if ($client->isAccessTokenExpired()) {
    $client->refreshToken($client->getRefreshToken());
	 updateTokenData($client->getAccessToken());
   // file_put_contents(CREDENTIALS_PATH, $client->getAccessToken());
  }
  return $client;
}

function listDriveDirectories(){
	$local_client=$getClient();
	$service = new Google_Service_Drive($localClient);
}

function updateTokenData($newToken){
	
	$credentialsPath = CREDENTIALS_PATH;
	  if(!file_exists(dirname($credentialsPath))) {
      mkdir(dirname($credentialsPath), 0700, true);
    }
    file_put_contents($credentialsPath, $newToken);
   // printf("Credentials saved to %s\n", $credentialsPath);
	
	
	//$updateQuery="UPDATE Device_DeviceData SET IK_DeviceData= ' ".$newDataString." ' where FK_DEVICE=".$ddForDevice['PK_Device']." AND FK_DeviceData=59";
	//$dbADO->Execute($updateQuery);
}
   function testMetadata($gclient){
   	$service = new Google_Service_Drive($gclient);
		$optParams = array(
  'maxResults' => 10,
);
$results = $service->files->listFiles($optParams);

if (count($results->getItems()) == 0) {
  print "No files found.\n";
} else {
  print "Files:\n";
  foreach ($results->getItems() as $file) {
    printf("%s (%s)\n", $file->getTitle(), $file->getId());
  }
}
   }
   
?>
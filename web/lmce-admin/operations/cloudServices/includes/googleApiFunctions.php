<?php

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
   
set_include_path('operations/cloudServices/google-api-php-client/src');	
//set_include_path('operations/cloudServices/includes');	
require_once('Google/autoload.php');
define('CREDENTIALS_PATH', '/tmp/user'.$_SESSION['userID'].'.json');
define('CLIENT_SECRET_PATH', '/var/www/lmce-admin/operations/cloudServices/includes/client_secret.json');
define('MIME_TYPE_GDRIVE_FOLDER', "application/vnd.google-apps.folder");

define('SCOPES', implode(' ', array(
   "https://www.googleapis.com/auth/drive")
));;


function refreshToken($token){	
	echo "Current Token:".$token;	
	return true;
}

function getClient($auth){

 	
	
  $latestToken=json_decode( file_get_contents(CREDENTIALS_PATH) , true);
  if(empty($latestToken)){
  	
  }
  $secret = file_get_contents(CLIENT_SECRET_PATH);
  if(empty( $latestToken ) && !empty($auth)){
  	$latestToken=$auth;
  }	
  $client = new Google_Client();
  $client->setApplicationName("LinuxMCE");
  $client->setScopes(SCOPES);  
  $client->setAuthConfigFile(CLIENT_SECRET_PATH);
  $client->setAccessType('offline');

  $client->setAccessToken(json_encode($latestToken));

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
  	'maxResults' => 50,
  	"q"=>" mimeType='application/vnd.google-apps.folder'",
  	"q"=>"title contains 'Linux' "
);
$results = $service->files->listFiles($optParams);

$fileData=$results->getItems();
$returnData="";

if (count($results->getItems()) == 0) {
 $returnData="No files found.\n";
} else {
 $returnData.="Files:<br>";
 $folderExists=false;
  foreach ($results->getItems() as $file) {
  	if($file->getTitle()==="LinuxMCE Security Images"){
  		$folderExists=true;
  	}
    $returnData.="<b>Filename:".$file->getTitle()."</b> ID:".$file->getId()."<br><br>";
  }
  
  if(!$folderExists){
  	setupFolder();
  }
}
 return $returnData;
 
   }
   
   function setupFolder(){
   	$client=getClient(array());
	if(!$client->getAccessToken())
	{
		return;
	}
	$service = new Google_Service_Drive($client);

	
	$file = new Google_Service_Drive_DriveFile();
	$file->setTitle("LinuxMCE Security Images");
	$file->setParents(array ("root"));
	$file->setDescription("LinuxMCE Security Photos");
	$file->setMimeType("application/vnd.google-apps.folder");
	$opts=array(        
        'mimeType' => "application/vnd.google-apps.folder"
      );
	 $createdFile = $service->files->insert($file, $opts);
	
 
	
   }
   
?>
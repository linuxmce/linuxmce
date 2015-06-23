<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
set_include_path('operations/cloudServices/google-api-php-client/src');	
require_once('Google/autoload.php');

  function completeAuth($mediadbADO, $dbADO, $output){

$client = new Google_Client();
$client->setAuthConfigFile('/tmp/client_secret.json');
$client->setRedirectUri('http://' . $_SERVER['HTTP_HOST'] . '/lmce-amin/index.php?section=cloudServices&app=gmail');
$client->addScope(Google_Service_Drive::DRIVE_METADATA_READONLY);

if (! isset($_GET['code'])) {
  $auth_url = $client->createAuthUrl();
  header('Location: ' . filter_var($auth_url, FILTER_SANITIZE_URL));
} else {
  $client->authenticate($_GET['code']);
  $_SESSION['access_token'] = $client->getAccessToken();
  $redirect_uri = 'http://' . $_SERVER['HTTP_HOST'] . '/lmce-admin/index.php?section=cloudServices&app=gmail';
  echo $_SESSION['access_token'];
 header('Location: ' . filter_var($redirect_uri, FILTER_SANITIZE_URL));
}
	$out='';

	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services | Authorize Gmail Complete');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	}
?>